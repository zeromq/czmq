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

#ifndef __ZLOOP_H_INCLUDED__
#define __ZLOOP_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif


//  Opaque class structure
typedef struct _zloop_t zloop_t;

//  @interface
//  Callback function for reactor events
typedef int (zloop_fn) (zloop_t *loop, zmq_pollitem_t *item, void *arg);

// Callback for reactor timer events
typedef int (zloop_timer_fn) (zloop_t *loop, int timer_id, void *arg);

//  Create a new zloop reactor
CZMQ_EXPORT zloop_t *
    zloop_new (void);

//  Destroy a reactor
CZMQ_EXPORT void
    zloop_destroy (zloop_t **self_p);

//  Register pollitem with the reactor. When the pollitem is ready, will call
//  the handler, passing the arg. Returns 0 if OK, -1 if there was an error.
//  If you register the pollitem more than once, each instance will invoke its
//  corresponding handler.
CZMQ_EXPORT int
    zloop_poller (zloop_t *self, zmq_pollitem_t *item, zloop_fn handler, void *arg);

//  Cancel a pollitem from the reactor, specified by socket or FD. If both
//  are specified, uses only socket. If multiple poll items exist for same
//  socket/FD, cancels ALL of them.
CZMQ_EXPORT void
    zloop_poller_end (zloop_t *self, zmq_pollitem_t *item);

//  Configure a registered pollitem to ignore errors. If you do not set this, 
//  then pollitems that have errors are removed from the reactor silently.
CZMQ_EXPORT void
    zloop_set_tolerant (zloop_t *self, zmq_pollitem_t *item);

//  Register a timer that expires after some delay and repeats some number of
//  times. At each expiry, will call the handler, passing the arg. To run a
//  timer forever, use 0 times. Returns a timer_id that is used to cancel the
//  timer in the future. Returns -1 if there was an error.
CZMQ_EXPORT int
    zloop_timer (zloop_t *self, size_t delay, size_t times, zloop_timer_fn handler, void *arg);

//  Cancel a specific timer identified by a specific timer_id (as returned by
//  zloop_timer).
CZMQ_EXPORT int
    zloop_timer_end (zloop_t *self, int timer_id);

//  Set verbose tracing of reactor on/off
CZMQ_EXPORT void
    zloop_set_verbose (zloop_t *self, bool verbose);

//  Start the reactor. Takes control of the thread and returns when the 0MQ
//  context is terminated or the process is interrupted, or any event handler
//  returns -1. Event handlers may register new sockets and timers, and
//  cancel sockets. Returns 0 if interrupted, -1 if cancelled by a handler.
CZMQ_EXPORT int
    zloop_start (zloop_t *self);

//  Self test of this class
CZMQ_EXPORT void
    zloop_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
