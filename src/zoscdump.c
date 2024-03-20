/*  =========================================================================
    zoscdump [options] <url>...

    A command line utility for receiving OSC messages similar to liblo's
    oscdump.

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
    zoscdump -
@discuss
@end
*/

#include "czmq_classes.h"

static bool verbose = false;

static int
print_help()
{
    puts ("zoscdump [options] <url>...");
    puts ("  --verbose / -v         verbose output");
    puts ("  --help / -h            this information");
    puts ("  <url>                  url to listen on, i.e udp://127.0.0.1:1234");
    return 0;
}

zsock_t *
determine_socket( const char *bind)
{
    zsock_t *retsock = NULL;
    zrex_t *rex = zrex_new (NULL);
    // determine transport
    if (zrex_eq(rex, bind, "^(.+)://.*") )
    {
        const char *transport = zrex_hit(rex, 1);

        if ( streq(transport, "ipc") )
            retsock = zsock_new_pull( bind );
#ifdef ZMQ_DGRAM
        else if ( streq(transport, "udp") )
             retsock = zsock_new_dgram(bind);
#endif
#ifdef ZMQ_STREAM
        else if ( streq(transport, "tcp") )
        {
            retsock = zsock_new(ZMQ_STREAM);
            zsock_bind(retsock, "%s", bind);
        }
#endif

        else
            zsys_error("Not a valid transport in %s", transport);

        if (retsock == NULL)
            zsys_error("can't bind socket to %s", bind);

        return retsock;
    }
    else
        zsys_error("can't determine transport from %s", bind);

    return NULL;
}


void
recv_dgram(zmsg_t *msg)
{
    char *sender = zmsg_popstr(msg);
    printf("%s ", sender);
    zframe_t *frame = zmsg_pop(msg);
    assert(frame);
    if (zframe_size(frame) > 0 )
    {
        zosc_t *oscmsg = zosc_fromframe(frame);
        assert(oscmsg);
        zosc_print(oscmsg);
    }
    else
    {
        zframe_destroy(&frame);
    }

    zstr_free(&sender);
}

void
recv_stream(zmsg_t *msg)
{
    zframe_t *senderid = zmsg_pop(msg);
    assert( zframe_size(senderid) == 5);
    zframe_t *frame = zmsg_pop(msg);
    assert(frame);
    if (zframe_size(frame) > 0 ) // on connection we receive a zero payload
    {
        zosc_t *oscmsg = zosc_fromframe(frame);
        assert(oscmsg);
        printf("%s ", zframe_strhex (senderid));
        zosc_print(oscmsg);
    }
    else
    {
        zsys_info("host id %s connected", zframe_strhex (senderid));
        zframe_destroy(&frame);
    }
    zframe_destroy(&senderid);
}

int
main (int argc, char *argv [])
{
    zargs_t *args = zargs_new(argc, argv);
    assert(args);

    if ( zargs_arguments(args) == 0 )
        return print_help();

    if ( zargs_hasx (args, "--help", "-h", NULL) )
        return print_help();

    if (zargs_hasx(args, "--verbose", "-v", NULL) )
        verbose = true;

    zpoller_t *poller = zpoller_new(NULL);
    assert(poller);
    zlist_t *sockets = zlist_new();
    assert(sockets);

    int ret = 0;

    const char *bind = zargs_first(args);
    while (bind)
    {
        zsock_t *sock = determine_socket(bind);
        if (sock == NULL)
        {
            ret = 1;
            break;
        }

        zlist_append(sockets, sock);
        if (verbose)
            zsys_info("Listening dgram socket on %s", bind);

        ret = zpoller_add(poller, sock);
        if (ret != 0 )
        {
            zsys_error("can't add bound socket %s to poller", bind);
            ret = 1;
            break;
        }

        bind = zargs_next(args);
    }
    zargs_destroy(&args);

    while ( ret == 0)
    {
        void *which = (void *) zpoller_wait(poller, -1);

        if (zpoller_terminated( poller ) || zpoller_expired( poller ) )
            break;

        zmsg_t *msg = zmsg_recv(which);
        assert(msg);

        switch ( zsock_type(which) )
        {
#ifdef ZMQ_DGRAM
        case ZMQ_DGRAM:
            recv_dgram(msg);
            break;
#endif
#ifdef ZMQ_STREAM
        case ZMQ_STREAM:
            recv_stream(msg);
            break;
#endif
        default:
            zsys_error("Unsupported socket type");
            break;
        }
        zmsg_destroy(&msg);
        fflush(stdout);
    }

    zpoller_destroy(&poller);
    zsock_t *sock = (zsock_t *)zlist_first(sockets);
    while (sock)
    {
        zsock_destroy(&sock);
        sock = (zsock_t *)zlist_next(sockets);
    }

    return ret;
}
