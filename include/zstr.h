/*  =========================================================================
    zstr - sending and receiving strings

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

#ifndef __ZSTR_H_INCLUDED__
#define __ZSTR_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Receive a string off a socket, caller must free it
CZMQ_EXPORT char *
    zstr_recv (void *socket);

//  Receive a string off a socket if socket had input waiting
CZMQ_EXPORT char *
    zstr_recv_nowait (void *socket);

//  Send a formatted string to a socket
CZMQ_EXPORT int
    zstr_send (void *socket, const char *format, ...);

//  Send a formatted string to a socket, with MORE flag
CZMQ_EXPORT int
    zstr_sendm (void *socket, const char *format, ...);

//  Send a series of strings (until NULL) as multipart data
//  Returns 0 if the strings could be sent OK, or -1 on error.
CZMQ_EXPORT int
    zstr_sendx (void *socket, const char *string, ...);

//  Receive a series of strings (until NULL) from multipart data
//  Each string is allocated and filled with string data; if there
//  are not enough frames, unallocated strings are set to NULL.
//  Returns -1 if the message could not be read, else returns the
//  number of strings filled, zero or more.
CZMQ_EXPORT int
    zstr_recvx (void *socket, char **string_p, ...);

//  Self test of this class
CZMQ_EXPORT int
    zstr_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
