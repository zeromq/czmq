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

/*
@header
    zudp -
@discuss
@end
*/

#include "czmq_classes.h"

//  Structure of our class
enum ZUDP_TYPE { ZUDP_UNICAST, ZUDP_MULTICAST, ZUDP_BROADCAST };
struct _zudp_t {
    SOCKET  udpsock;    //  our UDP socket
    int     sockflags;  //  the socket's type flags
    int     socktype;   //  our socket type
};

//  Some OS define IPV6_ADD_MEMBERSHIP some IPV6_JOIN_GROUP. They are identical
#ifndef IPV6_ADD_MEMBERSHIP
# define IPV6_ADD_MEMBERSHIP	IPV6_JOIN_GROUP
# define IPV6_DROP_MEMBERSHIP	IPV6_LEAVE_GROUP
#endif
//  --------------------------------------------------------------------------
//  Create a new zudp

zudp_t *
zudp_new (int type, bool reuse)
{
    zudp_t *self = (zudp_t *) zmalloc (sizeof (zudp_t));
    assert (self);
    self->socktype = type;
    //  Initialize class properties here
    self->sockflags = SOCK_DGRAM;    //  udp socket type
#ifdef CZMQ_HAVE_SOCK_CLOEXEC
    //  Ensure socket is closed by exec() functions.
    self->sockflags |= SOCK_CLOEXEC;
#endif

    if (zsys_ipv6 ())
        self->udpsock = socket (AF_INET6, self->sockflags, IPPROTO_UDP);
    else
        self->udpsock = socket (AF_INET, self->sockflags, IPPROTO_UDP);
    if (self->udpsock == INVALID_SOCKET) {
        zudp_error ("socket");
        zudp_destroy( &self );
        return NULL;    // todo cleanup!
    }

    if ( reuse )
    {
        int on = 1;
        //  Allow multiple owners to bind to socket; incoming
        //  messages will replicate to each owner
        if (setsockopt (self->udpsock, SOL_SOCKET, SO_REUSEADDR,
                        (char *) &on, sizeof (on)) == SOCKET_ERROR)
            zudp_error ("setsockopt (SO_REUSEADDR)");

#if defined (SO_REUSEPORT)
        //  On some platforms we have to ask to reuse the port
        if (setsockopt (self->udpsock, SOL_SOCKET, SO_REUSEPORT,
                        (char *) &on, sizeof (on)) == SOCKET_ERROR)
            zsys_socket_error ("setsockopt (SO_REUSEPORT)");
#endif
    }

    if ( self->socktype == ZUDP_BROADCAST )
    {
        int on = 1;
        if (setsockopt (self->udpsock, SOL_SOCKET, SO_BROADCAST,
                        (char *) &on, sizeof (on)) == SOCKET_ERROR)
            zsys_socket_error ("setsockopt (SO_BROADCAST)");
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zudp

void
zudp_destroy (zudp_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zudp_t *self = *self_p;
        //  Free class properties here
        if (self->udpsock > 0 ) close(self->udpsock);
        self->socktype = 0;
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  low level internal udp send method

int
zudp_send (zudp_t *self, zframe_t *frame, inaddr_t *address, int addrlen)
{
    assert (self);
    assert (frame);
    assert (address);

    if (sendto (self->udpsock,
        (char *) zframe_data (frame), (size_t) zframe_size (frame),
        0, //  Flags
        (struct sockaddr *) address, (socklen_t)addrlen) == -1)
    {
        zsys_debug ("zudp_send: failed, reason=%s", strerror (errno));
        return -1;              //  UDP send not possible
    }
    else
        return 0;
}

//  Send zframe to UDP socket to given address and port,
//  return -1 if sending failed (i.e. due to interface having
//  disappeared (happens easily with WiFi))
int
zudp_sendto (zudp_t *self, zframe_t *frame, const char *address, int port)
{
    assert(self);
    assert(frame);
    assert(zframe_is(frame));

    if ( zsys_ipv6() )
    {
        in6addr_t daddr;
        memset(&daddr, 0, sizeof(in6addr_t));
        daddr.sin6_port = htons( (ushort)port );
        daddr.sin6_family = AF_INET6;
        if ( inet_pton(AF_INET6, address, &daddr.sin6_addr) < 1 )
        {
            zsys_error("inet_pton conversion error %s", strerror(errno));
            return -1;
        }
        return zudp_send(self, frame, (inaddr_t *)&daddr, sizeof(in6addr_t));
    }
    else
    {
        inaddr_t daddr;
        memset(&daddr, 0, sizeof(inaddr_t));
        daddr.sin_port = htons( (ushort)port );
        daddr.sin_family = AF_INET;
        if ( inet_pton(AF_INET, address, &daddr.sin_addr) < 1 )
        {
            zsys_error("inet_pton conversion error %s", strerror(errno));
            return -1;
        }
        return zudp_send(self, frame, (inaddr_t *)&daddr, sizeof(inaddr_t));
    }
}

//  Receive zframe from UDP socket, and set address of peer that sent it
//  The peername must be a char [INET_ADDRSTRLEN] array if IPv6 is disabled or
//  NI_MAXHOST if it's enabled. Returns NULL when failing to get peer address.

zframe_t *
zudp_recv (zudp_t *self, char *peername, int peerlen)
{
    assert (self);
    char buffer [UDP_FRAME_MAX];
    in6addr_t address6;
    socklen_t address_len = sizeof (in6addr_t);
    ssize_t size = recvfrom (
        self->udpsock,
        buffer, UDP_FRAME_MAX,
        0,      //  Flags
        (struct sockaddr *) &address6, &address_len);

    if (size == SOCKET_ERROR)
        zsys_socket_error ("recvfrom");

    //  Get sender address as printable string
    int rc = getnameinfo ((struct sockaddr *) &address6, address_len,
                 peername, peerlen, NULL, 0, NI_NUMERICHOST);

    if (rc) {
        zsys_warning ("zsys_udp_recv: getnameinfo failed, reason=%s",
                gai_strerror (rc));
        return NULL;
    }

    //  Some platform's getnameinfo, like Solaris, appear not to append the
    //  interface name when parsing a link-local IPv6 address. These addresses
    //  cannot be used without the interface, so we must append it manually.
    //  On Windows, if_indextoname is only available from Vista.
#if !defined (__WINDOWS__) || (_WIN32_WINNT >= 0x0600)
    if (address6.sin6_family == AF_INET6 &&
            IN6_IS_ADDR_LINKLOCAL (&address6.sin6_addr) &&
            !strchr (peername, '%')) {
        char ifname [IF_NAMESIZE] = {0};
        if_indextoname (address6.sin6_scope_id, ifname);
        strcat (peername, "%");
        strcat (peername, ifname);
    }
#endif

    return zframe_new (buffer, (size_t)size);
}

// bind helpers
int
zudp_unicast_bind( zudp_t *self, const char *address, int port)
{
    assert(self);
    assert(address);
    assert(port);

    int rc;
    if ( zsys_ipv6() )
    {
        in6addr_t laddr;
        memset(&laddr, 0, sizeof(in6addr_t));
        laddr.sin6_port = htons( (ushort)port );
        laddr.sin6_family = AF_INET6;
        if ( streq(address, "*") || streq(address, "") )
        {
            laddr.sin6_addr = in6addr_any;
            rc = bind( self->udpsock, (struct sockaddr*)&laddr, sizeof (laddr) );
        }
        else
        {
            if ( inet_pton(AF_INET6, address, &laddr.sin6_addr) < 1 )
            {
                zsys_error("inet_pton conversion error %s", strerror(errno));
                return -1;
            }
            rc = bind( self->udpsock, (struct sockaddr*)&laddr, sizeof (laddr) );
        }
    }
    else
    {
        inaddr_t laddr;
        memset(&laddr, 0, sizeof(inaddr_t));
        laddr.sin_port = htons( (ushort)port );
        laddr.sin_family = AF_INET;
        if ( streq(address, "*") || streq(address, "") )
            laddr.sin_addr.s_addr = htonl(INADDR_ANY);
        else
        {
            if ( inet_pton(AF_INET, address, &laddr.sin_addr) < 1 )
            {
                zsys_error("inet_pton conversion error %s", strerror(errno));
                return -1;
            }
        }
        rc = bind( self->udpsock, (const struct sockaddr*)&laddr, sizeof (laddr) );
    }
    return rc;
}

int zudp_multicast_bind(zudp_t *self, const char *address, int port)
{
    assert(self);
    assert(address);
    assert(port);

    if ( streq(address, "*") || streq(address, "") )
    {
        zsys_error("You need to set a multicast address to bind to!");
        return -1;
    }

    int rc;
    if ( zsys_ipv6() )
    {
        in6addr_t laddr;
        memset(&laddr, 0, sizeof(in6addr_t));
        laddr.sin6_port = htons( (ushort)port );
        laddr.sin6_family = AF_INET6;

        // JOIN MEMBERSHIP
        struct ipv6_mreq group;
        group.ipv6mr_interface = 0;
        if ( inet_pton( AF_INET6, address, &group.ipv6mr_multiaddr ) < 1 )
        {
            zsys_error("inet_pton multicast address %s conversion error: %s", address, strerror(errno));
            return -1;
        }
        setsockopt( self->udpsock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &group, sizeof group );
        rc = bind( self->udpsock, (struct sockaddr*)&laddr, sizeof (laddr) );
        if ( rc == -1 ) zsys_error("multicast ipv6 bind error %s", strerror(errno));
    }
    else
    {
        inaddr_t laddr;
        memset(&laddr, 0, sizeof(inaddr_t));
        laddr.sin_port = htons( (ushort)port );
        laddr.sin_family = AF_INET;
        laddr.sin_addr.s_addr = htonl(INADDR_ANY); // the bind address! better use the nic ip

        // JOIN MEMBERSHIP
        struct ip_mreq group;
        if ( inet_pton( AF_INET, address, &group.imr_multiaddr.s_addr ) < 1 )
        {
            zsys_error("inet_pton multicast address %s conversion error: %s", address, strerror(errno));
            return -1;
        }
        group.imr_interface.s_addr = INADDR_ANY;
        rc = bind( self->udpsock, (const struct sockaddr*)&laddr, sizeof (laddr) );
        if (setsockopt(self->udpsock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)) < 0 )
        {
            zsys_error("Multicast setsockopt error: %s", strerror(errno));
            return -1;
        }
    }
    return rc;
}

int
zudp_broadcast_bind( zudp_t *self, const char *address, int port)
{
    return -1;
}

//  Bind a socket to an address and port number.
//  On failure, returns -1.
int
zudp_bind (zudp_t *self, const char *address, int port)
{
    assert(self);
    int rc = 0;
    switch ( self->socktype )
    {
        case ZUDP_UNICAST:
        {
            rc = zudp_unicast_bind(self, address, port );
            break;
        }
        case ZUDP_MULTICAST:
        {
            rc = zudp_multicast_bind(self, address, port );
            break;
        }
        case ZUDP_BROADCAST:
        {
            rc = zudp_broadcast_bind(self, address, port );
            break;
        }
    }

    return rc;
}


//  Return the socket's filedescriptor
int
zudp_fd (zudp_t *self)
{
    assert(self);
    assert(self->udpsock);
    return self->udpsock;
}

//  Handle an I/O error on some socket operation; will report and die on
//  fatal errors, and continue silently on "try again" errors.
void
zudp_error (const char *reason)
{
    zsys_socket_error(reason);
}

//  --------------------------------------------------------------------------
//  Self test of this class

// If your selftest reads SCMed fixture data, please keep it in
// src/selftest-ro; if your test creates filesystem objects, please
// do so under src/selftest-rw.
// The following pattern is suggested for C selftest code:
//    char *filename = NULL;
//    filename = zsys_sprintf ("%s/%s", SELFTEST_DIR_RO, "mytemplate.file");
//    assert (filename);
//    ... use the "filename" for I/O ...
//    zstr_free (&filename);
// This way the same "filename" variable can be reused for many subtests.
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

void
zudp_test (bool verbose)
{
    printf (" * zudp: ");

    //  @selftest
    //  Simple create/destroy test
    zudp_t *self = zudp_new (ZUDP_UNICAST, true);
    assert(self);
    zudp_destroy (&self);

    // simple send receive test
    zudp_t *sender = zudp_new( ZUDP_UNICAST, true);
    assert(sender);
    zudp_t *recvr = zudp_new( ZUDP_UNICAST, true);
    assert(recvr);
    int rc = zudp_bind( recvr, "*", 7777);
    assert( rc == 0 );

    zframe_t *f = zframe_new("hello", 5);
    assert(f);

    rc = zudp_sendto(sender, f, "127.0.0.1", 7777);
    assert( rc == 0 );
    char peername [INET_ADDRSTRLEN];
    zframe_t *r = zudp_recv(recvr, peername, INET_ADDRSTRLEN);
    assert( r );
    assert( zframe_size( r ) == 5 );
    assert( ! streq(peername, "") );
    zudp_destroy(&sender);
    zudp_destroy(&recvr);
    zframe_destroy(&f);
    zframe_destroy(&r);

    // multicast ipv4 send receive test
    zsys_set_ipv6(0);
    zudp_t *msender4 = zudp_new( ZUDP_MULTICAST, true);
    assert(msender4);
    zudp_t *mrecvr4 = zudp_new( ZUDP_MULTICAST, true);
    assert(mrecvr4);
    rc = zudp_bind( mrecvr4, "225.25.25.25", 7777);
    assert( rc == 0 );

    zframe_t *fm = zframe_new("hello", 5);
    assert(fm);
    rc = zudp_sendto(msender4, fm, "225.25.25.25", 7777);
    assert(rc == 0 );
    char mpeername [INET_ADDRSTRLEN];
    zframe_t *r4 = zudp_recv(mrecvr4, mpeername, INET_ADDRSTRLEN);
    assert( r4 );
    assert( zframe_size( r4 ) == 5 );
    assert( ! streq(mpeername, "") );
    zudp_destroy(&msender4);
    zudp_destroy(&mrecvr4);
    zframe_destroy( &fm );
    zframe_destroy( &r4 );

    if ( ! zsys_ipv6_available() )
    {
        zsys_info( "No IPv6 available skipping those tests" );
        return;
    }
    /// ***************
    /// IPV6 tests
    /// ***************
    // simple ipv6 send receive test
    zsys_set_ipv6(1);
    zudp_t *sender6 = zudp_new( ZUDP_UNICAST, true);
    assert(sender6);
    zudp_t *recvr6 = zudp_new( ZUDP_UNICAST, true);
    assert(recvr6);
    rc = zudp_bind( recvr6, "*", 7777);
    assert( rc == 0 );

    zframe_t *f6 = zframe_new("hello", 5);
    assert(f6);
    rc = zudp_sendto(sender6, f6, "::1", 7777);
    assert(rc == 0 );
    char peername6 [NI_MAXHOST];
    zframe_t *r6 = zudp_recv(recvr6, peername6, NI_MAXHOST);
    assert( r6 );
    assert( zframe_size( r6 ) == 5 );
    assert( ! streq(peername6, "") );
    zudp_destroy(&sender6);
    zudp_destroy(&recvr6);
    zframe_destroy( &f6 );
    zframe_destroy( &r6 );

    // multicast ipv6 send receive test
    zudp_t *msender6 = zudp_new( ZUDP_MULTICAST, true);
    assert(msender6);
    zudp_t *mrecvr6 = zudp_new( ZUDP_MULTICAST, true);
    assert(mrecvr6);
    rc = zudp_bind( mrecvr6, "ff12::feed:a:dead:beef", 7777);
    assert( rc == 0 );

    zframe_t *fm6 = zframe_new("hello", 5);
    assert(fm6);
    rc = zudp_sendto(msender6, fm6, "ff12::feed:a:dead:beef", 7777);
    assert(rc == 0 );
    char mpeername6 [NI_MAXHOST];
    zframe_t *rm6 = zudp_recv(mrecvr6, mpeername6, NI_MAXHOST);
    assert( rm6 );
    assert( zframe_size( rm6 ) == 5 );
    assert( zframe_streq(rm6, "hello" ) );
    assert( ! streq(mpeername6, "") );
    zudp_destroy(&msender6);
    zudp_destroy(&mrecvr6);
    zframe_destroy( &rm6 );
    zframe_destroy( &fm6 );

    //  @end
    printf ("OK\n");
}
