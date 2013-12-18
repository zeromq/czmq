/*
    =========================================================================
    zproxy - convenient zmq_proxy api

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
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

#ifndef __ZPROXY_H_INCLUDED__
#define __ZPROXY_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#define ZPROXY_QUEUE 1
#define ZPROXY_FORWARDER 2
#define ZPROXY_STREAMER 3
#define ZPROXY_HAS_PROXY 30201
#define ZPROXY_HAS_STEERABLE 40100

//  Opaque class structure
typedef struct _zproxy_t zproxy_t;

//  @interface
//  Create a new zproxy object
CZMQ_EXPORT zproxy_t *
    zproxy_new (zctx_t *ctx, int zproxy_type);

//  Destroy a zproxy object
CZMQ_EXPORT void
    zproxy_destroy (zproxy_t **self_p);

// Set the identity for the front end socket manually.
// Must be done before call to zproxy_bind!
CZMQ_EXPORT void
    zproxy_set_frontend_identity (zproxy_t *self, char *frontend_identity);

// Get frontend socket identity
CZMQ_EXPORT char *
    zproxy_frontend_identity (zproxy_t *self);

// Set the identity for the back end socket manually.
// Must be done before call to zproxy_bind!
CZMQ_EXPORT void
    zproxy_set_backend_identity (zproxy_t *self, char *backend_identity);

// Get backend socket identity
CZMQ_EXPORT char *
    zproxy_backend_identity (zproxy_t *self);

// Underlying libzmq supports zmq_proxy
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)

//  Get zproxy capture address
CZMQ_EXPORT char *
    zproxy_capture_addr (zproxy_t *self);

//  Get zproxy capture type
CZMQ_EXPORT int
    zproxy_capture_type (zproxy_t *self);

// Underlying libzmq also supports zmq_proxy_steerable
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)

//  Start a zproxy in an attached thread, binding to endpoints.
//  If capture_addr is not null, will create a capture socket.
//  If control_addr is not null, will use zmq_proxy_steerable
//  Returns 0 if OK, -1 if there was an error
CZMQ_EXPORT int
    zproxy_bind (zproxy_t *self, char *frontend_addr,
            char *backend_addr, char *capture_addr,
            char *control_addr);

// Pause a zproxy object
CZMQ_EXPORT void
    zproxy_pause (zproxy_t *self);

// Resume a zproxy object
CZMQ_EXPORT void
    zproxy_resume (zproxy_t *self);

// Terminate a zproxy object
CZMQ_EXPORT void
    zproxy_terminate (zproxy_t *self);

// Underlying libzmq supports zmq_proxy but not zmq_proxy_steerable
#else

//  Start and zmq_proxy in an attached thread, binding to endpoints.
//  If capture_addr is not null, will create a capture socket.
//  Returns 0 if OK, -1 if there was an error
CZMQ_EXPORT int
    zproxy_bind (zproxy_t *self, char *frontend_addr,
            char *backend_addr, char *capture_addr);
#endif

// Underlying libzmq supports zmq_device and does not support
// zmq_proxy nor zmq_proxy_steerable
#else

//  Start a zproxy in an attached thread, binding to endpoints.
//  Returns 0 if OK, -1 if there was an error
CZMQ_EXPORT int
    zproxy_bind (zproxy_t *self, char *frontend_addr,
            char *backend_addr);

#endif 

// Self test of this class
CZMQ_EXPORT int
    zproxy_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
