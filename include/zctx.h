/*  =========================================================================
    zctx - working with 0MQ contexts

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

#ifndef __ZCTX_H_INCLUDED__
#define __ZCTX_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif


//  Opaque class structure
typedef struct _zctx_t zctx_t;

//  @interface
//  Create new context, returns context object, replaces zmq_init
CZMQ_EXPORT zctx_t *
    zctx_new (void);

//  Destroy context and all sockets in it, replaces zmq_term
CZMQ_EXPORT void
    zctx_destroy (zctx_t **self_p);

//  @end
//  Create new shadow context, returns context object
//  For internal use only.
CZMQ_EXPORT zctx_t *
    zctx_shadow (zctx_t *self);

//  @interface
//  Raise default I/O threads from 1, for crazy heavy applications
//  The rule of thumb is one I/O thread per gigabyte of traffic in
//  or out. Call this method before creating any sockets on the context
//  or the setting will have no effect.
CZMQ_EXPORT void
    zctx_set_iothreads (zctx_t *self, int iothreads);

//  Set msecs to flush sockets when closing them, see the ZMQ_LINGER
//  man page section for more details. By default, set to zero, so
//  any in-transit messages are discarded when you destroy a socket or
//  a context.
CZMQ_EXPORT void
    zctx_set_linger (zctx_t *self, int linger);

//  Set initial high-water mark for inter-thread pipe sockets. Note that
//  this setting is separate from the default for normal sockets. You 
//  should change the default for pipe sockets *with care*. Too low values
//  will cause blocked threads, and an infinite setting can cause memory
//  exhaustion. The default, no matter the underlying ZeroMQ version, is
//  1,000.
CZMQ_EXPORT void
    zctx_set_pipehwm (zctx_t *self, int pipehwm);
    
//  Set initial send HWM for all new normal sockets created in context.
//  You can set this per-socket after the socket is created.
//  The default, no matter the underlying ZeroMQ version, is 1,000.
CZMQ_EXPORT void
    zctx_set_sndhwm (zctx_t *self, int sndhwm);
    
//  Set initial receive HWM for all new normal sockets created in context.
//  You can set this per-socket after the socket is created.
//  The default, no matter the underlying ZeroMQ version, is 1,000.
CZMQ_EXPORT void
    zctx_set_rcvhwm (zctx_t *self, int rcvhwm);

//  Return low-level 0MQ context object, will be NULL before first socket
//  is created. Use with care.
CZMQ_EXPORT void *
    zctx_underlying (zctx_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zctx_test (bool verbose);

//  Global signal indicator, TRUE when user presses Ctrl-C or the process
//  gets a SIGTERM signal.
CZMQ_EXPORT extern volatile int zctx_interrupted;
//  @end

//  Create socket within this context, for CZMQ use only
void *
    zctx__socket_new (zctx_t *self, int type);

//  Create pipe socket within this context, for CZMQ use only
void *
    zctx__socket_pipe (zctx_t *self);

//  Destroy socket within this context, for CZMQ use only
void
    zctx__socket_destroy (zctx_t *self, void *socket);
//  @end
    
#ifdef __cplusplus
}
#endif

#endif
