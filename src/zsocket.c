/*  =========================================================================
    zsocket - working with 0MQ sockets

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

/*
@header
    The zsocket class provides helper functions for 0MQ sockets. It doesn't
    wrap the 0MQ socket type, to avoid breaking all libzmq socket-related
    calls.
@discuss
@end
*/

#include "../include/czmq.h"

//  --------------------------------------------------------------------------
//  Create a new socket within our CZMQ context, replaces zmq_socket.
//  Use this to get automatic management of the socket at shutdown.
//  Note: SUB sockets do not automatically subscribe to everything; you
//  must set filters explicitly.

void *
zsocket_new (zctx_t *ctx, int type)
{
    void *self = zctx__socket_new (ctx, type);
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the socket. You must use this for any socket created via the
//  zsocket_new method. If socket is null, does nothing.

void
zsocket_destroy (zctx_t *ctx, void *self)
{
    if (self)
        zctx__socket_destroy (ctx, self);
}


//  --------------------------------------------------------------------------
//  Bind a socket to a formatted endpoint. If the port is specified as
//  '*', binds to any free port from ZSOCKET_DYNFROM to ZSOCKET_DYNTO
//  and returns the actual port number used.  Always returns the
//  port number if successful. Note that if a previous process or thread
//  used the same port, peers may connect to the caller thinking it was
//  the previous process/thread.

int
zsocket_bind (void *self, const char *format, ...)
{
    //  Ephemeral port needs 4 additional characters
    char endpoint [256 + 4];
    va_list argptr;
    va_start (argptr, format);
    int endpoint_size = vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);

    //  Port must be at end of endpoint
    if (endpoint [endpoint_size - 2] == ':'
    &&  endpoint [endpoint_size - 1] == '*') {
        int port = ZSOCKET_DYNFROM;
        while (port <= ZSOCKET_DYNTO) {
            //  Try to bind on the next plausible port
            sprintf (endpoint + endpoint_size - 1, "%d", port);
            if (zmq_bind (self, endpoint) == 0)
                return port;
            port++;
        }
        return -1;
    }
    else {
        //  Return actual port used for binding
        int rc = zmq_bind (self, endpoint);
        if (rc == 0)
            rc = atoi (strrchr (endpoint, ':') + 1);
        else
            rc = -1;
        return rc;
    }
}


//  --------------------------------------------------------------------------
//  Unbind a socket from a formatted endpoint.
//  Returns 0 if OK, -1 if the endpoint was invalid or the function
//  isn't supported.

int
zsocket_unbind (void *self, const char *format, ...)
{
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(3,2,0))
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_unbind (self, endpoint);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Connect a socket to a formatted endpoint
//  Returns 0 if the endpoint is valid, -1 if the connect failed.

int
zsocket_connect (void *self, const char *format, ...)
{
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_connect (self, endpoint);
}

//  --------------------------------------------------------------------------
//  Disconnect a socket from a formatted endpoint
//  Returns 0 if disconnection is complete -1 if the disconnection failed.

int
zsocket_disconnect (void *self, const char *format, ...)
{
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(3,2,0))
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_disconnect (self, endpoint);
#else
    return -1;
#endif
}

//  --------------------------------------------------------------------------
//  Poll for input events on the socket. Returns true if there is input
//  ready on the socket, else false.

bool
zsocket_poll (void *self, int msecs)
{
    zmq_pollitem_t items [] = { { self, 0, ZMQ_POLLIN, 0 } };
    int rc = zmq_poll (items, 1, msecs);
    return rc != -1 && (items [0].revents & ZMQ_POLLIN) != 0;
}


//  --------------------------------------------------------------------------
//  Returns socket type as printable constant string

char *
zsocket_type_str (void *self)
{
    char *type_name [] = {
        "PAIR", "PUB", "SUB", "REQ", "REP",
        "DEALER", "ROUTER", "PULL", "PUSH",
        "XPUB", "XSUB", "STREAM"
    };
    int type = zsocket_type (self);
#if ZMQ_VERSION_MAJOR == 4
    if (type < 0 || type > ZMQ_STREAM)
#else
    if (type < 0 || type > ZMQ_XSUB)
#endif
        return "UNKNOWN";
    else
        return type_name [type];
}

//  --------------------------------------------------------------------------
//  Send data over a socket as a single message frame.
//  Accepts these flags: ZFRAME_MORE and ZFRAME_DONTWAIT.

int
zsocket_sendmem (void *zocket, const void* data, size_t size, int flags)
{
    assert (zocket);
    assert (size == 0 || data);

    int snd_flags = (flags & ZFRAME_MORE)? ZMQ_SNDMORE : 0;
    snd_flags |= (flags & ZFRAME_DONTWAIT)? ZMQ_DONTWAIT : 0;

    zmq_msg_t msg;
    zmq_msg_init_size (&msg, size);
    memcpy (zmq_msg_data (&msg), data, size);

    int rc = zmq_sendmsg (zocket, &msg, snd_flags);
    return rc == -1? -1: 0;
}


//  --------------------------------------------------------------------------
//  Selftest

int
zsocket_test (bool verbose)
{
    printf (" * zsocket: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);

    //  Create a detached thread, let it run
    char *interf = "*";
    char *domain = "localhost";
    int service = 5560;

    void *writer = zsocket_new (ctx, ZMQ_PUSH);
    assert (writer);
    void *reader = zsocket_new (ctx, ZMQ_PULL);
    assert (reader);
    assert (streq (zsocket_type_str (writer), "PUSH"));
    assert (streq (zsocket_type_str (reader), "PULL"));
    int rc = zsocket_bind (writer, "tcp://%s:%d", interf, service);
    assert (rc == service);

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3,2,0))
    //  Check unbind
    rc = zsocket_unbind (writer, "tcp://%s:%d", interf, service);
    assert (rc == 0);

    //  In some cases and especially when running under Valgrind, doing
    //  a bind immediately after an unbind causes an EADDRINUSE error.
    //  Even a short sleep allows the OS to release the port for reuse.
    zclock_sleep (100);
    
    //  Bind again
    rc = zsocket_bind (writer, "tcp://%s:%d", interf, service);
    assert (rc == service);
#endif

    rc = zsocket_connect (reader, "tcp://%s:%d", domain, service);
    assert (rc == 0);
    zstr_send (writer, "HELLO");
    char *message = zstr_recv (reader);
    assert (message);
    assert (streq (message, "HELLO"));
    free (message);

    //  Test binding to ports
    int port = zsocket_bind (writer, "tcp://%s:*", interf);
    assert (port >= ZSOCKET_DYNFROM && port <= ZSOCKET_DYNTO);

    assert (zsocket_poll (writer, 100) == false);

    rc = zsocket_connect (reader, "txp://%s:%d", domain, service);
    assert (rc == -1);

    //  Test sending frames to socket
    rc = zsocket_sendmem (writer,"ABC", 3, ZFRAME_MORE);
    assert (rc == 0);
    rc = zsocket_sendmem (writer, "DEFG", 4, 0);
    assert (rc == 0);

    zframe_t *frame = zframe_recv (reader);
    assert (frame);
    assert (zframe_streq (frame, "ABC"));
    assert (zframe_more (frame));
    zframe_destroy (&frame);

    frame = zframe_recv (reader);
    assert (frame);
    assert (zframe_streq (frame, "DEFG"));
    assert (!zframe_more (frame));
    zframe_destroy (&frame);

    zsocket_destroy (ctx, writer);
    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
