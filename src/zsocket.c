/*  =========================================================================
    zsocket - working with 0MQ sockets (deprecated)

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

static int is_rand_initd = 0;

//  --------------------------------------------------------------------------
//  This functions returns a random number between args min and max.

static unsigned int
s_get_rand_in_range (unsigned int min, unsigned int max)
{
    if (!is_rand_initd ) {
        srandom (time (NULL));
        is_rand_initd = 1;
    }
    double scaled = (double)random () / (RAND_MAX + 1.0);
    return (unsigned int)((max - min + 1) * scaled) + min;
}

//  --------------------------------------------------------------------------
//  This function parses the endpoint string and returns the min and max range
//  values via the int pointers. If no such notation is present, the min and max
//  values are set to zero, and -1 is returned.
//  If range numbers are parsed, the func returns 0.

static int
s_parse_port_notation (const char *endpoint, int *min, int *max)
{
    char *colonptr = strrchr (endpoint,':'); // there are TWO :'s in the spec
    char *rangespec = strchr (colonptr, '[');
    if (rangespec) {
        char *rangesep = strchr (rangespec, '-');
        if (rangesep) {
            int p1 = atoi (rangespec+1);
            int p2 = atoi (rangesep+1);
            if (p1 > 0) {
                *min = p1;
            }
            *max = 0; // just in case
            if (p2 > 0) {
                *max = p2;
            }
            return 0;
        }
        else {
            int p1 = atoi (rangespec+1);
            if (p1 > 0) {
                *min = p1;
                *max = 0; // no max, set to 0
            }
            return 0;
        }
    }
    *min = 0;
    *max = 0;
    return -1;
}

//  --------------------------------------------------------------------------
//  Bind a socket to a formatted endpoint. If the port is specified as
//  '*', or '!',  binds to any free port from ZSOCKET_DYNFROM to ZSOCKET_DYNTO
//  and returns the actual port number used.  To override the given range, which
//  is the IANA designated range from C000 (49152) to FFFF (65535), after the * or !,
//  you can include the notation "[<min>-<max>]". 
//  '*' will test ports starting at 'min' (default ZSOCKET_DYNFROM), and increment 
//      upwards to 'max' (default ZSOCKET_DYNTO).
//  '!' will randomly choose ports from within the range. It will iterate from one
//      random port number to the next. It will give up if it attempts more than
//      total possible number of ports in the range, with a max of 10. Then it will
//      revert to a linear search (as with '*'). Therefore, '!' works best in large,
//      sparsely populated ranges.
//  The first open port is bound and its number returned.
//  Always returns the port number if successful. 
//  Returns -1 if not successful. 
//  Note that if a previous process or thread
//  used the same port, peers may connect to the caller thinking it was
//  the previous process/thread.


int
zsocket_bind (void *self, const char *format, ...)
{
    //  Ephemeral port needs 4 additional characters
    //  and range notation adds 8 ([,],-, and another (possibly) 5 digit port number)
    char endpoint [256 + 12];
    va_list argptr;
    va_start (argptr, format);
    vsnprintf (endpoint, 264, format, argptr);
    va_end (argptr);
    char *colonptr = strrchr (endpoint,':');

    //  Port must be at end of endpoint
    if (colonptr 
    &&  *(colonptr + 1) == '*') { // sequential search in range
        int port = ZSOCKET_DYNFROM;  // these values act as defaults
        int lim = ZSOCKET_DYNTO;

        int p1;
        int p2;
        int ret = s_parse_port_notation (endpoint, &p1, &p2);
        if (ret == 0 && p1 > 0)
            port = p1;
        if (ret == 0 && p2 > 0)
            lim = p2;

        while (port <= lim) {
            //  Try to bind on the next plausible port
            sprintf (colonptr + 1, "%d", port);
            if (zmq_bind (self, endpoint) == 0)
                return port;
            port++;
        }
        return -1;
    }
    else 
    if (colonptr
    &&  *(colonptr + 1)  == '!') { // Random search in range
        int port = ZSOCKET_DYNFROM;  // these values act as defaults
        int lim = ZSOCKET_DYNTO;

        int p1;
        int p2;
        int ret = s_parse_port_notation (endpoint, &p1, &p2);
        if (ret == 0 && p1 > 0)
            port = p1;
        if (ret == 0 && p2 > 0)
            lim = p2;

        int its=0;
        int limits = lim-port;
        if (limits > 10)
            limits = 10;            // arbitrary cutoff; if you can't get an usable port in 10 moves,
                                    // switch to a linear search

        do {
            int p1 = s_get_rand_in_range (port, lim);
            sprintf (colonptr + 1, "%d", p1);
            if (zmq_bind (self, endpoint) == 0)
                return p1;
            its++;
        } while (its < limits);   // if you end up with your range entirely (or almost entirely) allocated...

        while (port <= lim) {       // OK, if we have filled up the range to some extent, choosing new random ports
                                    // ends up being way inefficient, so we revert to a linear traversal to
                                    // find the few remaining (if any) ports.
            sprintf (colonptr + 1, "%d", port);
            if (zmq_bind (self, endpoint) == 0)
                return port;
            port++;
        }
        return -1;
    }
    else {
        //  Return actual port used for binding
        int port = zmq_bind (self, endpoint);
        if (port == 0)
            port = atoi (colonptr + 1);
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
zsocket_unbind (void *self, const char *format, ...)
{
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3,2,0))
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
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3,2,0))
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

const char *
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
zsocket_sendmem (void *self, const void *data, size_t size, int flags)
{
    assert (self);
    assert (size == 0 || data);

    int snd_flags = (flags & ZFRAME_MORE)? ZMQ_SNDMORE : 0;
    snd_flags |= (flags & ZFRAME_DONTWAIT)? ZMQ_DONTWAIT : 0;

    zmq_msg_t msg;
    zmq_msg_init_size (&msg, size);
    memcpy (zmq_msg_data (&msg), data, size);

    if (zmq_sendmsg (self, &msg, snd_flags) == -1) {
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
zsocket_signal (void *self)
{
    assert (self);
    return zstr_send (self, "");
}


//  --------------------------------------------------------------------------
//  Wait on a signal. Use this to coordinate between threads, over
//  pipe pairs. Blocks until the signal is received. Returns -1 on error,
//  0 on success.

int
zsocket_wait (void *self)
{
    assert (self);
    char *message = zstr_recv (self);
    if (message) {
        free (message);
        return 0;
    }
    else
        return -1;
}


#if (ZMQ_VERSION_MAJOR > 2)
//  --------------------------------------------------------------------------
//  Set socket high-water mark, emulating 2.x API

void
zsocket_set_hwm (void *self, int hwm)
{
    zsocket_set_sndhwm (self, hwm);
    zsocket_set_rcvhwm (self, hwm);
}
#endif

//  --------------------------------------------------------------------------
//  Selftest

void
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

    //  Test error state when connecting to an invalid socket type
    //  ('txp://' instead of 'tcp://', typo intentional)
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

    rc = zsocket_signal (writer);
    assert (rc == 0);
    rc = zsocket_wait (reader);
    assert (rc == 0);

    zsocket_destroy (ctx, reader);
    zsocket_destroy (ctx, writer);

    void *arr[7];
    int i3;
    for (i3 = 0; i3 < 7; i3++) {
       int rc3;
       arr[i3] = zsocket_new (ctx, ZMQ_REP);
       rc3 = zsocket_bind (arr[i3], "tcp://*:*[52000-52005]");
       if (i3 < 6)
           assert (rc3 == 52000+i3);
       else
           assert (rc3 == -1);
    }
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3,2,0))
    for (i3 = 0; i3 < 6; i3++) {
       int rc3 = zsocket_unbind (arr[i3], "tcp://*:%d", i3+52000);
       assert (rc3 == 0);
    }
    int   parr[7];
#endif
    for (i3 = 0; i3 < 7; i3++) {
       int rc3;
       arr[i3] = zsocket_new (ctx, ZMQ_REP);
       rc3 = zsocket_bind (arr[i3], "tcp://*:![53000-53005]");
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3,2,0))
       parr[i3] = rc3;
#endif
       if (i3 < 6)
           assert (rc3 <= 53005 && rc3 >=  53000);
       else
           assert (rc3 == -1);
    }
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3,2,0))
    for (i3 = 0; i3 < 6; i3++) {
       if (parr[i3] != -1) {
           int rc3 = zsocket_unbind (arr[i3], "tcp://*:%d", parr[i3]);
           assert (rc3 == 0);
       }
    }
#endif

    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
}
