/*  =========================================================================
    zloop - event-driven reactor

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of czmq, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
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

#include "../include/czmq_prelude.h"
#include "../include/zclock.h"
#include "../include/zctx.h"
#include "../include/zlist.h"
#include "../include/zstr.h"
#include "../include/zsocket.h"
#include "../include/zloop.h"

typedef struct _s_poller_t s_poller_t;
typedef struct _s_timer_t s_timer_t;

//  Structure of our class

struct _zloop_t {
    zlist_t *pollers;           //  List of poll items
    zlist_t *timers;            //  List of timers
    int poll_size;              //  Size of poll set
    zmq_pollitem_t *pollset;    //  zmq_poll set
    s_poller_t *pollact;        //  Pollers for this poll set
    Bool dirty;                 //  True if pollset needs rebuilding
    Bool verbose;               //  True if verbose tracing wanted
};

//  Pollers and timers are held as small structures of their own
struct _s_poller_t {
    zmq_pollitem_t item;
    zloop_fn *handler;
    void *arg;
};

struct _s_timer_t {
    size_t delay;
    size_t times;
    zloop_fn *handler;
    void *arg;
    int64_t when;               //  Clock time when alarm goes off
};

static s_poller_t *
s_poller_new (zmq_pollitem_t *item, zloop_fn handler, void *arg)
{
    s_poller_t *poller = (s_poller_t *) zmalloc (sizeof (s_poller_t));
    memcpy (&poller->item, item, sizeof (zmq_pollitem_t));
    poller->handler = handler;
    poller->arg = arg;
    return poller;
}

static s_timer_t *
s_timer_new (size_t delay, size_t times, zloop_fn handler, void *arg)
{
    s_timer_t *timer = (s_timer_t *) zmalloc (sizeof (s_timer_t));
    timer->delay = delay;
    timer->times = times;
    timer->handler = handler;
    timer->arg = arg;
    return timer;
}


//  --------------------------------------------------------------------------
//  Constructor

zloop_t *
zloop_new (void)
{
    zloop_t
        *self;

    self = (zloop_t *) zmalloc (sizeof (zloop_t));
    self->pollers = zlist_new ();
    self->timers = zlist_new ();
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

int
zloop_poller (zloop_t *self, zmq_pollitem_t *item, zloop_fn handler, void *arg)
{
    assert (self);
    zlist_push (self->pollers, s_poller_new (item, handler, arg));
    self->dirty = TRUE;         //  Rebuild will be needed
    if (self->verbose)
        zclock_log ("I: zloop: register %s poller (%p, %d)",
            item->socket? zsocket_type_str (item->socket): "FD",
            item->socket, item->fd);

    return 0;
}


//  --------------------------------------------------------------------------
//  Cancel a pollitem from the reactor, specified by socket or FD. If both
//  are specified, uses only socket. If multiple poll items exist for same
//  socket/FD, only first is cancelled.

void
zloop_cancel (zloop_t *self, void *socket, int fd)
{
    assert (self);
    s_poller_t *poller = (s_poller_t *) zlist_first (self->pollers);
    while (poller) {
        if ((socket && poller->item.socket == socket)
        ||  (fd && poller->item.fd == fd)) {
            zlist_remove (self->pollers, poller);
            self->dirty = TRUE;     //  Rebuild will be needed
            break;
        }
        poller = (s_poller_t *) zlist_next (self->pollers);
    }
    if (self->verbose)
        zclock_log ("I: zloop: cancel %s socket poller (%p, %d)",
            socket? zsocket_type_str (socket): "FD",
            socket, fd);
}


//  --------------------------------------------------------------------------
//  Register a timer that expires after some delay and repeats some number of
//  times. At each expiry, will call the handler, passing the arg. To
//  run a timer forever, use 0 times. Returns 0 if OK, -1 if there was an
//  error.

int
zloop_timer (zloop_t *self, size_t delay, size_t times, zloop_fn handler, void *arg)
{
    assert (self);
    zlist_push (self->timers, s_timer_new (delay, times, handler, arg));
    if (self->verbose)
        zclock_log ("I: zloop: register timer delay=%d times=%d", delay, times);
    return 0;
}


//  --------------------------------------------------------------------------
//  Set verbose tracing of reactor on/off
void
zloop_set_verbose (zloop_t *self, Bool verbose)
{
    self->verbose = verbose;
}


//  --------------------------------------------------------------------------
//  Start the reactor. Takes control of the thread and returns when the 0MQ
//  context is terminated or the process is interrupted, or any event handler
//  returns -1. Event handlers may register new sockets and timers, and
//  cancel sockets. Returns 0 if interrupted, -1 if cancelled by a handler.

int
zloop_start (zloop_t *self)
{
    assert (self);
    int rc = 0;

    //  Recalculate all timers now
    s_timer_t *timer = (s_timer_t *) zlist_first (self->timers);
    while (timer) {
        timer->when = zclock_time () + timer->delay;
        timer = (s_timer_t *) zlist_next (self->timers);
    }
    //  Main reactor loop
    while (!zctx_interrupted) {
        //  Rebuild pollitem set if necessary
        //  We hold an array of pollers that matches the pollset, so
        //  we can register/cancel pollers orthogonally to executing
        //  the pollset activity on pollers.
        uint item_nbr;
        if (self->dirty) {
            free (self->pollset);
            free (self->pollact);
            self->poll_size = zlist_size (self->pollers);
            self->pollset = (zmq_pollitem_t *) zmalloc (
                self->poll_size * sizeof (zmq_pollitem_t));
            self->pollact = (s_poller_t *) zmalloc (
                self->poll_size * sizeof (s_poller_t));

            s_poller_t *poller = (s_poller_t *) zlist_first (self->pollers);
            item_nbr = 0;
            while (poller) {
                self->pollset [item_nbr] = poller->item;
                self->pollact [item_nbr] = *poller;
                item_nbr++;
                poller = (s_poller_t *) zlist_next (self->pollers);
            }
        }
        //  Calculate tickless timer, up to 1 hour
        int64_t tickless = zclock_time () + 1000 * 3600;
        s_timer_t *timer = (s_timer_t *) zlist_first (self->timers);
        while (timer) {
            //  Find earliest timer
            if (tickless > timer->when)
                tickless = timer->when;
            timer = (s_timer_t *) zlist_next (self->timers);
        }
        long timeout = (long) (tickless - zclock_time ());
        if (timeout < 0)
            timeout = 0;
        if (self->verbose)
            zclock_log ("I: zloop: polling for %d msec", timeout);

        rc = zmq_poll (self->pollset, self->poll_size, timeout * ZMQ_POLL_MSEC);
        if (rc == -1 || zctx_interrupted) {
            if (self->verbose)
                zclock_log ("I: zloop: interrupted");
            rc = 0;
            break;              //  Context has been shut down
        }
        //  Handle any timers that have now expired
        timer = (s_timer_t *) zlist_first (self->timers);
        while (timer) {
            if (zclock_time () >= timer->when) {
                if (self->verbose)
                    zclock_log ("I: zloop: call timer handler");
                rc = timer->handler (self, NULL, timer->arg);
                if (rc == -1)
                    break;      //  Timer handler signalled break
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
        for (item_nbr = 0; item_nbr < self->poll_size; item_nbr++) {
            if (self->pollset [item_nbr].revents & ZMQ_POLLIN) {
                s_poller_t *poller = &self->pollact [item_nbr];
                assert (self->pollset [item_nbr].socket == poller->item.socket);
                if (self->verbose)
                    zclock_log ("I: zloop: call %s socket handler (%p, %d)",
                        poller->item.socket?
                            zsocket_type_str (poller->item.socket): "FD",
                        poller->item.socket, poller->item.fd);
                rc = poller->handler (self, &poller->item, poller->arg);
                if (rc == -1)
                    break;      //  Poller handler signalled break
            }
        }
        if (rc == -1)
            break;
    }
    return rc;
}


//  --------------------------------------------------------------------------
//  Selftest

int s_timer_event (zloop_t *loop, zmq_pollitem_t *item, void *output)
{
    zstr_send (output, "PING");
    return 0;
}

int s_socket_event (zloop_t *loop, zmq_pollitem_t *item, void *arg)
{
    //  Just end the reactor
    return -1;
}

int
zloop_test (Bool verbose)
{
    printf (" * zloop: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();

    void *output = zsocket_new (ctx, ZMQ_PAIR);
    zsocket_bind (output, "inproc://zloop.test");
    void *input = zsocket_new (ctx, ZMQ_PAIR);
    zsocket_connect (input, "inproc://zloop.test");

    zloop_t *loop = zloop_new ();
    assert (loop);
    zloop_set_verbose (loop, verbose);

    //  After 10 msecs, send a ping message to output
    zloop_timer (loop, 10, 1,  s_timer_event, output);
    //  When we get the ping message, end the reactor
    zmq_pollitem_t poll_input = { input, 0, ZMQ_POLLIN, 0 };
    zloop_poller (loop, &poll_input, s_socket_event, NULL);
    zloop_start (loop);

    zloop_destroy (&loop);
    assert (loop == NULL);

    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
    return 0;
}
