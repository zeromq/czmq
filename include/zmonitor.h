/*  =========================================================================
    zmonitor - socket event monitor

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZMONITOR_H_INCLUDED__
#define __ZMONITOR_H_INCLUDED__

//  This code needs backporting to work with ZMQ v3.2
#if (ZMQ_VERSION_MAJOR == 4)

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zmonitor_t zmonitor_t;

//  @interface
//  Create a new socket monitor
CZMQ_EXPORT zmonitor_t *
    zmonitor_new (zctx_t *ctx, void *socket, int events);

//  Destroy a socket monitor
CZMQ_EXPORT void
    zmonitor_destroy (zmonitor_t **self_p);

//  Receive a status message from the monitor; if no message arrives within
//  500 msec, or the call was interrupted, returns NULL.
CZMQ_EXPORT zmsg_t *
    zmonitor_recv (zmonitor_t *self);

//  Get the ZeroMQ socket, for polling 
CZMQ_EXPORT void *
    zmonitor_socket (zmonitor_t *self);

//  Enable verbose tracing of commands and activity
CZMQ_EXPORT void
    zmonitor_set_verbose (zmonitor_t *self, bool verbose);

// Self test of this class
CZMQ_EXPORT void
    zmonitor_test (bool verbose);

// @end

#ifdef __cplusplus
}
#endif

#endif          //  ZeroMQ 4.0 or later

#endif
