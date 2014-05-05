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

/*
@header
    The zsock class wraps the libzmq socket handle (a void *) with a proper
    structure that follows the CLASS rules for construction and destruction.
    CZMQ methods that accept a socket will accept either a zsock_t * or a
    libzmq void *, and will detect the type at runtime.
@discuss
@end
*/

#include "../include/czmq.h"

//  zsock_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZSOCK_TAG           0x0004cafe

//  This port range is defined by IANA for dynamic or private ports
//  We use this when choosing a port for dynamic binding.
#define DYNAMIC_FIRST       0xc000
#define DYNAMIC_LAST        0xffff

//  Structure of our class

struct _zsock_t {
    uint32_t tag;               //  Object tag for runtime detection
    void *handle;               //  The libzmq socket handle
    int type;                   //  The socket type
};


//  --------------------------------------------------------------------------
//  Create a new socket.
//  TODO: set-up global context safely using a mutex.

zsock_t *
zsock_new (int type)
{
    zsock_t *self = (zsock_t *) zmalloc (sizeof (zsock_t));
    if (!self)
        return NULL;

    self->tag = ZSOCK_TAG;
    self->handle = zsys_socket (type);
    self->type = type;
    if (!self->handle) {
        free (self);
        return NULL;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the socket. You must use this for any socket created via the
//  zsock_new method.

void
zsock_destroy (zsock_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zsock_t *self = *self_p;
        assert (zsock_is (self));
        self->tag = 0xDeadBeef;
        zsocket_set_linger (self->handle, 0);
        zmq_close (self->handle);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Bind a socket to a formatted endpoint. If the port is specified as '*'
//  and the endpoint starts with "tcp://", binds to an ephemeral TCP port in
//  a high range. Always returns the port number on successful TCP binds, else
//  returns zero on success. Returns -1 on failure. When using ephemeral ports,
//  note that ports may be reused by different threads, without clients being
//  aware.

int
zsock_bind (zsock_t *self, const char *format, ...)
{
    assert (self);
    assert (zsock_is (self));

    //  Ephemeral port endpoint needs 4 additional characters
    char endpoint [256 + 4];
    va_list argptr;
    va_start (argptr, format);
    int endpoint_size = vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);

    bool tcp_endpoint = (endpoint_size > 6
                         && memcmp (endpoint, "tcp://", 6) == 0);

    //  We implement ephemeral ports here ourselves, so that we can
    //  run on older versions of libzmq that don't do this properly.
    if (tcp_endpoint
    &&  endpoint [endpoint_size - 2] == ':'
    &&  endpoint [endpoint_size - 1] == '*') {
        //  Always start at DYNAMIC_FORST; this makes reuse collisions
        //  more likely, and forces protocol designers to think about
        //  this upfront.
        int port = DYNAMIC_FIRST;
        while (port <= DYNAMIC_LAST) {
            //  Try to bind on the next plausible port
            sprintf (endpoint + endpoint_size - 1, "%d", port);
            if (zmq_bind (self->handle, endpoint) == 0)
                return port;
            port++;
        }
        return -1;              //  Can't find a free port
    }
    else
    if (zmq_bind (self->handle, endpoint) == 0) {
        if (tcp_endpoint)
            //  Return actual port used for binding
            return atoi (strrchr (endpoint, ':') + 1);
        else
            return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Unbind a socket from a formatted endpoint.
//  Returns 0 if OK, -1 if the endpoint was invalid or the function
//  isn't supported.

int
zsock_unbind (zsock_t *self, const char *format, ...)
{
    assert (self);
    assert (zsock_is (self));

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(3,2,0))
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_unbind (self->handle, endpoint);
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
    assert (self);
    assert (zsock_is (self));

    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_connect (self->handle, endpoint);
}


//  --------------------------------------------------------------------------
//  Disconnect a socket from a formatted endpoint
//  Returns 0 if disconnection is complete -1 if the disconnection failed.

int
zsock_disconnect (zsock_t *self, const char *format, ...)
{
    assert (self);
    assert (zsock_is (self));

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION(3,2,0))
    char endpoint [256];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 256, format, argptr);
    va_end (argptr);
    return zmq_disconnect (self->handle, endpoint);
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Returns socket type as printable constant string

const char *
zsock_type_str (zsock_t *self)
{
    assert (self);
    assert (zsock_is (self));

    char *type_name [] = {
        "PAIR", "PUB", "SUB", "REQ", "REP",
        "DEALER", "ROUTER", "PULL", "PUSH",
        "XPUB", "XSUB", "STREAM"
    };
    return type_name [self->type];
}


//  --------------------------------------------------------------------------
//  Send a signal over a socket. A signal is a zero-byte message.
//  Signals are used primarily between threads, over pipe sockets.
//  Returns -1 if there was an error sending the signal.

int
zsock_signal (zsock_t *self)
{
    assert (self);
    assert (zsock_is (self));

    zmq_msg_t msg;
    zmq_msg_init_size (&msg, 0);
    if (zmq_sendmsg (self->handle, &msg, 0) == -1) {
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
    assert (self);
    assert (zsock_is (self));

    zmq_msg_t msg;
    zmq_msg_init (&msg);
    if (zmq_recvmsg (self->handle, &msg, 0) == -1)
        return -1;
    else
        return 0;
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zsock_t.

bool
zsock_is (void *self)
{
    assert (self);
    return ((zsock_t *) self)->tag == ZSOCK_TAG;
}


//  --------------------------------------------------------------------------
//  Probe the supplied reference. If it looks like a zsock_t instance,
//  return the underlying libzmq socket handle; else if it looks like
//  a libzmq socket handle, return the supplied value.

void *
zsock_resolve (void *self)
{
    assert (self);
    if (zsock_is (self))
        return ((zsock_t *) self)->handle;
    else
        return self;
}


//  --------------------------------------------------------------------------
//  Selftest

int
zsock_test (bool verbose)
{
    printf (" * zsock: ");

    //  @selftest
    int service = 5560;

    zsock_t *writer = zsock_new (ZMQ_PUSH);
    assert (writer);
    assert (zsock_resolve (writer) != writer);
    zsock_t *reader = zsock_new (ZMQ_PULL);
    assert (reader);
    assert (zsock_resolve (reader) != reader);
    assert (streq (zsock_type_str (writer), "PUSH"));
    assert (streq (zsock_type_str (reader), "PULL"));
    int rc = zsock_bind (writer, "tcp://%s:%d", "127.0.0.1", service);
    assert (rc == service);

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3,2,0))
    //  Check unbind
    rc = zsock_unbind (writer, "tcp://%s:%d", "127.0.0.1", service);
    assert (rc == 0);

    //  In some cases and especially when running under Valgrind, doing
    //  a bind immediately after an unbind causes an EADDRINUSE error.
    //  Even a short sleep allows the OS to release the port for reuse.
    zclock_sleep (100);

    //  Bind again
    rc = zsock_bind (writer, "tcp://%s:%d", "127.0.0.1", service);
    assert (rc == service);
#endif

    rc = zsock_connect (reader, "tcp://%s:%d", "localhost", service);
    assert (rc == 0);
    zstr_send (writer->handle, "HELLO");
    char *message = zstr_recv (reader->handle);
    assert (message);
    assert (streq (message, "HELLO"));
    free (message);

    //  Test binding to ports
    int port = zsock_bind (writer, "tcp://%s:*", "127.0.0.1");
    assert (port >= DYNAMIC_FIRST && port <= DYNAMIC_LAST);

    //  Test error state when connecting to an invalid socket type
    //  ('txp://' instead of 'tcp://', typo intentional)
    rc = zsock_connect (reader, "txp://localhost:%d", service);
    assert (rc == -1);

    rc = zsock_signal (writer);
    assert (rc == 0);
    rc = zsock_wait (reader);
    assert (rc == 0);

    zsock_destroy (&reader);
    zsock_destroy (&writer);
    //  @end

    printf ("OK\n");
    return 0;
}
