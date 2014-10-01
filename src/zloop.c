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

typedef struct _s_reader_t s_reader_t;
typedef struct _s_poller_t s_poller_t;
typedef struct _s_timer_t s_timer_t;

//  Structure of our class

struct _zloop_t {
    zlist_t *readers;           //  List of socket readers
    zlist_t *pollers;           //  List of poll items
    zlist_t *timers;            //  List of timers
    int last_timer_id;          //  Most recent timer id
    size_t poll_size;           //  Size of poll set
    zmq_pollitem_t *pollset;    //  zmq_poll set
    s_reader_t *readact;        //  Readers for this poll set
    s_poller_t *pollact;        //  Pollers for this poll set
    bool need_rebuild;          //  True if pollset needs rebuilding
    bool verbose;               //  True if verbose tracing wanted
    bool terminated;            //  True when stopped running
    zlist_t *zombies;           //  List of timers to kill
};

//  Reactor elements are held as structures of their own

struct _s_reader_t {
    zsock_t *sock;              //  Socket to read from
    zloop_reader_fn *handler;   //  Function to execute
    void *arg;                  //  Application argument to poll item
    int errors;                 //  If too many errors, kill reader
    bool tolerant;              //  Unless configured as tolerant
};

struct _s_poller_t {
    zmq_pollitem_t item;
    zloop_fn *handler;          //  Function to execute
    void *arg;                  //  Application argument to poll item
    int errors;                 //  If too many errors, kill poller
    bool tolerant;              //  Unless configured as tolerant
};

struct _s_timer_t {
    int timer_id;               //  Unique timer id, used to cancel timer
    zloop_timer_fn *handler;    //  Function to execute
    size_t delay;               //  Delay (ms) between executing
    size_t times;               //  Number of times to repeat, 0 for forever
    void *arg;                  //  Application argument to timer
    int64_t when;               //  Clock time when alarm goes off
};

static int
s_next_timer_id (zloop_t *self)
{
    return ++self->last_timer_id;
}

static s_reader_t *
s_reader_new (zsock_t *sock, zloop_reader_fn handler, void *arg)
{
    s_reader_t *reader = (s_reader_t *) zmalloc (sizeof (s_reader_t));
    if (reader) {
        reader->sock = sock;
        reader->handler = handler;
        reader->arg = arg;
        reader->tolerant = false;   //  By default, errors are bad
    }
    return reader;
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
    free (self->readact);
    free (self->pollact);
    self->pollset = NULL;
    self->readact = NULL;
    self->pollact = NULL;

    self->poll_size = zlist_size (self->readers) + zlist_size (self->pollers);
    self->pollset = (zmq_pollitem_t *) zmalloc (self->poll_size * sizeof (zmq_pollitem_t));
    if (!self->pollset)
        return -1;

    self->readact = (s_reader_t *) zmalloc (self->poll_size * sizeof (s_reader_t));
    if (!self->readact)
        return -1;

    self->pollact = (s_poller_t *) zmalloc (self->poll_size * sizeof (s_poller_t));
    if (!self->pollact)
        return -1;

    s_reader_t *reader = (s_reader_t *) zlist_first (self->readers);
    uint item_nbr = 0;
    while (reader) {
        zmq_pollitem_t poll_item = { zsock_resolve (reader->sock), 0, ZMQ_POLLIN };
        self->pollset [item_nbr] = poll_item;
        self->readact [item_nbr] = *reader;
        item_nbr++;
        reader = (s_reader_t *) zlist_next (self->readers);
    }
    s_poller_t *poller = (s_poller_t *) zlist_first (self->pollers);
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
    int64_t tickless = zclock_mono () + 1000 * 3600;
    s_timer_t *timer = (s_timer_t *) zlist_first (self->timers);
    while (timer) {
        //  Find earliest timer
        if (timer->when == -1)
            timer->when = timer->delay + zclock_mono ();
        if (tickless > timer->when)
            tickless = timer->when;
        timer = (s_timer_t *) zlist_next (self->timers);
    }
    long timeout = (long) (tickless - zclock_mono ());
    if (timeout < 0)
        timeout = 0;
    if (self->verbose)
        zsys_debug ("zloop polling for %d msec", (int) timeout);
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
    if (!self)
        return NULL;

    self->readers = zlist_new ();
    if (self->readers)
        self->pollers = zlist_new ();
    if (self->pollers)
        self->timers = zlist_new ();
    if (self->timers)
        self->zombies = zlist_new ();
    if (self->zombies)
        self->last_timer_id = 0;
    else
        zloop_destroy (&self);
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

        //  Destroy list of readers
        while (zlist_size (self->readers))
            free (zlist_pop (self->readers));
        zlist_destroy (&self->readers);

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
        free (self->readact);
        free (self->pollact);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Register socket reader with the reactor. When the reader has messages,
//  the reactor will call the handler, passing the arg. Returns 0 if OK, -1
//  if there was an error. If you register the same socket more than once,
//  each instance will invoke its corresponding handler.

int
zloop_reader (zloop_t *self, zsock_t *sock, zloop_reader_fn handler, void *arg)
{
    assert (self);
    assert (sock);

    s_reader_t *reader = s_reader_new (sock, handler, arg);
    if (reader) {
        if (zlist_append (self->readers, reader))
            return -1;

        self->need_rebuild = true;
        if (self->verbose)
            zsys_debug ("zloop: register %s reader", zsock_type_str (sock));
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Cancel a socket reader from the reactor. If multiple readers exist for
//  same socket, cancels ALL of them.

void
zloop_reader_end (zloop_t *self, zsock_t *sock)
{
    assert (self);
    assert (sock);

    s_reader_t *reader = (s_reader_t *) zlist_first (self->readers);
    while (reader) {
        if (reader->sock == sock) {
            zlist_remove (self->readers, reader);
            free (reader);
            self->need_rebuild = true;
        }
        reader = (s_reader_t *) zlist_next (self->readers);
    }
    if (self->verbose)
        zsys_debug ("zloop: cancel %s reader", zsock_type_str (sock));
}


//  --------------------------------------------------------------------------
//  Configure a registered reader to ignore errors. If you do not set this,
//  then reader that have errors are removed from the reactor silently.

void
zloop_reader_set_tolerant (zloop_t *self, zsock_t *sock)
{
    assert (self);
    assert (sock);

    s_reader_t *reader = (s_reader_t *) zlist_first (self->readers);
    while (reader) {
        if (reader->sock == sock)
            reader->tolerant = true;
        reader = (s_reader_t *) zlist_next (self->readers);
    }
}


//  --------------------------------------------------------------------------
//  Register low-level libzmq pollitem with the reactor. When the pollitem
//  is ready, will call the handler, passing the arg. Returns 0 if OK, -1
//  if there was an error. If you register the pollitem more than once, each
//  instance will invoke its corresponding handler. A pollitem with
//  socket=NULL and fd=0 means 'poll on FD zero'.

int
zloop_poller (zloop_t *self, zmq_pollitem_t *item, zloop_fn handler, void *arg)
{
    assert (self);

    if (  item->socket
       && streq (zsocket_type_str (item->socket), "UNKNOWN"))
        return -1;

    s_poller_t *poller = s_poller_new (item, handler, arg);
    if (poller) {
        if (zlist_append (self->pollers, poller))
            return -1;

        self->need_rebuild = true;
        if (self->verbose)
            zsys_debug ("zloop: register %s poller (%p, %d)",
                item->socket ? zsocket_type_str (item->socket) : "FD",
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

    s_poller_t *poller = (s_poller_t *) zlist_first (self->pollers);
    while (poller) {
        bool match = false;
        if (item->socket) {
            if (item->socket == poller->item.socket)
                match = true;
        }
        else
        if (item->fd == poller->item.fd)
            match = true;
        if (match) {
            zlist_remove (self->pollers, poller);
            free (poller);
            //  Force rebuild to avoid reading from freed poller
            self->need_rebuild = true;
        }
        poller = (s_poller_t *) zlist_next (self->pollers);
    }
    if (self->verbose)
        zsys_debug ("zloop: cancel %s poller (%p, %d)",
            item->socket ? zsocket_type_str (item->socket) : "FD",
            item->socket, item->fd);
}


//  --------------------------------------------------------------------------
//  Configure a registered poller to ignore errors. If you do not set this,
//  then poller that have errors are removed from the reactor silently.

void
zloop_poller_set_tolerant (zloop_t *self, zmq_pollitem_t *item)
{
    assert (self);

    //  Find matching poller(s) and mark as tolerant
    s_poller_t *poller = (s_poller_t *) zlist_first (self->pollers);
    while (poller) {
        bool match = false;
        if (item->socket) {
            if (item->socket == poller->item.socket)
                match = true;
        }
        else
        if (item->fd == poller->item.fd)
            match = true;
        if (match)
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
#ifdef __WINDOWS__
        zsys_debug ("zloop: register timer id=%d delay=%u times=%u",
            timer_id, delay, times);
#else
        zsys_debug ("zloop: register timer id=%d delay=%zd times=%zd",
            timer_id, delay, times);
#endif

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
        zsys_debug ("zloop: cancel timer id=%d", timer_id);

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
        timer->when = timer->delay + zclock_mono ();
        timer = (s_timer_t *) zlist_next (self->timers);
    }
    //  Main reactor loop
    while (!zsys_interrupted) {
        if (self->need_rebuild) {
            //  If s_rebuild_pollset() fails, break out of the loop and
            //  return its error
            rc = s_rebuild_pollset (self);
            if (rc)
                break;
        }
        rc = zmq_poll (self->pollset, (int) self->poll_size,
            s_tickless_timer (self) * ZMQ_POLL_MSEC);
        if (rc == -1 || zsys_interrupted) {
            if (self->verbose)
                zsys_debug ("zloop: interrupted (%d) - %s", rc,
                    zmq_strerror (zmq_errno ()));
            rc = 0;
            break;              //  Context has been shut down
        }
        //  Handle any timers that have now expired
        timer = (s_timer_t *) zlist_first (self->timers);
        while (timer) {
            if (zclock_mono () >= timer->when && timer->when != -1) {
                if (self->verbose)
                    zsys_debug ("zloop: call timer id=%d handler", timer->timer_id);
                rc = timer->handler (self, timer->timer_id, timer->arg);
                if (rc == -1)
                    break;      //  Timer handler signaled break
                if (timer->times && --timer->times == 0) {
                    zlist_remove (self->timers, timer);
                    free (timer);
                }
                else
                    timer->when += timer->delay;
            }
            timer = (s_timer_t *) zlist_next (self->timers);
        }
        //  Handle any readers and pollers that are ready
        size_t item_nbr;
        for (item_nbr = 0; item_nbr < self->poll_size && rc >= 0; item_nbr++) {
            s_reader_t *reader = &self->readact [item_nbr];
            if (reader->handler) {
                if (  (self->pollset [item_nbr].revents & ZMQ_POLLERR)
                   && !reader->tolerant) {
                    if (self->verbose)
                        zsys_warning ("zloop: can't read %s socket: %s",
                            zsock_type_str (reader->sock),
                            zmq_strerror (zmq_errno ()));
                    //  Give handler one chance to handle error, then kill
                    //  reader because it'll disrupt the reactor otherwise.
                    if (reader->errors++) {
                        zloop_reader_end (self, reader->sock);
                        self->pollset [item_nbr].revents = 0;
                    }
                }
                else
                    reader->errors = 0;     //  A non-error happened

                if (self->pollset [item_nbr].revents) {
                    if (self->verbose)
                        zsys_debug ("zloop: call %s socket handler",
                            zsock_type_str (reader->sock));
                    rc = reader->handler (self, reader->sock, reader->arg);
                    if (rc == -1 || self->need_rebuild)
                        break;
                }
            }
            else {
                s_poller_t *poller = &self->pollact [item_nbr];
                assert (self->pollset [item_nbr].socket == poller->item.socket);

                if (  (self->pollset [item_nbr].revents & ZMQ_POLLERR)
                   && !poller->tolerant) {
                    if (self->verbose)
                        zsys_warning ("zloop: can't poll %s socket (%p, %d): %s",
                            poller->item.socket ?
                            zsocket_type_str (poller->item.socket) : "FD",
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
                        zsys_debug ("zloop: call %s socket handler (%p, %d)",
                            poller->item.socket ?
                            zsocket_type_str (poller->item.socket) : "FD",
                            poller->item.socket, poller->item.fd);
                    rc = poller->handler (self, &self->pollset [item_nbr], poller->arg);
                    if (rc == -1 || self->need_rebuild)
                        break;
                }
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
s_socket_event (zloop_t *loop, zsock_t *handle, void *arg)
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
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new (ZMQ_PAIR);
    assert (output);
    zsock_bind (output, "inproc://zloop.test");

    zsock_t *input = zsock_new (ZMQ_PAIR);
    assert (input);
    zsock_connect (input, "inproc://zloop.test");

    zloop_t *loop = zloop_new ();
    assert (loop);
    zloop_set_verbose (loop, verbose);

    //  Create a timer that will be cancelled
    int timer_id = zloop_timer (loop, 1000, 1, s_timer_event, NULL);
    zloop_timer (loop, 5, 1, s_cancel_timer_event, &timer_id);

    //  After 20 msecs, send a ping message to output3
    zloop_timer (loop, 20, 1, s_timer_event, output);

    //  When we get the ping message, end the reactor
    rc = zloop_reader (loop, input, s_socket_event, NULL);
    assert (rc == 0);
    zloop_reader_set_tolerant (loop, input);
    zloop_start (loop);

    zloop_destroy (&loop);
    assert (loop == NULL);

    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end
    printf ("OK\n");
}
