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
    Some zsock methods take a void * "polymorphic" reference, which can be
    either a zsock_t or a zactor_t reference, or a libzmq void *.
@discuss
@end
*/

#define ZSOCK_NOCHECK // we are defining the methods here, so don't redirect symbols.

#include "czmq_classes.h"
#include "zsock_option.inc"

//  zsock_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZSOCK_TAG           0x0004cafe

//  This port range is defined by IANA for dynamic or private ports
//  We use this when choosing a port for dynamic binding.
#define DYNAMIC_FIRST       0xc000    // 49152
#define DYNAMIC_LAST        0xffff    // 65535

#define ZSOCK_BSEND_MAX_FRAMES 32  // Arbitrary limit, for now

//  Structure of our class

struct _zsock_t {
    uint32_t tag;               //  Object tag for runtime detection
    void *handle;               //  The libzmq socket handle
    char *endpoint;             //  Last bound endpoint, if any
    char *cache;                //  Holds last zsock_brecv strings
    int type;                   //  Socket type
    size_t cache_size;          //  Current size of cache
    uint32_t routing_id;        //  Routing ID for server sockets
};


//  --------------------------------------------------------------------------
//  Create a new socket. This macro passes the caller source and line
//  number so that CZMQ can report socket leaks intelligently. To switch
//  off this checking, which may be costly if you use a LOT of sockets,
//  define ZSOCK_NOCHECK before compiling your code. Returns the new
//  socket, or NULL if the new socket could not be created.

zsock_t *
zsock_new_checked (int type, const char *filename, size_t line_nbr)
{
    zsock_t *self = (zsock_t *) zmalloc (sizeof (zsock_t));
    assert (self);
    self->tag = ZSOCK_TAG;
    self->type = type;
    self->handle = zsys_socket (type, filename, line_nbr);
    assert (self->handle);
    return self;
}

zsock_t *
zsock_new (int type)
{
    return zsock_new_checked (type, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Destroy the socket. You must use this for any socket created via the
//  zsock_new method.

void
zsock_destroy_checked (zsock_t **self_p, const char *filename, size_t line_nbr)
{
    assert (self_p);
    if (*self_p) {
        zsock_t *self = *self_p;
        assert (zsock_is (self));
        self->tag = 0xDeadBeef;
        int rc = zsys_close (self->handle, filename, line_nbr);
        assert (rc == 0);
        free (self->endpoint);
        free (self->cache);
        free (self);
        *self_p = NULL;
    }
}

void zsock_destroy (zsock_t **self_p)
{
    zsock_destroy_checked (self_p, NULL, 0);
}


//  --------------------------------------------------------------------------
//  This interface includes some smart constructors, which create sockets with
//  additional set-up. In all of these, the endpoint is NULL, or starts with
//  '@' (bind) or '>' (connect). Multiple endpoints are allowed, separated by
//  commas. If endpoint does not start with '@' or '>', default action depends
//  on socket type.

//  Create a PUB socket. Default action is bind.

zsock_t *
zsock_new_pub_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_PUB, filename, line_nbr);
    if (zsock_attach (sock, endpoints, true))
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_pub (const char *endpoints)
{
    return zsock_new_pub_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a SUB socket, and optionally subscribe to some prefix string. Default
//  action is connect.

zsock_t *
zsock_new_sub_checked (const char *endpoints, const char *subscribe, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_SUB, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false) == 0) {
        if (subscribe)
            zsock_set_subscribe (sock, subscribe);
    }
    else
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_sub (const char *endpoints, const char *subscribe)
{
    return zsock_new_sub_checked (endpoints, subscribe, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a REQ socket. Default action is connect.

zsock_t *
zsock_new_req_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_REQ, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_req (const char *endpoints)
{
    return zsock_new_req_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a REP socket. Default action is bind.

zsock_t *
zsock_new_rep_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_REP, filename, line_nbr);
    if (zsock_attach (sock, endpoints, true))
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_rep (const char *endpoints)
{
    return zsock_new_rep_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a DEALER socket. Default action is connect.

zsock_t *
zsock_new_dealer_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_DEALER, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_dealer (const char *endpoints)
{
    return zsock_new_dealer_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a ROUTER socket. Default action is bind.

zsock_t *
zsock_new_router_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_ROUTER, filename, line_nbr);
    if (zsock_attach (sock, endpoints, true))
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_router (const char *endpoints)
{
    return zsock_new_router_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a PUSH socket. Default action is connect.

zsock_t *
zsock_new_push_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_PUSH, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_push (const char *endpoints)
{
    return zsock_new_push_checked(endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a PULL socket. Default action is bind.

zsock_t *
zsock_new_pull_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_PULL, filename, line_nbr);
    if (zsock_attach (sock, endpoints, true))
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_pull (const char *endpoints)
{
    return zsock_new_pull_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create an XPUB socket. Default action is bind.

zsock_t *
zsock_new_xpub_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_XPUB
    zsock_t *sock = zsock_new_checked (ZMQ_XPUB, filename, line_nbr);
    if (zsock_attach (sock, endpoints, true))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;            //  Not implemented
#endif
}

zsock_t *
zsock_new_xpub (const char *endpoints)
{
    return zsock_new_xpub_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create an XSUB socket. Default action is connect.

zsock_t *
zsock_new_xsub_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_XSUB
    zsock_t *sock = zsock_new_checked (ZMQ_XSUB, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;            //  Not implemented
#endif
}

zsock_t *
zsock_new_xsub (const char *endpoints)
{
    return zsock_new_xsub_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a PAIR socket. Default action is connect.

zsock_t *
zsock_new_pair_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
    zsock_t *sock = zsock_new_checked (ZMQ_PAIR, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
}

zsock_t *
zsock_new_pair (const char *endpoints)
{
    return zsock_new_pair_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a STREAM socket. Default action is connect.

zsock_t *
zsock_new_stream_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_STREAM
    zsock_t *sock = zsock_new_checked (ZMQ_STREAM, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;            //  Not implemented
#endif
}

zsock_t *
zsock_new_stream (const char *endpoints)
{
    return zsock_new_stream_checked (endpoints, NULL, 0);
}

//  --------------------------------------------------------------------------
//  Create a SERVER  socket. Default action is bind.

zsock_t *
zsock_new_server_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_SERVER
    zsock_t *sock = zsock_new_checked (ZMQ_SERVER, filename, line_nbr);
    if (zsock_attach (sock, endpoints, true))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;
#endif
}

zsock_t *
zsock_new_server (const char *endpoints)
{
    return zsock_new_server_checked (endpoints, NULL, 0);
}

//  --------------------------------------------------------------------------
//  Create a CLIENT socket. Default action is connect.

zsock_t *
zsock_new_client_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_CLIENT
    zsock_t *sock = zsock_new_checked (ZMQ_CLIENT, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;
#endif
}

zsock_t *
zsock_new_client (const char *endpoints)
{
    return zsock_new_client_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a RADIO socket. Default action is bind.

zsock_t *
zsock_new_radio_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_RADIO
    zsock_t *sock = zsock_new_checked (ZMQ_RADIO, filename, line_nbr);
    if (zsock_attach (sock, endpoints, true))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;
#endif
}

zsock_t *
zsock_new_radio (const char *endpoints)
{
    return zsock_new_radio_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a DISH socket. Default action is connect.

zsock_t *
zsock_new_dish_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_DISH
    zsock_t *sock = zsock_new_checked (ZMQ_DISH, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;
#endif
}

zsock_t *
zsock_new_dish (const char *endpoints)
{
    return zsock_new_dish_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a GATHER socket. Default action is bind.

zsock_t *
zsock_new_gather_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_GATHER
    zsock_t *sock = zsock_new_checked (ZMQ_GATHER, filename, line_nbr);
    if (zsock_attach (sock, endpoints, true))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;
#endif
}

zsock_t *
zsock_new_gather (const char *endpoints)
{
    return zsock_new_gather_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Create a scatter socket. Default action is connect.

zsock_t *
zsock_new_scatter_checked (const char *endpoints, const char *filename, size_t line_nbr)
{
#if defined ZMQ_SCATTER
    zsock_t *sock = zsock_new_checked (ZMQ_SCATTER, filename, line_nbr);
    if (zsock_attach (sock, endpoints, false))
        zsock_destroy (&sock);
    return sock;
#else
    return NULL;
#endif
}

zsock_t *
zsock_new_scatter (const char *endpoints)
{
    return zsock_new_scatter_checked (endpoints, NULL, 0);
}


//  --------------------------------------------------------------------------
//  Bind a socket to a formatted endpoint. For tcp:// endpoints, supports
//  ephemeral ports, if you specify the port number as "*". By default
//  zsock uses the IANA designated range from C000 (49152) to FFFF (65535).
//  To override this range, follow the "*" with "[first-last]". Either or
//  both first and last may be empty. To bind to a random port within the
//  range, use "!" in place of "*".
//
//  Examples:
//  tcp://127.0.0.1:*                bind to first free port from C000 up
//  tcp://127.0.0.1:!                bind to random port from C000 to FFFF
//  tcp://127.0.0.1:*[60000-]        bind to first free port from 60000 up
//  tcp://127.0.0.1:![-60000]        bind to random port from C000 to 60000
//  tcp://127.0.0.1:![55000-55999]   bind to random port from 55000-55999
//
//  On success, returns the actual port number used, for tcp:// endpoints,
//  and 0 for other transports. On failure, returns -1. Note that when using
//  ephemeral ports, a port may be reused by different services without
//  clients being aware. Protocols that run on ephemeral ports should take
//  this into account.

int
zsock_bind (zsock_t *self, const char *format, ...)
{
    assert (self);
    assert (zsock_is (self));

    //  Expand format to get full endpoint
    va_list argptr;
    va_start (argptr, format);
    char *endpoint = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (!endpoint)
        return -1;
    int rc;

    //  If tcp:// endpoint, parse to get or make port number
    zrex_t *rex = zrex_new (NULL);
    if (zrex_eq (rex, endpoint, "^tcp://.*:(\\d+)$")) {
        assert (zrex_hits (rex) == 2);
        int port = atoi (zrex_hit (rex, 1));
#if defined (HAVE_LIBSYSTEMD) && (ZMQ_VERSION >= ZMQ_MAKE_VERSION (4, 2, 0))
        if (zsys_auto_use_fd ()) {
            int last_handle = SD_LISTEN_FDS_START + sd_listen_fds (0);
            int handle;
            for (handle = SD_LISTEN_FDS_START; handle < last_handle; ++handle)
                if (sd_is_socket_inet (handle, AF_UNSPEC, SOCK_STREAM, 1,
                        (uint16_t) port) > 0) {
                    zsock_set_use_fd (self, handle);
                    break;
                }
        }
#endif
        if (zmq_bind (self->handle, endpoint) == 0)
            rc = port;
        else
            rc = -1;
    }
    else
    if (zrex_eq (rex, endpoint, "^(tcp://.*):([*!])(\\[(\\d+)?-(\\d+)?\\])?$")) {
        assert (zrex_hits (rex) == 6);
        const char *hostname, *opcode, *group, *first_str, *last_str;
        zrex_fetch (rex, &hostname, &opcode, &group, &first_str, &last_str, NULL);

        int first = *first_str? atoi (first_str): DYNAMIC_FIRST;
        int last = *last_str? atoi (last_str): DYNAMIC_LAST;

        //  This is how many times we'll try before giving up
        int attempts = last - first + 1;

        //  If operator is '*', take first available port.
        //  If operator is '!', take a random leap into our port space; we'll
        //  still scan sequentially to make sure we find a free slot rapidly.
        int port = first;
        if (streq (opcode, "!"))
            port += randof (attempts);

        rc = -1;                //  Assume we don't succeed
        while (rc == -1 && attempts--) {
            zstr_free (&endpoint);
            endpoint = zsys_sprintf ("%s:%d", hostname, port);
            if (!endpoint)
                break;
            if (zmq_bind (self->handle, endpoint) == 0)
                rc = port;
            if (++port > last)
                port = first;
        }
    }
    else {
#if defined (HAVE_LIBSYSTEMD) && (ZMQ_VERSION >= ZMQ_MAKE_VERSION (4, 2, 0))
        if (zsys_auto_use_fd () && zrex_eq (rex, endpoint, "^ipc://(.*)$")) {
            assert (zrex_hits (rex) == 2);
            const char *sock_path;
            zrex_fetch (rex, &sock_path, NULL);

            int last_handle = SD_LISTEN_FDS_START + sd_listen_fds (0);
            int handle;
            for (handle = SD_LISTEN_FDS_START; handle < last_handle; ++handle)
                if (sd_is_socket_unix (handle, SOCK_STREAM, 1, sock_path, 0) > 0) {
                    zsock_set_use_fd (self, handle);
                    break;
                }
        }
#endif
        rc = zmq_bind (self->handle, endpoint);
    }

    //  Store successful endpoint for later reference
    if (rc >= 0) {
        zstr_free (&self->endpoint);
        self->endpoint = endpoint;
    }
    else
        zstr_free (&endpoint);

    zrex_destroy (&rex);
    return rc;
}


//  --------------------------------------------------------------------------
//  Returns last bound endpoint, if any.

const char *
zsock_endpoint (zsock_t *self)
{
    assert (self);
    return self->endpoint;
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

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 2, 0))
    //  Expand format to get full endpoint
    va_list argptr;
    va_start (argptr, format);
    char *endpoint = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (!endpoint)
        return -1;

    int rc = zmq_unbind (self->handle, endpoint);
    zstr_free (&endpoint);
    return rc;
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

    //  Expand format to get full endpoint
    va_list argptr;
    va_start (argptr, format);
    char *endpoint = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (!endpoint)
        return -1;
    int rc = zmq_connect (self->handle, endpoint);

#if (ZMQ_VERSION < ZMQ_MAKE_VERSION (4, 0, 0))
    int retries = 4;
    while (rc == -1 && zmq_errno () == ECONNREFUSED && retries) {
        //  This bruteforces a synchronization between connecting and
        //  binding threads on ZeroMQ v3.2 and earlier, where the bind
        //  MUST happen before the connect on inproc transports.
        zclock_sleep (250);
        rc = zmq_connect (self->handle, endpoint);
        retries--;
    }
#endif
    zstr_free (&endpoint);
    return rc;
}


//  --------------------------------------------------------------------------
//  Disconnect a socket from a formatted endpoint
//  Returns 0 if disconnection is complete -1 if the disconnection failed.

int
zsock_disconnect (zsock_t *self, const char *format, ...)
{
    assert (self);
    assert (zsock_is (self));

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 2, 0))
    //  Expand format to get full endpoint
    va_list argptr;
    va_start (argptr, format);
    char *endpoint = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (!endpoint)
        return -1;
    int rc = zmq_disconnect (self->handle, endpoint);
    zstr_free (&endpoint);
    return rc;
#else
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Attach a socket to zero or more endpoints. If endpoints is not null,
//  parses as list of ZeroMQ endpoints, separated by commas, and prefixed by
//  '@' (to bind the socket) or '>' (to attach the socket). Returns 0 if all
//  endpoints were valid, or -1 if there was a syntax error. If the endpoint
//  does not start with '@' or '>', the serverish argument defines whether
//  it is used to bind (serverish = true) or connect (serverish = false).

int
zsock_attach (zsock_t *self, const char *endpoints, bool serverish)
{
    assert (self);
    if (!endpoints)
        return 0;

    //  We hold each individual endpoint here
    char endpoint [256];
    while (*endpoints) {
        const char *delimiter = strchr (endpoints, ',');
        if (!delimiter)
            delimiter = endpoints + strlen (endpoints);
        if (delimiter - endpoints > 255)
            return -1;
        memcpy (endpoint, endpoints, delimiter - endpoints);
        endpoint [delimiter - endpoints] = 0;

        int rc;
        if (endpoint [0] == '@')
            rc = zsock_bind (self, "%s", endpoint + 1);
        else
        if (endpoint [0] == '>')
            rc = zsock_connect (self, "%s", endpoint + 1);
        else
        if (serverish)
            rc = zsock_bind (self, "%s", endpoint);
        else
            rc = zsock_connect (self, "%s", endpoint);

        if (rc == -1)
            return -1;          //  Bad endpoint syntax

        if (*delimiter == 0)
            break;
        endpoints = delimiter + 1;
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Returns socket type as printable constant string.

const char *
zsock_type_str (zsock_t *self)
{
    assert (self);
    return zsys_sockname (self->type);
}


//  --------------------------------------------------------------------------
//  Send a 'picture' message to the socket (or actor). The picture is a
//  string that defines the type of each frame. This makes it easy to send
//  a complex multiframe message in one call. The picture can contain any
//  of these characters, each corresponding to one or two arguments:
//
//      i = int (signed)
//      1 = uint8_t
//      2 = uint16_t
//      4 = uint32_t
//      8 = uint64_t
//      s = char *
//      b = byte *, int (2 arguments)
//      c = zchunk_t *
//      f = zframe_t *
//      h = zhashx_t *
//      U = zuuid_t *
//      p = void * (sends the pointer value, only meaningful over inproc)
//      m = zmsg_t * (sends all frames in the zmsg)
//      z = sends zero-sized frame (0 arguments)
//      u = uint (deprecated)
//
//  Note that s, b, c, and f are encoded the same way and the choice is
//  offered as a convenience to the sender, which may or may not already
//  have data in a zchunk or zframe. Does not change or take ownership of
//  any arguments. Returns 0 if successful, -1 if sending failed for any
//  reason.

int
zsock_send (void *self, const char *picture, ...)
{
    va_list argptr;
    va_start (argptr, picture);
    int rc = zsock_vsend (self, picture, argptr);
    va_end (argptr);
    return rc;
}


//  --------------------------------------------------------------------------
//  Send a 'picture' message to the socket (or actor). This is a va_list
//  version of zsock_send (), so please consult its documentation for the
//  details.

int
zsock_vsend (void *self, const char *picture, va_list argptr)
{
    assert (self);
    assert (picture);

    zmsg_t *msg = zmsg_new ();
    while (*picture) {
        if (*picture == 'i')
            zmsg_addstrf (msg, "%d", va_arg (argptr, int));
        else
        if (*picture == '1')
            zmsg_addstrf (msg, "%" PRIu8, (uint8_t) va_arg (argptr, int));
        else
        if (*picture == '2')
            zmsg_addstrf (msg, "%" PRIu16, (uint16_t) va_arg (argptr, int));
        else
        if (*picture == '4')
            zmsg_addstrf (msg, "%" PRIu32, va_arg (argptr, uint32_t));
        else
        if (*picture == '8')
            zmsg_addstrf (msg, "%" PRIu64, va_arg (argptr, uint64_t));
        else
        if (*picture == 'u')    //  Deprecated, use 4 or 8 instead
            zmsg_addstrf (msg, "%ud", va_arg (argptr, uint));
        else
        if (*picture == 's')
            zmsg_addstr (msg, va_arg (argptr, char *));
        else
        if (*picture == 'b') {
            //  Note function arguments may be expanded in reverse order,
            //  so we cannot use va_arg macro twice in a single call
            byte *data = va_arg (argptr, byte *);
            zmsg_addmem (msg, data, va_arg (argptr, int));
        }
        else
        if (*picture == 'c') {
            zchunk_t *chunk = va_arg (argptr, zchunk_t *);
            assert (zchunk_is (chunk));
            zmsg_addmem (msg, zchunk_data (chunk), zchunk_size (chunk));
        }
        else
        if (*picture == 'f') {
            zframe_t *frame = va_arg (argptr, zframe_t *);
            assert (zframe_is (frame));
            zmsg_addmem (msg, zframe_data (frame), zframe_size (frame));
        }
        else
        if (*picture == 'U') {
            zuuid_t *uuid = va_arg (argptr, zuuid_t *);
            zmsg_addmem (msg, zuuid_data (uuid), zuuid_size (uuid));
        }
        else
        if (*picture == 'p') {
            void *pointer = va_arg (argptr, void *);
            zmsg_addmem (msg, &pointer, sizeof (void *));
        }
        else
        if (*picture == 'h') {
            zhashx_t *hash = va_arg (argptr, zhashx_t *);
            zframe_t *frame = zhashx_pack (hash);
            zmsg_append (msg, &frame);
        }
        else
        if (*picture == 'm') {
            zframe_t *frame;
            zmsg_t *zmsg = va_arg (argptr, zmsg_t *);
            for (frame = zmsg_first (zmsg); frame;
                 frame = zmsg_next (zmsg) ) {
                zframe_t *frame_dup = zframe_dup (frame);
                zmsg_append (msg, &frame_dup);
            }
        }
        else
        if (*picture == 'z')
            zmsg_addmem (msg, NULL, 0);
        else {
            zsys_error ("zsock: invalid picture element '%c'", *picture);
            assert (false);
        }
        picture++;
    }

    int rc = zmsg_send (&msg, self);
    if (rc != 0)
        zmsg_destroy (&msg);

    return rc;
}


//  --------------------------------------------------------------------------
//  Receive a 'picture' message to the socket (or actor). See zsock_send for
//  the format and meaning of the picture. Returns the picture elements into
//  a series of pointers as provided by the caller:
//
//      i = int * (stores signed integer)
//      1 = uint8_t * (stores 8-bit unsigned integer)
//      2 = uint16_t * (stores 16-bit unsigned integer)
//      4 = uint32_t * (stores 32-bit unsigned integer)
//      8 = uint64_t * (stores 64-bit unsigned integer)
//      s = char ** (allocates new string)
//      b = byte **, size_t * (2 arguments) (allocates memory)
//      c = zchunk_t ** (creates zchunk)
//      f = zframe_t ** (creates zframe)
//      h = zhashx_t ** (creates zhashx)
//      U = zuuid_t * (creates a zuuid with the data)
//      p = void ** (stores pointer)
//      m = zmsg_t ** (creates a zmsg with the remaing frames)
//      z = null, asserts empty frame (0 arguments)
//      u = uint * (stores unsigned integer, deprecated)
//
//  Note that zsock_recv creates the returned objects, and the caller must
//  destroy them when finished with them. The supplied pointers do not need
//  to be initialized. Returns 0 if successful, or -1 if it failed to recv
//  a message, in which case the pointers are not modified. When message
//  frames are truncated (a short message), sets return values to zero/null.
//  If an argument pointer is NULL, does not store any value (skips it).
//  An 'n' picture matches an empty frame; if the message does not match,
//  the method will return -1.

int
zsock_recv (void *self, const char *picture, ...)
{
    va_list argptr;
    va_start (argptr, picture);
    int rc = zsock_vrecv (self, picture, argptr);
    va_end (argptr);
    return rc;
}


//  Receive a 'picture' message from the socket (or actor). This is a
//  va_list version of zsock_recv (), so please consult its documentation
//  for the details.

int
zsock_vrecv (void *self, const char *picture, va_list argptr)
{
    assert (self);
    assert (picture);
    zmsg_t *msg = zmsg_recv (self);
    if (!msg)
        return -1;              //  Interrupted

    //  Filter a signal that may come from a dying actor
    if (zmsg_signal (msg) >= 0) {
        zmsg_destroy (&msg);
        return -1;
    }
    //  Now parse message according to picture argument
    int rc = 0;
    while (*picture) {
        if (*picture == 'i') {
            char *string = zmsg_popstr (msg);
            int *int_p = va_arg (argptr, int *);
            if (int_p)
                *int_p = string? atoi (string): 0;
            free (string);
        }
        else
        if (*picture == '1') {
            char *string = zmsg_popstr (msg);
            uint8_t *uint8_p = va_arg (argptr, uint8_t *);
            if (uint8_p)
                *uint8_p = string? (uint8_t) atoi (string): 0;
            free (string);
        }
        else
        if (*picture == '2') {
            char *string = zmsg_popstr (msg);
            uint16_t *uint16_p = va_arg (argptr, uint16_t *);
            if (uint16_p)
                *uint16_p = string? (uint16_t) atol (string): 0;
            free (string);
        }
        else
        if (*picture == '4') {
            char *string = zmsg_popstr (msg);
            uint32_t *uint32_p = va_arg (argptr, uint32_t *);
            if (uint32_p)
                *uint32_p = string? (uint32_t) strtoul (string, NULL, 10): 0;
            free (string);
        }
        else
        if (*picture == '8') {
            char *string = zmsg_popstr (msg);
            uint64_t *uint64_p = va_arg (argptr, uint64_t *);
            if (uint64_p)
                *uint64_p = string? (uint64_t) strtoull (string, NULL, 10): 0;
            free (string);
        }
        else
        if (*picture == 'u') {  //  Deprecated, use 4 or 8 instead
            char *string = zmsg_popstr (msg);
            uint *uint_p = va_arg (argptr, uint *);
            if (uint_p)
                *uint_p = string? (uint) strtoul (string, NULL, 10): 0;
            free (string);
        }
        else
        if (*picture == 's') {
            char *string = zmsg_popstr (msg);
            char **string_p = va_arg (argptr, char **);
            if (string_p)
                *string_p = string;
            else
                free (string);
        }
        else
        if (*picture == 'b') {
            zframe_t *frame = zmsg_pop (msg);
            byte **data_p = va_arg (argptr, byte **);
            size_t *size = va_arg (argptr, size_t *);
            if (data_p) {
                if (frame) {
                    *size = zframe_size (frame);
                    *data_p = (byte *) malloc (*size);
                    memcpy (*data_p, zframe_data (frame), *size);
                }
                else {
                    *data_p = NULL;
                    *size = 0;
                }
            }
            zframe_destroy (&frame);
        }
        else
        if (*picture == 'c') {
            zframe_t *frame = zmsg_pop (msg);
            zchunk_t **chunk_p = va_arg (argptr, zchunk_t **);
            if (chunk_p) {
                if (frame)
                    *chunk_p = zchunk_new (zframe_data (frame), zframe_size (frame));
                else
                    *chunk_p = NULL;
            }
            zframe_destroy (&frame);
        }
        else
        if (*picture == 'f') {
            zframe_t *frame = zmsg_pop (msg);
            zframe_t **frame_p = va_arg (argptr, zframe_t **);
            if (frame_p)
                *frame_p = frame;
            else
                zframe_destroy (&frame);
        }
        else
        if (*picture == 'U') {
            zframe_t *frame = zmsg_pop (msg);
            zuuid_t **uuid_p = va_arg (argptr, zuuid_t **);
            if (uuid_p) {
                if (frame) {
                    *uuid_p = zuuid_new ();
                    zuuid_set (*uuid_p, zframe_data (frame));
                }
                else
                    *uuid_p = NULL;
            }
            zframe_destroy (&frame);
        }
        else
        if (*picture == 'p') {
            zframe_t *frame = zmsg_pop (msg);
            void **pointer_p = va_arg (argptr, void **);
            if (pointer_p) {
                if (frame) {
                    if (zframe_size (frame) == sizeof (void *))
                        *pointer_p = *((void **) zframe_data (frame));
                    else
                        rc = -1;
                }
                else
                    *pointer_p = NULL;
            }
            zframe_destroy (&frame);
        }
        else
        if (*picture == 'h') {
            zframe_t *frame = zmsg_pop (msg);
            zhashx_t **hash_p = va_arg (argptr, zhashx_t **);
            if (hash_p) {
                if (frame)
                    *hash_p = zhashx_unpack (frame);
                else
                    *hash_p = NULL;
            }
            zframe_destroy (&frame);
        }
        else
        if (*picture == 'm') {
            zmsg_t **zmsg_p = va_arg (argptr, zmsg_t **);
            if (zmsg_p) {
                *zmsg_p = zmsg_new ();
                zframe_t *frame;
                while ((frame = zmsg_pop (msg)))
                    zmsg_append (*zmsg_p, &frame);
            }
        }
        else
        if (*picture == 'z') {
            zframe_t *frame = zmsg_pop (msg);
            if (frame && zframe_size (frame) != 0)
                rc = -1;
            zframe_destroy (&frame);
        }
        else {
            zsys_error ("zsock: invalid picture element '%c'", *picture);
            assert (false);
        }
        picture++;
    }
    zmsg_destroy (&msg);
    return rc;
}


//  --------------------------------------------------------------------------
//  Network data encoding macros that we use in bsend/brecv

//  Put a 1-byte number to the frame
#define PUT_NUMBER1(host) { \
    *(byte *) needle = (host); \
    needle++; \
}

//  Put a 2-byte number to the frame
#define PUT_NUMBER2(host) { \
    needle [0] = (byte) (((host) >> 8)  & 255); \
    needle [1] = (byte) (((host))       & 255); \
    needle += 2; \
}

//  Put a 4-byte number to the frame
#define PUT_NUMBER4(host) { \
    needle [0] = (byte) (((host) >> 24) & 255); \
    needle [1] = (byte) (((host) >> 16) & 255); \
    needle [2] = (byte) (((host) >> 8)  & 255); \
    needle [3] = (byte) (((host))       & 255); \
    needle += 4; \
}

//  Put a 8-byte number to the frame
#define PUT_NUMBER8(host) { \
    needle [0] = (byte) (((host) >> 56) & 255); \
    needle [1] = (byte) (((host) >> 48) & 255); \
    needle [2] = (byte) (((host) >> 40) & 255); \
    needle [3] = (byte) (((host) >> 32) & 255); \
    needle [4] = (byte) (((host) >> 24) & 255); \
    needle [5] = (byte) (((host) >> 16) & 255); \
    needle [6] = (byte) (((host) >> 8)  & 255); \
    needle [7] = (byte) (((host))       & 255); \
    needle += 8; \
}

//  Get a 1-byte number from the frame
#define GET_NUMBER1(host) { \
    if (needle + 1 > ceiling) \
        goto malformed; \
    (host) = *(byte *) needle; \
    needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (needle + 2 > ceiling) \
        goto malformed; \
    (host) = ((uint16_t) (needle [0]) << 8) \
           +  (uint16_t) (needle [1]); \
    needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (needle + 4 > ceiling) \
        goto malformed; \
    (host) = ((uint32_t) (needle [0]) << 24) \
           + ((uint32_t) (needle [1]) << 16) \
           + ((uint32_t) (needle [2]) << 8) \
           +  (uint32_t) (needle [3]); \
    needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (needle + 8 > ceiling) \
        goto malformed; \
    (host) = ((uint64_t) (needle [0]) << 56) \
           + ((uint64_t) (needle [1]) << 48) \
           + ((uint64_t) (needle [2]) << 40) \
           + ((uint64_t) (needle [3]) << 32) \
           + ((uint64_t) (needle [4]) << 24) \
           + ((uint64_t) (needle [5]) << 16) \
           + ((uint64_t) (needle [6]) << 8) \
           +  (uint64_t) (needle [7]); \
    needle += 8; \
}


//  --------------------------------------------------------------------------
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
//      u       zuuid_t *               type = "uuid"
//      m       zmsg_t *                type = "msg"
//      p       void *, sends pointer value, only over inproc
//
//  Does not change or take ownership of any arguments. Returns 0 if
//  successful, -1 if sending failed for any reason.

int
zsock_bsend (void *self, const char *picture, ...)
{
    assert (self);
    assert (picture);

    //  Pass 1: calculate total size of data frame
    size_t frame_size = 0;
    zframe_t *frames [ZSOCK_BSEND_MAX_FRAMES]; //  Non-data frames to send
    size_t nbr_frames = 0;                     //  Size of this table
    va_list argptr;
    va_start (argptr, picture);
    const char *picptr = picture;
    while (*picptr) {
        if (*picptr == '1') {
            va_arg (argptr, int);
            frame_size += 1;
        }
        else
        if (*picptr == '2') {
            va_arg (argptr, int);
            frame_size += 2;
        }
        else
        if (*picptr == '4') {
            va_arg (argptr, uint32_t);
            frame_size += 4;
        }
        else
        if (*picptr == '8') {
            va_arg (argptr, uint64_t);
            frame_size += 8;
        }
        else
        if (*picptr == 's') {
            char *string = va_arg (argptr, char *);
            frame_size += 1 + (string? strlen (string): 0);
        }
        else
        if (*picptr == 'S') {
            char *string = va_arg (argptr, char *);
            frame_size += 4 + (string? strlen (string): 0);
        }
        else
        if (*picptr == 'c') {
            zchunk_t *chunk = va_arg (argptr, zchunk_t *);
            frame_size += 4 + (chunk? zchunk_size (chunk): 0);
        }
        else
        if (*picptr == 'p') {
            va_arg (argptr, void *);
            frame_size += sizeof (void *);
        }
        else
        if (*picptr == 'f') {
            zframe_t *frame = va_arg (argptr, zframe_t *);
            assert (nbr_frames < ZSOCK_BSEND_MAX_FRAMES - 1);
            frames [nbr_frames++] = frame;
        }
        else
        if (*picptr == 'u') {
            zuuid_t *uuid = va_arg (argptr, zuuid_t *);
            frame_size += zuuid_size (uuid);
        }
        else
        if (*picptr == 'm') {
            if (picptr [1]) {
                zsys_error ("zsock_bsend: 'm' (zmsg) only valid at end of picptr");
                assert (false);
            }
            zmsg_t *msg = va_arg (argptr, zmsg_t *);
            if (msg) {
                zframe_t *frame = zmsg_first (msg);
                while (frame) {
                    assert (nbr_frames < ZSOCK_BSEND_MAX_FRAMES - 1);
                    frames [nbr_frames++] = frame;
                    frame = zmsg_next (msg);
                }
            }
            else
                frames [nbr_frames++] = zframe_new_empty ();
        }
        else {
            zsys_error ("zsock_bsend: invalid picptr element '%c'", *picptr);
            assert (false);
        }
        picptr++;
    }
    va_end (argptr);

    //  Pass 2: encode data into data frame
    zmq_msg_t msg;
    zmq_msg_init_size (&msg, frame_size);
    byte *needle = (byte *) zmq_msg_data (&msg);

    //  Set routing id if self is zsock
#if defined ZMQ_SERVER
    if (zsock_is (self) && zsock_routing_id ((zsock_t *)self) != 0)
        zmq_msg_set_routing_id (&msg, zsock_routing_id ((zsock_t *)self));
#endif

    va_start (argptr, picture);
    picptr = picture;
    while (*picptr) {
        if (*picptr == '1') {
            int number1 = va_arg (argptr, int);
            PUT_NUMBER1 (number1);
        }
        else
        if (*picptr == '2') {
            int number2 = va_arg (argptr, int);
            PUT_NUMBER2 (number2);
        }
        else
        if (*picptr == '4') {
            uint32_t number4 = va_arg (argptr, uint32_t);
            PUT_NUMBER4 (number4);
        }
        else
        if (*picptr == '8') {
            uint64_t number8 = va_arg (argptr, uint64_t);
            PUT_NUMBER8 (number8);
        }
        else
        if (*picptr == 'p') {
            void *pointer = va_arg (argptr, void *);
            memcpy (needle, &pointer, sizeof (void *));
            needle += sizeof (void *);
        }
        else
        if (*picptr == 's') {
            char *string = va_arg (argptr, char *);
            if (!string)
                string = "";
            size_t string_size = strlen (string);
            PUT_NUMBER1 ((byte) string_size);
            memcpy (needle, string, string_size);
            needle += string_size;
        }
        else
        if (*picptr == 'S') {
            char *string = va_arg (argptr, char *);
            if (!string)
                string = "";
            size_t string_size = strlen (string);
            PUT_NUMBER4 (string_size);
            memcpy (needle, string, string_size);
            needle += string_size;
        }
        else
        if (*picptr == 'c') {
            zchunk_t *chunk = va_arg (argptr, zchunk_t *);
            if (chunk) {
                PUT_NUMBER4 (zchunk_size (chunk));
                memcpy (needle, zchunk_data (chunk), zchunk_size (chunk));
                needle += zchunk_size (chunk);
            }
        }
        else
        if (*picptr == 'u') {
            zuuid_t *uuid = va_arg (argptr, zuuid_t *);
            if (uuid) {
                memcpy (needle, zuuid_data (uuid), zuuid_size (uuid));
                needle += zuuid_size (uuid);
            }
        }
        picptr++;
    }
    va_end (argptr);

    //  Now send the data frame
    void *handle = zsock_resolve (self);
    int rc = zmq_msg_send (&msg, handle, nbr_frames? ZMQ_SNDMORE: 0);
    if (rc >= 0) {
        //  Now send any additional frames
        unsigned int frame_nbr;
        for (frame_nbr = 0; frame_nbr < nbr_frames; frame_nbr++) {
            bool more = frame_nbr < nbr_frames - 1;
            rc = zframe_send (&frames [frame_nbr], self,
                              ZFRAME_REUSE + (more? ZFRAME_MORE: 0));
            if (rc == -1)
                break;
        }
    }
    return rc >= 0? 0: -1;
}


//  --------------------------------------------------------------------------
//  Receive a binary encoded 'picture' message from the socket (or actor).
//  This method is similar to zsock_recv, except the arguments are encoded
//  in a binary format that is compatible with zproto, and is designed to
//  reduce memory allocations. The pattern argument is a string that defines
//  the type of each argument. See zsock_bsend for the supported argument
//  types. All arguments must be pointers; this call sets them to point to
//  values held on a per-socket basis.
//  Note that zsock_brecv creates the returned objects, and the caller must
//  destroy them when finished with them. The supplied pointers do not need
//  to be initialized. Returns 0 if successful, or -1 if it failed to read
//  a message.

//  This is the largest size we allow for an incoming longstr or chunk (1M)
#define MAX_ALLOC_SIZE      1024 * 1024

int
zsock_brecv (void *selfish, const char *picture, ...)
{
    assert (selfish);           //  Kind of self
    assert (picture);

    zmq_msg_t msg;
    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, zsock_resolve (selfish), 0) == -1)
        return -1;              //  Interrupted

    //  If we don't have a string cache, create one now with arbitrary
    //  value; this will grow if needed. Do not use an initial size less
    //  than 256, or cache expansion will not work properly.
    zsock_t *self = (zsock_t *) selfish;
    if (zactor_is (selfish))
        self = zactor_sock ((zactor_t *) selfish);

    if (!self->cache) {
        self->cache = (char *) malloc (512);
        self->cache_size = 512;
    }
    //  Last received strings are cached per socket
    uint cache_used = 0;
    byte *needle = (byte *) zmq_msg_data (&msg);
    byte *ceiling = needle + zmq_msg_size (&msg);

    //  If selfish is zsock get routing id from msg
#if defined ZMQ_SERVER
    if (zsock_is (selfish) && zsock_type (self) == ZMQ_SERVER)
        zsock_set_routing_id (self, zmq_msg_routing_id (&msg));
#endif

    va_list argptr;
    va_start (argptr, picture);
    const char *picptr = picture;
    while (*picptr) {
        if (*picptr == '1') {
            uint8_t *number1_p = va_arg (argptr, uint8_t *);
            GET_NUMBER1 (*number1_p);
        }
        else
        if (*picptr == '2') {
            uint16_t *number2_p = va_arg (argptr, uint16_t *);
            GET_NUMBER2 (*number2_p);
        }
        else
        if (*picptr == '4') {
            uint32_t *number4_p = va_arg (argptr, uint32_t *);
            GET_NUMBER4 (*number4_p);
        }
        else
        if (*picptr == '8') {
            uint64_t *number8_p = va_arg (argptr, uint64_t *);
            GET_NUMBER8 (*number8_p);
        }
        else
        if (*picptr == 'p') {
            void **pointer_p = va_arg (argptr, void **);
            memcpy (pointer_p, needle, sizeof (void *));
            needle += sizeof (void *);
        }
        else
        if (*picptr == 's') {
            char **string_p = va_arg (argptr, char **);
            uint string_size;
            GET_NUMBER1 (string_size);
            if (needle + string_size > ceiling)
                goto malformed;
            //  Expand cache if we need to; string is guaranteed to fit into
            //  expansion space
            if (cache_used + string_size > self->cache_size) {
                puts ("REALLOC");
                self->cache_size *= 2;
                self->cache = (char *) realloc (self->cache, self->cache_size);
                assert (self->cache);
            }
            *string_p = self->cache + cache_used;
            memcpy (*string_p, needle, string_size);
            cache_used += string_size;
            self->cache [cache_used++] = 0;
            needle += string_size;
        }
        else
        if (*picptr == 'S') {
            char **string_p = va_arg (argptr, char **);
            size_t string_size;
            GET_NUMBER4 (string_size);
            if (string_size > MAX_ALLOC_SIZE
            ||  needle + string_size > (ceiling))
                goto malformed;
            *string_p = (char *) malloc (string_size + 1);
            assert (string_p);
            memcpy (*string_p, needle, string_size);
            (*string_p) [string_size] = 0;
            needle += string_size;
        }
        else
        if (*picptr == 'c') {
            zchunk_t **chunk_p = va_arg (argptr, zchunk_t **);
            size_t chunk_size;
            GET_NUMBER4 (chunk_size);
            if (chunk_size > MAX_ALLOC_SIZE
            ||  needle + chunk_size > (ceiling))
                goto malformed;
            *chunk_p = zchunk_new (needle, chunk_size);
            needle += chunk_size;
        }
        else
        if (*picptr == 'u') {
            zuuid_t **uuid_p = va_arg (argptr, zuuid_t **);
            *uuid_p = zuuid_new ();
            if (zuuid_size (*uuid_p) > MAX_ALLOC_SIZE
            ||  needle + zuuid_size (*uuid_p) > (ceiling))
                goto malformed;
            zuuid_set (*uuid_p, needle);
            needle += zuuid_size (*uuid_p);
        }
        else
        if (*picptr == 'f') {
            zframe_t **frame_p = va_arg (argptr, zframe_t **);
            //  Get next frame off socket
            if (!zsock_rcvmore (self))
                goto malformed;
            *frame_p = zframe_recv (self);
        }
        else
        if (*picptr == 'm') {
            if (picptr [1]) {
                zsys_error ("zsock_brecv: 'm' (zmsg) only valid at end of picptr");
                assert (false);
            }
            zmsg_t **msg_p = va_arg (argptr, zmsg_t **);
            //  Get zero or more remaining frames
            if (!zsock_rcvmore (self))
                goto malformed;
            *msg_p = zmsg_recv (self);
        }
        else {
            zsys_error ("zsock_brecv: invalid picptr element '%c'", *picptr);
            assert (false);
        }
        picptr++;
    }
    va_end (argptr);
    zmq_msg_close (&msg);
    return 0;

    //  Error return
    malformed:
        zmq_msg_close (&msg);
        return -1;              //  Invalid message
}


//  --------------------------------------------------------------------------
//  Return socket routing ID if any. This returns 0 if the socket is not
//  of type ZMQ_SERVER or if no request was already received on it.

uint32_t
zsock_routing_id (zsock_t *self)
{
    return self->routing_id;
}


//  --------------------------------------------------------------------------
//  Set routing ID on socket. The socket MUST be of type ZMQ_SERVER.
//  This will be used when sending messages on the socket via the zsock API.

void
zsock_set_routing_id (zsock_t *self, uint32_t routing_id)
{
    assert (self);
    self->routing_id = routing_id;
}


//  --------------------------------------------------------------------------
//  Set socket to use unbounded pipes (HWM=0); use this in cases when you are
//  totally certain the message volume can fit in memory. This method works
//  across all versions of ZeroMQ. Takes a polymorphic socket reference.

void
zsock_set_unbounded (void *self)
{
#if (ZMQ_VERSION_MAJOR == 2)
    zsock_set_hwm (self, 0);
#else
    zsock_set_sndhwm (self, 0);
    zsock_set_rcvhwm (self, 0);
#endif
}

//  --------------------------------------------------------------------------
//  Send a signal over a socket. A signal is a short message carrying a
//  success/failure code (by convention, 0 means OK). Signals are encoded
//  to be distinguishable from "normal" messages. Accepts a zsock_t or a
//  zactor_t argument, and returns 0 if successful, -1 if the signal could
//  not be sent. Takes a polymorphic socket reference.

int
zsock_signal (void *self, byte status)
{
    assert (self);
    zmsg_t *msg = zmsg_new_signal (status);
    return zmsg_send (&msg, self);
}


//  --------------------------------------------------------------------------
//  Wait on a signal. Use this to coordinate between threads, over pipe
//  pairs. Blocks until the signal is received. Returns -1 on error, 0 or
//  greater on success. Accepts a zsock_t or a zactor_t as argument.
//  Takes a polymorphic socket reference.

int
zsock_wait (void *self)
{
    assert (self);

    //  Loop and discard messages until we get a signal value or interrupt.
    while (true) {
        zmsg_t *msg = zmsg_recv (self);
        if (!msg)
            return -1;
        int rc = zmsg_signal (msg);
        zmsg_destroy (&msg);
        if (rc >= 0)
            return rc;
    }
    return -1;
}


//  --------------------------------------------------------------------------
//  If there is a partial message still waiting on the socket, remove and
//  discard it. This is useful when reading partial messages, to get specific
//  message types.

void
zsock_flush (void *self)
{
    if (zsock_rcvmore (self)) {
        zmsg_t *msg = zmsg_recv (self);
        zmsg_destroy (&msg);
    }
}


//  --------------------------------------------------------------------------
//  Join a group for the RADIO-DISH pattern. Call only on ZMQ_DISH.
//  Returns 0 if OK, -1 if failed.

int
zsock_join (void *self, const char *group)
{
    assert (self);
#ifdef ZMQ_DISH
    return zmq_join (zsock_resolve (self), group);
#else
    errno = EINVAL;
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Leave a group for the RADIO-DISH pattern. Call only on ZMQ_DISH.
//  Returns 0 if OK, -1 if failed.

int
zsock_leave (void *self, const char *group)
{
    assert (self);
#ifdef ZMQ_DISH
    return zmq_leave (zsock_resolve (self), group);
#else
    errno = EINVAL;
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zsock_t.
//  Takes a polymorphic socket reference.

bool
zsock_is (void *self)
{
    assert (self);
    return ((zsock_t *) self)->tag == ZSOCK_TAG;
}


//  --------------------------------------------------------------------------
//  Probe the supplied 'self' pointer. Takes a polymorphic socket reference.
//  If self is a zactor_t, zsock_t, or libzmq socket handle, returns the
//  libzmq socket handle. If self is a valid file descriptor, returns NULL.
//  Else returns self as-is.

void *
zsock_resolve (void *self)
{
    assert (self);
    if (zactor_is (self))
        return zactor_resolve (self);

    if (zsock_is (self))
        return ((zsock_t *) self)->handle;

    //  Check if we have a valid ZMQ socket by probing the socket type
    int type;
    size_t option_len = sizeof (int);
    if (zmq_getsockopt (self, ZMQ_TYPE, &type, &option_len) == 0)
        return self;

    //  Check if self is a valid FD or socket FD
    //  TODO: this code should move to zsys_isfd () as we don't like
    //  non-portable code outside of that class.
    int sock_type = -1;
#if defined (__WINDOWS__)
    int sock_type_size = sizeof (int);
    int rc = getsockopt (*(SOCKET *) self, SOL_SOCKET, SO_TYPE, (char *) &sock_type, &sock_type_size);
    if (rc == 0)
        return NULL;        //  It's a socket descriptor
#else
    socklen_t sock_type_size = sizeof (socklen_t);
    int rc = getsockopt (*(SOCKET *) self, SOL_SOCKET, SO_TYPE, (char *) &sock_type, &sock_type_size);
    if (rc == 0 || (rc == -1 && errno == ENOTSOCK))
        return NULL;        //  It's a socket FD or FD
#endif
    //  Socket appears to be something else, return it as-is
    return self;
}


//  We use the gossip messages for some test cases
#include "zgossip_msg.h"

//  --------------------------------------------------------------------------
//  Selftest

void
zsock_test (bool verbose)
{
    printf (" * zsock: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zsock_t *writer = zsock_new_push ("@tcp://127.0.0.1:5560");
    assert (writer);
    assert (zsock_resolve (writer) != writer);
    assert (streq (zsock_type_str (writer), "PUSH"));

    int rc;
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 2, 0))
    //  Check unbind
    rc = zsock_unbind (writer, "tcp://127.0.0.1:%d", 5560);
    assert (rc == 0);

    //  In some cases and especially when running under Valgrind, doing
    //  a bind immediately after an unbind causes an EADDRINUSE error.
    //  Even a short sleep allows the OS to release the port for reuse.
    zclock_sleep (100);

    //  Bind again
    rc = zsock_bind (writer, "tcp://127.0.0.1:%d", 5560);
    assert (rc == 5560);
    assert (streq (zsock_endpoint (writer), "tcp://127.0.0.1:5560"));
#endif

    zsock_t *reader = zsock_new_pull (">tcp://127.0.0.1:5560");
    assert (reader);
    assert (zsock_resolve (reader) != reader);
    assert (streq (zsock_type_str (reader), "PULL"));

    //  Basic Hello, World
    zstr_send (writer, "Hello, World");
    zmsg_t *msg = zmsg_recv (reader);
    assert (msg);
    char *string = zmsg_popstr (msg);
    assert (streq (string, "Hello, World"));
    free (string);
    zmsg_destroy (&msg);

    //  Test resolve libzmq socket
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 2, 0))
    void *zmq_ctx = zmq_ctx_new ();
#else
    void *zmq_ctx = zmq_ctx_new (1);
#endif
    assert (zmq_ctx);
    void *zmq_sock = zmq_socket (zmq_ctx, ZMQ_PUB);
    assert (zmq_sock);
    assert (zsock_resolve (zmq_sock) == zmq_sock);
    zmq_close (zmq_sock);
    zmq_ctx_term (zmq_ctx);

    //  Test resolve zsock
    zsock_t *resolve = zsock_new_pub("@tcp://127.0.0.1:5561");
    assert (resolve);
    assert (zsock_resolve (resolve) == resolve->handle);
    zsock_destroy (&resolve);

    //  Test resolve FD
    SOCKET fd = zsock_fd (reader);
    assert (zsock_resolve ((void *) &fd) == NULL);

    //  Test binding to ephemeral ports, sequential and random
    int port = zsock_bind (writer, "tcp://127.0.0.1:*");
    assert (port >= DYNAMIC_FIRST && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[50000-]");
    assert (port >= 50000 && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[-50001]");
    assert (port >= DYNAMIC_FIRST && port <= 50001);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[60000-60050]");
    assert (port >= 60000 && port <= 60050);

    port = zsock_bind (writer, "tcp://127.0.0.1:!");
    assert (port >= DYNAMIC_FIRST && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:![50000-]");
    assert (port >= 50000 && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:![-50001]");
    assert (port >= DYNAMIC_FIRST && port <= 50001);
    port = zsock_bind (writer, "tcp://127.0.0.1:![60000-60050]");
    assert (port >= 60000 && port <= 60050);

    //  Test zsock_attach method
    zsock_t *server = zsock_new (ZMQ_DEALER);
    assert (server);
    rc = zsock_attach (server, "@inproc://myendpoint,tcp://127.0.0.1:5556,inproc://others", true);
    assert (rc == 0);
    rc = zsock_attach (server, "", false);
    assert (rc == 0);
    rc = zsock_attach (server, NULL, true);
    assert (rc == 0);
    rc = zsock_attach (server, ">a,@b, c,, ", false);
    assert (rc == -1);
    zsock_destroy (&server);

    //  Test zsock_endpoint method
    rc = zsock_bind (writer, "inproc://test.%s", "writer");
    assert (rc == 0);
    assert (streq (zsock_endpoint (writer), "inproc://test.writer"));

    //  Test error state when connecting to an invalid socket type
    //  ('txp://' instead of 'tcp://', typo intentional)
    rc = zsock_connect (reader, "txp://127.0.0.1:5560");
    assert (rc == -1);

    //  Test signal/wait methods
    rc = zsock_signal (writer, 123);
    assert (rc == 0);
    rc = zsock_wait (reader);
    assert (rc == 123);

    //  Test zsock_send/recv pictures
    uint8_t  number1 = 123;
    uint16_t number2 = 123 * 123;
    uint32_t number4 = 123 * 123 * 123;
    uint64_t number4_MAX = UINT32_MAX;
    uint64_t number8 = 123 * 123 * 123 * 123;
    uint64_t number8_MAX = UINT64_MAX;

    zchunk_t *chunk = zchunk_new ("HELLO", 5);
    assert (chunk);
    zframe_t *frame = zframe_new ("WORLD", 5);
    assert (frame);
    zhashx_t *hash = zhashx_new ();
    assert (hash);
    zuuid_t *uuid = zuuid_new ();
    assert (uuid);
    zhashx_set_destructor (hash, (zhashx_destructor_fn *) zstr_free);
    zhashx_set_duplicator (hash, (zhashx_duplicator_fn *) strdup);
    zhashx_insert (hash, "1", "value A");
    zhashx_insert (hash, "2", "value B");
    char *original = "pointer";

    //  Test zsock_recv into each supported type
    zsock_send (writer, "i124488zsbcfUhp",
                -12345, number1, number2, number4, number4_MAX,
                number8, number8_MAX,
                "This is a string", "ABCDE", 5,
                chunk, frame, uuid, hash, original);
    char *uuid_str = strdup (zuuid_str (uuid));
    zchunk_destroy (&chunk);
    zframe_destroy (&frame);
    zuuid_destroy (&uuid);
    zhashx_destroy (&hash);

    int integer;
    byte *data;
    size_t size;
    char *pointer;
    number8_MAX = number8 = number4 = number2 = number1 = 0;
    rc = zsock_recv (reader, "i124488zsbcfUhp",
                     &integer, &number1, &number2, &number4, &number4_MAX,
                     &number8, &number8_MAX, &string, &data, &size, &chunk,
                     &frame, &uuid, &hash, &pointer);
    assert (rc == 0);
    assert (integer == -12345);
    assert (number1 == 123);
    assert (number2 == 123 * 123);
    assert (number4 == 123 * 123 * 123);
    assert (number4_MAX == UINT32_MAX);
    assert (number8 == 123 * 123 * 123 * 123);
    assert (number8_MAX == UINT64_MAX);
    assert (streq (string, "This is a string"));
    assert (memcmp (data, "ABCDE", 5) == 0);
    assert (size == 5);
    assert (memcmp (zchunk_data (chunk), "HELLO", 5) == 0);
    assert (zchunk_size (chunk) == 5);
    assert (streq (uuid_str, zuuid_str (uuid)));
    assert (memcmp (zframe_data (frame), "WORLD", 5) == 0);
    assert (zframe_size (frame) == 5);
    char *value = (char *) zhashx_lookup (hash, "1");
    assert (streq (value, "value A"));
    value = (char *) zhashx_lookup (hash, "2");
    assert (streq (value, "value B"));
    assert (original == pointer);
    free (string);
    free (data);
    free (uuid_str);
    zframe_destroy (&frame);
    zchunk_destroy (&chunk);
    zhashx_destroy (&hash);
    zuuid_destroy (&uuid);

    //  Test zsock_recv of short message; this lets us return a failure
    //  with a status code and then nothing else; the receiver will get
    //  the status code and NULL/zero for all other values
    zsock_send (writer, "i", -1);
    zsock_recv (reader, "izsbcfp",
        &integer, &string, &data, &size, &chunk, &frame, &pointer);
    assert (integer == -1);
    assert (string == NULL);
    assert (data == NULL);
    assert (size == 0);
    assert (chunk == NULL);
    assert (frame == NULL);
    assert (pointer == NULL);

    msg = zmsg_new ();
    zmsg_addstr (msg, "frame 1");
    zmsg_addstr (msg, "frame 2");
    zsock_send (writer, "szm", "header", msg);
    zmsg_destroy (&msg);

    zsock_recv (reader, "szm", &string, &msg);

    assert (streq ("header", string));
    assert (zmsg_size (msg) == 2);
    assert (zframe_streq (zmsg_first (msg), "frame 1"));
    assert (zframe_streq (zmsg_next (msg), "frame 2"));
    zstr_free (&string);
    zmsg_destroy (&msg);

    //  Test zsock_recv with null arguments
    chunk = zchunk_new ("HELLO", 5);
    assert (chunk);
    frame = zframe_new ("WORLD", 5);
    assert (frame);
    zsock_send (writer, "izsbcfp",
                -12345, "This is a string", "ABCDE", 5, chunk, frame, original);
    zframe_destroy (&frame);
    zchunk_destroy (&chunk);
    zsock_recv (reader, "izsbcfp", &integer, NULL, NULL, NULL, &chunk, NULL, NULL);
    assert (integer == -12345);
    assert (memcmp (zchunk_data (chunk), "HELLO", 5) == 0);
    assert (zchunk_size (chunk) == 5);
    zchunk_destroy (&chunk);

    //  Test zsock_bsend/brecv pictures with binary encoding
    frame = zframe_new ("Hello", 5);
    chunk = zchunk_new ("World", 5);

    msg = zmsg_new ();
    zmsg_addstr (msg, "Hello");
    zmsg_addstr (msg, "World");

    zsock_bsend (writer, "1248sSpcfm",
                 number1, number2, number4, number8,
                 "Hello, World",
                 "Goodbye cruel World!",
                 original,
                 chunk, frame, msg);
    zchunk_destroy (&chunk);
    zframe_destroy (&frame);
    zmsg_destroy (&msg);

    number8 = number4 = number2 = number1 = 0;
    char *longstr;
    zsock_brecv (reader, "1248sSpcfm",
                 &number1, &number2, &number4, &number8,
                 &string, &longstr,
                 &pointer,
                 &chunk, &frame, &msg);
    assert (number1 == 123);
    assert (number2 == 123 * 123);
    assert (number4 == 123 * 123 * 123);
    assert (number8 == 123 * 123 * 123 * 123);
    assert (streq (string, "Hello, World"));
    assert (streq (longstr, "Goodbye cruel World!"));
    assert (pointer == original);
    zstr_free (&longstr);
    zchunk_destroy (&chunk);
    zframe_destroy (&frame);
    zmsg_destroy (&msg);

#ifdef ZMQ_SERVER

    //  Test zsock_bsend/brecv pictures with binary encoding on SERVER and CLIENT sockets
    server = zsock_new_server ("tcp://127.0.0.1:5561");
    assert (server);
    zsock_t* client = zsock_new_client ("tcp://127.0.0.1:5561");
    assert (client);

    //  From client to server
    chunk = zchunk_new ("World", 5);
    zsock_bsend (client, "1248sSpc",
                 number1, number2, number4, number8,
                 "Hello, World",
                 "Goodbye cruel World!",
                 original,
                 chunk);
    zchunk_destroy (&chunk);

    number8 = number4 = number2 = number1 = 0;
    zsock_brecv (server, "1248sSpc",
                 &number1, &number2, &number4, &number8,
                 &string, &longstr,
                 &pointer,
                 &chunk);
    assert (number1 == 123);
    assert (number2 == 123 * 123);
    assert (number4 == 123 * 123 * 123);
    assert (number8 == 123 * 123 * 123 * 123);
    assert (streq (string, "Hello, World"));
    assert (streq (longstr, "Goodbye cruel World!"));
    assert (pointer == original);
    assert (zsock_routing_id (server));
    zstr_free (&longstr);
    zchunk_destroy (&chunk);

    //  From server to client
    chunk = zchunk_new ("World", 5);
    zsock_bsend (server, "1248sSpc",
                 number1, number2, number4, number8,
                 "Hello, World",
                 "Goodbye cruel World!",
                 original,
                 chunk);
    zchunk_destroy (&chunk);

    number8 = number4 = number2 = number1 = 0;
    zsock_brecv (client, "1248sSpc",
                 &number1, &number2, &number4, &number8,
                 &string, &longstr,
                 &pointer,
                 &chunk);
    assert (number1 == 123);
    assert (number2 == 123 * 123);
    assert (number4 == 123 * 123 * 123);
    assert (number8 == 123 * 123 * 123 * 123);
    assert (streq (string, "Hello, World"));
    assert (streq (longstr, "Goodbye cruel World!"));
    assert (pointer == original);
    assert (zsock_routing_id (client) == 0);
    zstr_free (&longstr);
    zchunk_destroy (&chunk);

    zsock_destroy (&client);
    zsock_destroy (&server);

#endif

#ifdef ZMQ_SCATTER

    zsock_t* gather = zsock_new_gather ("inproc://test-gather-scatter");
    assert (gather);
    zsock_t* scatter = zsock_new_scatter ("inproc://test-gather-scatter");
    assert (scatter);

    rc = zstr_send (scatter, "HELLO");
    assert (rc == 0);

    char* message;
    message = zstr_recv (gather);
    assert (streq(message, "HELLO"));
    zstr_free (&message);    

    zsock_destroy (&gather);
    zsock_destroy (&scatter);

#endif

    //  Check that we can send a zproto format message
    zsock_bsend (writer, "1111sS4", 0xAA, 0xA0, 0x02, 0x01, "key", "value", 1234);
    zgossip_msg_t *gossip = zgossip_msg_new ();
    zgossip_msg_recv (gossip, reader);
    assert (zgossip_msg_id (gossip) == ZGOSSIP_MSG_PUBLISH);
    zgossip_msg_destroy (&gossip);

    zsock_destroy (&reader);
    zsock_destroy (&writer);

    //  @end
    printf ("OK\n");
    zsock_option_test (verbose);
}
