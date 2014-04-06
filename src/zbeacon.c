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
    received asynchronously in the background. The zbeacon API provides a
    incoming beacons on a ZeroMQ socket (the pipe) that you can configure,
    poll on, and receive messages on. Incoming beacons are always delivered
    as two frames: the ipaddress of the sender (a string), and the beacon
    data itself (binary, as published).
@discuss
@end
*/

#include "../include/czmq.h"
#include "platform.h"
#if defined (__UNIX__) && defined (HAVE_GETIFADDRS) && defined (HAVE_FREEIFADDRS)
#include <net/if.h>
#endif

//  Constants
#define INTERVAL_DFLT  1000         //  Default interval = 1 second

//  Structure of our class
struct _zbeacon_t {
    void *pipe;                 //  Pipe through to backend agent
    char *hostname;             //  Our own address as string
};


//  Background task does the real I/O
static void
    s_agent_task (void *args, zctx_t *ctx, void *pipe);


//  --------------------------------------------------------------------------
//  Create a new beacon

zbeacon_t *
zbeacon_new (zctx_t *ctx, int port_nbr)
{
    zbeacon_t *self = (zbeacon_t *) zmalloc (sizeof (zbeacon_t));
    assert (self);

    //  Start background agent and wait for it to initialize
    assert (ctx);
    self->pipe = zthread_fork (ctx, s_agent_task, NULL);
    if (self->pipe) {
        zstr_sendf (self->pipe, "%d", port_nbr);
        self->hostname = zstr_recv (self->pipe);
    }
    else {
        free (self);
        self = NULL;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zbeacon_destroy (zbeacon_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zbeacon_t *self = *self_p;
        zstr_send (self->pipe, "TERMINATE");
        char *reply = zstr_recv (self->pipe);
        zstr_free (&reply);
        free (self->hostname);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Set broadcast interval in milliseconds

void
zbeacon_set_interval (zbeacon_t *self, int interval)
{
    assert (self);
    zstr_sendm (self->pipe, "INTERVAL");
    zstr_sendf (self->pipe, "%d", interval);
}


//  --------------------------------------------------------------------------
//  Filter out any beacon that looks exactly like ours

void
zbeacon_noecho (zbeacon_t *self)
{
    assert (self);
    zstr_send (self->pipe, "NOECHO");
}


//  --------------------------------------------------------------------------
//  Start broadcasting beacon to peers at the specified interval

void
zbeacon_publish (zbeacon_t *self, byte *transmit, size_t size)
{
    assert (self);
    assert (transmit);
    assert (size > 0 && size <= UDP_FRAME_MAX);
    zmsg_t *msg = zmsg_new ();
    zmsg_addstr (msg, "PUBLISH");
    zmsg_addmem (msg, transmit, size);
    zmsg_send (&msg, self->pipe);
}


//  --------------------------------------------------------------------------
//  Stop broadcasting beacons

void
zbeacon_silence (zbeacon_t *self)
{
    assert (self);
    zstr_send (self->pipe, "SILENCE");
}


//  --------------------------------------------------------------------------
//  Start listening to other peers; zero-sized filter means get everything

void
zbeacon_subscribe (zbeacon_t *self, byte *filter, size_t size)
{
    assert (self);
    assert (size <= UDP_FRAME_MAX);
    zmsg_t *msg = zmsg_new ();
    zmsg_addstr (msg, "SUBSCRIBE");
    zmsg_addmem (msg, filter, size);
    zmsg_send (&msg, self->pipe);
}


//  --------------------------------------------------------------------------
//  Stop listening to other peers

void
zbeacon_unsubscribe (zbeacon_t *self)
{
    zstr_send (self->pipe, "UNSUBSCRIBE");
}


//  --------------------------------------------------------------------------
//  Get beacon ZeroMQ socket, for polling or receiving messages

void *
zbeacon_socket (zbeacon_t *self)
{
    assert (self);
    return self->pipe;
}


//  --------------------------------------------------------------------------
//  Return our own IP address as printable string

char *
zbeacon_hostname (zbeacon_t *self)
{
    assert (self);
    return self->hostname;
}


//  --------------------------------------------------------------------------
//  Self test of this class
//
// If this test fails with an unable to bind error, try specifying the default
// interface to be used for zbeacon with the ZSYS_INTERFACE environment
// variable.

void
zbeacon_test (bool verbose)
{
    printf (" * zbeacon: ");

    //  @selftest
    //  Basic test: create a service and announce it
    zctx_t *ctx = zctx_new ();

    //  Create a service socket and bind to an ephemeral port
    void *service = zsocket_new (ctx, ZMQ_PUB);
    int port_nbr = zsocket_bind (service, "tcp://*:*");
    
    //  Create beacon to broadcast our service
    byte announcement [2] = { (port_nbr >> 8) & 0xFF, port_nbr & 0xFF };
    zbeacon_t *service_beacon = zbeacon_new (ctx, 9999);
    zbeacon_set_interval (service_beacon, 100);
    zbeacon_publish (service_beacon, announcement, 2);

    //  Create beacon to lookup service
    zbeacon_t *client_beacon = zbeacon_new (ctx, 9999);
    zbeacon_subscribe (client_beacon, NULL, 0);

    //  Wait for at most 1/2 second if there's no broadcast networking
    zsocket_set_rcvtimeo (zbeacon_socket (client_beacon), 500);

    char *ipaddress = zstr_recv (zbeacon_socket (client_beacon));
    if (ipaddress) {
        zframe_t *content = zframe_recv (zbeacon_socket (client_beacon));
        int received_port = (zframe_data (content) [0] << 8)
                        +  zframe_data (content) [1];
        assert (received_port == port_nbr);
        zframe_destroy (&content);
        zbeacon_silence (service_beacon);
        zstr_free (&ipaddress);
    }
    zbeacon_destroy (&client_beacon);
    zbeacon_destroy (&service_beacon);
    
    zbeacon_t *node1 = zbeacon_new (ctx, 5670);
    zbeacon_t *node2 = zbeacon_new (ctx, 5670);
    zbeacon_t *node3 = zbeacon_new (ctx, 5670);

    assert (*zbeacon_hostname (node1));
    assert (*zbeacon_hostname (node2));
    assert (*zbeacon_hostname (node3));
    
    zbeacon_set_interval (node1, 250);
    zbeacon_set_interval (node2, 250);
    zbeacon_set_interval (node3, 250);
    zbeacon_noecho (node1);
    zbeacon_publish (node1, (byte *) "NODE/1", 6);
    zbeacon_publish (node2, (byte *) "NODE/2", 6);
    zbeacon_publish (node3, (byte *) "GARBAGE", 7);
    zbeacon_subscribe (node1, (byte *) "NODE", 4);

    //  Poll on three API sockets at once
    zpoller_t *poller = zpoller_new (
        zbeacon_socket (node1), 
        zbeacon_socket (node2), 
        zbeacon_socket (node3), NULL);
        
    int64_t stop_at = zclock_time () + 1000;
    while (zclock_time () < stop_at) {
        long timeout = (long) (stop_at - zclock_time ());
        if (timeout < 0)
            timeout = 0;
        void *which = zpoller_wait (poller, timeout * ZMQ_POLL_MSEC);
        if (which) {
            assert (which == zbeacon_socket (node1));
            char *ipaddress, *beacon;
            zstr_recvx (zbeacon_socket (node1), &ipaddress, &beacon, NULL);
            assert (streq (beacon, "NODE/2"));
            zstr_free (&ipaddress);
            zstr_free (&beacon);
        }
    }
    zpoller_destroy (&poller);
    
    //  Stop listening
    zbeacon_unsubscribe (node1);
    
    //  Stop all node broadcasts
    zbeacon_silence (node1);
    zbeacon_silence (node2);
    zbeacon_silence (node3);

    //  Destroy the test nodes
    zbeacon_destroy (&node1);
    zbeacon_destroy (&node2);
    zbeacon_destroy (&node3);
    
    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
}


//  --------------------------------------------------------------------------
//  Backend agent implementation

//  Agent instance
    
typedef struct {
    void *pipe;                 //  Socket to talk back to application
    SOCKET udpsock;             //  UDP socket for send/recv
    int port_nbr;               //  UDP port number we work on
    int interval;               //  Beacon broadcast interval
    bool enabled;               //  Are we broadcasting?
    bool noecho;                //  Ignore own (unique) beacons?
    bool terminated;            //  API shut us down
    int64_t ping_at;            //  Next broadcast time
    zframe_t *transmit;         //  Beacon transmit data
    zframe_t *filter;           //  Beacon filter data
    inaddr_t address;           //  Our own address
    inaddr_t broadcast;         //  Our broadcast address
} agent_t;

//  Prototypes for local functions we use in the agent

static agent_t *
    s_agent_new (void *pipe, int port_nbr);
static void
    s_agent_destroy (agent_t **self_p);
static void
    s_get_interface (agent_t *self);
static void
    s_api_command (agent_t *self);
static void
    s_beacon_recv (agent_t *self);


//  This is the background task

static void
s_agent_task (void *args, zctx_t *ctx, void *pipe)
{
    //  Get port argument from caller
    char *port_str = zstr_recv (pipe);
    assert (port_str);

    //  Create agent instance
    agent_t *self = s_agent_new (pipe, atoi (port_str));
    zstr_free (&port_str);
    if (!self)
        return;                 //  Give up if we couldn't initialize

    while (!self->terminated) {
        //  Poll on API pipe and on UDP socket
        zmq_pollitem_t pollitems [] = {
            { self->pipe, 0, ZMQ_POLLIN, 0 },
            { NULL, self->udpsock, ZMQ_POLLIN, 0 }
        };
        long timeout = -1;
        if (self->transmit) {
            timeout = (long) (self->ping_at - zclock_time ());
            if (timeout < 0)
                timeout = 0;
        }
        if (zmq_poll (pollitems, 2, timeout * ZMQ_POLL_MSEC) == -1)
            break;              //  Interrupted

        if (pollitems [0].revents & ZMQ_POLLIN)
            s_api_command (self);
        if (pollitems [1].revents & ZMQ_POLLIN)
            s_beacon_recv (self);

        if (self->transmit
        &&  zclock_time () >= self->ping_at) {
            //  Send beacon to any listening peers
            zsys_udp_send (self->udpsock, self->transmit, &self->broadcast);
            self->ping_at = zclock_time () + self->interval;
        }
    }
    s_agent_destroy (&self);
}


//  Create and initialize new agent instance

static agent_t *
s_agent_new (void *pipe, int port_nbr)
{
    agent_t *self = (agent_t *) zmalloc (sizeof (agent_t));
    assert (self);
    
    self->pipe = pipe;
    self->port_nbr = port_nbr;
    self->interval = INTERVAL_DFLT;

    //  Create our UDP socket
    self->udpsock = zsys_udp_new (false);
    if (self->udpsock == INVALID_SOCKET) {
        free (self);        //  No more file handles - forget it
        return NULL;
    }

    //  PROBLEM: this design will not survive the network interface
    //  being killed and restarted while the program is running
    //  Get the network interface
    s_get_interface (self);

    //  Bind to the port on all interfaces
#if (defined (__WINDOWS__))
    inaddr_t sockaddr = self->address;
#else
#ifndef gai_strerrorA
    #define gai_strerrorA gai_strerror
#endif
    inaddr_t sockaddr = self->broadcast;
#endif
    if (bind (self->udpsock, (struct sockaddr *) &sockaddr, sizeof (sockaddr)) == SOCKET_ERROR)
        zsys_socket_error ("bind");

    //  Send our hostname back to API
    //  PROBLEM: this hostname will not be accurate when the node
    //  has more than one active interface.
    char hostname [255];
    int rc = getnameinfo ((struct sockaddr *) &self->address, sizeof (self->address),
                           hostname, 255, NULL, 0, NI_NUMERICHOST);
    if (rc) {
        puts (gai_strerrorA (rc));
        strcpy (hostname, "127.0.0.1");
    }
    zstr_send (pipe, hostname);
    
    return self;
}


//  Get the actual network interface we're working on.
//  Currently implemented for POSIX and for Windows

static void
s_get_interface (agent_t *self)
{
#if defined (__UNIX__) && defined (HAVE_GETIFADDRS) && defined (HAVE_FREEIFADDRS)
    struct ifaddrs *interfaces;
    if (getifaddrs (&interfaces) == 0) {
        int num_interfaces = 0;
        struct ifaddrs *interface = interfaces;
        while (interface) {
            //  On Solaris, loopback interfaces have a NULL in ifa_broadaddr
            if  (interface->ifa_broadaddr
            && !(interface->ifa_flags & IFF_LOOPBACK)       //  Ignore loopback interface
            &&  (interface->ifa_flags & IFF_BROADCAST)      //  Only use interfaces that have BROADCAST
            && !(interface->ifa_flags & IFF_POINTOPOINT)    //  Ignore point to point interfaces.
            &&   interface->ifa_addr
            &&  (interface->ifa_addr->sa_family == AF_INET)) {
                self->address = *(inaddr_t *) interface->ifa_addr;
                self->broadcast = *(inaddr_t *) interface->ifa_broadaddr;
                self->broadcast.sin_port = htons (self->port_nbr);

                //  If the returned broadcast address is the same as source
                //  address, build the broadcast address from the source
                //  address and netmask.
                if (self->address.sin_addr.s_addr == self->broadcast.sin_addr.s_addr)
                    self->broadcast.sin_addr.s_addr
                        |= ~(((inaddr_t *) interface->ifa_netmask)->sin_addr.s_addr);

                //  Keep track of the number of interfaces on this host
                if (self->address.sin_addr.s_addr != ntohl (INADDR_LOOPBACK))
                    num_interfaces++;

                //  If this is the specified interface then move on
                if (streq (interface->ifa_name, zsys_interface ()))
                    break;
            }
            interface = interface->ifa_next;
        }
        if (strlen (zsys_interface ()) == 0) {
            if (num_interfaces == 0) {
                //  Subnet broadcast addresses don't work on some platforms 
                //  but is assumed to work if the interface is specified.
                self->broadcast.sin_family = AF_INET;
                self->broadcast.sin_addr.s_addr = INADDR_BROADCAST;
                self->broadcast.sin_port = htons (self->port_nbr);
            }
            else
            if (num_interfaces > 1) {
                //  Our source address is unknown in this case so set it to
                //  INADDR_ANY so self->hostname isn't set to an incorrect IP.
                self->address = self->broadcast;
                self->address.sin_addr.s_addr = INADDR_ANY;
            }
        }
    }
    freeifaddrs (interfaces);

#else
    //  Our default if we fail to find an interface
    self->broadcast.sin_family = AF_INET;
    self->broadcast.sin_addr.s_addr = INADDR_BROADCAST;
    self->broadcast.sin_port = htons (self->port_nbr);

    //  Our source address is unknown in this case so set it to
    //  INADDR_ANY so self->hostname isn't set to an incorrect IP.
    self->address = self->broadcast;
    self->address.sin_addr.s_addr = INADDR_ANY;
    if (strlen (zsys_interface ()) == 0)
        return;         //  Use defaults

#   if defined (__UNIX__)
    struct ifreq ifr;
    memset (&ifr, 0, sizeof (ifr));

    int sock = 0;
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
        return;         //  Use defaults

    //  Get interface address
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy (ifr.ifr_name, zsys_interface (), sizeof (ifr.ifr_name));
    int rc = ioctl (sock, SIOCGIFADDR, (caddr_t) &ifr, sizeof (struct ifreq));
    if (rc == -1)
        zsys_socket_error ("siocgifaddr");

    //  Get interface broadcast address
    memcpy (&self->address, ((inaddr_t *) &ifr.ifr_addr), sizeof (inaddr_t));
    rc = ioctl (sock, SIOCGIFBRDADDR, (caddr_t) &ifr, sizeof (struct ifreq));
    if (rc == -1)
        zsys_socket_error ("siocgifbrdaddr");

    memcpy (&self->broadcast, ((inaddr_t *) &ifr.ifr_broadaddr), sizeof (inaddr_t));
    close (sock);

#   elif defined (__WINDOWS__)
    ULONG addr_size = 0;
    DWORD rc = GetAdaptersAddresses (AF_INET,
        GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &addr_size);
    assert (rc == ERROR_BUFFER_OVERFLOW);

    PIP_ADAPTER_ADDRESSES pip_addresses = (PIP_ADAPTER_ADDRESSES) malloc (addr_size);
    rc = GetAdaptersAddresses (AF_INET,
        GAA_FLAG_INCLUDE_PREFIX, NULL, pip_addresses, &addr_size);
    assert (rc == NO_ERROR);

    PIP_ADAPTER_ADDRESSES cur_address = pip_addresses;
    while (cur_address) {
        PIP_ADAPTER_UNICAST_ADDRESS pUnicast = cur_address->FirstUnicastAddress;
        PIP_ADAPTER_PREFIX pPrefix = cur_address->FirstPrefix;

        if (pUnicast && pPrefix) {
            self->address = *(inaddr_t *)(pUnicast->Address.lpSockaddr);
            self->broadcast = *(inaddr_t *)(pPrefix->Address.lpSockaddr);
            self->broadcast.sin_addr.s_addr
                |= htonl ((1 << (32 - pPrefix->PrefixLength)) - 1);
        }
        cur_address = cur_address->Next;
    }
    free (pip_addresses);
#   else
#       error "Interface detection TBD on this operating system"
#   endif
#endif
}

//  Handle command from API

static void
s_api_command (agent_t *self)
{
    char *command = zstr_recv (self->pipe);
    if (streq (command, "INTERVAL")) {
        char *interval = zstr_recv (self->pipe);
        self->interval = atoi (interval);
        zstr_free (&interval);
    }
    else
    if (streq (command, "NOECHO"))
        self->noecho = true;
    else
    if (streq (command, "PUBLISH")) {
        zframe_destroy (&self->transmit);
        self->transmit = zframe_recv (self->pipe);
        assert (self->transmit);
        //  Start broadcasting immediately
        self->ping_at = zclock_time ();
    }
    else
    if (streq (command, "SILENCE"))
        zframe_destroy (&self->transmit);
    else
    if (streq (command, "SUBSCRIBE")) {
        zframe_destroy (&self->filter);
        self->filter = zframe_recv (self->pipe);
    }
    else
    if (streq (command, "UNSUBSCRIBE"))
        zframe_destroy (&self->filter);
    else
    if (streq (command, "TERMINATE")) {
        self->terminated = true;
        zstr_send (self->pipe, "OK");
    }
    else
        printf ("E: unexpected API command '%s'\n", command);
    
    zstr_free (&command);
}


//  Receive and filter the waiting beacon

static void
s_beacon_recv (agent_t *self)
{
    assert (self);

    char peername [INET_ADDRSTRLEN];
    zframe_t *frame = zsys_udp_recv (self->udpsock, peername);
    
    //  If filter is set, check that beacon matches it
    bool is_valid = false;
    if (self->filter) {
        byte  *filter_data = zframe_data (self->filter);
        size_t filter_size = zframe_size (self->filter);
        if (zframe_size (frame) >= filter_size
        && memcmp (zframe_data (frame), filter_data, filter_size) == 0)
            is_valid = true;
    }
    //  If valid, check for echoed beacons (i.e. our own broadcast)
    if (is_valid && self->noecho && self->transmit) {
        byte  *transmit_data = zframe_data (self->transmit);
        size_t transmit_size = zframe_size (self->transmit);
        if (zframe_size (frame) == transmit_size
        && memcmp (zframe_data (frame), transmit_data, transmit_size) == 0)
            is_valid = false;
    }
    //  If still a valid beacon, send on to the API
    if (is_valid) {
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, peername);
        zmsg_add (msg, frame);
        zmsg_send (&msg, self->pipe);
    }
    else
        zframe_destroy (&frame);
}


//  Destroy agent instance

static void
s_agent_destroy (agent_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        agent_t *self = *self_p;
        closesocket (self->udpsock);
        zframe_destroy (&self->transmit);
        zframe_destroy (&self->filter);
        free (self);
        *self_p = NULL;
    }
}
