/*  =========================================================================
    zgossip - decentralized configuration management

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
    Implements a gossip protocol for decentralized configuration management.
    Your applications nodes form a loosely connected network (which can have
    cycles), and publish name/value tuples. Each node re-distributes the new
    tuples it receives, so that the entire network eventually achieves a
    consistent state. The current design does not expire tuples.

    Provides these commands (sent as multipart strings to the actor):

    * BIND endpoint -- binds the gossip service to specified endpoint
    * PORT -- returns the last TCP port, if any, used for binding
    * LOAD configfile -- load configuration from specified file
    * SET configpath value -- set configuration path = value
    * SAVE configfile -- save configuration to specified file
    * CONNECT endpoint -- connect the gossip service to the specified peer
    * PUBLISH key value -- publish a key/value pair to the gossip cluster
    * STATUS -- return number of key/value pairs held by gossip service
    * ZAP DOMAIN domain -- set the ZAP DOMAIN domain = value

    Returns these messages:

    * PORT number -- reply to PORT command
    * STATUS number -- reply to STATUS command
    * DELIVER key value -- new tuple delivered from network
@discuss
    The gossip protocol distributes information around a loosely-connected
    network of gossip services. The information consists of name/value pairs
    published by applications at any point in the network. The goal of the
    gossip protocol is to create eventual consistency between all the using
    applications.

    The name/value pairs (tuples) can be used for configuration data, for
    status updates, for presence, or for discovery. When used for discovery,
    the gossip protocol works as an alternative to e.g. UDP beaconing.

    The gossip network consists of a set of loosely-coupled nodes that
    exchange tuples. Nodes can be connected across arbitrary transports,
    so the gossip network can have nodes that communicate over inproc,
    over IPC, and/or over TCP, at the same time.

    Each node runs the same stack, which is a server-client hybrid using
    a modified Harmony pattern (from Chapter 8 of the Guide):
    http://zguide.zeromq.org/page:all#True-Peer-Connectivity-Harmony-Pattern

    Each node provides a ROUTER socket that accepts client connections on an
    key defined by the application via a BIND command. The state machine
    for these connections is in zgossip.xml, and the generated code is in
    zgossip_engine.inc.

    Each node additionally creates outbound connections via DEALER sockets
    to a set of servers ("remotes"), and under control of the calling app,
    which sends CONNECT commands for each configured remote.

    The messages between client and server are defined in zgossip_msg.xml.
    We built this stack using the zeromq/zproto toolkit.

    To join the gossip network, a node connects to one or more peers. Each
    peer acts as a forwarder. This loosely-coupled network can scale to
    thousands of nodes. However the gossip protocol is NOT designed to be
    efficient, and should not be used for application data, as the same
    tuples may be sent many times across the network.

    The basic logic of the gossip service is to accept PUBLISH messages
    from its owning application, and to forward these to every remote, and
    every client it talks to. When a node gets a duplicate tuple, it throws
    it away. When a node gets a new tuple, it stores it, and forwards it as
    just described.

    At present there is no way to expire tuples from the network.

    The assumptions in this design are:

    * The data set is slow-changing. Thus, the cost of the gossip protocol
      is irrelevant with respect to other traffic.
@end
*/

#include "czmq_classes.h"

// TODO- project.xml?
#ifdef CZMQ_BUILD_DRAFT_API
//  DRAFT-API: Security
#define CZMQ_ZGOSSIP_ZAP_DOMAIN "global"
#endif


//  ---------------------------------------------------------------------
//  Forward declarations for the two main classes we use here

typedef struct _server_t server_t;
typedef struct _client_t client_t;
typedef struct _tuple_t tuple_t;

//  ---------------------------------------------------------------------
//  This structure defines the context for each running server. Store
//  whatever properties and structures you need for the server.

struct _server_t {
    //  These properties must always be present in the server_t
    //  and are set by the generated engine; do not modify them!
    zsock_t *pipe;              //  Actor pipe back to caller
    zconfig_t *config;          //  Current loaded configuration

    //  Add any properties you need here
    zhashx_t *active_remotes;   //  Active (i.e. really connected) remote servers, indexed by their endpoint
    zhashx_t *remotes;          //  All remote servers, connected or not, indexed by their endpoint
    zhashx_t *tuples;           //  Tuples, indexed by key
    zlistx_t *monitors;         //  Zmonitors for remote servers

    tuple_t *cur_tuple;         //  Holds current tuple to publish
    zgossip_msg_t *message;     //  Message to broadcast

    char *public_key;
    char *secret_key;

#ifdef CZMQ_BUILD_DRAFT_API
    //  DRAFT-API: Security
    char *zap_domain;
#endif
};

//  ---------------------------------------------------------------------
//  This structure defines the state for each client connection. It will
//  be passed to each action in the 'self' argument.

struct _client_t {
    //  These properties must always be present in the client_t
    //  and are set by the generated engine; do not modify them!
    server_t *server;           //  Reference to parent server
    zgossip_msg_t *message;     //  Message in and out
};


//  ---------------------------------------------------------------------
//  This structure defines one tuple that we track

struct _tuple_t {
    zhashx_t *container;         //  Hash table that holds this item
    char *key;                  //  Tuple key
    char *value;                //  Tuple value
};

//  Callback when we remove a tuple from its container

static void
tuple_free (void *argument)
{
    tuple_t *self = (tuple_t *) argument;
    freen (self->key);
    freen (self->value);
    freen (self);
}

//  Handle traffic from remote servers and their monitors
static int
remote_handler (zloop_t *loop, zsock_t *remote, void *argument);
static int
monitor_handler (zloop_t *loop, zsock_t *monitor, void *argument);

//  ---------------------------------------------------------------------
//  Include the generated server engine

#include "zgossip_engine.inc"

//  Allocate properties and structures for a new server instance.
//  Return 0 if OK, or -1 if there was an error.

static int
server_initialize (server_t *self)
{
    //  Default timeout for clients is one second; the caller can
    //  override this with a SET message.
    engine_configure (self, "server/timeout", "1000");
    self->message = zgossip_msg_new ();

    self->remotes = zhashx_new ();
    assert (self->remotes);

    self->active_remotes = zhashx_new ();
    assert (self->active_remotes);

    self->tuples = zhashx_new ();
    assert (self->tuples);
    
    self->monitors = zlistx_new ();
    assert (self->monitors);
    zlistx_set_destructor (self->monitors, (czmq_destructor *) zactor_destroy);

#ifdef CZMQ_BUILD_DRAFT_API
    // DRAFT-API: Security
    self->zap_domain = strdup(CZMQ_ZGOSSIP_ZAP_DOMAIN);
#endif

    return 0;
}

//  Free properties and structures for a server instance

static void
server_terminate (server_t *self)
{
    zgossip_msg_destroy (&self->message);
    zlistx_destroy (&self->monitors);
    zsock_t *remote = (zsock_t *) zhashx_first (self->remotes);
    while (remote) {
        zsock_destroy (&remote);
        remote = (zsock_t *) zhashx_next (self->remotes);
    }
    zhashx_destroy (&self->remotes);
    zhashx_destroy (&self->active_remotes);
    zhashx_destroy (&self->tuples);
    zstr_free (&self->public_key);
    zstr_free (&self->secret_key);
#ifdef CZMQ_BUILD_DRAFT_API
    //  DRAFT-API: Security
    zstr_free (&self->zap_domain);
#endif
}

//  Connect to a remote server
static void
#ifdef CZMQ_BUILD_DRAFT_API
// DRAFT-API: Security
server_connect (server_t *self, const char *endpoint, const char *public_key)
#else
server_connect (server_t *self, const char *endpoint)
#endif
{
    zsock_t *remote = (zsock_t *) zhashx_lookup (self->active_remotes, endpoint);
    if (!remote) {
        remote = zsock_new (ZMQ_DEALER);
        assert (remote);          //  No recovery if exhausted
        
#ifdef CZMQ_BUILD_DRAFT_API
        //  DRAFT-API: Security
        if (public_key){
            zcert_t *cert = zcert_new_from_txt (self->public_key, self->secret_key);
            zcert_apply(cert, remote);
            zsock_set_curve_serverkey (remote, public_key);
#ifndef ZMQ_CURVE
            // legacy ZMQ support
            // inline incase the underlying assert is removed
            bool ZMQ_CURVE = false;
#endif
            assert (zsock_mechanism (remote) == ZMQ_CURVE);
            zcert_destroy(&cert);
        }
#endif
        //  Never block on sending; we use an infinite HWM and buffer as many
        //  messages as needed in outgoing pipes. Note that the maximum number
        //  is the overall tuple set size.
        zsock_set_unbounded (remote);
        
        if (zsock_connect (remote, "%s", endpoint)) {
            zsys_warning ("bad zgossip endpoint '%s'", endpoint);
            zsock_destroy (&remote);
            return;
        }
        
        //  Monitor this remote server for incoming messages
        engine_handle_socket (self, remote, remote_handler);

#if defined (ZMQ_EVENT_ALL)
        //  Monitor this remote server for disconnection / reconnection
        zactor_t *monitor = zactor_new (zmonitor, remote);
        zlistx_add_end (self->monitors, monitor);
        //zstr_send (monitor, "VERBOSE");
#if defined (ZMQ_EVENT_HANDSHAKE_SUCCEEDED)
        zstr_sendx (monitor, "LISTEN", "DISCONNECTED", "HANDSHAKE_SUCCEEDED", NULL);
#else
        zstr_sendx (monitor, "LISTEN", "DISCONNECTED", NULL);
#endif
        zstr_send (monitor, "START");
        zsock_wait (monitor);
        engine_handle_socket (self, zactor_sock (monitor), monitor_handler);
#endif

        zhashx_insert (self->active_remotes, endpoint, remote);
        zhashx_insert (self->remotes, endpoint, remote);
        
    }
    
    //  Send HELLO
    zgossip_msg_t *gossip = zgossip_msg_new ();
    zgossip_msg_set_id (gossip, ZGOSSIP_MSG_HELLO);
    zgossip_msg_send (gossip, remote);

    //  And then PUBLISH for each tuple we have
    tuple_t *tuple = (tuple_t *) zhashx_first (self->tuples);
    while (tuple) {
        zgossip_msg_set_id (gossip, ZGOSSIP_MSG_PUBLISH);
        zgossip_msg_set_key (gossip, tuple->key);
        zgossip_msg_set_value (gossip, tuple->value);
        zgossip_msg_send (gossip, remote);
        tuple = (tuple_t *) zhashx_next (self->tuples);
    }
    zgossip_msg_destroy (&gossip);
    
}


//  Process an incoming tuple on this server.

static void
server_accept (server_t *self, const char *key, const char *value)
{
    tuple_t *tuple = (tuple_t *) zhashx_lookup (self->tuples, key);
    if (tuple && streq (tuple->value, value))
        return;                 //  Duplicate tuple, do nothing

    //  Create new tuple
    tuple = (tuple_t *) zmalloc (sizeof (tuple_t));
    assert (tuple);
    tuple->container = self->tuples;
    tuple->key = strdup (key);
    tuple->value = strdup (value);

    //  Store new tuple
    zhashx_update (tuple->container, key, tuple);
    zhashx_freefn (tuple->container, key, tuple_free);

    //  Deliver to calling application
    zstr_sendx (self->pipe, "DELIVER", key, value, NULL);

    //  Hold in server context so we can broadcast to all clients
    self->cur_tuple = tuple;
    engine_broadcast_event (self, NULL, forward_event);

    //  Copy new tuple announcement to all remotes
    zgossip_msg_t *gossip = zgossip_msg_new ();
    zgossip_msg_set_id (gossip, ZGOSSIP_MSG_PUBLISH);
    zsock_t *remote = (zsock_t *) zhashx_first (self->active_remotes);
    while (remote) {
        zgossip_msg_set_key (gossip, tuple->key);
        zgossip_msg_set_value (gossip, tuple->value);
        zgossip_msg_send (gossip, remote);
        remote = (zsock_t *) zhashx_next (self->active_remotes);
    }
    zgossip_msg_destroy (&gossip);
}

//  Process server API method, return reply message if any

static zmsg_t *
server_method (server_t *self, const char *method, zmsg_t *msg)
{
    //  Connect to a remote
    zmsg_t *reply = NULL;
    if (streq (method, "CONNECT")) {
        char *endpoint = zmsg_popstr (msg);
        assert (endpoint);
#ifdef CZMQ_BUILD_DRAFT_API
        //  DRAFT-API: Security
        // leaving this in here for now because if/def changes the server_connect
        // function args. it doesn't look like server_connect is used anywhere else
        // but want to leave this in until we're sure this is stable..
        char *public_key = zmsg_popstr (msg);
        server_connect (self, endpoint, public_key);
        zstr_free (&public_key);
#else
        server_connect (self, endpoint);
#endif
        zstr_free (&endpoint);
    }
    else
    if (streq (method, "PUBLISH")) {
        char *key = zmsg_popstr (msg);
        char *value = zmsg_popstr (msg);
        server_accept (self, key, value);

        zstr_free (&key);
        zstr_free (&value);
    }
    else
    if (streq (method, "STATUS")) {
        //  Return number of tuples we have stored
        reply = zmsg_new ();
        assert (reply);
        zmsg_addstr (reply, "STATUS");
        zmsg_addstrf (reply, "%d", (int) zhashx_size (self->tuples));
    }
#ifdef CZMQ_BUILD_DRAFT_API
    // DRAFT-API: Security
    else
    if (streq (method, "SET PUBLICKEY")) {
        char *key = zmsg_popstr (msg);
        self->public_key = strdup (key);
        assert (self->public_key);
        zstr_free (&key);
    }
    else
    if (streq (method, "SET SECRETKEY")) {
        char *key = zmsg_popstr (msg);
        self->secret_key = strdup(key);
        assert (self->secret_key);
        zstr_free (&key);
    }
    else
    if (streq (method, "ZAP DOMAIN")) {
        char *value = zmsg_popstr (msg);
        zstr_free(&self->zap_domain);
        self->zap_domain = strdup(value);
        assert (self->zap_domain);
        zstr_free (&value);
    }
#endif

#ifdef CZMQ_BUILD_DRAFT_API
    // DRAFT-API: TTL
    else
    if (streq (method, "UNPUBLISH")) {
        char *key = zmsg_popstr (msg);
        assert (key);
        if (self->tuples) {
            zhashx_delete(self->tuples, key);
        }
       zstr_free (&key);
    }
#endif
    else
        zsys_error ("unknown zgossip method '%s'", method);

    return reply;
}

//  Apply new configuration.

static void
server_configuration (server_t *self, zconfig_t *config)
{
    ZPROTO_UNUSED(self);
    ZPROTO_UNUSED(config);
    //  Apply new configuration
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
//  get_first_tuple
//

static void
get_first_tuple (client_t *self)
{
    tuple_t *tuple = (tuple_t *) zhashx_first (self->server->tuples);
    if (tuple) {
        zgossip_msg_set_key (self->message, tuple->key);
        zgossip_msg_set_value (self->message, tuple->value);
        engine_set_next_event (self, ok_event);
    }
    else
        engine_set_next_event (self, finished_event);
}


//  --------------------------------------------------------------------------
//  get_next_tuple
//

static void
get_next_tuple (client_t *self)
{
    tuple_t *tuple = (tuple_t *) zhashx_next (self->server->tuples);
    if (tuple) {
        zgossip_msg_set_key (self->message, tuple->key);
        zgossip_msg_set_value (self->message, tuple->value);
        engine_set_next_event (self, ok_event);
    }
    else
        engine_set_next_event (self, finished_event);
}


//  --------------------------------------------------------------------------
//  store_tuple_if_new
//

static void
store_tuple_if_new (client_t *self)
{
    server_accept (self->server,
                   zgossip_msg_key (self->message),
                   zgossip_msg_value (self->message));
}


//  --------------------------------------------------------------------------
//  get_tuple_to_forward
//

static void
get_tuple_to_forward (client_t *self)
{
    //  Hold this in server->cur_tuple so it's available to all
    //  clients; the whole broadcast operation happens in one thread
    //  so there's no risk of confusion here.
    tuple_t *tuple = self->server->cur_tuple;
    zgossip_msg_set_key (self->message, tuple->key);
    zgossip_msg_set_value (self->message, tuple->value);
}


//  --------------------------------------------------------------------------
//  Handle messages coming from remotes

static int
remote_handler (zloop_t *loop, zsock_t *remote, void *argument)
{
    server_t *self = (server_t *) argument;
    if (zgossip_msg_recv (self->message, remote))
        return -1;          //  Interrupted

    if (zgossip_msg_id (self->message) == ZGOSSIP_MSG_PUBLISH)
        server_accept (self,
                       zgossip_msg_key (self->message),
                       zgossip_msg_value (self->message));
    else
    if (zgossip_msg_id (self->message) == ZGOSSIP_MSG_INVALID) {
        //  Connection was reset, so send HELLO again
        zgossip_msg_set_id (self->message, ZGOSSIP_MSG_HELLO);
        zgossip_msg_send (self->message, remote);
    }
    else
    if (zgossip_msg_id (self->message) == ZGOSSIP_MSG_PONG)
        assert (true);   //  Do nothing with PONGs

    return 0;
}

//  --------------------------------------------------------------------------
//  Handle messages coming from monitors

static int
monitor_handler (zloop_t *loop, zsock_t *monitor_socket, void *argument)
{
    server_t *self = (server_t *) argument;
    zactor_t *monitor = (zactor_t *) zlistx_first (self->monitors);
    while (monitor && monitor_socket != zactor_sock (monitor)) {
        monitor = (zactor_t *) zlistx_next (self->monitors);
    }
    assert(monitor);
    zmsg_t *msg = zmsg_recv (monitor);
    if (msg) {
        char *event = zmsg_popstr (msg);
        assert (event);
        zframe_t *number = zmsg_pop (msg);
        zframe_destroy (&number);
        char *endpoint = zmsg_popstr (msg);
        assert (endpoint);
        zsock_t *remote = (zsock_t *) zhashx_lookup (self->active_remotes, endpoint);
        if (streq (event, "DISCONNECTED") && remote) {
            // Remote server is disconnected : dereference it from active remotes
            zhashx_delete (self->active_remotes, endpoint);
            
        } else if (streq (event, "HANDSHAKE_SUCCEEDED") && !remote) {
            // Remote socket is not in active remotes but is monitored:
            // it's a reconnection, restart connect process for this endpoint
            remote = (zsock_t *) zhashx_lookup (self->remotes, endpoint);
            assert (remote);
            zhashx_insert (self->active_remotes, endpoint, remote);
#ifdef CZMQ_BUILD_DRAFT_API
            server_connect (self, endpoint, self->public_key);
#else
            server_connect (self, endpoint);
#endif
        }
        free(event);
        free(endpoint);
        zmsg_destroy (&msg);
    }
    return 0;
}

//  --------------------------------------------------------------------------
//  Selftest

void
zgossip_test (bool verbose)
{
    printf (" * zgossip: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    //  Test basic client-to-server operation of the protocol
    zactor_t *server = zactor_new (zgossip, "server");
    assert (server);
    if (verbose)
        zstr_send (server, "VERBOSE");
    zstr_sendx (server, "BIND", "inproc://zgossip", NULL);

    zsock_t *client = zsock_new (ZMQ_DEALER);
    assert (client);
    zsock_set_rcvtimeo (client, 2000);
    int rc = zsock_connect (client, "inproc://zgossip");
    assert (rc == 0);

    //  Send HELLO, which gets no message
    zgossip_msg_t *message = zgossip_msg_new ();
    zgossip_msg_set_id (message, ZGOSSIP_MSG_HELLO);
    zgossip_msg_send (message, client);

    //  Send PING, expect PONG back
    zgossip_msg_set_id (message, ZGOSSIP_MSG_PING);
    zgossip_msg_send (message, client);
    zgossip_msg_recv (message, client);
    assert (zgossip_msg_id (message) == ZGOSSIP_MSG_PONG);
    zgossip_msg_destroy (&message);

    zactor_destroy (&server);
    zsock_destroy (&client);

    //  Test peer-to-peer operations
    zactor_t *base = zactor_new (zgossip, "base");
    assert (base);
    if (verbose)
        zstr_send (base, "VERBOSE");
    //  Set a 100msec timeout on clients so we can test expiry
    zstr_sendx (base, "SET", "server/timeout", "100", NULL);
    zstr_sendx (base, "BIND", "inproc://base", NULL);

    zactor_t *alpha = zactor_new (zgossip, "alpha");
    assert (alpha);

    if (verbose)
        zstr_send (alpha, "VERBOSE");

    zstr_sendx (alpha, "CONNECT", "inproc://base", NULL);

    zstr_sendx (alpha, "PUBLISH", "inproc://alpha-1", "service1", NULL);
    zstr_sendx (alpha, "PUBLISH", "inproc://alpha-2", "service2", NULL);

    zactor_t *beta = zactor_new (zgossip, "beta");
    assert (beta);

    if (verbose)
        zstr_send (beta, "VERBOSE");

    zstr_sendx (beta, "CONNECT", "inproc://base", NULL);

    zstr_sendx (beta, "PUBLISH", "inproc://beta-1", "service1", NULL);
    zstr_sendx (beta, "PUBLISH", "inproc://beta-2", "service2", NULL);

    //  got nothing
    zclock_sleep (200);

    zstr_send (alpha, "STATUS");
    char *command, *status, *key, *value;

    zstr_recvx (alpha, &command, &key, &value, NULL);

    assert (streq (command, "DELIVER"));
    assert (streq (key, "inproc://alpha-1"));
    assert (streq (value, "service1"));

    zstr_free (&command);
    zstr_free (&key);
    zstr_free (&value);

    zstr_recvx (alpha, &command, &key, &value, NULL);
    assert (streq (command, "DELIVER"));
    assert (streq (key, "inproc://alpha-2"));
    assert (streq (value, "service2"));
    zstr_free (&command);
    zstr_free (&key);
    zstr_free (&value);

    zstr_recvx (alpha, &command, &key, &value, NULL);
    assert (streq (command, "DELIVER"));
    assert (streq (key, "inproc://beta-1"));
    assert (streq (value, "service1"));
    zstr_free (&command);
    zstr_free (&key);
    zstr_free (&value);

    zstr_recvx (alpha, &command, &key, &value, NULL);
    assert (streq (command, "DELIVER"));
    assert (streq (key, "inproc://beta-2"));
    assert (streq (value, "service2"));
    zstr_free (&command);
    zstr_free (&key);
    zstr_free (&value);

    zstr_recvx (alpha, &command, &status, NULL);
    assert (streq (command, "STATUS"));
    assert (atoi (status) == 4);
    zstr_free (&command);
    zstr_free (&status);

    zactor_destroy (&base);
    zactor_destroy (&alpha);
    zactor_destroy (&beta);

#ifdef CZMQ_BUILD_DRAFT_API
    //  DRAFT-API: Security
    // curve
    if (zsys_has_curve()) {
        if (verbose)
            printf("testing CURVE support");
        zclock_sleep (2000);
        zactor_t *auth = zactor_new(zauth, NULL);
        assert (auth);
        if (verbose) {
            zstr_sendx (auth, "VERBOSE", NULL);
            zsock_wait (auth);
        }
        zstr_sendx(auth,"ALLOW","127.0.0.1",NULL);
        zsock_wait(auth);
        zstr_sendx (auth, "CURVE", CURVE_ALLOW_ANY, NULL);
        zsock_wait (auth);

        server = zactor_new (zgossip, "server");
        if (verbose)
            zstr_send (server, "VERBOSE");
        assert (server);

        zcert_t *client1_cert = zcert_new ();
        zcert_t *server_cert = zcert_new ();

        zstr_sendx (server, "SET PUBLICKEY", zcert_public_txt (server_cert), NULL);
        zstr_sendx (server, "SET SECRETKEY", zcert_secret_txt (server_cert), NULL);
        zstr_sendx (server, "ZAP DOMAIN", "TEST", NULL);

        zstr_sendx (server, "BIND", "tcp://127.0.0.1:*", NULL);
        zstr_sendx (server, "PORT", NULL);
        zstr_recvx (server, &command, &value, NULL);
        assert (streq (command, "PORT"));
        int port = atoi (value);
        zstr_free (&command);
        zstr_free (&value);
        char endpoint [32];
        sprintf (endpoint, "tcp://127.0.0.1:%d", port);

        zactor_t *client1 = zactor_new (zgossip, "client");
        if (verbose)
            zstr_send (client1, "VERBOSE");
        assert (client1);

        zstr_sendx (client1, "SET PUBLICKEY", zcert_public_txt (client1_cert), NULL);
        zstr_sendx (client1, "SET SECRETKEY", zcert_secret_txt (client1_cert), NULL);
        zstr_sendx (client1, "ZAP DOMAIN", "TEST", NULL);

        const char *public_txt = zcert_public_txt (server_cert);
        zstr_sendx (client1, "CONNECT", endpoint, public_txt, NULL);
        zstr_sendx (client1, "PUBLISH", "tcp://127.0.0.1:9001", "service1", NULL);

        zclock_sleep (500);

        zstr_send (server, "STATUS");
        zclock_sleep (500);

        zstr_recvx (server, &command, &key, &value, NULL);
        assert (streq (command, "DELIVER"));
        assert (streq (value, "service1"));

        zstr_free (&command);
        zstr_free (&key);
        zstr_free (&value);

        zstr_sendx (client1, "$TERM", NULL);
        zstr_sendx (server, "$TERM", NULL);

        zclock_sleep(500);

        zcert_destroy (&client1_cert);
        zcert_destroy (&server_cert);

        zactor_destroy (&client1);
        zactor_destroy (&server);
        zactor_destroy (&auth);
    }
#endif

#if defined (__WINDOWS__)
    zsys_shutdown();
#endif

    //  @end
    printf ("OK\n");
}
