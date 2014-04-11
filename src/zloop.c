/*  =========================================================================
    zloop - event-driven reactor

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The zloop class provides an event-driven reactor pattern. The reactor
    handles zmq_pollitem_t items (pollers or writers, sockets or fds), and
    once-off or repeated timers. Its resolution is 1 msec. It uses a tickless
    timer to reduce CPU interrupts in inactive processes.
@discuss
@end
*/

#include "../include/czmq.h"

typedef struct _s_poller_t s_poller_t;
typedef struct _s_timer_t s_timer_t;

//  Structure of our class

struct _zloop_t {
    zlist_t *pollers;           //  List of poll items
    zlist_t *timers;            //  List of timers
    int last_timer_id;          //  Most recent timer id 
    size_t poll_size;           //  Size of poll set
    zmq_pollitem_t *pollset;    //  zmq_poll set
    s_poller_t *pollact;        //  Pollers for this poll set
    bool need_rebuild;          //  True if pollset needs rebuilding
    bool verbose;               //  True if verbose tracing wanted
    bool terminated;            //  True when stopped running
    zlist_t *zombies;           //  List of timers to kill
};

//  Pollers and timers are held as small structures of their own
struct _s_poller_t {
    zmq_pollitem_t item;
    zloop_fn *handler;
    void *arg;                  //  Application argument to poll item
    int errors;                 //  If too many errors, kill poller
    bool tolerant;              //  Unless configured as tolerant
};

struct _s_timer_t {
    int timer_id;               //  Unique timer id, used to cancel timer
    size_t delay;               //  Delay (ms) between executing
    size_t times;               //  Number of times to repeat, 0 for forever
    zloop_timer_fn *handler;    //  Function to execute
    void *arg;                  //  Application argument to timer
    int64_t when;               //  Clock time when alarm goes off
};

static int
s_next_timer_id (zloop_t *self)
{
    return ++self->last_timer_id;
}

static s_poller_t *
s_poller_new (zmq_pollitem_t *item, zloop_fn handler, void *arg)
{
    s_poller_t *poller = (s_poller_t *) zmalloc (sizeof (s_poller_t));
    if (poller) {
        poller->item = *item;
        poller->handler = handler;
        poller->arg = arg;
        poller->tolerant = false;   //  By default, errors are bad
    }
    return poller;
}

static s_timer_t *
s_timer_new (int timer_id, size_t delay, size_t times, zloop_timer_fn handler, void *arg)
{
    s_timer_t *timer = (s_timer_t *) zmalloc (sizeof (s_timer_t));
    if (timer) {
        timer->timer_id = timer_id;
        timer->delay = delay;
        timer->times = times;
        timer->handler = handler;
        timer->arg = arg;
        timer->when = -1;           //  Indicates a new timer
    }
    return timer;
}

//  Remove timer with specified id, if it exists

static void
s_timer_remove (zloop_t *self, int timer_id)
{
    s_timer_t *timer = (s_timer_t *) zlist_first (self->timers);
    while (timer) {
        if (timer->timer_id == timer_id) {
            zlist_remove (self->timers, timer);
            free (timer);
            break;
        }
        timer = (s_timer_t *) zlist_next (self->timers);
    }
}


//  We hold an array of pollers that matches the pollset, so we can
//  register/cancel pollers orthogonally to executing the pollset
//  activity on pollers. Returns 0 on success, -1 on failure.

static int
s_rebuild_pollset (zloop_t *self)
{
    free (self->pollset);
    free (self->pollact);
    self->pollset = NULL;
    self->pollact = NULL;

    self->poll_size = zlist_size (self->pollers);
    self->pollset = (zmq_pollitem_t *) zmalloc (
        self->poll_size * sizeof (zmq_pollitem_t));
    if (!self->pollset)
        return -1;

    self->pollact = (s_poller_t *) zmalloc (
        self->poll_size * sizeof (s_poller_t));
    if (!self->pollact)
        return -1;

    s_poller_t *poller = (s_poller_t *) zlist_first (self->pollers);
    uint item_nbr = 0;
    while (poller) {
        self->pollset [item_nbr] = poller->item;
        self->pollact [item_nbr] = *poller;
        item_nbr++;
        poller = (s_poller_t *) zlist_next (self->pollers);
    }
    self->need_rebuild = false;
    return 0;
}

static long
s_tickless_timer (zloop_t *self)
{
    //  Calculate tickless timer, up to 1 hour
    int64_t tickless = zclock_time () + 1000 * 3600;
    s_timer_t *timer = (s_timer_t *) zlist_first (self->timers);
    while (timer) {
        //  Find earliest timer
        if (timer->when == -1)
            timer->when = timer->delay + zclock_time ();
        if (tickless > timer->when)
            tickless = timer->when;
        timer = (s_timer_t *) zlist_next (self->timers);
    }
    long timeout = (long) (tickless - zclock_time ());
    if (timeout < 0)
        timeout = 0;
    if (self->verbose)
	zclock_log ("I: zloop: polling for %d msec", (int)timeout);
    return timeout;
}


//  --------------------------------------------------------------------------
//  Constructor

zloop_t *
zloop_new (void)
{
    zloop_t
        *self;

    self = (zloop_t *) zmalloc (sizeof (zloop_t));
    if (self) {
        self->pollers = zlist_new ();
        self->timers  = zlist_new ();
        self->zombies = zlist_new ();
        self->last_timer_id = 0;
        if (!self->pollers
        ||  !self->timers
        ||  !self->zombies) {
            zlist_destroy (&self->pollers);
            zlist_destroy (&self->timers);
            zlist_destroy (&self->zombies);
            free (self);
            return NULL;
        }
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zloop_destroy (zloop_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zloop_t *self = *self_p;

        //  Destroy list of pollers
        while (zlist_size (self->pollers))
            free (zlist_pop (self->pollers));
        zlist_destroy (&self->pollers);

        //  Destroy list of timers
        while (zlist_size (self->timers))
            free (zlist_pop (self->timers));
        zlist_destroy (&self->timers);

        //  Destroy zombie timer list
        //  Which must always be empty here
        assert (zlist_size (self->zombies) == 0);
        zlist_destroy (&self->zombies);

        free (self->pollset);
        free (self->pollact);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Register pollitem with the reactor. When the pollitem is ready, will call
//  the handler, passing the arg. Returns 0 if OK, -1 if there was an error.
//  If you register the pollitem more than once, each instance will invoke its
//  corresponding handler.
//  A pollitem with socket=NULL and fd=0 will be interpreted as 'poll on fd zero'.

int
zloop_poller (zloop_t *self, zmq_pollitem_t *item, zloop_fn handler, void *arg)
{
    assert (self);

    if (item->socket
    &&  streq (zsocket_type_str (item->socket), "UNKNOWN"))
        return -1;

    s_poller_t *poller = s_poller_new (item, handler, arg);
    if (poller) {
        if (zlist_append (self->pollers, poller))
            return -1;

        self->need_rebuild = true;
        if (self->verbose)
            zclock_log ("I: zloop: register %s poller (%p, %d)",
                item->socket? zsocket_type_str (item->socket): "FD",
                item->socket, item->fd);
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Cancel a pollitem from the reactor, specified by socket or FD. If both
//  are specified, uses only socket. If multiple poll items exist for same
//  socket/FD, cancels ALL of them.

void
zloop_poller_end (zloop_t *self, zmq_pollitem_t *item)
{
    assert (self);
    assert (item->socket || item->fd);

    s_poller_t *poller = (s_poller_t *) zlist_first (self->pollers);
    while (poller) {
        if ((item->socket && item->socket == poller->item.socket)
        ||  (item->fd     && item->fd     == poller->item.fd)) {
            zlist_remove (self->pollers, poller);
            free (poller);
            //  Force rebuild to avoid reading from freed poller
            self->need_rebuild = true;
        }
        poller = (s_poller_t *) zlist_next (self->pollers);
    }
    if (self->verbose)
        zclock_log ("I: zloop: cancel %s poller (%p, %d)",
            item->socket? zsocket_type_str (item->socket): "FD",
            item->socket, item->fd);
}


//  --------------------------------------------------------------------------
//  Configure a registered pollitem to ignore errors. If you do not set this, 
//  then pollitems that have errors are removed from the reactor silently.

void
zloop_set_tolerant (zloop_t *self, zmq_pollitem_t *item)
{
    assert (self);
    assert (item->socket || item->fd);

    //  Find matching poller(s) and mark as tolerant
    s_poller_t *poller = (s_poller_t *) zlist_first (self->pollers);
    while (poller) {
        if ((item->socket && item->socket == poller->item.socket)
        ||  (item->fd     && item->fd     == poller->item.fd))
            poller->tolerant = true;
            
        poller = (s_poller_t *) zlist_next (self->pollers);
    }
}


//  --------------------------------------------------------------------------
//  Register a timer that expires after some delay and repeats some number of
//  times. At each expiry, will call the handler, passing the arg. To run a
//  timer forever, use 0 times. Returns a timer_id that is used to cancel the
//  timer in the future. Returns -1 if there was an error.

int
zloop_timer (zloop_t *self, size_t delay, size_t times, zloop_timer_fn handler, void *arg)
{
    assert (self);
    int timer_id = s_next_timer_id (self);
    s_timer_t *timer = s_timer_new (timer_id, delay, times, handler, arg);
    if (!timer)
        return -1;
    if (zlist_append (self->timers, timer))
        return -1;
    if (self->verbose)
        zclock_log ("I: zloop: register timer id=%d delay=%zd times=%zd", 
                    timer_id, delay, times);
    
    return timer_id;
}


//  --------------------------------------------------------------------------
//  Cancel a timer by timer id (as returned by zloop_timer()).
//  Returns 0 on success.

int
zloop_timer_end (zloop_t *self, int timer_id)
{
    assert (self);
    
    if (self->terminated)
        s_timer_remove (self, timer_id);
    else
    //  We cannot touch self->timers because we may be executing that
    //  from inside the poll loop. So, we hold the arg on the zombie
    //  list, and process that list when we're done executing timers.
    //  This hack lets us store an integer timer ID as a pointer
    if (zlist_append (self->zombies, (byte *) NULL + timer_id))
        return -1;

    if (self->verbose)
        zclock_log ("I: zloop: cancel timer id=%d", timer_id);
    
    return 0;
}


//  --------------------------------------------------------------------------
//  Set verbose tracing of reactor on/off

void
zloop_set_verbose (zloop_t *self, bool verbose)
{
    assert (self);
    self->verbose = verbose;
}


//  --------------------------------------------------------------------------
//  Start the reactor. Takes control of the thread and returns when the 0MQ
//  context is terminated or the process is interrupted, or any event handler
//  returns -1. Event handlers may register new sockets and timers, and
//  cancel sockets. Returns 0 if interrupted, -1 if canceled by a handler,
//  positive on internal error

int
zloop_start (zloop_t *self)
{
    assert (self);
    int rc = 0;

    //  Recalculate all timers now
    s_timer_t *timer = (s_timer_t *) zlist_first (self->timers);
    while (timer) {
        timer->when = timer->delay + zclock_time ();
        timer = (s_timer_t *) zlist_next (self->timers);
    }
    //  Main reactor loop
    while (!zctx_interrupted) {
        if (self->need_rebuild) {
            //  If s_rebuild_pollset() fails, break out of the loop and
            //  return its error
            rc = s_rebuild_pollset (self);
            if (rc)
                break;
        }
        rc = zmq_poll (self->pollset, (int) self->poll_size,
                       s_tickless_timer (self) * ZMQ_POLL_MSEC);
        if (rc == -1 || zctx_interrupted) {
            if (self->verbose)
                zclock_log ("I: zloop: interrupted (%d) - %s", rc, 
                            zmq_strerror (zmq_errno ()));
            rc = 0;
            break;              //  Context has been shut down
        }
        //  Handle any timers that have now expired
        timer = (s_timer_t *) zlist_first (self->timers);
        while (timer) {
            if (zclock_time () >= timer->when && timer->when != -1) {
                if (self->verbose)
                    zclock_log ("I: zloop: call timer id=%d handler", timer->timer_id);
                rc = timer->handler (self, timer->timer_id, timer->arg);
                if (rc == -1)
                    break;      //  Timer handler signaled break
                if (timer->times && --timer->times == 0) {
                    zlist_remove (self->timers, timer);
                    free (timer);
                }
                else
                    timer->when = timer->delay + zclock_time ();
            }
            timer = (s_timer_t *) zlist_next (self->timers);
        }
        //  Handle any pollers that are ready
        size_t item_nbr;
        for (item_nbr = 0; item_nbr < self->poll_size && rc >= 0; item_nbr++) {
            s_poller_t *poller = &self->pollact [item_nbr];
            assert (self->pollset [item_nbr].socket == poller->item.socket);
            
            if ((self->pollset [item_nbr].revents & ZMQ_POLLERR)
            && !poller->tolerant) {
                if (self->verbose)
                    zclock_log ("W: zloop: can't poll %s socket (%p, %d): %s",
                        poller->item.socket?
                            zsocket_type_str (poller->item.socket): "FD",
                        poller->item.socket, poller->item.fd,
                        zmq_strerror (zmq_errno ()));
                //  Give handler one chance to handle error, then kill
                //  poller because it'll disrupt the reactor otherwise.
                if (poller->errors++) {
                    zloop_poller_end (self, &poller->item);
                    self->pollset [item_nbr].revents = 0;
                }
            }
            else
                poller->errors = 0;     //  A non-error happened

            if (self->pollset [item_nbr].revents) {
                if (self->verbose)
                    zclock_log ("I: zloop: call %s socket handler (%p, %d)",
                        poller->item.socket?
                            zsocket_type_str (poller->item.socket): "FD",
                        poller->item.socket, poller->item.fd);
                rc = poller->handler (self, &self->pollset [item_nbr], poller->arg);
                if (rc == -1 || self->need_rebuild)
                    break; 
            }
        }
        //  Now handle any timer zombies
        //  This is going to be slow if we have many timers; we might use
        //  a faster lookup on the timer list.
        while (zlist_size (self->zombies)) {
            //  Get timer_id back from pointer
            int timer_id = (byte *) zlist_pop (self->zombies) - (byte *) NULL;
            s_timer_remove (self, timer_id);
        }
        if (rc == -1)
            break;
    }
    self->terminated = true;
    return rc;
}


//  --------------------------------------------------------------------------
//  Selftest

static int
s_cancel_timer_event (zloop_t *loop, int timer_id, void *arg)
{
    //  We are handling timer 2, and will cancel timer 1
    int cancel_timer_id = *((int *) arg);
    return zloop_timer_end (loop, cancel_timer_id);
}

static int
s_timer_event (zloop_t *loop, int timer_id, void *output)
{
    zstr_send (output, "PING");
    return 0;
}

static int
s_socket_event (zloop_t *loop, zmq_pollitem_t *item, void *arg)
{
    //  Just end the reactor
    return -1;
}

void
zloop_test (bool verbose)
{
    printf (" * zloop: ");
    int rc = 0;
    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);

    void *output = zsocket_new (ctx, ZMQ_PAIR);
    assert (output);
    zsocket_bind (output, "inproc://zloop.test");
    void *input = zsocket_new (ctx, ZMQ_PAIR);
    assert (input);
    zsocket_connect (input, "inproc://zloop.test");

    zloop_t *loop = zloop_new ();
    assert (loop);
    zloop_set_verbose (loop, verbose);

    //  Create a timer that will be canceled
    int timer_id = zloop_timer (loop, 1000, 1, s_timer_event, NULL);
    zloop_timer (loop, 5, 1, s_cancel_timer_event, &timer_id);
    
    //  After 20 msecs, send a ping message to output
    zloop_timer (loop, 20, 1, s_timer_event, output);
    
    //  When we get the ping message, end the reactor
    zmq_pollitem_t poll_input = { input, 0, ZMQ_POLLIN };
    rc = zloop_poller (loop, &poll_input, s_socket_event, NULL);    
    assert (rc == 0);
    zloop_set_tolerant (loop, &poll_input);
    zloop_start (loop);

    zloop_destroy (&loop);
    assert (loop == NULL);

    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
}
