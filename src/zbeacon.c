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

#include "platform.h"
#include "../include/czmq.h"

//  --------------------------------------------------------------------------
//  The self_t structure holds the state for one actor instance

typedef struct {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    SOCKET udpsock;             //  UDP socket for send/recv
    int port_nbr;               //  UDP port number we work on
    int interval;               //  Beacon broadcast interval
    int64_t ping_at;            //  Next broadcast time
    zframe_t *transmit;         //  Beacon transmit data
    zframe_t *filter;           //  Beacon filter data
    inaddr_t broadcast;         //  Our broadcast address
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
} self_t;

static self_t *
s_self_new (zsock_t *pipe)
{
    self_t *self = (self_t *) zmalloc (sizeof (self_t));
    self->pipe = pipe;
    self->poller = zpoller_new (self->pipe, NULL);
    return self;
}

static void
s_self_destroy (self_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        self_t *self = *self_p;
        zpoller_destroy (&self->poller);
        zframe_destroy (&self->transmit);
        zframe_destroy (&self->filter);
        zsys_udp_close (self->udpsock);
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Prepare beacon to work on specified UPD port, reply hostname to
//  pipe (or "" if this failed)

static void
s_self_configure (self_t *self, int port_nbr)
{
    assert (port_nbr);
    self->port_nbr = port_nbr;

    //  Create our UDP socket
    self->udpsock = zsys_udp_new (false);
    if (self->udpsock == INVALID_SOCKET) {
        zstr_send (self->pipe, "");
        return;
    }
    //  Get the network interface fro ZSYS_INTERFACE or else use first
    //  broadcast interface defined on system. ZSYS_INTERFACE=* means
    //  use INADDR_ANY + INADDR_BROADCAST.
    const char *iface = zsys_interface ();
    in_addr_t bind_to = 0;
    in_addr_t send_to = 0;
    
    if (streq (iface, "*")) {
        //  Wildcard means bind to INADDR_ANY and send to INADDR_BROADCAST
        bind_to = INADDR_ANY;
        send_to = INADDR_BROADCAST;
    }
    else {
        //  Look for matching interface, or first ziflist item
        ziflist_t *iflist = ziflist_new ();
        const char *name = ziflist_first (iflist);
        while (name) {
            if (streq (iface, name) || streq (iface, "")) {
                //  Using inet_addr instead of inet_aton or inet_atop 
                //  because these are not supported in Win XP
                send_to = inet_addr (ziflist_broadcast (iflist));
                bind_to = inet_addr (ziflist_address (iflist));
                if (self->verbose)
                    zsys_info ("zbeacon: using address=%s broadcast=%s",
                        ziflist_address (iflist), ziflist_broadcast (iflist));
                break;      //  iface is known, so allow it
            }
            name = ziflist_next (iflist);
        }
        ziflist_destroy (&iflist);
    }
    if (bind_to) {
        self->broadcast.sin_family = AF_INET;
        self->broadcast.sin_port = htons (self->port_nbr);
        self->broadcast.sin_addr.s_addr = send_to;
        inaddr_t address = self->broadcast;
        address.sin_addr.s_addr = bind_to;
        //  Bind to the port on all interfaces
#if (defined (__WINDOWS__))
        inaddr_t sockaddr = address;
#elif (defined (__APPLE__))
        inaddr_t sockaddr = self->broadcast;
        sockaddr.sin_addr.s_addr = htons (INADDR_ANY);
#else
        inaddr_t sockaddr = self->broadcast;
#endif
        //  Bind must succeed; we treat failure here as a hard violation (assert)
        if (bind (self->udpsock, (struct sockaddr *) &sockaddr, sizeof (inaddr_t)))
            zsys_socket_error ("bind");

        //  Send our hostname back to API
        char hostname [NI_MAXHOST];
        if (!getnameinfo ((struct sockaddr *) &address, sizeof (inaddr_t),
            hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST)) {
            if (self->verbose)
                zsys_info ("zbeacon: configured, hostname=%s", hostname);
            zstr_send (self->pipe, hostname);
        }
        else
            zstr_send (self->pipe, "");
    }
    else {
        zsys_error ("No broadcast interface found, (ZSYS_INTERFACE=%s)", iface);
        zstr_send (self->pipe, "");
    }
}


//  --------------------------------------------------------------------------
//  Handle a command from calling application

static int
s_self_handle_pipe (self_t *self)
{
    //  Get the whole message off the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
        return -1;                  //  Interrupted

    char *command = zmsg_popstr (request);
    if (self->verbose)
        zsys_info ("zbeacon: API command=%s", command);
    
    if (streq (command, "VERBOSE"))
        self->verbose = true;
    else
    if (streq (command, "CONFIGURE")) {
        char *port_value = zmsg_popstr (request);
        s_self_configure (self, atoi (port_value));
        free (port_value);
    }
    else
    if (streq (command, "PUBLISH")) {
        zframe_destroy (&self->transmit);
        self->transmit = zmsg_pop (request);
        char *interval = zmsg_popstr (request);
        self->interval = atoi (interval);
        zstr_free (&interval);
        assert (zframe_size (self->transmit) <= UDP_FRAME_MAX);
        //  Start broadcasting immediately
        self->ping_at = zclock_mono ();
    }
    else
    if (streq (command, "SILENCE"))
        zframe_destroy (&self->transmit);
    else
    if (streq (command, "SUBSCRIBE")) {
        zframe_destroy (&self->filter);
        self->filter = zmsg_pop (request);
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
    zmsg_destroy (&request);
    return 0;
}


//  --------------------------------------------------------------------------
//  Receive and filter the waiting beacon

static void
s_self_handle_udp (self_t *self)
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
        zmsg_addstr (msg, peername);
        zmsg_add (msg, frame);
        zmsg_send (&msg, self->pipe);
    }
    else
        zframe_destroy (&frame);
}


//  --------------------------------------------------------------------------
//  zbeacon() implements the zbeacon actor interface

void
zbeacon (zsock_t *pipe, void *args)
{
    self_t *self = s_self_new (pipe);
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
        if (zmq_poll (pollitems, self->udpsock? 2: 1, timeout * ZMQ_POLL_MSEC) == -1)
            break;              //  Interrupted

        if (pollitems [0].revents & ZMQ_POLLIN)
            s_self_handle_pipe (self);
        if (pollitems [1].revents & ZMQ_POLLIN)
            s_self_handle_udp (self);

        if (self->transmit
        &&  zclock_mono () >= self->ping_at) {
            //  Send beacon to any listening peers
            zsys_udp_send (self->udpsock, self->transmit, &self->broadcast);
            self->ping_at = zclock_mono () + self->interval;
        }
    }
    s_self_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Selftest

void
zbeacon_test (bool verbose)
{
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

    //  Beacon will use UDP port 9999
    zstr_sendx (speaker, "CONFIGURE", "9999", NULL);
    char *hostname = zstr_recv (speaker);
    if (!*hostname) {
        printf ("OK (skipping test, no UDP broadcasting)\n");
        zactor_destroy (&speaker);
        free (hostname);
        return;
    }
    free (hostname);

    //  We will broadcast the magic value 0xCAFE
    byte announcement [2] = { 0xCA, 0xFE };
    zmsg_t *msg = zmsg_new ();
    zmsg_addstr (msg, "PUBLISH");
    zmsg_addmem (msg, announcement, 2);
    zmsg_addstrf (msg, "%d", 100);
    zmsg_send (&msg, speaker);

    //  Create listener beacon on port 9999 to lookup service
    zactor_t *listener = zactor_new (zbeacon, NULL);
    if (verbose)
        zstr_sendx (listener, "VERBOSE", NULL);
    zstr_sendx (listener, "CONFIGURE", "9999", NULL);
    hostname = zstr_recv (listener);
    assert (*hostname);
    free (hostname);

    msg = zmsg_new ();
    zmsg_addstr (msg, "SUBSCRIBE");
    zmsg_addmem (msg, "", 0);
    zmsg_send (&msg, listener);

    //  Wait for at most 1/2 second if there's no broadcasting
    zactor_set_rcvtimeo (listener, 500);
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
    zstr_sendx (node1, "CONFIGURE", "5670", NULL);
    hostname = zstr_recv (node1);
    assert (*hostname);
    free (hostname);

    zactor_t *node2 = zactor_new (zbeacon, NULL);
    assert (node2);
    zstr_sendx (node2, "CONFIGURE", "5670", NULL);
    hostname = zstr_recv (node2);
    assert (*hostname);
    free (hostname);

    zactor_t *node3 = zactor_new (zbeacon, NULL);
    assert (node3);
    zstr_sendx (node3, "CONFIGURE", "5670", NULL);
    hostname = zstr_recv (node3);
    assert (*hostname);
    free (hostname);

    msg = zmsg_new ();
    zmsg_addstr (msg, "PUBLISH");
    zmsg_addmem (msg, "NODE/1", 6);
    zmsg_addstr (msg, "250");
    zmsg_send (&msg, node1);

    msg = zmsg_new ();
    zmsg_addstr (msg, "PUBLISH");
    zmsg_addmem (msg, "NODE/2", 6);
    zmsg_addstr (msg, "250");
    zmsg_send (&msg, node2);

    msg = zmsg_new ();
    zmsg_addstr (msg, "PUBLISH");
    zmsg_addmem (msg, "GARBAGE", 7);
    zmsg_addstr (msg, "250");
    zmsg_send (&msg, node3);

    msg = zmsg_new ();
    zmsg_addstr (msg, "SUBSCRIBE");
    zmsg_addmem (msg, "NODE", 4);
    zmsg_send (&msg, node1);

    //  Poll on three API sockets at once
    zpoller_t *poller = zpoller_new (node1, node2, node3, NULL);
    int64_t stop_at = zclock_mono () + 1000;
    while (zclock_mono () < stop_at) {
        long timeout = (long) (stop_at - zclock_mono ());
        if (timeout < 0)
            timeout = 0;
        void *which = zpoller_wait (poller, timeout * ZMQ_POLL_MSEC);
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
    //  @end
    printf ("OK\n");
}
