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

//  Opaque class structure
typedef struct _zproxy_t zproxy_t;

//  @interface
//  Create a new zproxy object
CZMQ_EXPORT zproxy_t *
    zproxy_new (zctx_t *ctx, int zproxy_type);

//  Destroy a zproxy object
CZMQ_EXPORT void
    zproxy_destroy (zproxy_t **self_p);

//  Start and zmq_proxy in an attached thread, binding to endpoints
//  Returns 0 if OK, -1 if there was an error
CZMQ_EXPORT int
    zproxy_bind (zproxy_t *self, const char *frontend_addr,
            const char *backend_addr, const char *capture_addr);

//  Get zproxy type
CZMQ_EXPORT int
    zproxy_type (zproxy_t *self);

//  Get zproxy frontend address
CZMQ_EXPORT char *
    zproxy_frontend_addr (zproxy_t *self);

//  Get zproxy frontend type
CZMQ_EXPORT int
    zproxy_frontend_type (zproxy_t *self);

//  Get zproxy backend address
CZMQ_EXPORT char *
    zproxy_backend_addr (zproxy_t *self);

//  Get zproxy backend type
CZMQ_EXPORT int
    zproxy_backend_type (zproxy_t *self);

//  Get zproxy capture address
CZMQ_EXPORT char *
    zproxy_capture_addr (zproxy_t *self);

//  Get zproxy capture type
CZMQ_EXPORT int
    zproxy_capture_type (zproxy_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zproxy_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
