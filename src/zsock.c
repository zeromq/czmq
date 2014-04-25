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

/*
@header
    The zsock class provides helper functions for 0MQ sockets. It doesn't
    wrap the 0MQ socket type, to avoid breaking all libzmq socket-related
    calls.
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zsock_t {
    void *socket;               //  Our socket
    void *context;              //  Our 0MQ context
    int type;                   //  The socket type
};

//  --------------------------------------------------------------------------
//  Create a new socket within our CZMQ context, replaces zmq_socket.
//  Use this to get automatic management of the socket at shutdown.
//  Note: SUB sockets do not automatically subscribe to everything; you
//  must set filters explicitly.

zsock_t *
zsock_new (zctx_t *ctx, int type)
{
    assert(ctx);
    zsock_t *self = (zsock_t *) zmalloc (sizeof (zsock_t));
    if (!self)
        return NULL;

    self->socket = zctx__socket_new (ctx, type);
    self->context = ctx;
    self->type = type;

    if (!self->socket) {
        free (self);
        return NULL;
    }

    return self;
}

//  --------------------------------------------------------------------------
//  Destroy the socket. You must use this for any socket created via the
//  zsock_new method. If socket is null, does nothing.

void
zsock_destroy (zsock_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zsock_t *self = *self_p;

        zctx__socket_destroy (self->context, self->socket);

        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Bind a socket to a formatted endpoint. If the port is specified as
//  '*', binds to any free port from ZSOCKET_DYNFROM to ZSOCKET_DYNTO
//  and returns the actual port number used.  Always returns the
//  port number if successful. Note that if a previous process or thread
//  used the same port, peers may connect to the caller thinking it was
//  the previous process/thread.

int
zsock_bind (zsock_t *self, const char *format, ...)
{
    assert(self);
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
            if (zmq_bind (self->socket, endpoint) == 0)
                return port;
            port++;
        }
        return -1;
    }
    else {
        //  Return actual port used for binding
        int port = zmq_bind (self->socket, endpoint);
        if (port == 0)
            port = atoi (strrchr (endpoint, ':') + 1);
        else
            port = -1;
        return port;
    }
}


//  --------------------------------------------------------------------------
//  Unbind a socket from a formatted endpoint.
//  Returns 0 if OK, -1 if the endpoint was invalid or the function
//  isn't supported.

int
zsock_unbind (zsock_t *self, const char *format, ...)
{
    assert(self);
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(3,2,0))
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_unbind (self->socket, endpoint);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Connect a socket to a formatted endpoint
//  Returns 0 if the endpoint is valid, -1 if the connect failed.

int
zsock_connect (zsock_t *self, const char *format, ...)
{
    assert(self);
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_connect (self->socket, endpoint);
}

//  --------------------------------------------------------------------------
//  Disconnect a socket from a formatted endpoint
//  Returns 0 if disconnection is complete -1 if the disconnection failed.

int
zsock_disconnect (zsock_t *self, const char *format, ...)
{
    assert(self);
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(3,2,0))
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_disconnect (self->socket, endpoint);
#else
    return -1;
#endif
}

//  --------------------------------------------------------------------------
//  Poll for input events on the socket. Returns true if there is input
//  ready on the socket, else false.

bool
zsock_poll (zsock_t *self, int msecs)
{
    assert(self);
    zmq_pollitem_t items [] = { { self->socket, 0, ZMQ_POLLIN, 0 } };
    int rc = zmq_poll (items, 1, msecs);
    return rc != -1 && (items [0].revents & ZMQ_POLLIN) != 0;
}


//  --------------------------------------------------------------------------
//  Returns socket type as printable constant string

const char *
zsock_type_str (zsock_t *self)
{
    assert(self);
    char *type_name [] = {
        "PAIR", "PUB", "SUB", "REQ", "REP",
        "DEALER", "ROUTER", "PULL", "PUSH",
        "XPUB", "XSUB", "STREAM"
    };
    int type = zsock_type (self);
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
zsock_sendmem (zsock_t *self, const void *data, size_t size, int flags)
{
    assert (self);
    assert (size == 0 || data);

    int snd_flags = (flags & ZFRAME_MORE)? ZMQ_SNDMORE : 0;
    snd_flags |= (flags & ZFRAME_DONTWAIT)? ZMQ_DONTWAIT : 0;

    zmq_msg_t msg;
    zmq_msg_init_size (&msg, size);
    memcpy (zmq_msg_data (&msg), data, size);

    if (zmq_sendmsg (self->socket, &msg, snd_flags) == -1) {
        zmq_msg_close (&msg);
        return -1;
    }
    else
        return 0;
}


//  --------------------------------------------------------------------------
//  Send a signal over a socket. A signal is a zero-byte message.
//  Signals are used primarily between threads, over pipe sockets.
//  Returns -1 if there was an error sending the signal.

int
zsock_signal (zsock_t *self)
{
    assert(self);
    zmq_msg_t msg;
    zmq_msg_init_size (&msg, 0);
    if (zmq_sendmsg (self->socket, &msg, 0) == -1) {
        zmq_msg_close (&msg);
        return -1;
    }
    else
        return 0;
}


//  --------------------------------------------------------------------------
//  Wait on a signal. Use this to coordinate between threads, over
//  pipe pairs. Blocks until the signal is received. Returns -1 on error,
//  0 on success.

int
zsock_wait (zsock_t *self)
{
    assert(self);
    zmq_msg_t msg;
    zmq_msg_init (&msg);
    if (zmq_recvmsg (self->socket, &msg, 0) == -1)
        return -1;
    else
        return 0;
}


//  --------------------------------------------------------------------------
//  Return the encapsulated 0MQ socket.

void *
zsock_socket (zsock_t *self)
{
    assert(self);
	return self->socket;
}


//  --------------------------------------------------------------------------
//  Send frame to socket, destroy after sending unless ZFRAME_REUSE is
//  set or the attempt to send the message errors out.
//  Return -1 on error, 0 on success.
int
zsock_send_frame(zsock_t *self, zframe_t **frame_p, int flags)
{
    assert(self);
    return zframe_send(frame_p, self->socket, flags);
}


//  --------------------------------------------------------------------------
//  Receive frame from socket, returns zframe_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  zsock_recv_frame_nowait().
zframe_t *
zsock_recv_frame (zsock_t *self)
{
    assert(self);
    return zframe_recv(self->socket);
}


//  --------------------------------------------------------------------------
//  Receive a new frame off the socket. Returns newly allocated frame, or
//  NULL if there was no input waiting, or if the read was interrupted.
zframe_t *
zsock_recv_frame_nowait (zsock_t *self)
{
    assert(self);
    return zframe_recv_nowait(self->socket);
}

//  --------------------------------------------------------------------------
//  Send message to socket, destroy after sending. If the message has no
//  frames, sends nothing but destroys the message anyhow. Safe to call
//  if zmsg is null.
int
zsock_send_msg (zsock_t *self, zmsg_t **msg_p)
{
    assert(self);
    return zmsg_send(msg_p, self->socket);
}


//  --------------------------------------------------------------------------
//  Receive message from socket, returns zmsg_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  the zloop class or zsock_msg_recv_nowait() or zmq_poll to check for socket 
//  input before receiving.
zmsg_t *
zsock_recv_msg (zsock_t *self)
{
    assert(self);
    return zmsg_recv(self->socket);
}


//  --------------------------------------------------------------------------
//  Receive message from socket, returns zmsg_t object, or NULL either if there was
//  no input waiting, or the recv was interrupted.
zmsg_t *
zsock_recv_msg_nowait (zsock_t *self)
{
    assert(self);
    return zmsg_recv_nowait(self->socket);
}


//  --------------------------------------------------------------------------
//  include generated code

#include "zsock_option.c"


//  --------------------------------------------------------------------------
//  Selftest

int
zsock_test (bool verbose)
{
    printf (" * zsock: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);

    //  Create a detached thread, let it run
    char *interf = "*";
    char *domain = "localhost";
    int service = 5560;

    zsock_t *writer = zsock_new (ctx, ZMQ_PUSH);
    assert (writer);
    zsock_t *reader = zsock_new (ctx, ZMQ_PULL);
    assert (reader);
    assert (streq (zsock_type_str (writer), "PUSH"));
    assert (streq (zsock_type_str (reader), "PULL"));
    int rc = zsock_bind (writer, "tcp://%s:%d", interf, service);
    assert (rc == service);

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3,2,0))
    //  Check unbind
    rc = zsock_unbind (writer, "tcp://%s:%d", interf, service);
    assert (rc == 0);

    //  In some cases and especially when running under Valgrind, doing
    //  a bind immediately after an unbind causes an EADDRINUSE error.
    //  Even a short sleep allows the OS to release the port for reuse.
    zclock_sleep (100);

    //  Bind again
    rc = zsock_bind (writer, "tcp://%s:%d", interf, service);
    assert (rc == service);
#endif

    rc = zsock_connect (reader, "tcp://%s:%d", domain, service);
    assert (rc == 0);
    zstr_send (writer->socket, "HELLO");
    char *message = zstr_recv (reader->socket);
    assert (message);
    assert (streq (message, "HELLO"));
    free (message);

    //  Test binding to ports
    int port = zsock_bind (writer, "tcp://%s:*", interf);
    assert (port >= ZSOCKET_DYNFROM && port <= ZSOCKET_DYNTO);

    assert (zsock_poll (writer, 100) == false);

    //  Test error state when connecting to an invalid socket type
    //  ('txp://' instead of 'tcp://', typo intentional)
    rc = zsock_connect (reader, "txp://%s:%d", domain, service);
    assert (rc == -1);

    //  Test sending frames to socket
    rc = zsock_sendmem (writer, "ABC", 3, ZFRAME_MORE);
    assert (rc == 0);
    rc = zsock_sendmem (writer, "DEFG", 4, 0);
    assert (rc == 0);

    zframe_t *frame = zsock_recv_frame (reader);
    assert (frame);
    assert (zframe_streq (frame, "ABC"));
    assert (zframe_more (frame));
    zframe_destroy (&frame);

    frame = zsock_recv_frame (reader);
    assert (frame);
    assert (zframe_streq (frame, "DEFG"));
    assert (!zframe_more (frame));
    zframe_destroy (&frame);

    rc = zsock_signal (writer);
    assert (rc == 0);
    rc = zsock_wait (reader);
    assert (rc == 0);

    zsock_destroy (&reader);
    zsock_destroy (&writer);
    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
