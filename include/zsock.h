/*  =========================================================================
    zsock - working with 0MQ sockets

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZSOCK_H_INCLUDED__
#define __ZSOCK_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  This port range is defined by IANA for dynamic or private ports
//  We use this when choosing a port for dynamic binding.
#define ZSOCK_DYNFROM     0xc000
#define ZSOCK_DYNTO       0xffff

//  Callback function for zero-copy methods
typedef void (zsock_free_fn) (void *data, void *arg);

//  Opaque class structure
typedef struct _zsock_t zsock_t;

//  Create a new socket within our CZMQ context, replaces zmq_socket.
//  Use this to get automatic management of the socket at shutdown.
//  Note: SUB sockets do not automatically subscribe to everything; you
//  must set filters explicitly.
CZMQ_EXPORT zsock_t *
    zsock_new (zctx_t *self, int type);

//  Destroy a socket.
CZMQ_EXPORT void
    zsock_destroy (zsock_t **p_self);

//  Bind a socket to a formatted endpoint. If the port is specified as
//  '*', binds to any free port from ZSOCK_DYNFROM to ZSOCK_DYNTO
//  and returns the actual port number used. Otherwise asserts that the
//  bind succeeded with the specified port number. Always returns the
//  port number if successful.
CZMQ_EXPORT int
    zsock_bind (zsock_t *self, const char *format, ...);

//  Unbind a socket from a formatted endpoint.
//  Returns 0 if OK, -1 if the endpoint was invalid or the function
//  isn't supported.
CZMQ_EXPORT int
    zsock_unbind (zsock_t *self, const char *format, ...);

//  Connect a socket to a formatted endpoint
//  Returns 0 if OK, -1 if the endpoint was invalid.
CZMQ_EXPORT int
    zsock_connect (zsock_t *self, const char *format, ...);

//  Disconnect a socket from a formatted endpoint
//  Returns 0 if OK, -1 if the endpoint was invalid or the function
//  isn't supported.
CZMQ_EXPORT int
    zsock_disconnect (zsock_t *self, const char *format, ...);

//  Poll for input events on the socket. Returns TRUE if there is input
//  ready on the socket, else FALSE.
CZMQ_EXPORT bool
    zsock_poll (zsock_t *self, int msecs);

//  Returns socket type as printable constant string
CZMQ_EXPORT const char *
    zsock_type_str (zsock_t *self);

//  Send data over a socket as a single message frame.
//  Accepts these flags: ZFRAME_MORE and ZFRAME_DONTWAIT.
CZMQ_EXPORT int
    zsock_sendmem (zsock_t *self, const void *data, size_t size, int flags);

//  Send a signal over a socket. A signal is a zero-byte message.
//  Signals are used primarily between threads, over pipe sockets.
//  Returns -1 if there was an error sending the signal.
CZMQ_EXPORT int
    zsock_signal (zsock_t *self);

//  Wait on a signal. Use this to coordinate between threads, over
//  pipe pairs. Returns -1 on error, 0 on success.
CZMQ_EXPORT int
    zsock_wait (zsock_t *self);

//  Send data over a socket as a single message frame.
//  Returns -1 on error, 0 on success
CZMQ_EXPORT int
    zsock_sendmem (zsock_t *self, const void *data, size_t size, int flags);

//  Self test of this class
CZMQ_EXPORT int
    zsock_test (bool verbose);

//  -------------------------------------------------------------------------
//  Extensions

//  Opaque class structures
typedef struct _zframe_t zframe_t;
typedef struct _zmsg_t zmsg_t;

//  Return a pointer to the 0MQ socket.
CZMQ_EXPORT void *
    zsock_socket (zsock_t *self);

//  Send frame to socket, destroy after sending unless ZFRAME_REUSE is
//  set or the attempt to send the message errors out.
//  Return -1 on error, 0 on success.
CZMQ_EXPORT int
    zsock_send_frame(zsock_t *self, zframe_t **frame_p, int flags);

//  Receive frame from socket, returns zframe_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  zsock_recv_frame_nowait().
CZMQ_EXPORT zframe_t *
    zsock_recv_frame (zsock_t *self);

//  Receive a new frame off the socket. Returns newly allocated frame, or
//  NULL if there was no input waiting, or if the read was interrupted.
CZMQ_EXPORT zframe_t *
    zsock_recv_frame_nowait (zsock_t *self);

//  Send message to socket, destroy after sending. If the message has no
//  frames, sends nothing but destroys the message anyhow. Safe to call
//  if zmsg is null.
CZMQ_EXPORT int
    zsock_send_msg (zsock_t *self, zmsg_t **msg_p);

//  Receive message from socket, returns zmsg_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  the zloop class or zsock_msg_recv_nowait() or zmq_poll to check for socket 
//  input before receiving.
CZMQ_EXPORT zmsg_t *
    zsock_recv_msg (zsock_t *self);

//  Receive message from socket, returns zmsg_t object, or NULL either if there was
//  no input waiting, or the recv was interrupted.
CZMQ_EXPORT zmsg_t *
    zsock_recv_msg_nowait (zsock_t *self);

//  @end

//  Compiler hints
CZMQ_EXPORT int zsock_bind (zsock_t *self, const char *format, ...) CHECK_PRINTF (2);
CZMQ_EXPORT int zsock_unbind (zsock_t *self, const char *format, ...) CHECK_PRINTF (2);
CZMQ_EXPORT int zsock_connect (zsock_t *self, const char *format, ...) CHECK_PRINTF (2);
CZMQ_EXPORT int zsock_disconnect (zsock_t *self, const char *format, ...) CHECK_PRINTF (2);

#ifdef __cplusplus
}
#endif

// include generated code
#include "zsock_option.h"

#endif
