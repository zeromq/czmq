/*  =========================================================================
    zstr - sending and receiving strings

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZSTR_H_INCLUDED__
#define __ZSTR_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Receive C string from socket. Caller must free returned string using
//  zstr_free(). Returns NULL if the context is being terminated or the
//  process was interrupted.
CZMQ_EXPORT char *
    zstr_recv (void *source);

//  Send a C string to a socket, as a frame. The string is sent without
//  trailing null byte; to read this you can use zstr_recv, or a similar
//  method that adds a null terminator on the received string. String
//  may be NULL, which is sent as "".
CZMQ_EXPORT int
    zstr_send (void *dest, const char *string);

//  Send a C string to a socket, as zstr_send(), with a MORE flag, so that
//  you can send further strings in the same multi-part message.
CZMQ_EXPORT int
    zstr_sendm (void *dest, const char *string);

//  Send a formatted string to a socket. Note that you should NOT use
//  user-supplied strings in the format (they may contain '%' which
//  will create security holes).
CZMQ_EXPORT int
    zstr_sendf (void *dest, const char *format, ...);

//  Send a formatted string to a socket, as for zstr_sendf(), with a
//  MORE flag, so that you can send further strings in the same multi-part
//  message.
CZMQ_EXPORT int
    zstr_sendfm (void *dest, const char *format, ...);

//  Send a series of strings (until NULL) as multipart data
//  Returns 0 if the strings could be sent OK, or -1 on error.
CZMQ_EXPORT int
    zstr_sendx (void *dest, const char *string, ...);

//  Receive a series of strings (until NULL) from multipart data.
//  Each string is allocated and filled with string data; if there
//  are not enough frames, unallocated strings are set to NULL.
//  Returns -1 if the message could not be read, else returns the
//  number of strings filled, zero or more. Free each returned string
//  using zstr_free(). If not enough strings are provided, remaining
//  multipart frames in the message are dropped.
CZMQ_EXPORT int
    zstr_recvx (void *source, char **string_p, ...);

//  Free a provided string, and nullify the parent pointer. Safe to call on
//  a null pointer.
CZMQ_EXPORT void
    zstr_free (char **string_p);

//  Self test of this class
CZMQ_EXPORT void
    zstr_test (bool verbose);
//  @end

//  DEPRECATED as poor style -- callers should use zloop or zpoller
//  Receive C string from socket, if socket had input ready. Caller must
//  free returned string using zstr_free. Returns NULL if there was no input
//  waiting, or if the context was terminated. Use zctx_interrupted to exit
//  any loop that relies on this method.
CZMQ_EXPORT char *
    zstr_recv_nowait (void *source);

//  Compiler hints
CZMQ_EXPORT int zstr_sendf (void *dest, const char *format, ...) CHECK_PRINTF (2);

#ifdef __cplusplus
}
#endif

#endif
