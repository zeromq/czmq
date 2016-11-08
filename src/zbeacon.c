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
    int port_nbr;               //  UDP port number we work on
    int interval;               //  Beacon broadcast interval
    int64_t ping_at;            //  Next broadcast time
    zframe_t *transmit;         //  Beacon transmit data
    zframe_t *filter;           //  Beacon filter data
    inaddr_t broadcast;         //  Our broadcast address
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
        if (self->udpsock) // don't close STDIN
            zsys_udp_close (self->udpsock);
        free (self);
        *self_p = NULL;
    }
}

static self_t *
s_self_new (zsock_t *pipe)
{
    self_t *self = (self_t *) zmalloc (sizeof (self_t));
    assert (self);
    self->pipe = pipe;
    return self;
}


//  --------------------------------------------------------------------------
//  Prepare beacon to work on specified UPD port.

static void
s_self_prepare_udp (self_t *self)
{
    //  Create our UDP socket
    if (self->udpsock)
        zsys_udp_close (self->udpsock);

    self->hostname [0] = 0;
    self->udpsock = zsys_udp_new (false);
    if (self->udpsock == INVALID_SOCKET)
        return;

    //  Get the network interface fro ZSYS_INTERFACE or else use first
    //  broadcast interface defined on system. ZSYS_INTERFACE=* means
    //  use INADDR_ANY + INADDR_BROADCAST.
    const char *iface = zsys_interface ();
    in_addr_t bind_to = 0;
    in_addr_t send_to = 0;
    int found_iface = 0;

    if (streq (iface, "*")) {
        //  Wildcard means bind to INADDR_ANY and send to INADDR_BROADCAST
        bind_to = INADDR_ANY;
        send_to = INADDR_BROADCAST;
        found_iface = 1;
    }
    else {
        //  Look for matching interface, or first ziflist item
        ziflist_t *iflist = ziflist_new ();
        assert (iflist);
        const char *name = ziflist_first (iflist);
        while (name) {
            if (streq (iface, name) || streq (iface, "")) {
                //  Using inet_addr instead of inet_aton or inet_atop
                //  because these are not supported in Win XP
                send_to = inet_addr (ziflist_broadcast (iflist));
                bind_to = inet_addr (ziflist_address (iflist));
                if (self->verbose)
                    zsys_info ("zbeacon: interface=%s address=%s broadcast=%s",
                               name, ziflist_address (iflist), ziflist_broadcast (iflist));
                found_iface = 1;
                break;      //  iface is known, so allow it
            }
            name = ziflist_next (iflist);
        }
        ziflist_destroy (&iflist);
    }
    if (found_iface) {
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
        //  If bind fails, we close the socket for opening again later (next poll interval)
        if (bind (self->udpsock, (struct sockaddr *) &sockaddr, sizeof (inaddr_t)))
        {
            zsys_debug ("zbeacon: Unable to bind to broadcast address, reason=%s", strerror (errno));
            zsys_udp_close (self->udpsock);
            self->udpsock = INVALID_SOCKET;
            return;
        }

        //  Get our hostname so we can send it back to the API
        if (address.sin_addr.s_addr == INADDR_ANY) {
            strcpy(self->hostname, "*");
            if (self->verbose)
                zsys_info ("zbeacon: configured, hostname=%s", self->hostname);
        }
        else if (getnameinfo ((struct sockaddr *) &address, sizeof (inaddr_t),
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

    char peername [INET_ADDRSTRLEN];
    zframe_t *frame = zsys_udp_recv (self->udpsock, peername, INET_ADDRSTRLEN);

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
            if (!self->udpsock || self->udpsock == INVALID_SOCKET ||
                zsys_udp_send (self->udpsock, self->transmit, &self->broadcast, sizeof(inaddr_t)))
            {
                const char *reason = (!self->udpsock || self->udpsock == INVALID_SOCKET) ? "invalid socket" : strerror (errno);
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

    zsock_send (speaker, "si", "CONFIGURE", 9999);
    char *hostname = zstr_recv (speaker);
    if (!*hostname) {
        printf ("OK (skipping test, no UDP broadcasting)\n");
        zactor_destroy (&speaker);
        free (hostname);
        return;
    }
    free (hostname);

    //  Create listener beacon on port 9999 to lookup service
    zactor_t *listener = zactor_new (zbeacon, NULL);
    assert (listener);
    if (verbose)
        zstr_sendx (listener, "VERBOSE", NULL);
    zsock_send (listener, "si", "CONFIGURE", 9999);
    hostname = zstr_recv (listener);
    assert (*hostname);
    free (hostname);

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
    free (hostname);

    zactor_t *node2 = zactor_new (zbeacon, NULL);
    assert (node2);
    zsock_send (node2, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node2);
    assert (*hostname);
    free (hostname);

    zactor_t *node3 = zactor_new (zbeacon, NULL);
    assert (node3);
    zsock_send (node3, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node3);
    assert (*hostname);
    free (hostname);

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
