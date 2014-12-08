/*  =========================================================================
    zbeacon - LAN discovery and presence (deprecated)

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
    This class is deprecated in CZMQ v3; it works together with zctx, zsocket,
    and other deprecated V2 classes. New applications should use the V3 zauth
    interface, based on zactor, together with the zsock class for sockets.
@end
*/

#include "platform.h"
#include "../include/czmq.h"

//  Constants
#define INTERVAL_DFLT  1000         //  Default interval = 1 second

//  Structure of our class
struct _zbeacon_t {
    void *pipe;                 //  Pipe through to backend agent
    char *hostname;             //  Our own address as string
    zctx_t *ctx;                //  TODO: actorize this class
};


//  Background task does the real I/O
static void
s_agent_task (void *args, zctx_t *ctx, void *pipe);


//  --------------------------------------------------------------------------
//  Create a new beacon on a certain UDP port. If the system does not
//  support UDP broadcasts (lacking a useful interface), returns NULL.
//  To force the beacon to operate on a given port, set the environment
//  variable ZSYS_INTERFACE, or call zsys_set_interface() beforehand.
//  If you are using the new zsock API then pass NULL as the ctx here.

zbeacon_t *
zbeacon_new (zctx_t *ctx, int port_nbr)
{
    zbeacon_t *self = (zbeacon_t *) zmalloc (sizeof (zbeacon_t));
    if (!self)
        return NULL;

    //  If user passes a ctx, use that, else take the global context from
    //  zsys and use that. This provides compatibility with old zsocket
    //  and new zsock APIs.
    if (ctx)
        self->ctx = zctx_shadow (ctx);
    else
        self->ctx = zctx_shadow_zmq_ctx (zsys_init ());

    //  Start background agent and wait for it to initialize
    self->pipe = zthread_fork (self->ctx, s_agent_task, NULL);
    if (self->pipe) {
        zstr_sendf (self->pipe, "%d", port_nbr);
        self->hostname = zstr_recv (self->pipe);
        if (streq (self->hostname, "-")) {
            free (self->hostname);
            free (self);
            self = NULL;
        }
    }
    else
        zbeacon_destroy (&self);
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
        if (self->pipe) {
            zstr_send (self->pipe, "TERMINATE");
            char *reply = zstr_recv (self->pipe);
            zstr_free (&reply);
        }
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
    assert (msg);
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
    assert (msg);
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
//  Backend agent implementation

//  Agent instance

typedef struct {
    void *pipe;                 //  Socket to talk back to application
    SOCKET udpsock;             //  UDP socket for send/recv
    int port_nbr;               //  UDP port number we work on
    int interval;               //  Beacon broadcast interval
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
            timeout = (long) (self->ping_at - zclock_mono ());
            if (timeout < 0)
                timeout = 0;
        }
        if (zmq_poll (pollitems, 2, timeout * ZMQ_POLL_MSEC) == -1)
            break;              //  Interrupted

        if (pollitems [0].revents & ZMQ_POLLIN)
            s_api_command (self);
        if (pollitems [1].revents & ZMQ_POLLIN)
            s_beacon_recv (self);

        if (  self->transmit
           && zclock_mono () >= self->ping_at) {
            //  Send beacon to any listening peers
            zsys_udp_send (self->udpsock, self->transmit, &self->broadcast);
            self->ping_at = zclock_mono () + self->interval;
        }
    }
    s_agent_destroy (&self);
}


//  Create and initialize new agent instance

static agent_t *
s_agent_new (void *pipe, int port_nbr)
{
    agent_t *self = (agent_t *) zmalloc (sizeof (agent_t));
    if (!self)
        return NULL;

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
#elif (defined (__APPLE__))
    inaddr_t sockaddr = self->broadcast;
    sockaddr.sin_addr.s_addr = htons (INADDR_ANY);
#else
    inaddr_t sockaddr = self->broadcast;
#endif
    int rc = bind (self->udpsock, (struct sockaddr *) &sockaddr, sizeof (inaddr_t));
    if (rc == SOCKET_ERROR)
        zsys_socket_error ("bind");

    //  Send our hostname back to API
    //  PROBLEM: this hostname will not be accurate when the node
    //  has more than one active interface.
    char hostname [NI_MAXHOST];
    rc = getnameinfo ((struct sockaddr *) &self->address,
                      sizeof (inaddr_t), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
    if (rc == 0) {
        //  It all looks OK
        zstr_send (pipe, hostname);
        return self;
    }
    else {
        //  Interface looks unsupported, abort
        zstr_send (pipe, "-");
        zsys_udp_close (self->udpsock);
        free (self);
        return NULL;
    }
}


//  Get the actual network interface we're working on.

static void
s_get_interface (agent_t *self)
{
    const char *iface = zsys_interface ();
    if (streq (iface, "*")) {
        //  Force binding to INADDR_ANY and sending to INADDR_BROADCAST
        self->broadcast.sin_family = AF_INET;
        self->broadcast.sin_addr.s_addr = INADDR_BROADCAST;
        self->broadcast.sin_port = htons (self->port_nbr);
        self->address = self->broadcast;
        self->address.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        ziflist_t *iflist = ziflist_new ();
        assert (iflist);
        const char *name = ziflist_first (iflist);
        if (*iface) {
            while (name) {
                if (streq (iface, name))
                    break;      //  iface is known, so allow it
                name = ziflist_next (iflist);
            }
        }
        if (name) {
            //  Using inet_addr instead of inet_aton or inet_atop because these
            //  are not supported in Win XP
            self->broadcast.sin_family = AF_INET;
            self->broadcast.sin_addr.s_addr = inet_addr (ziflist_broadcast (iflist));
            self->broadcast.sin_port = htons (self->port_nbr);
            self->address = self->broadcast;
            self->address.sin_addr.s_addr = inet_addr (ziflist_address (iflist));
        }
        else
            zsys_error ("No adapter found, ZSYS_INTERFACE=%s isn't helping", iface);

        ziflist_destroy (&iflist);
    }
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
        self->ping_at = zclock_mono ();
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
        zsys_error ("zbeacon: unexpected API command '%s'\n", command);

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
        if (  zframe_size (frame) >= filter_size
           && memcmp (zframe_data (frame), filter_data, filter_size) == 0)
            is_valid = true;
    }
    //  If valid, check for echoed beacons (i.e. our own broadcast)
    if (is_valid && self->noecho && self->transmit) {
        byte  *transmit_data = zframe_data (self->transmit);
        size_t transmit_size = zframe_size (self->transmit);
        if (  zframe_size (frame) == transmit_size
           && memcmp (zframe_data (frame), transmit_data, transmit_size) == 0)
            is_valid = false;
    }
    //  If still a valid beacon, send on to the API
    if (is_valid) {
        zmsg_t *msg = zmsg_new ();
        assert (msg);
        zmsg_addstr (msg, peername);
        zmsg_append (msg, &frame);
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
        zsys_udp_close (self->udpsock);
        zframe_destroy (&self->transmit);
        zframe_destroy (&self->filter);
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Self test of this class
//
// If this test fails with an unable to bind error, try specifying the default
// interface to be used for zbeacon with the ZSYS_INTERFACE environment
// variable.

void
zbeacon_v2_test (bool verbose)
{
    printf (" * zbeacon (deprecated): ");

    //  @selftest
    //  Create beacon to broadcast our service
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    zbeacon_t *service_beacon = zbeacon_new (ctx, 9999);
    if (service_beacon == NULL) {
        printf ("OK (skipping test, no UDP discovery)\n");
        return;
    }
    //  Create a service socket and bind to an ephemeral port
    zsock_t *service = zsock_new (ZMQ_PUB);
    assert (service);
    int port_nbr = zsock_bind (service, "tcp://127.0.0.1:*");
    byte announcement [2] = { (port_nbr >> 8) & 0xFF, port_nbr & 0xFF };
    zbeacon_set_interval (service_beacon, 100);
    zbeacon_publish (service_beacon, announcement, 2);

    //  Create beacon to lookup service
    zbeacon_t *client_beacon = zbeacon_new (ctx, 9999);
    assert (client_beacon);
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
    assert (node1);
    zbeacon_t *node2 = zbeacon_new (ctx, 5670);
    assert (node2);
    zbeacon_t *node3 = zbeacon_new (ctx, 5670);
    assert (node3);

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
    assert (poller);

    int64_t stop_at = zclock_mono () + 1000;
    while (zclock_mono () < stop_at) {
        long timeout = (long) (stop_at - zclock_mono ());
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

    zsock_destroy (&service);
    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
}
