/*  =========================================================================
    ztimerset - timer set

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
    ztimerset - timer set
@discuss
@end
*/

#include "czmq_classes.h"

//  Structure of our class

struct _ztimerset_t {
    void* zmq_timers;
};


//  --------------------------------------------------------------------------
//  Create a new ztimerset

ztimerset_t *
ztimerset_new (void)
{
#ifdef ZMQ_HAVE_TIMERS
    ztimerset_t *self = (ztimerset_t *) zmalloc (sizeof (ztimerset_t));
    assert (self);

    self->zmq_timers = zmq_timers_new ();
    assert (self->zmq_timers);

    return self;
#else
    return NULL;
#endif
}


//  --------------------------------------------------------------------------
//  Destroy the ztimerset

void
ztimerset_destroy (ztimerset_t **self_p)
{
#ifdef ZMQ_HAVE_TIMERS
    assert (self_p);
    if (*self_p) {
        ztimerset_t *self = *self_p;
        zmq_timers_destroy (&self->zmq_timers);
        free (self);
        *self_p = NULL;
    }
#endif
}


//  --------------------------------------------------------------------------
//  Add a timer to the set. Returns timer id if OK, -1 on failure.
int
ztimerset_add (ztimerset_t *self, size_t interval, ztimerset_fn handler, void *arg)
{
#ifdef ZMQ_HAVE_TIMERS
    return zmq_timers_add (self->zmq_timers, interval, handler, arg);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Cancel a timer. Returns 0 if OK, -1 on failure.
int
ztimerset_cancel (ztimerset_t *self, int timer_id)
{
#ifdef ZMQ_HAVE_TIMERS
    return zmq_timers_cancel (self->zmq_timers, timer_id);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Set timer interval. Returns 0 if OK, -1 on failure.
//  This method is slow, canceling the timer and adding a new one yield better performance.
int
ztimerset_set_interval (ztimerset_t *self, int timer_id, size_t interval)
{
#ifdef ZMQ_HAVE_TIMERS
    return zmq_timers_set_interval (self->zmq_timers, timer_id, interval);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Reset timer to start interval counting from current time. Returns 0 if OK, -1 on failure.
//  This method is slow, canceling the timer and adding a new one yield better performance.
int
ztimerset_reset (ztimerset_t *self, int timer_id)
{
#ifdef ZMQ_HAVE_TIMERS
    return zmq_timers_reset (self->zmq_timers, timer_id);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Return the time until the next interval.
//  Should be used as timeout parameter for the zpoller wait method.
//  The timeout is in msec.
int
ztimerset_timeout (ztimerset_t *self)
{
#ifdef ZMQ_HAVE_TIMERS
    return zmq_timers_timeout (self->zmq_timers);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Invoke callback function of all timers which their interval has elapsed.
//  Should be call after zpoller wait method.
//  Returns 0 if OK, -1 on failure.
int
ztimerset_execute (ztimerset_t *self)
{
#ifdef ZMQ_HAVE_TIMERS
    return zmq_timers_execute (self->zmq_timers);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Self test of this class

void handler(int timer_id, void* arg)
{
    *((bool*)arg) = true;
}

void
ztimerset_test (bool verbose)
{
#ifdef ZMQ_HAVE_TIMERS
    printf (" * ztimerset: ");

    //  @selftest
    //  Simple create/destroy test
    ztimerset_t *self = ztimerset_new ();
    assert (self);

    //  Adding timer
    bool timer_invoked = false;
    int timer_id = ztimerset_add (self, 100, handler, &timer_invoked);
    assert (timer_id != -1);
    int rc = ztimerset_execute (self);
    assert (rc == 0);
    assert (!timer_invoked);
    int timeout = ztimerset_timeout (self);
    assert (timeout > 0);
    zclock_sleep (timeout);
    rc = ztimerset_execute (self);
    assert (rc == 0);
    assert (timer_invoked);

    //  Cancel timer
    timeout = ztimerset_timeout (self);
    assert (timeout > 0);
    rc = ztimerset_cancel (self, timer_id);
    assert (rc == 0);
    timeout = ztimerset_timeout (self);
    assert(timeout == -1);

    //  Reset a timer
    timer_id = ztimerset_add (self, 100, handler, &timer_invoked);
    assert (timer_id != -1);
    timeout = ztimerset_timeout (self);
    assert (timeout > 0);
    zclock_sleep (timeout / 2);
    timeout = ztimerset_timeout (self);
    rc = ztimerset_reset(self, timer_id);
    assert (rc == 0);
    int timeout2 = ztimerset_timeout (self);
    assert (timeout2 > timeout);
    rc = ztimerset_cancel (self, timer_id);
    assert (rc == 0);

    //  Set interval
    timer_id = ztimerset_add (self, 100, handler, &timer_invoked);
    assert (timer_id != -1);
    timeout = ztimerset_timeout (self);
    rc = ztimerset_set_interval(self, timer_id, 200);
    timeout2 = ztimerset_timeout (self);
    assert (timeout2 > timeout);

    ztimerset_destroy (&self);
    //  @end
    printf ("OK\n");
#endif
}
