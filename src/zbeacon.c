/*  =========================================================================
    zbeacon - LAN discovery and presence

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
    The zbeacon class implements a peer-to-peer discovery service for local
    networks. A beacon can broadcast and/or capture service announcements
    using UDP messages on the local area network. This implementation uses
    IPv4 UDP broadcasts. You can define the format of your outgoing beacons,
    and set a filter that validates incoming beacons. Beacons are sent and
    received asynchronously in the background.
@discuss
    This class replaces zbeacon_v2, and is meant for applications that use
    the CZMQ v3 API (meaning, zsock).
@end
*/

#include "czmq_classes.h"

//  Constants
#define INTERVAL_DFLT  1000         //  Default interval = 1 second

//  --------------------------------------------------------------------------
//  The self_t structure holds the state for one actor instance

typedef struct {
    zsock_t *pipe;              //  Actor command pipe
    SOCKET udpsock;             //  UDP socket for send/recv
    SOCKET udpsock_send;        //  UDP socket for IPv6 send
    char port_nbr [7];          //  UDP port number we work on
    int interval;               //  Beacon broadcast interval
    int64_t ping_at;            //  Next broadcast time
    zframe_t *transmit;         //  Beacon transmit data
    zframe_t *filter;           //  Beacon filter data
    inaddr_storage_t broadcast; //  Our broadcast address
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
    char hostname [NI_MAXHOST]; //  Saved host name
} self_t;

static void
s_self_destroy (self_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        self_t *self = *self_p;
        zframe_destroy (&self->transmit);
        zframe_destroy (&self->filter);
        if (self->udpsock != INVALID_SOCKET)
            zsys_udp_close (self->udpsock);
        if (self->udpsock_send != INVALID_SOCKET)
            zsys_udp_close (self->udpsock_send);
        freen (self);
        *self_p = NULL;
    }
}

static self_t *
s_self_new (zsock_t *pipe)
{
    self_t *self = (self_t *) zmalloc (sizeof (self_t));
    assert (self);
    self->pipe = pipe;
    self->udpsock = INVALID_SOCKET;
    self->udpsock_send = INVALID_SOCKET;
    return self;
}


//  --------------------------------------------------------------------------
//  Prepare beacon to work on specified UPD port.

static void
s_self_prepare_udp (self_t *self)
{
    //  Create our UDP socket
    if (self->udpsock != INVALID_SOCKET)
        zsys_udp_close (self->udpsock);
    if (self->udpsock_send != INVALID_SOCKET)
        zsys_udp_close (self->udpsock_send);

    bool enable_multicast = zsys_ipv4_mcast_address () != NULL;

    self->hostname [0] = 0;
    //  For IPv6 we need two sockets. At least on Linux, IPv6 multicast packets
    //  are NOT received despite joining the group and setting the interface
    //  option UNLESS the socket is bound to in6_addrany, which means the kernel
    //  will select an arbitrary IP address as the source when sending beacons
    //  out. This breaks zbeacon as the protocol uses the source address of a
    //  beacon to find the endpoint of a peer, which is then random and
    //  useless (could even be associated with a different interface, eg: a
    //  virtual bridge).
    //  As a workaround, use a different socket to send packets. So the socket
    //  that receives can be bound to in6_addrany, and the socket that sends
    //  can be bound to the actual intended host address.
    self->udpsock = zsys_udp_new (enable_multicast);
    if (self->udpsock == INVALID_SOCKET) {
        self->udpsock_send = INVALID_SOCKET;
        return;
    }
    self->udpsock_send = zsys_udp_new (enable_multicast);
    if (self->udpsock_send == INVALID_SOCKET) {
        zsys_udp_close (self->udpsock);
        self->udpsock = INVALID_SOCKET;
        return;
    }

    //  Get the network interface fro ZSYS_INTERFACE or else use first
    //  broadcast interface defined on system. ZSYS_INTERFACE=* means
    //  use INADDR_ANY + INADDR_BROADCAST.
    const char *iface = zsys_interface ();
    struct addrinfo *bind_to = NULL;
    struct addrinfo *send_to = NULL;
    struct addrinfo hint;
    memset (&hint, 0, sizeof(struct addrinfo));
    hint.ai_flags = AI_NUMERICHOST;
#if HAVE_DECL_AI_V4MAPPED
    hint.ai_flags |= AI_V4MAPPED;
#endif
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_protocol = IPPROTO_UDP;
    hint.ai_family = zsys_ipv6 () ? AF_INET6 : AF_INET;
    int rc;
    int found_iface = 0;
    unsigned int if_index = 0;

    if (streq (iface, "*")) {
        //  Wildcard means bind to INADDR_ANY and send to INADDR_BROADCAST
        //  IE - getaddrinfo with NULL as first parameter or 255.255.255.255
        //  (or IPv6 multicast link-local all-node group ff02::1
        hint.ai_flags = hint.ai_flags | AI_PASSIVE;
        rc = getaddrinfo (NULL, self->port_nbr, &hint, &bind_to);
        assert (rc == 0);

        if (zsys_ipv6()) {
            //  Default is link-local all-node multicast group
            rc = getaddrinfo (zsys_ipv6_mcast_address (), self->port_nbr, &hint,
                    &send_to);
            assert (rc == 0);
        }
        else if (enable_multicast) {
            rc = getaddrinfo (zsys_ipv4_mcast_address(), self->port_nbr, &hint,
                    &send_to);
            assert (rc == 0);
        }
        else {
            rc = getaddrinfo ("255.255.255.255", self->port_nbr, &hint,
                    &send_to);
            assert (rc == 0);
        }

        found_iface = 1;
    }
    // if ZSYS_INTERFACE is a single digit, use the corresponding interface in
    // the interface list
    else if (strlen (iface) == 1 && iface[0] >= '0' && iface[0] <= '9')
    {
        int if_number = atoi (iface);
        ziflist_t *iflist = ziflist_new_ipv6 ();
        assert (iflist);
        const char *name = ziflist_first (iflist);
        int idx = -1;
        while (name) {
            idx++;
            if (idx == if_number &&
                    ((ziflist_is_ipv6 (iflist) && zsys_ipv6 ()) ||
                            (!ziflist_is_ipv6 (iflist) && !zsys_ipv6 ()))) {
                //  Using inet_addr instead of inet_aton or inet_atop
                //  because these are not supported in Win XP
                rc = getaddrinfo (ziflist_address (iflist), self->port_nbr,
                        &hint, &bind_to);
                assert (rc == 0);
                if (enable_multicast) {
                    rc = getaddrinfo (zsys_ipv4_mcast_address (), self->port_nbr,
                            &hint, &send_to);
                }
                else {
                    rc = getaddrinfo (ziflist_broadcast (iflist), self->port_nbr,
                            &hint, &send_to);
                }
                assert (rc == 0);
                if_index = if_nametoindex (name);
                if (self->verbose && !enable_multicast)
                    zsys_info ("zbeacon: interface=%s address=%s broadcast=%s",
                               name, ziflist_address (iflist),
                               ziflist_broadcast (iflist));
                if (self->verbose && enable_multicast)
                    zsys_info ("zbeacon: interface=%s address=%s multicast=%s",
                               name, ziflist_address (iflist),
                               zsys_ipv4_mcast_address ());
                found_iface = 1;
                break;      //  iface is known, so allow it
            }
            name = ziflist_next (iflist);
        }
        ziflist_destroy (&iflist);
    }
    else if (zsys_ipv6 () && strneq("", zsys_ipv6_address ()) && strneq (iface, "")) {
        rc = getaddrinfo (zsys_ipv6_address (), self->port_nbr,
                &hint, &bind_to);
        assert (rc == 0);
        //  A user might set a link-local address without appending %iface
        if (IN6_IS_ADDR_LINKLOCAL (&((in6addr_t *)bind_to->ai_addr)->sin6_addr) &&
                !strchr (zsys_ipv6_address (), '%')) {
            char address_and_iface [NI_MAXHOST] = {0};
            strcat (address_and_iface, zsys_ipv6_address ());
            strcat (address_and_iface, "%");
            strcat (address_and_iface, iface);
            rc = getaddrinfo (address_and_iface, self->port_nbr, &hint, &bind_to);
            assert (rc == 0);
        }
        rc = getaddrinfo (zsys_ipv6_mcast_address (), self->port_nbr,
                &hint, &send_to);
        assert (rc == 0);
        if_index = if_nametoindex (iface);

        if (self->verbose)
            zsys_info ("zbeacon: interface=%s address=%s broadcast=%s",
                    iface, zsys_ipv6_address (), zsys_ipv6_mcast_address ());
        found_iface = 1;
    }
    else {
        //  Look for matching interface, or first ziflist item
        ziflist_t *iflist = ziflist_new_ipv6 ();
        assert (iflist);
        const char *name = ziflist_first (iflist);
        while (name) {
            //  If IPv6 is not enabled ignore IPv6 interfaces.
            if ( (streq (iface, name) || streq (iface, "") ||
                    streq (ziflist_address (iflist), iface)) &&
                        ((ziflist_is_ipv6 (iflist) && zsys_ipv6 ()) ||
                            (!ziflist_is_ipv6 (iflist) && !zsys_ipv6 ()))) {
                rc = getaddrinfo (ziflist_address (iflist), self->port_nbr,
                        &hint, &bind_to);
                assert (rc == 0);
                if (enable_multicast) {
                    rc = getaddrinfo (zsys_ipv4_mcast_address (),
                            self->port_nbr, &hint, &send_to);
                }
                else {
                    rc = getaddrinfo (ziflist_broadcast (iflist),
                            self->port_nbr, &hint, &send_to);
                }
                assert (rc == 0);
                assert (rc == 0);
                if_index = if_nametoindex (name);

                if (self->verbose && !enable_multicast)
                    zsys_info ("zbeacon: interface=%s address=%s broadcast=%s",
                               name, ziflist_address (iflist), ziflist_broadcast (iflist));
                if (self->verbose && enable_multicast)
                    zsys_info ("zbeacon: interface=%s address=%s multicast=%s",
                               name, ziflist_address (iflist),
                               zsys_ipv4_mcast_address ());
                found_iface = 1;
                break;      //  iface is known, so allow it
            }
            name = ziflist_next (iflist);
        }
        ziflist_destroy (&iflist);
    }
    if (found_iface) {
        inaddr_storage_t bind_address;

        //  On Windows we bind to the host address
        //  On *NIX we bind to INADDR_ANY or in6addr_any, otherwise multicast
        //  packets will be filtered out despite joining the group
#if (defined (__WINDOWS__))
        memcpy (&bind_address, bind_to->ai_addr, bind_to->ai_addrlen);
#else
        memcpy (&bind_address, send_to->ai_addr, send_to->ai_addrlen);
        if (zsys_ipv6 ())
            bind_address.__inaddr_u.__addr6.sin6_addr = in6addr_any;
        else
            bind_address.__inaddr_u.__addr.sin_addr.s_addr = htonl (INADDR_ANY);
#endif
        memcpy (&self->broadcast, send_to->ai_addr, send_to->ai_addrlen);

        if (zsys_ipv6()) {
            struct ipv6_mreq mreq;
            mreq.ipv6mr_interface = if_index;
            memcpy (&mreq.ipv6mr_multiaddr,
                    &(((in6addr_t *)(send_to->ai_addr))->sin6_addr),
                    sizeof (struct in6_addr));

            if (setsockopt (self->udpsock, IPPROTO_IPV6, IPV6_JOIN_GROUP,
                    (char *)&mreq, sizeof (mreq)))
                zsys_socket_error ("zbeacon: setsockopt IPV6_JOIN_GROUP failed");

            if (setsockopt (self->udpsock, IPPROTO_IPV6, IPV6_MULTICAST_IF,
                    (char *)&if_index, sizeof (if_index)))
                zsys_socket_error ("zbeacon: setsockopt IPV6_MULTICAST_IF failed");

            if (setsockopt (self->udpsock_send, IPPROTO_IPV6, IPV6_JOIN_GROUP,
                    (char *)&mreq, sizeof (mreq)))
                zsys_socket_error ("zbeacon: setsockopt IPV6_JOIN_GROUP failed");

            if (setsockopt (self->udpsock_send, IPPROTO_IPV6, IPV6_MULTICAST_IF,
                    (char *)&if_index, sizeof (if_index)))
                zsys_socket_error ("zbeacon: setsockopt IPV6_MULTICAST_IF failed");
        }
        else if(enable_multicast){
            struct ip_mreq mreq;
            memcpy (&mreq.imr_interface,
                    &(((inaddr_t *) (bind_to->ai_addr))->sin_addr),
                    sizeof (struct in_addr));
            memcpy (&mreq.imr_multiaddr,
                    &(((inaddr_t *) (send_to->ai_addr))->sin_addr),
                    sizeof (struct in_addr));

            if (setsockopt (self->udpsock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                            (char *) &mreq, sizeof (mreq)))
                zsys_socket_error (
                  "zbeacon: setsockopt IP_ADD_MEMBERSHIP failed");

            if (setsockopt (self->udpsock, IPPROTO_IP, IP_MULTICAST_IF,
                            (char *) &if_index, sizeof (if_index)))
                zsys_socket_error (
                  "zbeacon: setsockopt IP_MULTICAST_IF failed");

            if (setsockopt (self->udpsock_send, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                            (char *) &mreq, sizeof (mreq)))
                zsys_socket_error (
                  "zbeacon: setsockopt IP_ADD_MEMBERSHIP failed");

            if (setsockopt (self->udpsock_send, IPPROTO_IP, IP_MULTICAST_IF,
                            (char *) &if_index, sizeof (if_index)))
                zsys_socket_error (
                  "zbeacon: setsockopt IP_MULTICAST_IF failed");
        }

        //  If bind fails, we close the socket for opening again later (next poll interval)
        if (bind (self->udpsock_send, bind_to->ai_addr, bind_to->ai_addrlen) ||
                bind (self->udpsock, (struct sockaddr *)&bind_address,
                zsys_ipv6 () ? sizeof (in6addr_t) : sizeof (inaddr_t))) {
            zsys_debug ("zbeacon: Unable to bind to broadcast address, reason=%s", strerror (errno));
            zsys_udp_close (self->udpsock);
            self->udpsock = INVALID_SOCKET;
            zsys_udp_close (self->udpsock_send);
            self->udpsock_send = INVALID_SOCKET;
        }
        else if (streq (iface, "*")) {
            strcpy(self->hostname, "*");
            if (self->verbose)
                zsys_info ("zbeacon: configured, hostname=%s", self->hostname);
        }
        else if (getnameinfo (bind_to->ai_addr, bind_to->ai_addrlen,
                        self->hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
            if (self->verbose)
                zsys_info ("zbeacon: configured, hostname=%s", self->hostname);
        }
    }
    else
    {
        //  No valid interface. Close the socket so that we can try again later
        zsys_udp_close(self->udpsock);
        self->udpsock = INVALID_SOCKET;
        zsys_udp_close (self->udpsock_send);
        self->udpsock_send = INVALID_SOCKET;
    }

    freeaddrinfo (bind_to);
    freeaddrinfo (send_to);
}


//  --------------------------------------------------------------------------
//  Prepare beacon to work on specified UPD port, reply hostname to
//  pipe (or "" if this failed)

static void
s_self_configure (self_t *self, int port_nbr)
{
    assert (port_nbr);
    snprintf (self->port_nbr, 7, "%d", port_nbr);
    s_self_prepare_udp (self);
    zstr_send (self->pipe, self->hostname);
    if (streq (self->hostname, ""))
        zsys_error ("No broadcast interface found, (ZSYS_INTERFACE=%s)", zsys_interface ());
}


//  --------------------------------------------------------------------------
//  Handle a command from calling application

static int
s_self_handle_pipe (self_t *self)
{
    //  Get just the command off the pipe
    char *command = zstr_recv (self->pipe);
    if (!command)
        return -1;                  //  Interrupted

    if (self->verbose)
        zsys_info ("zbeacon: API command=%s", command);

    if (streq (command, "VERBOSE"))
        self->verbose = true;
    else
    if (streq (command, "CONFIGURE")) {
        int port;
        int rc = zsock_recv (self->pipe, "i", &port);
        assert (rc == 0);
        s_self_configure (self, port);
    }
    else
    if (streq (command, "PUBLISH")) {
        zframe_destroy (&self->transmit);
        zsock_recv (self->pipe, "fi", &self->transmit, &self->interval);
        assert (zframe_size (self->transmit) <= UDP_FRAME_MAX);
        if (self->interval == 0)
            self->interval = INTERVAL_DFLT;
        //  Start broadcasting immediately
        self->ping_at = zclock_mono ();
    }
    else
    if (streq (command, "SILENCE"))
        zframe_destroy (&self->transmit);
    else
    if (streq (command, "SUBSCRIBE")) {
        zframe_destroy (&self->filter);
        self->filter = zframe_recv (self->pipe);
        assert (zframe_size (self->filter) <= UDP_FRAME_MAX);
    }
    else
    if (streq (command, "UNSUBSCRIBE"))
        zframe_destroy (&self->filter);
    else
    if (streq (command, "$TERM"))
        self->terminated = true;
    else {
        zsys_error ("zbeacon: - invalid command: %s", command);
        assert (false);
    }
    zstr_free (&command);
    return 0;
}


//  --------------------------------------------------------------------------
//  Receive and filter the waiting beacon

static void
s_self_handle_udp (self_t *self)
{
    assert (self);

    char peername [NI_MAXHOST];
    zframe_t *frame = zsys_udp_recv (self->udpsock, peername, NI_MAXHOST);
    if (!frame)
        return;

    //  If filter is set, check that beacon matches it
    bool is_valid = false;
    if (self->filter) {
        byte  *filter_data = zframe_data (self->filter);
        size_t filter_size = zframe_size (self->filter);
        if (zframe_size (frame) >= filter_size
        && memcmp (zframe_data (frame), filter_data, filter_size) == 0)
            is_valid = true;
    }
    //  If valid, discard our own broadcasts, which UDP echoes to us
    if (is_valid && self->transmit) {
        byte  *transmit_data = zframe_data (self->transmit);
        size_t transmit_size = zframe_size (self->transmit);
        if (zframe_size (frame) == transmit_size
        && memcmp (zframe_data (frame), transmit_data, transmit_size) == 0)
            is_valid = false;
    }
    //  If still a valid beacon, send on to the API
    if (is_valid) {
        zmsg_t *msg = zmsg_new ();
        assert (msg);
        zmsg_addstr (msg, peername);
        zmsg_append (msg, &frame);
        if (zmsg_send (&msg, self->pipe) < 0)
            zmsg_destroy (&msg);
    }
    else
        zframe_destroy (&frame);
}


//  --------------------------------------------------------------------------
//  Send the beacon over UDP

static int
s_emit_beacon (self_t *self)
{
#if defined (__WINDOWS__)
    // Windows doesn't broadcast on all interfaces when using INADDR_BROADCAST
    // only the interface with the highest metric (as seen in `route print`)
    // so send a packet per interface to each broadcast address
    if (streq (zsys_interface (), "*") && !zsys_ipv6 ()) {
        INTERFACE_INFO interface_list [64];
        DWORD bytes_received = 0;

        int rc = WSAIoctl (self->udpsock, SIO_GET_INTERFACE_LIST, 0, 0,
            &interface_list, sizeof (interface_list), &bytes_received, NULL, NULL);
        assert (rc != SOCKET_ERROR);

        int num_interfaces = bytes_received / sizeof (INTERFACE_INFO);

        // iiBroadcastAddress is always 255.255.255.255 need to calculate the specific broadcast address using the netmask
        // keep the same parameters as self->broadcast but just replace the address for each interface
        inaddr_t addr;
        memcpy(&addr, &self->broadcast, sizeof (inaddr_t));

        for (int i = 0; i < num_interfaces; ++i)
        {
            addr.sin_addr.S_un.S_addr = (interface_list[i].iiAddress.AddressIn.sin_addr.S_un.S_addr
                | ~(interface_list[i].iiNetmask.AddressIn.sin_addr.S_un.S_addr));

            if (zsys_udp_send (self->udpsock_send, self->transmit,
                (inaddr_t *)&addr, sizeof (inaddr_t))) {
                // Send failed, cause zbeacon to re-init socket
                return -1;
            }
        }
        return 0;
    }
#endif

    return zsys_udp_send (self->udpsock_send, self->transmit,
        (inaddr_t *)&self->broadcast,
        zsys_ipv6 () ? sizeof (in6addr_t) : sizeof (inaddr_t));
}


//  --------------------------------------------------------------------------
//  zbeacon() implements the zbeacon actor interface

void
zbeacon (zsock_t *pipe, void *args)
{
    self_t *self = s_self_new (pipe);
    assert (self);
    //  Signal successful initialization
    zsock_signal (pipe, 0);

    while (!self->terminated) {
        //  Poll on API pipe and on UDP socket
        zmq_pollitem_t pollitems [] = {
            { zsock_resolve (self->pipe), 0, ZMQ_POLLIN, 0 },
            { NULL, self->udpsock, ZMQ_POLLIN, 0 }
        };
        long timeout = -1;
        if (self->transmit) {
            timeout = (long) (self->ping_at - zclock_mono ());
            if (timeout < 0)
                timeout = 0;
        }
        int pollset_size = (self->udpsock && self->udpsock != INVALID_SOCKET) ? 2: 1;
        if (zmq_poll (pollitems, pollset_size, timeout * ZMQ_POLL_MSEC) == -1)
            break;              //  Interrupted

        if (pollitems [0].revents & ZMQ_POLLIN)
            s_self_handle_pipe (self);
        if (pollitems [1].revents & ZMQ_POLLIN)
            s_self_handle_udp (self);

        if (self->transmit
        &&  zclock_mono () >= self->ping_at) {
            //  Send beacon to any listening peers
            if (!self->udpsock_send || self->udpsock_send == INVALID_SOCKET || s_emit_beacon(self))
            {
                const char *reason = (!self->udpsock_send || self->udpsock_send == INVALID_SOCKET) ? "invalid socket" : strerror (errno);
                zsys_debug ("zbeacon: failed to transmit, attempting reconnection. reason=%s", reason);
                //  Try to recreate UDP socket on interface
                s_self_prepare_udp (self);
            }
            self->ping_at = zclock_mono () + self->interval;
        }
    }
    s_self_destroy (&self);
}

//  --------------------------------------------------------------------------
//  Selftest Multicast

static void zbeacon_ipv4_mcast_test (bool verbose)
{
    printf (" * zbeacon_ipv4_mcast: ");
    if (verbose)
        printf ("\n");

    //  Before starting test configure beacon for ipv4 multicast
    zsys_set_ipv4_mcast_address ("239.0.0.0");

    //  Check that udp multicast is enabled
    {
        SOCKET sock = zsys_udp_new (true);
        if (sock == INVALID_SOCKET) {
            //  multicast may not be enabled during test so skip!
            printf ("SKIPPED - Is IPv4 UDP multicast allowed?\n");
            //  Unset multicast
            zsys_set_ipv4_mcast_address (NULL);
            return;
        }
        unsigned int if_index = 1;
        if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&if_index,
                sizeof(if_index)) != 0){
            //  multicast may not be enabled during test so skip!
            printf ("SKIPPED - Is IPv4 UDP multicast allowed?\n");
            zsys_udp_close (sock);
            //  Unset multicast
            zsys_set_ipv4_mcast_address (NULL);
            return;
        }
        zsys_udp_close (sock);
    }

    //  @selftest
    //  Test 1 - two beacons, one speaking, one listening
    //  Create speaker beacon to broadcast our service
    zactor_t *speaker = zactor_new (zbeacon, NULL);
    assert (speaker);
    if (verbose)
        zstr_sendx (speaker, "VERBOSE", NULL);

    zsock_send (speaker, "si", "CONFIGURE", 9999);
    char *hostname = zstr_recv (speaker);
    if (!*hostname) {
        printf ("OK (skipping test, no UDP broadcasting)\n");
        zactor_destroy (&speaker);
        freen (hostname);
        return;
    }
    freen (hostname);

    //  Create listener beacon on port 9999 to lookup service
    zactor_t *listener = zactor_new (zbeacon, NULL);
    assert (listener);
    if (verbose)
        zstr_sendx (listener, "VERBOSE", NULL);
    zsock_send (listener, "si", "CONFIGURE", 9999);
    hostname = zstr_recv (listener);
    assert (*hostname);
    freen (hostname);

    //  We will broadcast the magic value 0xCAFE
    byte announcement [2] = { 0xCA, 0xFE };
    zsock_send (speaker, "sbi", "PUBLISH", announcement, 2, 100);
    //  We will listen to anything (empty subscription)
    zsock_send (listener, "sb", "SUBSCRIBE", "", 0);

    //  Wait for at most 1/2 second if there's no broadcasting
    zsock_set_rcvtimeo (listener, 500);
    char *ipaddress = zstr_recv (listener);
    if (ipaddress) {
        zframe_t *content = zframe_recv (listener);
        assert (zframe_size (content) == 2);
        assert (zframe_data (content) [0] == 0xCA);
        assert (zframe_data (content) [1] == 0xFE);
        zframe_destroy (&content);
        zstr_free (&ipaddress);
        zstr_sendx (speaker, "SILENCE", NULL);
    }
    zactor_destroy (&listener);
    zactor_destroy (&speaker);

    //  Test subscription filter using a 3-node setup
    zactor_t *node1 = zactor_new (zbeacon, NULL);
    assert (node1);
    zsock_send (node1, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node1);
    assert (*hostname);
    freen (hostname);

    zactor_t *node2 = zactor_new (zbeacon, NULL);
    assert (node2);
    zsock_send (node2, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node2);
    assert (*hostname);
    freen (hostname);

    zactor_t *node3 = zactor_new (zbeacon, NULL);
    assert (node3);
    zsock_send (node3, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node3);
    assert (*hostname);
    freen (hostname);

    zsock_send (node1, "sbi", "PUBLISH", "NODE/1", 6, 250);
    zsock_send (node2, "sbi", "PUBLISH", "NODE/2", 6, 250);
    zsock_send (node3, "sbi", "PUBLISH", "RANDOM", 6, 250);
    zsock_send (node1, "sb", "SUBSCRIBE", "NODE", 4);

    //  Poll on three API sockets at once
    zpoller_t *poller = zpoller_new (node1, node2, node3, NULL);
    assert (poller);
    int64_t stop_at = zclock_mono () + 1000;
    while (zclock_mono () < stop_at) {
        long timeout = (long) (stop_at - zclock_mono ());
        if (timeout < 0)
            timeout = 0;
        void *which = zpoller_wait (poller, timeout);
        if (which) {
            assert (which == node1);
            char *ipaddress, *received;
            zstr_recvx (node1, &ipaddress, &received, NULL);
            assert (streq (received, "NODE/2"));
            zstr_free (&ipaddress);
            zstr_free (&received);
        }
    }
    zpoller_destroy (&poller);

    //  Stop listening
    zstr_sendx (node1, "UNSUBSCRIBE", NULL);

    //  Stop all node broadcasts
    zstr_sendx (node1, "SILENCE", NULL);
    zstr_sendx (node2, "SILENCE", NULL);
    zstr_sendx (node3, "SILENCE", NULL);

    //  Destroy the test nodes
    zactor_destroy (&node1);
    zactor_destroy (&node2);
    zactor_destroy (&node3);

    //  Unset multicast
    zsys_set_ipv4_mcast_address (NULL);

    //  @end
    printf ("OK\n");
}


//  --------------------------------------------------------------------------
//  Selftest

void
zbeacon_test (bool verbose)
{
    zbeacon_ipv4_mcast_test (verbose);

    printf (" * zbeacon: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    //  Test 1 - two beacons, one speaking, one listening
    //  Create speaker beacon to broadcast our service
    zactor_t *speaker = zactor_new (zbeacon, NULL);
    assert (speaker);
    if (verbose)
        zstr_sendx (speaker, "VERBOSE", NULL);

    zsock_send (speaker, "si", "CONFIGURE", 9999);
    char *hostname = zstr_recv (speaker);
    if (!*hostname) {
        printf ("OK (skipping test, no UDP broadcasting)\n");
        zactor_destroy (&speaker);
        freen (hostname);
        return;
    }
    freen (hostname);

    //  Create listener beacon on port 9999 to lookup service
    zactor_t *listener = zactor_new (zbeacon, NULL);
    assert (listener);
    if (verbose)
        zstr_sendx (listener, "VERBOSE", NULL);
    zsock_send (listener, "si", "CONFIGURE", 9999);
    hostname = zstr_recv (listener);
    assert (*hostname);
    freen (hostname);

    //  We will broadcast the magic value 0xCAFE
    byte announcement [2] = { 0xCA, 0xFE };
    zsock_send (speaker, "sbi", "PUBLISH", announcement, 2, 100);
    //  We will listen to anything (empty subscription)
    zsock_send (listener, "sb", "SUBSCRIBE", "", 0);

    //  Wait for at most 1/2 second if there's no broadcasting
    zsock_set_rcvtimeo (listener, 500);
    char *ipaddress = zstr_recv (listener);
    if (ipaddress) {
        zframe_t *content = zframe_recv (listener);
        assert (zframe_size (content) == 2);
        assert (zframe_data (content) [0] == 0xCA);
        assert (zframe_data (content) [1] == 0xFE);
        zframe_destroy (&content);
        zstr_free (&ipaddress);
        zstr_sendx (speaker, "SILENCE", NULL);
    }
    zactor_destroy (&listener);
    zactor_destroy (&speaker);

    //  Test subscription filter using a 3-node setup
    zactor_t *node1 = zactor_new (zbeacon, NULL);
    assert (node1);
    zsock_send (node1, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node1);
    assert (*hostname);
    freen (hostname);

    zactor_t *node2 = zactor_new (zbeacon, NULL);
    assert (node2);
    zsock_send (node2, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node2);
    assert (*hostname);
    freen (hostname);

    zactor_t *node3 = zactor_new (zbeacon, NULL);
    assert (node3);
    zsock_send (node3, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node3);
    assert (*hostname);
    freen (hostname);

    zsock_send (node1, "sbi", "PUBLISH", "NODE/1", 6, 250);
    zsock_send (node2, "sbi", "PUBLISH", "NODE/2", 6, 250);
    zsock_send (node3, "sbi", "PUBLISH", "RANDOM", 6, 250);
    zsock_send (node1, "sb", "SUBSCRIBE", "NODE", 4);

    //  Poll on three API sockets at once
    zpoller_t *poller = zpoller_new (node1, node2, node3, NULL);
    assert (poller);
    int64_t stop_at = zclock_mono () + 1000;
    while (zclock_mono () < stop_at) {
        long timeout = (long) (stop_at - zclock_mono ());
        if (timeout < 0)
            timeout = 0;
        void *which = zpoller_wait (poller, timeout);
        if (which) {
            assert (which == node1);
            char *ipaddress, *received;
            zstr_recvx (node1, &ipaddress, &received, NULL);
            assert (streq (received, "NODE/2"));
            zstr_free (&ipaddress);
            zstr_free (&received);
        }
    }
    zpoller_destroy (&poller);

    //  Stop listening
    zstr_sendx (node1, "UNSUBSCRIBE", NULL);

    //  Stop all node broadcasts
    zstr_sendx (node1, "SILENCE", NULL);
    zstr_sendx (node2, "SILENCE", NULL);
    zstr_sendx (node3, "SILENCE", NULL);

    //  Destroy the test nodes
    zactor_destroy (&node1);
    zactor_destroy (&node2);
    zactor_destroy (&node3);

#if defined (__WINDOWS__)
    zsys_shutdown();
#endif
    //  @end
    printf ("OK\n");
}
