/*  =========================================================================
    zsock - high-level socket API that hides libzmq contexts and sockets

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

//  Create a new socket.
CZMQ_EXPORT zsock_t *
    zsock_new (int type);

//  Destroy the socket. You must use this for any socket created via the
//  zsock_new method.
CZMQ_EXPORT void
    zsock_destroy (zsock_t **p_self);

//  Bind a socket to a formatted endpoint. If the port is specified as '*'
//  and the endpoint starts with "tcp://", binds to an ephemeral TCP port in
//  a high range. Always returns the port number on successful TCP binds, else
//  returns zero on success. Returns -1 on failure. When using ephemeral ports,
//  note that ports may be reused by different threads, without clients being
//  aware.
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

//  Returns socket type as printable constant string
CZMQ_EXPORT const char *
    zsock_type_str (zsock_t *self);

//  Send a signal over a socket. A signal is a zero-byte message.
//  Signals are used primarily between threads, over pipe sockets.
//  Returns -1 if there was an error sending the signal.
CZMQ_EXPORT int
    zsock_signal (zsock_t *self);

//  Wait on a signal. Use this to coordinate between threads, over
//  pipe pairs. Returns -1 on error, 0 on success.
CZMQ_EXPORT int
    zsock_wait (zsock_t *self);

//  Probe the supplied object, and report if it looks like a zsock_t.
CZMQ_EXPORT bool
    zsock_is (void *self);

//  Probe the supplied reference. If it looks like a zsock_t instance,
//  return the underlying libzmq socket handle; else if it looks like
//  a libzmq socket handle, return the supplied value.
CZMQ_EXPORT void *
    zsock_resolve (void *self);

//  Self test of this class
CZMQ_EXPORT int
    zsock_test (bool verbose);

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
