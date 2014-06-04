/*  =========================================================================
    zgossip_engine - zgossip server engine

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

    * The XML model used for this code generation: zgossip.xml
    * The code generation script that built this file: zproto_server_c
    ************************************************************************

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/


//  ---------------------------------------------------------------------
//  State machine constants

typedef enum {
    start_state = 1,
    have_service_state = 2,
    connected_state = 3,
    external_state = 4
} state_t;

typedef enum {
    terminate_event = -1,
    NULL_event = 0,
    hello_event = 1,
    ok_event = 2,
    finished_event = 3,
    announce_event = 4,
    ping_event = 5,
    forward_event = 6,
    expired_event = 7
} event_t;

//  Names for animation and error reporting
static char *
s_state_name [] = {
    "(NONE)",
    "start",
    "have service",
    "connected",
    "external"
};

static char *
s_event_name [] = {
    "(NONE)",
    "HELLO",
    "ok",
    "finished",
    "ANNOUNCE",
    "PING",
    "forward",
    "expired"
};
 

//  ---------------------------------------------------------------------
//  Context for the whole server task. This embeds the application-level
//  server context at its start (the entire structure, not a reference),
//  so we can cast a pointer between server_t and s_server_t arbitrarily.

typedef struct {
    server_t server;            //  Application-level server context
    zsock_t *pipe;              //  Socket to back to caller API
    zsock_t *router;            //  Socket to talk to clients
    int port;                   //  Server port bound to
    zloop_t *loop;              //  Reactor for server sockets
    zhash_t *clients;           //  Clients we're connected to
    zconfig_t *config;          //  Configuration tree
    zlog_t *log;                //  Server logger
    uint client_id;             //  Client identifier counter
    size_t timeout;             //  Default client expiry timeout
    bool animate;               //  Is animation enabled?
} s_server_t;


//  ---------------------------------------------------------------------
//  Context for each connected client. This embeds the application-level
//  client context at its start (the entire structure, not a reference),
//  so we can cast a pointer between client_t and s_client_t arbitrarily.

typedef struct {
    client_t client;            //  Application-level client context
    s_server_t *server;         //  Parent server context
    char *hashkey;              //  Key into server->clients hash
    zframe_t *routing_id;       //  Routing_id back to client
    uint unique_id;             //  Client identifier in server
    zlist_t *mailbox;           //  Incoming messages
    state_t state;              //  Current state
    event_t event;              //  Current event
    event_t next_event;         //  The next event
    event_t exception;          //  Exception event, if any
    int expiry_timer;           //  zloop timer for client timeouts
    int wakeup_timer;           //  zloop timer for client alarms
    event_t wakeup_event;       //  Wake up with this event
    char log_prefix [41];       //  Log prefix string
} s_client_t;

static int
    server_initialize (server_t *self);
static void
    server_terminate (server_t *self);
static zmsg_t *
    server_method (server_t *self, const char *method, zmsg_t *msg);
static int
    client_initialize (client_t *self);
static void
    client_terminate (client_t *self);
static void
    s_client_execute (s_client_t *client, int event);
static int
    s_client_wakeup (zloop_t *loop, int timer_id, void *argument);
static void
    get_first_service (client_t *self);
static void
    get_next_service (client_t *self);
static void
    store_service_if_new (client_t *self);
static void
    get_service_to_forward (client_t *self);

//  ---------------------------------------------------------------------
//  These methods are an internal API for actions

//  Set the next event, needed in at least one action in an internal
//  state; otherwise the state machine will wait for a message on the
//  router socket and treat that as the event.

static void
engine_set_next_event (client_t *client, event_t event)
{
    if (client) {
        s_client_t *self = (s_client_t *) client;
        self->next_event = event;
    }
}

//  Raise an exception with 'event', halting any actions in progress.
//  Continues execution of actions defined for the exception event.

static void
engine_set_exception (client_t *client, event_t event)
{
    if (client) {
        s_client_t *self = (s_client_t *) client;
        self->exception = event;
    }
}

//  Set wakeup alarm after 'delay' msecs. The next state should
//  handle the wakeup event. The alarm is cancelled on any other
//  event.

static void
engine_set_wakeup_event (client_t *client, size_t delay, event_t event)
{
    if (client) {
        s_client_t *self = (s_client_t *) client;
        if (self->wakeup_timer) {
            zloop_timer_end (self->server->loop, self->wakeup_timer);
            self->wakeup_timer = 0;
        }
        self->wakeup_timer = zloop_timer (
            self->server->loop, delay, 1, s_client_wakeup, self);
        self->wakeup_event = event;
    }
}

//  Execute 'event' on specified client. Use this to send events to
//  other clients. Cancels any wakeup alarm on that client.

static void
engine_send_event (client_t *client, event_t event)
{
    if (client) {
        s_client_t *self = (s_client_t *) client;
        s_client_execute (self, event);
    }
}

//  Execute 'event' on all clients known to the server. If you pass a
//  client argument, that client will not receive the broadcast. If you
//  want to pass any arguments, store them in the server context.

static void
engine_broadcast_event (server_t *server, client_t *client, event_t event)
{
    if (server) {
        s_server_t *self = (s_server_t *) server;
        zlist_t *keys = zhash_keys (self->clients);
        char *key = (char *) zlist_first (keys);
        while (key) {
            s_client_t *target = (s_client_t *) zhash_lookup (self->clients, key);
            if (client && target != (s_client_t *) client)
                s_client_execute (target, event);
            key = (char *) zlist_next (keys);
        }
        zlist_destroy (&keys);
    }
}

//  Poll socket for activity, invoke handler on any received message.
//  Handler must be a CZMQ zloop_fn function; receives server as arg.

static void
engine_handle_socket (server_t *server, zsock_t *socket, zloop_reader_fn handler)
{
    if (server) {
        s_server_t *self = (s_server_t *) server;
        if (handler) {
            int rc = zloop_reader (self->loop, socket, handler, self);
            assert (rc == 0);
            zloop_reader_set_tolerant (self->loop, socket);
        }
        else
            zloop_reader_end (self->loop, socket);
    }
}

//  Register monitor function that will be called at regular intervals
//  by the server engine

static void
engine_set_monitor (server_t *server, size_t interval, zloop_timer_fn monitor)
{
    if (server) {
        s_server_t *self = (s_server_t *) server;
        int rc = zloop_timer (self->loop, interval, 0, monitor, self);
        assert (rc >= 0);
    }
}

//  Send log data for a specific client to the server log. Accepts a
//  printf format.

static void
engine_log (client_t *client, const char *format, ...)
{
    if (client) {
        s_client_t *self = (s_client_t *) client;
        va_list argptr;
        va_start (argptr, format);
        char *string = zsys_vprintf (format, argptr);
        va_end (argptr);
        zlog_debug (self->server->log, "%s: %s", self->log_prefix, string);
        free (string);
    }
}

//  Send log data to the server log. Accepts a printf format.

static void
engine_server_log (server_t *server, const char *format, ...)
{
    if (server) {
        s_server_t *self = (s_server_t *) server;
        va_list argptr;
        va_start (argptr, format);
        char *string = zsys_vprintf (format, argptr);
        va_end (argptr);
        zlog_debug (self->log, "%s", string);
        free (string);
    }
}

//  Set log file prefix; this string will be added to log data, to make
//  log data more searchable. The string is truncated to ~20 chars.

static void
engine_set_log_prefix (client_t *client, const char *string)
{
    if (client) {
        s_client_t *self = (s_client_t *) client;
        snprintf (self->log_prefix, sizeof (self->log_prefix) - 1,
            "%6d:%-33s", self->unique_id, string);
    }
}

//  Set a configuration value in the server's configuration tree.
//  The properties this engine uses are: server/animate,
//  server/timeout, and server/background. You can also configure
//  other abitrary properties.

static void
engine_configure (server_t *server, const char *path, const char *value)
{
    if (server) {
        s_server_t *self = (s_server_t *) server;
        zconfig_put (self->config, path, value);
    }
}

//  Pedantic compilers don't like unused functions, so we call the whole
//  API, passing null references. It's nasty and horrid and sufficient.

static void
s_satisfy_pedantic_compilers (void)
{
    engine_set_next_event (NULL, 0);
    engine_set_exception (NULL, 0);
    engine_set_wakeup_event (NULL, 0, 0);
    engine_send_event (NULL, 0);
    engine_broadcast_event (NULL, NULL, 0);
    engine_handle_socket (NULL, 0, NULL);
    engine_set_monitor (NULL, 0, NULL);
    engine_log (NULL, NULL);
    engine_server_log (NULL, NULL);
    engine_set_log_prefix (NULL, NULL);
    engine_configure (NULL, NULL, NULL);
}


//  ---------------------------------------------------------------------
//  Generic methods on protocol messages
//  TODO: replace with lookup table, since ID is one byte

static event_t
s_protocol_event (zgossip_msg_t *request)
{
    assert (request);
    switch (zgossip_msg_id (request)) {
        case ZGOSSIP_MSG_HELLO:
            return hello_event;
            break;
        case ZGOSSIP_MSG_ANNOUNCE:
            return announce_event;
            break;
        case ZGOSSIP_MSG_PING:
            return ping_event;
            break;
        default:
            //  Invalid zgossip_msg_t
            return terminate_event;
    }
}


//  ---------------------------------------------------------------------
//  Client methods

static s_client_t *
s_client_new (s_server_t *server, zframe_t *routing_id)
{
    s_client_t *self = (s_client_t *) zmalloc (sizeof (s_client_t));
    assert (self);
    assert ((s_client_t *) &self->client == self);
    
    self->server = server;
    self->hashkey = zframe_strhex (routing_id);
    self->routing_id = zframe_dup (routing_id);
    self->mailbox = zlist_new ();
    self->unique_id = server->client_id++;
    engine_set_log_prefix (&self->client, "");

    self->client.server = (server_t *) server;
    self->client.reply = zgossip_msg_new (0);
    zgossip_msg_set_routing_id (self->client.reply, self->routing_id);

    //  Give application chance to initialize and set next event
    self->state = start_state;
    self->event = NULL_event;
    client_initialize (&self->client);
    return self;
}

static void
s_client_destroy (s_client_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        s_client_t *self = *self_p;
        zgossip_msg_destroy (&self->client.request);
        zgossip_msg_destroy (&self->client.reply);
        
        if (self->wakeup_timer)
            zloop_timer_end (self->server->loop, self->wakeup_timer);
        if (self->expiry_timer)
            zloop_timer_end (self->server->loop, self->expiry_timer);

        //  Empty and destroy mailbox
        zgossip_msg_t *request = (zgossip_msg_t *) zlist_first (self->mailbox);
        while (request) {
            zgossip_msg_destroy (&request);
            request = (zgossip_msg_t *) zlist_next (self->mailbox);
        }
        zlist_destroy (&self->mailbox);
        zframe_destroy (&self->routing_id);
        client_terminate (&self->client);
        free (self->hashkey);
        free (self);
        *self_p = NULL;
    }
}

//  Callback when we remove client from 'clients' hash table
static void
s_client_free (void *argument)
{
    s_client_t *client = (s_client_t *) argument;
    s_client_destroy (&client);
}

//  Do we accept a request off the mailbox? If so, return the event for
//  the message, and load the message into the current client request.
//  If not, return NULL_event;

static event_t
s_client_filter_mailbox (s_client_t *self)
{
    assert (self);
    
    zgossip_msg_t *request = (zgossip_msg_t *) zlist_first (self->mailbox);
    while (request) {
        //  Check whether current state can process event
        //  This should be changed to a pre-built lookup table
        event_t event = s_protocol_event (request);
        bool event_is_valid;
        if (self->state == start_state)
            event_is_valid = true;
        else
        if (self->state == connected_state)
            event_is_valid = true;
        else
        if (self->state == external_state)
            event_is_valid = true;
        else
            event_is_valid = false;
            
        if (event_is_valid) {
            zlist_remove (self->mailbox, request);
            zgossip_msg_destroy (&self->client.request);
            self->client.request = request;
            return event;
        }
        request = (zgossip_msg_t *) zlist_next (self->mailbox);
    }
    return NULL_event;
}


//  Execute state machine as long as we have events

static void
s_client_execute (s_client_t *self, int event)
{
    self->next_event = event;
    //  Cancel wakeup timer, if any was pending
    if (self->wakeup_timer) {
        zloop_timer_end (self->server->loop, self->wakeup_timer);
        self->wakeup_timer = 0;
    }
    while (self->next_event != NULL_event) {
        self->event = self->next_event;
        self->next_event = NULL_event;
        self->exception = NULL_event;
        if (self->server->animate) {
            zlog_debug (self->server->log,
                "%s: %s:", self->log_prefix, s_state_name [self->state]);
            zlog_debug (self->server->log,
                "%s:     %s", self->log_prefix, s_event_name [self->event]);
        }
        switch (self->state) {
            case start_state:
                if (self->event == hello_event) {
                    if (!self->exception) {
                        //  get first service
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ get first service", self->log_prefix);
                        get_first_service (&self->client);
                    }
                    if (!self->exception)
                        self->state = have_service_state;
                }
                else
                if (self->event == expired_event) {
                    if (!self->exception) {
                        //  terminate
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ terminate", self->log_prefix);
                        self->next_event = terminate_event;
                    }
                }
                else {
                    //  Handle unexpected protocol events
                    if (!self->exception) {
                        //  send invalid
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ send INVALID", self->log_prefix);
                        zgossip_msg_set_id (self->client.reply, ZGOSSIP_MSG_INVALID);
                        zgossip_msg_send (&self->client.reply, self->server->router);
                        self->client.reply = zgossip_msg_new (0);
                        zgossip_msg_set_routing_id (self->client.reply, self->routing_id);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ terminate", self->log_prefix);
                        self->next_event = terminate_event;
                    }
                }
                break;

            case have_service_state:
                if (self->event == ok_event) {
                    if (!self->exception) {
                        //  send announce
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ send ANNOUNCE", self->log_prefix);
                        zgossip_msg_set_id (self->client.reply, ZGOSSIP_MSG_ANNOUNCE);
                        zgossip_msg_send (&self->client.reply, self->server->router);
                        self->client.reply = zgossip_msg_new (0);
                        zgossip_msg_set_routing_id (self->client.reply, self->routing_id);
                    }
                    if (!self->exception) {
                        //  get next service
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ get next service", self->log_prefix);
                        get_next_service (&self->client);
                    }
                }
                else
                if (self->event == finished_event) {
                    if (!self->exception)
                        self->state = connected_state;
                }
                else {
                    //  Handle unexpected internal events
                    zlog_warning (self->server->log, "%s: unhandled event %s in %s",
                        self->log_prefix,
                        s_event_name [self->event],
                        s_state_name [self->state]);
                    assert (false);
                }
                break;

            case connected_state:
                if (self->event == announce_event) {
                    if (!self->exception) {
                        //  store service if new
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ store service if new", self->log_prefix);
                        store_service_if_new (&self->client);
                    }
                }
                else
                if (self->event == ping_event) {
                    if (!self->exception) {
                        //  send pong
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ send PONG", self->log_prefix);
                        zgossip_msg_set_id (self->client.reply, ZGOSSIP_MSG_PONG);
                        zgossip_msg_send (&self->client.reply, self->server->router);
                        self->client.reply = zgossip_msg_new (0);
                        zgossip_msg_set_routing_id (self->client.reply, self->routing_id);
                    }
                }
                else
                if (self->event == forward_event) {
                    if (!self->exception) {
                        //  get service to forward
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ get service to forward", self->log_prefix);
                        get_service_to_forward (&self->client);
                    }
                    if (!self->exception) {
                        //  send announce
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ send ANNOUNCE", self->log_prefix);
                        zgossip_msg_set_id (self->client.reply, ZGOSSIP_MSG_ANNOUNCE);
                        zgossip_msg_send (&self->client.reply, self->server->router);
                        self->client.reply = zgossip_msg_new (0);
                        zgossip_msg_set_routing_id (self->client.reply, self->routing_id);
                    }
                }
                else
                if (self->event == expired_event) {
                    if (!self->exception) {
                        //  terminate
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ terminate", self->log_prefix);
                        self->next_event = terminate_event;
                    }
                }
                else {
                    //  Handle unexpected protocol events
                    if (!self->exception) {
                        //  send invalid
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ send INVALID", self->log_prefix);
                        zgossip_msg_set_id (self->client.reply, ZGOSSIP_MSG_INVALID);
                        zgossip_msg_send (&self->client.reply, self->server->router);
                        self->client.reply = zgossip_msg_new (0);
                        zgossip_msg_set_routing_id (self->client.reply, self->routing_id);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ terminate", self->log_prefix);
                        self->next_event = terminate_event;
                    }
                }
                break;

            case external_state:
                if (self->event == expired_event) {
                    if (!self->exception) {
                        //  terminate
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ terminate", self->log_prefix);
                        self->next_event = terminate_event;
                    }
                }
                {
                    //  Handle unexpected protocol events
                    if (!self->exception) {
                        //  send invalid
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ send INVALID", self->log_prefix);
                        zgossip_msg_set_id (self->client.reply, ZGOSSIP_MSG_INVALID);
                        zgossip_msg_send (&self->client.reply, self->server->router);
                        self->client.reply = zgossip_msg_new (0);
                        zgossip_msg_set_routing_id (self->client.reply, self->routing_id);
                    }
                    if (!self->exception) {
                        //  terminate
                        if (self->server->animate)
                            zlog_debug (self->server->log,
                                "%s:         $ terminate", self->log_prefix);
                        self->next_event = terminate_event;
                    }
                }
                break;
        }
        //  If we had an exception event, interrupt normal programming
        if (self->exception) {
            if (self->server->animate)
                zlog_debug (self->server->log,
                    "%s:         ! %s",
                    self->log_prefix, s_event_name [self->exception]);

            self->next_event = self->exception;
        }
        if (self->next_event == terminate_event) {
            //  Automatically calls s_client_destroy
            zhash_delete (self->server->clients, self->hashkey);
            break;
        }
        else {
            if (self->server->animate)
                zlog_debug (self->server->log,
                    "%s:         > %s",
                    self->log_prefix, s_state_name [self->state]);

            if (self->next_event == NULL_event)
                //  Get next valid message from mailbox, if any
                self->next_event = s_client_filter_mailbox (self);
        }
    }
}

//  zloop callback when client inactivity timer expires

static int
s_client_expired (zloop_t *loop, int timer_id, void *argument)
{
    s_client_t *self = (s_client_t *) argument;
    s_client_execute (self, expired_event);
    return 0;
}

//  zloop callback when client wakeup timer expires

static int
s_client_wakeup (zloop_t *loop, int timer_id, void *argument)
{
    s_client_t *self = (s_client_t *) argument;
    s_client_execute (self, self->wakeup_event);
    return 0;
}


//  Server methods

static void
s_server_config_self (s_server_t *self)
{
    //  Built-in server configuration options
    //  
    //  Animation is disabled by default
    self->animate = atoi (
        zconfig_resolve (self->config, "server/animate", "0"));

    //  Default client timeout is 60 seconds
    self->timeout = atoi (
        zconfig_resolve (self->config, "server/timeout", "60000"));

    //  Do we want to run server in the background?
    int background = atoi (
        zconfig_resolve (self->config, "server/background", "0"));
    if (!background)
        zlog_set_foreground (self->log, true);
}

static s_server_t *
s_server_new (zsock_t *pipe)
{
    s_server_t *self = (s_server_t *) zmalloc (sizeof (s_server_t));
    assert (self);
    assert ((s_server_t *) &self->server == self);

    self->pipe = pipe;
    self->router = zsock_new (ZMQ_ROUTER);
    self->clients = zhash_new ();
    self->log = zlog_new ("zgossip");
    self->config = zconfig_new ("root", NULL);
    self->loop = zloop_new ();
    srandom ((unsigned int) zclock_time ());
    self->client_id = randof (1000);
    s_server_config_self (self);

    //  Initialize application server context
    self->server.log = self->log;
    self->server.config = self->config;
    server_initialize (&self->server);

    s_satisfy_pedantic_compilers ();
    return self;
}

static void
s_server_destroy (s_server_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        s_server_t *self = *self_p;
        server_terminate (&self->server);
        zsock_destroy (&self->router);
        zconfig_destroy (&self->config);
        zhash_destroy (&self->clients);
        zloop_destroy (&self->loop);
        zlog_destroy (&self->log);
        free (self);
        *self_p = NULL;
    }
}

//  Apply configuration tree:
//   * apply server configuration
//   * print any echo items in top-level sections
//   * apply sections that match methods

static void
s_server_apply_config (s_server_t *self)
{
    //  Apply echo commands and class methods
    zconfig_t *section = zconfig_locate (self->config, "zgossip");
    if (section)
        section = zconfig_child (section);

    while (section) {
        if (streq (zconfig_name (section), "echo"))
            zlog_notice (self->log, "%s", zconfig_value (section));
        else
        if (streq (zconfig_name (section), "bind")) {
            char *endpoint = zconfig_resolve (section, "endpoint", "?");
            int rc = zsock_bind (self->router, "%s", endpoint);
            assert (rc != -1);
        }
        section = zconfig_next (section);
    }
    s_server_config_self (self);
}

//  Process message from pipe

static int
s_server_api_message (zloop_t *loop, zsock_t *reader, void *argument)
{
    s_server_t *self = (s_server_t *) argument;
    zmsg_t *msg = zmsg_recv (self->pipe);
    if (!msg)
        return -1;              //  Interrupted; exit zloop
    char *method = zmsg_popstr (msg);
    if (streq (method, "$TERM")) {
        free (method);
        zmsg_destroy (&msg);
        return -1;
    }
    else
    if (streq (method, "BIND")) {
        char *endpoint = zmsg_popstr (msg);
        self->port = zsock_bind (self->router, "%s", endpoint);
        puts (endpoint);
        puts (zmq_strerror (zmq_errno ()));
        assert (self->port != -1);
        zstr_sendf (self->pipe, "%d", self->port);
        free (endpoint);
    }
    else
    if (streq (method, "CONFIGURE")) {
        char *config_file = zmsg_popstr (msg);
        zconfig_destroy (&self->config);
        self->config = zconfig_load (config_file);
        if (self->config) {
            s_server_apply_config (self);
            self->server.config = self->config;
        }
        else {
            zlog_warning (self->log,
                "cannot load config file '%s'\n", config_file);
            self->config = zconfig_new ("root", NULL);
        }
        free (config_file);
    }
    else
    if (streq (method, "SET")) {
        char *path = zmsg_popstr (msg);
        char *value = zmsg_popstr (msg);
        zconfig_put (self->config, path, value);
        s_server_config_self (self);
        free (path);
        free (value);
    }
    else {
        //  Execute custom method
        zmsg_t *reply = server_method (&self->server, method, msg);
        //  If reply isn't null, send it to caller
        zmsg_send (&reply, self->pipe);
    }
    free (method);
    zmsg_destroy (&msg);
    return 0;
}

//  Handle a message (a protocol request) from the client

static int
s_server_client_message (zloop_t *loop, zsock_t *reader, void *argument)
{
    s_server_t *self = (s_server_t *) argument;
    zgossip_msg_t *msg = zgossip_msg_recv (self->router);
    if (!msg)
        return -1;              //  Interrupted; exit zloop

    char *hashkey = zframe_strhex (zgossip_msg_routing_id (msg));
    s_client_t *client = (s_client_t *) zhash_lookup (self->clients, hashkey);
    if (client == NULL) {
        client = s_client_new (self, zgossip_msg_routing_id (msg));
        zhash_insert (self->clients, hashkey, client);
        zhash_freefn (self->clients, hashkey, s_client_free);
    }
    free (hashkey);

    //  Any input from client counts as activity
    if (client->expiry_timer)
        zloop_timer_end (self->loop, client->expiry_timer);
    //  Reset expiry timer
    client->expiry_timer = zloop_timer (
        self->loop, self->timeout, 1, s_client_expired, client);
        
    //  Queue request and possibly pass it to client state machine
    zlist_append (client->mailbox, msg);
    event_t event = s_client_filter_mailbox (client);
    if (event != NULL_event)
        s_client_execute (client, event);
        
    return 0;
}

//  Watch server config file and reload if changed

static int
s_watch_server_config (zloop_t *loop, int timer_id, void *argument)
{
    s_server_t *self = (s_server_t *) argument;
    if (zconfig_has_changed (self->config)
    &&  zconfig_reload (&self->config) == 0) {
        s_server_config_self (self);
        self->server.config = self->config;
        zlog_notice (self->log, "reloaded configuration from %s",
            zconfig_filename (self->config));
    }
    return 0;
}


//  ---------------------------------------------------------------------
//  This is the server actor, which polls its two sockets and processes
//  incoming messages

void
zgossip (zsock_t *pipe, void *args)
{
    //  Initialize
    s_server_t *self = s_server_new (pipe);
    assert (self);
    zsock_signal (pipe, 0);

    //  Set-up server monitor to watch for config file changes
    engine_set_monitor ((server_t *) self, 1000, s_watch_server_config);
    //  Set up handler for the two main sockets the server uses
    engine_handle_socket ((server_t *) self, self->pipe, s_server_api_message);
    engine_handle_socket ((server_t *) self, self->router, s_server_client_message);

    //  Run reactor until there's a termination signal
    zloop_start (self->loop);

    //  Reactor has ended
    s_server_destroy (&self);
}
