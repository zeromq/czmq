/*  =========================================================================
    zsock_monitor - socket event monitor

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZSOCK_MONITOR_H_INCLUDED__
#define __ZSOCK_MONITOR_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new socket monitor
CZMQ_EXPORT zsock_monitor_t *
    zsock_monitor_new (zsock_t *sock);

//  Destroy a socket monitor
CZMQ_EXPORT void
    zsock_monitor_destroy (zsock_monitor_t **self_p);

//  Receive a status message from the monitor; if no message arrives within
//  500 msec, or the call was interrupted, returns NULL.
CZMQ_EXPORT zmsg_t *
    zsock_monitor_recv (zsock_monitor_t *self);

//  Enable verbose tracing of commands and activity
CZMQ_EXPORT void
    zsock_monitor_set_verbose (zsock_monitor_t *self, bool verbose);

//  Listen to a specified event
CZMQ_EXPORT void
    zsock_monitor_listen (zsock_monitor_t *self, int event);

//  Start the monitor; once started, new calls to zsock_monitor_event() will
//  have no effect.
CZMQ_EXPORT void
    zsock_monitor_start (zsock_monitor_t *self);

//  Return the status pipe, to allow polling for monitor data.
CZMQ_EXPORT zsock_t *
    zsock_monitor_handle (zsock_monitor_t *self);

// Self test of this class
CZMQ_EXPORT void
    zsock_monitor_test (bool verbose);

// @end

#ifdef __cplusplus
}
#endif

#endif
