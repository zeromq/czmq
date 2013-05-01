/*  =========================================================================
    zbeacon - LAN service announcement and discovery

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

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
    The zbeacon module implements a peer-to-peer discovery service for local
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

#if !defined(__WINDOWS__)
#   include "platform.h"
#endif

#if defined (HAVE_LINUX_WIRELESS_H)
#   include <linux/wireless.h>
#else
#   if defined (HAVE_NET_IF_H)
#       include <net/if.h>
#   endif
#   if defined (HAVE_NET_IF_MEDIA_H)
#       include <net/if_media.h>
#   endif
#endif

#if defined(__UTYPE_SUNSOLARIS) || defined(__UTYPE_SUNOS)
#   include<sys/sockio.h>
#endif

#if defined (__WINDOWS__)
#   if (_WIN32_WINNT < 0x0501)
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0501
#   endif
#   include <ws2tcpip.h>            //  For getnameinfo ()
#   include <iphlpapi.h>            //  For GetAdaptersAddresses ()
#endif

//  Basic WinSock compatibility
#if !defined(__WINDOWS__)
typedef int SOCKET;
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

//  Constants
#define BEACON_MAX      255         //  Max size of beacon data
#define INTERVAL_DFLT  1000         //  Default interval = 1 second

//  Internet socket address structure
typedef struct sockaddr_in inaddr_t;

//  Structure of our class
struct _zbeacon_t {
    zctx_t *ctx;                //  Private 0MQ context
    void *pipe;                 //  Pipe through to backend agent
    char *hostname;             //  Our own address as string
};


//  Background task does the real I/O
static void
    s_agent_task (void *args, zctx_t *ctx, void *pipe);


//  --------------------------------------------------------------------------
//  Create a new beacon

zbeacon_t *
zbeacon_new (int port_nbr)
{
    zbeacon_t *self = (zbeacon_t *) zmalloc (sizeof (zbeacon_t));

    //  For now, we use a context per beacon instance
    self->ctx = zctx_new ();

    //  Start beacon background agent
    self->pipe = zthread_fork (self->ctx, s_agent_task, NULL);
    
    //  Configure agent with arguments
    zstr_send (self->pipe, "%d", port_nbr);

    //  Agent replies with our host name
    self->hostname = zstr_recv (self->pipe);

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
        free (zstr_recv (self->pipe));
        zctx_destroy (&self->ctx);
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
    zstr_send (self->pipe, "%d", interval);
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
    assert (size > 0 && size <= BEACON_MAX);
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
    assert (size <= BEACON_MAX);
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
//  Get beacon socket handle, for polling

void *
zbeacon_pipe (zbeacon_t *self)
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

void
zbeacon_test (bool verbose)
{
    printf (" * zbeacon: ");

    //  Basic test: create a service and announce it
    zctx_t *ctx = zctx_new ();

    //  Create a service socket and bind to an ephemeral port
    void *service = zsocket_new (ctx, ZMQ_PUB);
    int port_nbr = zsocket_bind (service, "tcp://*:*");
    
    //  Create beacon to broadcast our service
    byte announcement [2] = { (port_nbr >> 8) & 0xFF, port_nbr & 0xFF };
    zbeacon_t *service_beacon = zbeacon_new (9999);
    zbeacon_set_interval (service_beacon, 100);
    zbeacon_publish (service_beacon, announcement, 2);

    //  Create beacon to lookup service
    zbeacon_t *client_beacon = zbeacon_new (9999);
    zbeacon_subscribe (client_beacon, NULL, 0);

    //  Wait for at most 1/2 second if there's no broadcast networking
    zsocket_set_rcvtimeo (zbeacon_pipe (client_beacon), 500);

    char *ipaddress = zstr_recv (zbeacon_pipe (client_beacon));
    if (ipaddress) {
        zframe_t *content = zframe_recv (zbeacon_pipe (client_beacon));
        int received_port = (zframe_data (content) [0] << 8)
                        +  zframe_data (content) [1];
        assert (received_port == port_nbr);
        zframe_destroy (&content);
        free (ipaddress);
    }
    zbeacon_destroy (&client_beacon);
    zbeacon_destroy (&service_beacon);
    zctx_destroy (&ctx);
    
    //  @selftest
    zbeacon_t *node1 = zbeacon_new (5670);
    zbeacon_t *node2 = zbeacon_new (5670);
    zbeacon_t *node3 = zbeacon_new (5670);

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

    //  Poll on API pipe and on UDP socket
    zmq_pollitem_t pollitems [] = {
        { zbeacon_pipe (node1), 0, ZMQ_POLLIN, 0 },
        { zbeacon_pipe (node2), 0, ZMQ_POLLIN, 0 },
        { zbeacon_pipe (node3), 0, ZMQ_POLLIN, 0 }
    };
    uint64_t stop_at = zclock_time () + 1000;
    while (zclock_time () < stop_at) {
        long timeout = (long) (stop_at - zclock_time ());
        if (timeout < 0)
            timeout = 0;
        if (zmq_poll (pollitems, 3, timeout * ZMQ_POLL_MSEC) == -1)
            break;              //  Interrupted

        //  We cannot get messages on nodes 2 and 3
        assert ((pollitems [1].revents & ZMQ_POLLIN) == 0);
        assert ((pollitems [2].revents & ZMQ_POLLIN) == 0);

        //  If we get a message on node 1, it must be NODE/2
        if (pollitems [0].revents & ZMQ_POLLIN) {
            char *ipaddress = zstr_recv (zbeacon_pipe (node1));
            char *beacon = zstr_recv (zbeacon_pipe (node1));
            assert (streq (beacon, "NODE/2"));
            free (ipaddress);
            free (beacon);
        }
    }
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
    uint64_t ping_at;           //  Next broadcast time
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
    s_handle_io_error (char *reason);
static void
    s_get_interface (agent_t *self);
static bool
    s_wireless_nic (const char* name);
static void
    s_api_command (agent_t *self);
static void
    s_beacon_send (agent_t *self);
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
    free (port_str);

    while (!zctx_interrupted) {
        //  Poll on API pipe and on UDP socket
        zmq_pollitem_t pollitems [] = {
            { self->pipe, 0, ZMQ_POLLIN, 0 },
            { 0, self->udpsock, ZMQ_POLLIN, 0 }
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
            s_beacon_send (self);
            self->ping_at = zclock_time () + self->interval;
        }
        if (self->terminated)
            break;
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
    self->udpsock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (self->udpsock == INVALID_SOCKET)
        s_handle_io_error ("socket");

    //  Ask operating system to let us do broadcasts from socket
    int on = 1;
    if (setsockopt (self->udpsock, SOL_SOCKET, SO_BROADCAST,
                   (char *) &on, sizeof (on)) == SOCKET_ERROR)
        s_handle_io_error ("setsockopt (SO_BROADCAST)");

    //  Allow multiple owners to bind to socket; incoming
    //  messages will replicate to each owner
    if (setsockopt (self->udpsock, SOL_SOCKET, SO_REUSEADDR,
                   (char *) &on, sizeof (on)) == SOCKET_ERROR)
        s_handle_io_error ("setsockopt (SO_REUSEADDR)");

#if defined (SO_REUSEPORT)
    //  On some platforms we have to ask to reuse the port
    if (setsockopt (self->udpsock, SOL_SOCKET, SO_REUSEPORT,
                    (char *) &on, sizeof (on)) == SOCKET_ERROR)
        s_handle_io_error ("setsockopt (SO_REUSEPORT)");
#endif
    //  PROBLEM: this design will not survive the network interface
    //  being killed and restarted while the program is running

    //  Bind to the port on all interfaces
    inaddr_t sockaddr = { 0 };
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons (self->port_nbr);
    sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    if (bind (self->udpsock, (struct sockaddr *) &sockaddr, sizeof (sockaddr)) == SOCKET_ERROR)
        s_handle_io_error ("bind");

    //  Get the network interface
    s_get_interface (self);

    //  Send our hostname back to API
    char hostname [INET_ADDRSTRLEN];
    getnameinfo ((struct sockaddr *) &self->address, sizeof (self->address),
                 hostname, INET_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);
    zstr_send (pipe, hostname);
    
    return self;
}


//  Handle error from I/O operation

static void
s_handle_io_error (char *reason)
{
#if defined (__WINDOWS__)
    switch (WSAGetLastError ()) {
        case WSAEINTR:        errno = EINTR;      break;
        case WSAEBADF:        errno = EBADF;      break;
        case WSAEWOULDBLOCK:  errno = EAGAIN;     break;
        case WSAEINPROGRESS:  errno = EAGAIN;     break;
        case WSAENETDOWN:     errno = ENETDOWN;   break;
        case WSAECONNRESET:   errno = ECONNRESET; break;
        case WSAECONNABORTED: errno = EPIPE;      break;
        case WSAESHUTDOWN:    errno = ECONNRESET; break;
        case WSAEINVAL:       errno = EPIPE;      break;
        default:              errno = GetLastError ();
    }
#endif
    if (errno == EAGAIN
    ||  errno == ENETDOWN
    ||  errno == EHOSTUNREACH
    ||  errno == ENETUNREACH
    ||  errno == EINTR
    ||  errno == EPIPE
    ||  errno == ECONNRESET
#if !defined (__WINDOWS__)
    ||  errno == EPROTO
    ||  errno == ENOPROTOOPT
    ||  errno == EHOSTDOWN
    ||  errno == EOPNOTSUPP
    ||  errno == EWOULDBLOCK
#endif
#if defined (ENONET)
    ||  errno == ENONET
#endif
    )
        return;             //  Ignore error and try again
    else {
        zclock_log ("E: (UDP) error '%s' on %s", strerror (errno), reason);
        assert (false);
    }
}


//  Get the actual network interface we're working on
//  Currently implemented for POSIX and for Windows

static void
s_get_interface (agent_t *self)
{
#if defined (__UNIX__)
#   if defined (HAVE_GETIFADDRS) && defined (HAVE_FREEIFADDRS)
    struct ifaddrs *interfaces;
    if (getifaddrs (&interfaces) == 0) {
        struct ifaddrs *interface = interfaces;
        while (interface) {
            //  Hopefully the last interface will be WiFi or Ethernet
            if (interface->ifa_addr &&
                (interface->ifa_addr->sa_family == AF_INET)) {
                self->address = *(inaddr_t *) interface->ifa_addr;
                self->broadcast = *(inaddr_t *) interface->ifa_broadaddr;
                self->broadcast.sin_port = htons (self->port_nbr);
                if (s_wireless_nic (interface->ifa_name))
                    break;
            }
            interface = interface->ifa_next;
        }
    }
    freeifaddrs (interfaces);
#   else
    struct ifreq ifr;
    memset (&ifr, 0, sizeof (ifr));

#   if !defined (CZMQ_HAVE_ANDROID)
    //  TODO: Using hardcoded wlan0 is ugly
    if (!s_wireless_nic ("wlan0"))
        s_handle_io_error ("wlan0 not exist");
#   endif

    int sock = 0;
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
        s_handle_io_error ("socket");

    //  Get interface address
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy (ifr.ifr_name, "wlan0", sizeof (ifr.ifr_name));
    int rc = ioctl (sock, SIOCGIFADDR, (caddr_t) &ifr, sizeof (struct ifreq));
    if (rc == -1)
        s_handle_io_error ("siocgifaddr");

    //  Get interface broadcast address
    memcpy (&self->address, ((inaddr_t *) &ifr.ifr_addr),
        sizeof (inaddr_t));
    rc = ioctl (sock, SIOCGIFBRDADDR, (caddr_t) &ifr, sizeof (struct ifreq));
    if (rc == -1)
        s_handle_io_error ("siocgifbrdaddr");

    memcpy (&self->broadcast, ((inaddr_t *) &ifr.ifr_broadaddr), sizeof (inaddr_t));
    close (sock);
#   endif

#   elif defined (__WINDOWS__)
    //  Currently does not filter for wireless NIC
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
            self->broadcast.sin_addr.s_addr |= htonl ((1 << (32 - pPrefix->PrefixLength)) - 1);
        }
        cur_address = cur_address->Next;
    }
    free (pip_addresses);
#   else
#       error "Interface detection TBD on this operating system"
#   endif

    //  Set broadcast address and port
    self->broadcast.sin_addr.s_addr = INADDR_BROADCAST;
    self->broadcast.sin_port = htons (self->port_nbr);
}

//  Check if a given NIC name is wireless

static bool
s_wireless_nic (const char *name)
{
    SOCKET sock = socket (AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
        s_handle_io_error ("socket");

    bool is_nic = false;
#if defined (SIOCGIFMEDIA)
    struct ifmediareq ifmr;
    memset (&ifmr, 0, sizeof (struct ifmediareq));
    strncpy(ifmr.ifm_name, name, sizeof (ifmr.ifm_name));
    int res = ioctl (sock, SIOCGIFMEDIA, (caddr_t) &ifmr);
    if (res != -1)
        is_nic = (IFM_TYPE (ifmr.ifm_current) == IFM_IEEE80211);

#elif defined (SIOCGIWNAME)
    struct iwreq wrq;
    memset (&wrq, 0, sizeof (struct iwreq));
    strncpy (wrq.ifr_name, name, sizeof (wrq.ifr_name));
    int res = ioctl (sock, SIOCGIWNAME, (caddr_t) &wrq);
    if (res != -1)
        is_nic = true;
#endif
    closesocket (sock);
    return is_nic;
}

//  Handle command from API

static void
s_api_command (agent_t *self)
{
    char *command = zstr_recv (self->pipe);
    if (streq (command, "INTERVAL")) {
        char *interval = zstr_recv (self->pipe);
        self->interval = atoi (interval);
        free (interval);
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
    
    free (command);
}

//  Receive and filter the waiting beacon

static void
s_beacon_recv (agent_t *self)
{
    assert (self);

    socklen_t si_len = sizeof (inaddr_t);
    inaddr_t sender;
    byte buffer [BEACON_MAX];
    ssize_t size = recvfrom (
        self->udpsock,
        (char *) buffer, BEACON_MAX,
        0,      //  Flags
        (struct sockaddr *) &sender, &si_len);
    if (size == SOCKET_ERROR)
        s_handle_io_error ("recvfrom");
    
    //  Get sender address as printable string
    char peername [INET_ADDRSTRLEN];
#if (defined (__WINDOWS__))
    getnameinfo ((struct sockaddr *) &sender, si_len,
                peername, INET_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);
#else
    inet_ntop (AF_INET, &sender.sin_addr, peername, si_len);
#endif
    
    //  If filter is set, check that beacon matches it
    bool is_valid = false;
    if (self->filter) {
        byte  *filter_data = zframe_data (self->filter);
        size_t filter_size = zframe_size (self->filter);
        if (size >= filter_size && memcmp (buffer, filter_data, filter_size) == 0)
            is_valid = true;
    }
    //  If valid, check for echoed beacons (i.e. our own broadcast)
    if (is_valid && self->noecho) {
        byte  *transmit_data = zframe_data (self->transmit);
        size_t transmit_size = zframe_size (self->transmit);
        if (size == transmit_size && memcmp (buffer, transmit_data, transmit_size) == 0)
            is_valid = false;
    }
    //  If still a valid beacon, send on to the API
    if (is_valid) {
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, peername);
        zmsg_addmem (msg, buffer, size);
        zmsg_send (&msg, self->pipe);
    }
}

//  Send beacon to any listening peers

static void
s_beacon_send (agent_t *self)
{
    //  Send UDP broadcast packet now
    assert (self->transmit);
    ssize_t size = sendto (
        self->udpsock,
        (char *) zframe_data (self->transmit), zframe_size (self->transmit),
        0,      //  Flags
        (struct sockaddr *) &self->broadcast, sizeof (inaddr_t));
    //  Sending can fail if the OS is blocking multicast. In such cases we
    //  don't try to report the error. We might log this or send to an error
    //  console at some point.
        if (size == SOCKET_ERROR)
            /* s_handle_io_error ("sendto") */ ;
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
