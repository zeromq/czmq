/*  =========================================================================
    zudp - class description

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ZUDP_H_INCLUDED
#define ZUDP_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @warning THE FOLLOWING @INTERFACE BLOCK IS AUTO-GENERATED BY ZPROJECT
//  @warning Please edit the model at "api/zudp.api" to make changes.
//  @interface
//  This is a draft class, and may change without notice. It is disabled in
//  stable builds by default. If you use this in applications, please ask
//  for it to be pushed to stable state. Use --enable-drafts to enable.
#ifdef CZMQ_BUILD_DRAFT_API
//  *** Draft method, for development use, may change without warning ***
//  Create UDP socket of the given type. Set the reuse option if
//  you need to reuse the socket's address.
CZMQ_EXPORT zudp_t *
    zudp_new (int type, bool reuse);

//  *** Draft method, for development use, may change without warning ***
//  Close a UDP socket
CZMQ_EXPORT void
    zudp_destroy (zudp_t **self_p);

//  *** Draft method, for development use, may change without warning ***
//  Send zframe to UDP socket to given address and port,
//  return -1 if sending failed (i.e. due to interface having
//  disappeared (happens easily with WiFi))
CZMQ_EXPORT int
    zudp_sendto (zudp_t *self, zframe_t *frame, const char *address, int port);

//  *** Draft method, for development use, may change without warning ***
//  Receive zframe from UDP socket, and set address of peer that sent it
//  The peername must be a char [INET_ADDRSTRLEN] array if IPv6 is disabled or
//  NI_MAXHOST if it's enabled. Returns NULL when failing to get peer address.
CZMQ_EXPORT zframe_t *
    zudp_recv (zudp_t *self, char *peername, int peerlen);

//  *** Draft method, for development use, may change without warning ***
//  Bind a socket to a and address and port number.
//  On failure, returns -1.
CZMQ_EXPORT int
    zudp_bind (zudp_t *self, const char *address, int port);

//  *** Draft method, for development use, may change without warning ***
//  Return the socket's filedescriptor
CZMQ_EXPORT int
    zudp_fd (zudp_t *self);

//  *** Draft method, for development use, may change without warning ***
//  Handle an I/O error on some socket operation; will report and die on
//  fatal errors, and continue silently on "try again" errors.
CZMQ_EXPORT void
    zudp_error (const char *reason);

//  *** Draft method, for development use, may change without warning ***
//  Self test of this class.
CZMQ_EXPORT void
    zudp_test (bool verbose);

#endif // CZMQ_BUILD_DRAFT_API
//  @end

#ifdef __cplusplus
}
#endif

#endif
