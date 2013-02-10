/*  =========================================================================
    zsocket - working with 0MQ sockets

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZSOCKET_H_INCLUDED__
#define __ZSOCKET_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  This port range is defined by IANA for dynamic or private ports
//  We use this when choosing a port for dynamic binding.
#define ZSOCKET_DYNFROM     0xc000
#define ZSOCKET_DYNTO       0xffff

//  Create a new socket within our CZMQ context, replaces zmq_socket.
//  Use this to get automatic management of the socket at shutdown.
//  Note: SUB sockets do not automatically subscribe to everything; you
//  must set filters explicitly.
CZMQ_EXPORT void *
    zsocket_new (zctx_t *self, int type);

//  Destroy a socket within our CZMQ context, replaces zmq_close.
CZMQ_EXPORT void
    zsocket_destroy (zctx_t *self, void *socket);

//  Bind a socket to a formatted endpoint. If the port is specified as
//  '*', binds to any free port from ZSOCKET_DYNFROM to ZSOCKET_DYNTO
//  and returns the actual port number used. Otherwise asserts that the
//  bind succeeded with the specified port number. Always returns the
//  port number if successful.
CZMQ_EXPORT int
    zsocket_bind (void *socket, const char *format, ...);

//  Connect a socket to a formatted endpoint
//  Returns 0 if OK, -1 if the endpoint was invalid.
CZMQ_EXPORT int
    zsocket_connect (void *socket, const char *format, ...);

//  Disonnect a socket from a formatted endpoint
//  Returns 0 if OK, -1 if the endpoint was invalid or the function
//  isn't supported.
CZMQ_EXPORT int
    zsocket_disconnect (void *socket, const char *format, ...);

//  Poll for input events on the socket. Returns TRUE if there is input
//  ready on the socket, else FALSE.
CZMQ_EXPORT bool
    zsocket_poll (void *socket, int msecs);
    
//  Returns socket type as printable constant string
CZMQ_EXPORT char *
    zsocket_type_str (void *socket);

//  Self test of this class
CZMQ_EXPORT int
    zsocket_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
