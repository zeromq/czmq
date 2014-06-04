/*  =========================================================================
    zgossip - gossip discovery service

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
    Implements a gossip protocol (RFC TBD).
@discuss
    The gossip protocol solves the problem of discovering a set of services
    on a local area network. It provides an alternative to UDP beacons as
    implemented by zbeacon.

    The gossip network consists of a set of loosely-coupled nodes that
    exchange endpoint=service pairs. The endpoints must be addressable by
    all nodes, so can be sockets in a process (inproc://), sockets on a
    box (ipc://), or sockets on a network (tcp://). Mixing will not work.

    Every node runs the same stack, which is a server-client hybrid using
    a modified Harmony pattern (from Chapter 8 of the Guide):

    http://zguide.zeromq.org/page:all#True-Peer-Connectivity-Harmony-Pattern

    Each node provides a ROUTER socket that accepts client connections on an
    endpoint defined by the application via a BIND command. The state machine
    for these connections is in zgossip.xml, and the generated code is in
    zgossip_engine.h.

    Each node additionally creates outbound connections via DEALER sockets
    to a set of peers, and under control of the calling application, which
    sends CONNECT commands for each configured peer.

    The messages between client and server are defined in zgossip_msg.xml.
    This stack is built using the zeromq/zproto toolkit.

    The goal is that connecting to any other node is sufficient to connect
    to the whole network. Each node accepts service announcements from its
    owning application, and forwards these to the peers it is connected to,
    either as server, or as client.

    The protocol uses ping-pong heartbeating to monitor presence. This code
    doesn't do anything with expired peers yet.

    The gossip network should be very scalable, as we need only two sockets
    per node to get the equivalence of full interconnection.

    The assumptions in this design are:

    * All nodes are on the same ZeroMQ transport, fully interaddressable.
    
    * The topology is slow changing. Thus, the cost of the gossip protocol
      is irrelevant with respect to other traffic.

    TODO:
     * Broadcast ANNOUNCE commands from the application to all clients, as
       well as all servers.

     * Write a large-scale test case that simulates some thousands of nodes
       coming and going. This can run in one process, using inproc.
@end
*/

#include "../include/czmq.h"
#include "zgossip_msg.h"


//  ---------------------------------------------------------------------
//  Forward declarations for the two main classes we use here

typedef struct _server_t server_t;
typedef struct _client_t client_t;
typedef struct _service_t service_t;

//  ---------------------------------------------------------------------
//  This structure defines the context for each running server. Store
//  whatever properties and structures you need for the server.

struct _server_t {
    //  These properties must always be present in the server_t
    //  and are set by the generated engine; do not modify them!
    zlog_t *log;                //  For any logging needed
    zconfig_t *config;          //  Current loaded configuration
    
    //  Add any properties you need here
    zlist_t *peers;             //  Peers, as zsock_t instances
    zhash_t *services;          //  Services, indexed by endpoint

    service_t *cur_service;     //  Holds current service we're announcing
};

//  ---------------------------------------------------------------------
//  This structure defines the state for each client connection. It will
//  be passed to each action in the 'self' argument.

struct _client_t {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine; do not modify them!
    server_t *server;           //  Reference to parent server
    zgossip_msg_t *request;     //  Last received request
    zgossip_msg_t *reply;       //  Reply to send out, if any

    zlist_t *endpoints;         //  List of known services, by endpoint
    char *endpoint;             //  Next endpoint, if any
};


//  ---------------------------------------------------------------------
//  This structure defines one service that we track. Either the
//  client property, or the peer property (a server peer) must be
//  provided.

struct _service_t {
    zhash_t *container;         //  Hash table that holds this item
    client_t *client;           //  Originating client, or
    zsock_t *peer;              //  Originating server peer
    char *endpoint;             //  Service endpoint
    char *name;                 //  Service name
};

//  Callback when we remove service from container

static void
service_free (void *argument)
{
    service_t *self = (service_t *) argument;
    free (self->endpoint);
    free (self->name);
    free (self);
}

//  Handle traffic from peer servers
static int
    peer_handler (zloop_t *loop, zsock_t *peer, void *argument);
    
//  Process a ANNOUNCE message
static void
    process_announce (server_t *server, client_t *client,
                      zsock_t *peer, zgossip_msg_t *msg);


//  ---------------------------------------------------------------------
//  Include the generated server engine

#include "zgossip_engine.h"

//  Allocate properties and structures for a new server instance.
//  Return 0 if OK, or -1 if there was an error.

static int
server_initialize (server_t *self)
{
    //  Default timeout for clients is one second; the caller can
    //  override this with a SET message.
    engine_configure (self, "server/timeout", "1000");
    self->peers = zlist_new ();
    self->services = zhash_new ();
    return 0;
}

//  Free properties and structures for a server instance

static void
server_terminate (server_t *self)
{
    while (zlist_size (self->peers) > 0) {
        zsock_t *peer = (zsock_t *) zlist_pop (self->peers);
        zsock_destroy (&peer);
    }
    zlist_destroy (&self->peers);
    zhash_destroy (&self->services);
}

//  Process server API method, return reply message if any

static zmsg_t *
server_method (server_t *self, const char *method, zmsg_t *msg)
{
    //  Connect to a peer; note that all CONNECTs must be done before
    //  any service announcements.
    if (streq (method, "CONNECT")) {
        char *endpoint = zmsg_popstr (msg);
        assert (endpoint);
        zsock_t *peer = zsock_new (ZMQ_DEALER);
        assert (peer);          //  No recovery if exhausted
        int rc = zsock_connect (peer, "%s", endpoint);
        if (rc) {
            zclock_log ("E (zgossip): bad endpoint '%s'", endpoint);
            zsock_destroy (&peer);
        }
        else {
            zgossip_msg_send_hello (peer);
            zlist_append (self->peers, peer);
            engine_handle_socket (self, peer, peer_handler);
        }
        zstr_free (&endpoint);
    }
    else
    if (streq (method, "ANNOUNCE")) {
        char *endpoint = zmsg_popstr (msg);
        char *service = zmsg_popstr (msg);
        //  Copy announcement to all peers we're connected to
        //  TODO: broadcast to all clients
        zsock_t *peer = (zsock_t *) zlist_first (self->peers);
        while (peer) {
            zgossip_msg_send_announce (peer, endpoint, service);
            peer = (zsock_t *) zlist_next (self->peers);
        }
    }
    else
        zclock_log ("E (zgossip): unknown method '%s'", method);
    
    return NULL;
}


//  Allocate properties and structures for a new client connection and
//  optionally engine_set_next_event (). Return 0 if OK, or -1 on error.

static int
client_initialize (client_t *self)
{
    //  Construct properties here
    return 0;
}

//  Free properties and structures for a client connection

static void
client_terminate (client_t *self)
{
    //  Destroy properties here
}


//  --------------------------------------------------------------------------
//  get_first_service
//

static void
get_first_service (client_t *self)
{
    assert (!self->endpoints);
    self->endpoints = zhash_keys (self->server->services);
    self->endpoint = (char *) zlist_first (self->endpoints);
    get_next_service (self);
}


//  --------------------------------------------------------------------------
//  get_next_service
//

static void
get_next_service (client_t *self)
{
    if (self->endpoint) {
        service_t *service = (service_t *) zhash_lookup (
            self->server->services, self->endpoint);
        assert (service);
        zgossip_msg_set_endpoint (self->reply, service->endpoint);
        zgossip_msg_set_service (self->reply, service->name);
        engine_set_next_event (self, ok_event);
        self->endpoint = (char *) zlist_next (self->endpoints);
    }
    else {
        zlist_destroy (&self->endpoints);
        engine_set_next_event (self, finished_event);
    }
}


//  --------------------------------------------------------------------------
//  store_service_if_new
//

static void
store_service_if_new (client_t *self)
{
    process_announce (self->server, self, NULL, self->request);
}


//  Process a ANNOUNCE message

static void
process_announce (server_t *server, client_t *client,
                  zsock_t *peer, zgossip_msg_t *msg)
{
    service_t *service = (service_t *) zhash_lookup (
        server->services, zgossip_msg_endpoint (msg));
    if (service && streq (service->name, zgossip_msg_service (msg)))
        return;                 //  Service already known so do nothing

    //  Create new service
    service = (service_t *) zmalloc (sizeof (service_t));
    assert (service);
    assert (client || peer);
    service->container = server->services;
    service->client = client;
    service->peer = peer;
    service->endpoint = strdup (zgossip_msg_endpoint (msg));
    service->name = strdup (zgossip_msg_service (msg));

    //  Store new service using endpoint as key
    zhash_update (service->container, service->endpoint, service);
    zhash_freefn (service->container, service->endpoint, service_free);

    //  hold it in server context so we can
    //  broadcast that to all clients
    server->cur_service = service;

    //  Forward new service to all peers except the one it came from
    engine_broadcast_event (server, client, forward_event);
}


//  --------------------------------------------------------------------------
//  get_service_to_forward
//

static void
get_service_to_forward (client_t *self)
{
    //  Hold this in server->cur_service so it's available to all
    //  clients; the whole broadcast operation happens in one thread
    //  so there's no risk of confusion here.
    service_t *service = self->server->cur_service;
    zgossip_msg_set_endpoint (self->reply, service->endpoint);
    zgossip_msg_set_service (self->reply, service->name);
}


//  --------------------------------------------------------------------------
//  Handle messages coming from peer servers

static int
peer_handler (zloop_t *loop, zsock_t *peer, void *argument)
{
    zgossip_msg_t *msg = zgossip_msg_recv (peer);
    if (!msg)
        return -1;              //  Interrupted

    if (zgossip_msg_id (msg) == ZGOSSIP_MSG_ANNOUNCE)
        process_announce ((server_t *) argument, NULL, peer, msg);
    else
    if (zgossip_msg_id (msg) == ZGOSSIP_MSG_INVALID) {
        //  Connection was reset, so send HELLO again
        zgossip_msg_send_hello (peer);
    }
    else
    if (zgossip_msg_id (msg) == ZGOSSIP_MSG_PONG)
        ;   //  Do nothing with PONGs
        
    zgossip_msg_destroy (&msg);
    return 0;
}


//  --------------------------------------------------------------------------
//  Selftest

void
zgossip_test (bool verbose)
{
    printf (" * zgossip: \n");
    
    //  @selftest
    //  Test basic client-to-server operation
    zactor_t *server = zactor_new (zgossip, NULL);
    zstr_sendx (server, "SET", "server/animate", verbose? "1": "0", NULL);
    zstr_sendx (server, "BIND", "ipc:///tmp/zgossip", NULL);
    char *port_str = zstr_recv (server);
    assert (streq (port_str, "0"));
    zstr_free (&port_str);

    zsock_t *client = zsock_new (ZMQ_DEALER);
    assert (client);
    zsock_set_rcvtimeo (client, 2000);
    zsock_connect (client, "ipc:///tmp/zgossip");

    //  Send HELLO, which gets no reply
    zgossip_msg_t *request, *reply;
    request = zgossip_msg_new (ZGOSSIP_MSG_HELLO);
    zgossip_msg_send (&request, client);

    //  Send PING, expect PONG back
    request = zgossip_msg_new (ZGOSSIP_MSG_PING);
    zgossip_msg_send (&request, client);
    reply = zgossip_msg_recv (client);
    assert (reply);
    assert (zgossip_msg_id (reply) == ZGOSSIP_MSG_PONG);
    zgossip_msg_destroy (&reply);
    
    zactor_destroy (&server);

    zsock_destroy (&client);
    zactor_destroy (&server);

    //  Test peer-to-peer operations
    zactor_t *base = zactor_new (zgossip, NULL);
    assert (base);
    zstr_sendx (base, "SET", "server/animate", verbose? "1": "0", NULL);
    //  Set a 100msec timeout on clients so we can test expiry
    zstr_sendx (base, "SET", "server/timeout", "100", NULL);
    zstr_sendx (base, "BIND", "inproc://base", NULL);
    port_str = zstr_recv (base);
    assert (streq (port_str, "0"));
    zstr_free (&port_str);

    zactor_t *alpha = zactor_new (zgossip, NULL);
    assert (alpha);
    zstr_sendx (alpha, "CONNECT", "inproc://base", NULL);
    zstr_sendx (alpha, "ANNOUNCE", "inproc://alpha-1", "service1", NULL);
    zstr_sendx (alpha, "ANNOUNCE", "inproc://alpha-2", "service2", NULL);

    zactor_t *beta = zactor_new (zgossip, NULL);
    assert (beta);
    zstr_sendx (beta, "CONNECT", "inproc://base", NULL);
    zstr_sendx (beta, "ANNOUNCE", "inproc://beta-1", "service1", NULL);
    zstr_sendx (beta, "ANNOUNCE", "inproc://beta-2", "service2", NULL);

    //  got nothing
    zclock_sleep (200);
    
    zactor_destroy (&base);
    zactor_destroy (&alpha);
    zactor_destroy (&beta);
    
    //  @end
    printf ("OK\n");
}
