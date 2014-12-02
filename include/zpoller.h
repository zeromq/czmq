/*  =========================================================================
    zpoller - trivial socket poller class

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __zpoller_H_INCLUDED__
#define __zpoller_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create new poller; the reader can be a libzmq socket (void *), a zsock_t
//  instance, or a zactor_t instance.
CZMQ_EXPORT zpoller_t *
    zpoller_new (void *reader, ...);

//  Destroy a poller
CZMQ_EXPORT void
    zpoller_destroy (zpoller_t **self_p);

//  Add a reader to be polled. Returns 0 if OK, -1 on failure. The reader may
//  be a libzmq void * socket, a zsock_t instance, or a zactor_t instance.
CZMQ_EXPORT int
    zpoller_add (zpoller_t *self, void *reader);

//  Remove a reader from the poller; returns 0 if OK, -1 on failure. The
//  reader may be a libzmq void * socket, a zsock_t instance, or a zactor_t
//  instance.
CZMQ_EXPORT int
    zpoller_remove (zpoller_t *self, void *reader);

//  Poll the registered readers for I/O, return first reader that has input.
//  The reader will be a libzmq void * socket, or a zsock_t or zactor_t
//  instance as specified in zpoller_new/zpoller_add. The order that
//  sockets are defined in the poll list affects their priority. If you
//  need a balanced poll, use the low level zmq_poll method directly. If
//  the poll call was interrupted (SIGINT), or the ZMQ context was
//  destroyed, or the timeout expired, returns NULL. You can test the
//  actual exit condition by calling zpoller_expired () and
//  zpoller_terminated (). The timeout is in msec.
CZMQ_EXPORT void *
    zpoller_wait (zpoller_t *self, int timeout);

//  Return true if the last zpoller_wait () call ended because the timeout
//  expired, without any error.
CZMQ_EXPORT bool
    zpoller_expired (zpoller_t *self);

//  Return true if the last zpoller_wait () call ended because the process
//  was interrupted, or the parent context was destroyed.
CZMQ_EXPORT bool
    zpoller_terminated (zpoller_t *self);

//  Self test of this class
CZMQ_EXPORT void
    zpoller_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif

