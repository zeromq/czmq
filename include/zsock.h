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

#if defined (ZSOCK_NOCHECK)
#   define zsock_new(t) zsock_new_((t), NULL, 0)
#   define zsock_new_pub(e) zsock_new_pub_((e), NULL, 0)
#   define zsock_new_sub(e,s) zsock_new_sub_((e), (s), NULL, 0)
#   define zsock_new_req(e) zsock_new_req_((e), NULL, 0)
#   define zsock_new_rep(e) zsock_new_rep_((e), NULL, 0)
#   define zsock_new_dealer(e) zsock_new_dealer_((e), NULL, 0)
#   define zsock_new_router(e) zsock_new_router_((e), NULL, 0)
#   define zsock_new_pull(e) zsock_new_pull_((e), NULL, 0)
#   define zsock_new_push(e) zsock_new_push_((e), NULL, 0)
#   define zsock_new_xpub(e) zsock_new_xpub_((e), NULL, 0)
#   define zsock_new_xsub(e) zsock_new_xsub_((e), NULL, 0)
#   define zsock_new_pair(e) zsock_new_pair_((e), NULL, 0)
#   define zsock_new_stream(e) zsock_new_stream_((e), NULL, 0)
#   define zsock_destroy(t) zsock_destroy_((t), NULL, 0)
#else
#   define zsock_new(t) zsock_new_((t), __FILE__, __LINE__)
#   define zsock_new_pub(e) zsock_new_pub_((e), __FILE__, __LINE__)
#   define zsock_new_sub(e,s) zsock_new_sub_((e), (s), __FILE__, __LINE__)
#   define zsock_new_req(e) zsock_new_req_((e), __FILE__, __LINE__)
#   define zsock_new_rep(e) zsock_new_rep_((e), __FILE__, __LINE__)
#   define zsock_new_dealer(e) zsock_new_dealer_((e), __FILE__, __LINE__)
#   define zsock_new_router(e) zsock_new_router_((e), __FILE__, __LINE__)
#   define zsock_new_pull(e) zsock_new_pull_((e), __FILE__, __LINE__)
#   define zsock_new_push(e) zsock_new_push_((e), __FILE__, __LINE__)
#   define zsock_new_xpub(e) zsock_new_xpub_((e), __FILE__, __LINE__)
#   define zsock_new_xsub(e) zsock_new_xsub_((e), __FILE__, __LINE__)
#   define zsock_new_pair(e) zsock_new_pair_((e), __FILE__, __LINE__)
#   define zsock_new_stream(e) zsock_new_stream_((e), __FILE__, __LINE__)
#   define zsock_destroy(t) zsock_destroy_((t), __FILE__, __LINE__)
#endif

//  Create a new socket. This macro passes the caller source and line
//  number so that CZMQ can report socket leaks intelligently. To switch
//  off this checking, which may be costly if you use a LOT of sockets,
//  define ZSOCK_NOCHECK before compiling your code. Returns the new
//  socket, or NULL if the new socket could not be created.
CZMQ_EXPORT zsock_t *
    zsock_new_ (int type, const char *filename, size_t line_nbr);

//  Destroy the socket. You must use this for any socket created via the
//  zsock_new method.
CZMQ_EXPORT void
    zsock_destroy_ (zsock_t **self_p, const char *filename, size_t line_nbr);

//  Smart constructors, which create sockets with additional set-up. In all of
//  these, the endpoint is NULL, or starts with '@' (connect) or '>' (bind).
//  Multiple endpoints are allowed, separated by commas. If endpoint does not
//  start with '@' or '>', default action depends on socket type.

//  Create a PUB socket. Default action is bind.
CZMQ_EXPORT zsock_t *
    zsock_new_pub_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create a SUB socket, and optionally subscribe to some prefix string. Default
//  action is connect.
CZMQ_EXPORT zsock_t *
    zsock_new_sub_ (const char *endpoint, const char *subscribe, const char *filename, size_t line_nbr);

//  Create a REQ socket. Default action is connect.
CZMQ_EXPORT zsock_t *
    zsock_new_req_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create a REP socket. Default action is bind.
CZMQ_EXPORT zsock_t *
    zsock_new_rep_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create a DEALER socket. Default action is connect.
CZMQ_EXPORT zsock_t *
    zsock_new_dealer_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create a ROUTER socket. Default action is bind.
CZMQ_EXPORT zsock_t *
    zsock_new_router_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create a PUSH socket. Default action is connect.
CZMQ_EXPORT zsock_t *
    zsock_new_push_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create a PULL socket. Default action is bind.
CZMQ_EXPORT zsock_t *
    zsock_new_pull_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create an XPUB socket. Default action is bind.
CZMQ_EXPORT zsock_t *
    zsock_new_xpub_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create an XSUB socket. Default action is connect.
CZMQ_EXPORT zsock_t *
    zsock_new_xsub_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create a PAIR socket. Default action is connect.
CZMQ_EXPORT zsock_t *
    zsock_new_pair_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Create a STREAM socket. Default action is connect.
CZMQ_EXPORT zsock_t *
    zsock_new_stream_ (const char *endpoint, const char *filename, size_t line_nbr);

//  Bind a socket to a formatted endpoint. For tcp:// endpoints, supports
//  ephemeral ports, if you specify the port number as "*". By default
//  zsock uses the IANA designated range from C000 (49152) to FFFF (65535).
//  To override this range, follow the "*" with "[first-last]". Either or
//  both first and last may be empty. To bind to a random port within the
//  range, use "!" in place of "*".
//
//  Examples:
//      tcp://127.0.0.1:*           bind to first free port from C000 up
//      tcp://127.0.0.1:!           bind to random port from C000 to FFFF
//      tcp://127.0.0.1:*[60000-]   bind to first free port from 60000 up
//      tcp://127.0.0.1:![-60000]   bind to random port from C000 to 60000
//      tcp://127.0.0.1:![55000-55999]
//                                  bind to random port from 55000 to 55999
//
//  On success, returns the actual port number used, for tcp:// endpoints,
//  and 0 for other transports. On failure, returns -1. Note that when using
//  ephemeral ports, a port may be reused by different services without
//  clients being aware. Protocols that run on ephemeral ports should take
//  this into account.
CZMQ_EXPORT int
    zsock_bind (zsock_t *self, const char *format, ...);

//  Returns last bound endpoint, if any.
CZMQ_EXPORT char *
    zsock_endpoint (zsock_t *self);

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

//  Attach a socket to zero or more endpoints. If endpoints is not null,
//  parses as list of ZeroMQ endpoints, separated by commas, and prefixed by
//  '@' (to bind the socket) or '>' (to attach the socket). Returns 0 if all
//  endpoints were valid, or -1 if there was a syntax error. If the endpoint
//  does not start with '@' or '>', the serverish argument defines whether
//  it is used to bind (serverish = true) or connect (serverish = false).
CZMQ_EXPORT int
    zsock_attach (zsock_t *self, const char *endpoints, bool serverish);

//  Returns socket type as printable constant string.
CZMQ_EXPORT const char *
    zsock_type_str (zsock_t *self);

//  Send a 'picture' message to the socket (or actor). The picture is a
//  string that defines the type of each frame. This makes it easy to send
//  a complex multiframe message in one call. The picture can contain any
//  of these characters, each corresponding to one or two arguments:
//
//      i = int
//      u = uint
//      s = char *
//      b = byte *, size_t (2 arguments)
//      c = zchunk_t *
//      f = zframe_t *
//      h = zhash_t *
//      p = void * (sends the pointer value, only meaningful over inproc)
//      m = zmsg_t * (sends all frames in the zmsg)
//      z = sends zero-sized frame (0 arguments)
//
//  Note that s, b, c, and f are encoded the same way and the choice is
//  offered as a convenience to the sender, which may or may not already
//  have data in a zchunk or zframe. Does not change or take ownership of
//  any arguments. Returns 0 if successful, -1 if sending failed for any
//  reason.
CZMQ_EXPORT int
    zsock_send (void *self, const char *picture, ...);

//  Receive a 'picture' message to the socket (or actor). See zsock_send for
//  the format and meaning of the picture. Returns the picture elements into
//  a series of pointers as provided by the caller:
//
//      i = int * (stores integer)
//      u = uint * (stores unsigned integer)
//      s = char ** (allocates new string)
//      b = byte **, size_t * (2 arguments) (allocates memory)
//      c = zchunk_t ** (creates zchunk)
//      f = zframe_t ** (creates zframe)
//      p = void ** (stores pointer)
//      h = zhash_t ** (creates zhash)
//      m = zmsg_t ** (creates a zmsg with the remaing frames)
//      z = null, asserts empty frame (0 arguments)
//
//  Note that zsock_recv creates the returned objects, and the caller must
//  destroy them when finished with them. The supplied pointers do not need
//  to be initialized. Returns 0 if successful, or -1 if it failed to recv
//  a message, in which case the pointers are not modified. When message
//  frames are truncated (a short message), sets return values to zero/null.
//  If an argument pointer is NULL, does not store any value (skips it).
//  An 'n' picture matches an empty frame; if the message does not match,
//  the method will return -1.
CZMQ_EXPORT int
    zsock_recv (void *self, const char *picture, ...);

//  Send a binary encoded 'picture' message to the socket (or actor). This
//  method is similar to zsock_send, except the arguments are encoded in a
//  binary format that is compatible with zproto, and is designed to reduce
//  memory allocations. The pattern argument is a string that defines the
//  type of each argument. Supports these argument types:
//
//   pattern    C type                  zproto type:
//      1       uint8_t                 type = "number" size = "1"
//      2       uint16_t                type = "number" size = "2"
//      4       uint32_t                type = "number" size = "3"
//      8       uint64_t                type = "number" size = "4"
//      s       char *, 0-255 chars     type = "string"
//      S       char *, 0-2^32-1 chars  type = "longstr"
//      c       zchunk_t *              type = "chunk"
//      f       zframe_t *              type = "frame"
//      m       zmsg_t *                type = "msg"
//      p       void *, sends pointer value, only over inproc
//
//  Does not change or take ownership of any arguments. Returns 0 if
//  successful, -1 if sending failed for any reason.
CZMQ_EXPORT int
    zsock_bsend (void *self, const char *picture, ...);
    
//  Receive a binary encoded 'picture' message from the socket (or actor).
//  This method is similar to zsock_recv, except the arguments are encoded
//  in a binary format that is compatible with zproto, and is designed to
//  reduce memory allocations. The pattern argument is a string that defines
//  the type of each argument. See zsock_bsend for the supported argument
//  types. All arguments must be pointers; this call sets them to point to
//  values held on a per-socket basis. Do not modify or destroy the returned
//  values. Returns 0 if successful, or -1 if it failed to read a message.
CZMQ_EXPORT int
    zsock_brecv (void *self, const char *picture, ...);
    
//  Set socket to use unbounded pipes (HWM=0); use this in cases when you are
//  totally certain the message volume can fit in memory. This method works
//  across all versions of ZeroMQ. Takes a polymorphic socket reference.
CZMQ_EXPORT void
    zsock_set_unbounded (void *self);

//  Send a signal over a socket. A signal is a short message carrying a
//  success/failure code (by convention, 0 means OK). Signals are encoded
//  to be distinguishable from "normal" messages. Accepts a zock_t or a
//  zactor_t argument, and returns 0 if successful, -1 if the signal could
//  not be sent. Takes a polymorphic socket reference.
CZMQ_EXPORT int
    zsock_signal (void *self, byte status);
    
//  Wait on a signal. Use this to coordinate between threads, over pipe
//  pairs. Blocks until the signal is received. Returns -1 on error, 0 or
//  greater on success. Accepts a zsock_t or a zactor_t as argument.
//  Takes a polymorphic socket reference.
CZMQ_EXPORT int
    zsock_wait (void *self);

//  If there is a partial message still waiting on the socket, remove and
//  discard it. This is useful when reading partial messages, to get specific
//  message types.
CZMQ_EXPORT void
    zsock_flush (void *self);

//  Probe the supplied object, and report if it looks like a zsock_t.
//  Takes a polymorphic socket reference.
CZMQ_EXPORT bool
    zsock_is (void *self);

//  Probe the supplied reference. If it looks like a zsock_t instance, return
//  the underlying libzmq socket handle; elsie if it looks like a file
//  descriptor, return NULL; else if it looks like a libzmq socket handle,
//  return the supplied value. Takes a polymorphic socket reference.
CZMQ_EXPORT void *
    zsock_resolve (void *self);

//  Self test of this class
CZMQ_EXPORT void
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
