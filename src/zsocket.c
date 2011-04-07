/*  =========================================================================
    zsocket - working with 0MQ sockets

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of libzapi, the high-level C binding for 0MQ:
    http://libzapi.zeromq.org.

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

/*
@header
    The zsocket class provides helper functions for 0MQ sockets. It doesn't
    wrap the 0MQ socket type, to avoid breaking all libzmq socket-related
    calls. Automatically subscribes SUB sockets to "".
@discuss
@end
*/

#include "../include/zapi_prelude.h"
#include "../include/zctx.h"
#include "../include/zclock.h"
#include "../include/zstr.h"
#include "../include/zsockopt.h"
#include "../include/zsocket.h"


//  --------------------------------------------------------------------------
//  Create a new socket within our libzapi context, replaces zmq_socket.
//  If the socket is a SUB socket, automatically subscribes to everything.
//  Use this to get automatic management of the socket at shutdown.

void *
zsocket_new (zctx_t *ctx, int type)
{
    void *socket = zctx__socket_new (ctx, type);
    if (type == ZMQ_SUB)
        zsockopt_set_subscribe (socket, "");
    return socket;
}


//  --------------------------------------------------------------------------
//  Destroy the socket. You must use this for any socket created via the
//  zsocket_new method.

void
zsocket_destroy (zctx_t *ctx, void *socket)
{
    zctx__socket_destroy (ctx, socket);
}


//  --------------------------------------------------------------------------
//  Bind a socket to a formatted endpoint
//  Checks with assertion that the bind was valid

void
zsocket_bind (void *socket, const char *format, ...)
{
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    int rc = zmq_bind (socket, endpoint);
    assert (rc == 0);
}


//  --------------------------------------------------------------------------
//  Connect a socket to a formatted endpoint
//  Checks with assertion that the connect was valid

void
zsocket_connect (void *socket, const char *format, ...)
{
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    int rc = zmq_connect (socket, endpoint);
    assert (rc == 0);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zsocket_test (Bool verbose)
{
    printf (" * zsocket: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();

    //  Create a detached thread, let it run
    char *interf = "*";
    char *domain = "localhost";
    int service = 5555;

    void *writer = zsocket_new (ctx, ZMQ_PUSH);
    void *reader = zsocket_new (ctx, ZMQ_PULL);
    zsocket_bind (writer, "tcp://%s:%d", interf, service);
    zsocket_connect (reader, "tcp://%s:%d", domain, service);
    zstr_send (writer, "HELLO");
    char *message = zstr_recv (reader);
    assert (message);
    assert (streq (message, "HELLO"));
    free (message);
    zsocket_destroy (ctx, writer);

    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
