/*  =========================================================================
    zmonitor - socket event monitor

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
    The zmonitor class provides an API for obtaining socket events such as
    connected, listen, disconnected, etc. Socket events are only available
    for sockets connecting or bound to ipc:// and tcp:// endpoints.
@discuss
    This class wraps the ZMQ socket monitor API, see zmq_socket_monitor for
    details. Currently this class requires libzmq v4.0 or later and is empty
    on older versions of libzmq.
@end
*/

#include "../include/czmq.h"

//  This code needs backporting to work with ZMQ v3.2
#if (ZMQ_VERSION_MAJOR == 4)

//  Structure of our class
struct _zmonitor_t {
    void *socket;               //  Socket being monitored
    void *pipe;                 //  Pipe through to backend agent
};


//  Background task does the real I/O
static void
    s_agent_task (void *args, zctx_t *ctx, void *pipe);


//  --------------------------------------------------------------------------
//  Create a new socket monitor

zmonitor_t *
zmonitor_new (zctx_t *ctx, void *socket, int events)
{
    zmonitor_t *self = (zmonitor_t *) zmalloc (sizeof (zmonitor_t));
    assert (self);

    //  Start background agent to connect to the inproc monitor socket
    assert (ctx);
    self->pipe = zthread_fork (ctx, s_agent_task, NULL);
    if (self->pipe) {
        self->socket = socket;
        
        //  Register a monitor endpoint on the socket
        char *monitor_endpoint = (char *) zmalloc (100);
        sprintf (monitor_endpoint, "inproc://zmonitor-%p", self->socket);
        int rc = zmq_socket_monitor (self->socket, monitor_endpoint, events);
        assert (rc == 0);
        
        //  Configure backend agent with monitor endpoint
        zstr_sendf (self->pipe, "%s", monitor_endpoint);
        free (monitor_endpoint);

        char *status = zstr_recv (self->pipe);
        if (strneq (status, "OK"))
            zmonitor_destroy (&self);
        zstr_free (&status);
    }
    else {
        free (self);
        self = NULL;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a socket monitor

void
zmonitor_destroy (zmonitor_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmonitor_t *self = *self_p;
        //  Deregister monitor endpoint
        zmq_socket_monitor (self->socket, NULL, 0);
        zstr_send (self->pipe, "TERMINATE");
        char *reply = zstr_recv (self->pipe);
        zstr_free (&reply);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive a status message from the monitor; if no message arrives within
//  500 msec, or the call was interrupted, returns NULL.

zmsg_t *
zmonitor_recv (zmonitor_t *self)
{
    zsocket_set_rcvtimeo (self->pipe, 500);
    return zmsg_recv (self->pipe);
}


//  --------------------------------------------------------------------------
//  Get the ZeroMQ socket, for polling

void *
zmonitor_socket (zmonitor_t *self)
{
    assert (self);
    return self->pipe;
}


//  --------------------------------------------------------------------------
//  Enable verbose tracing of commands and activity

void
zmonitor_set_verbose (zmonitor_t *self, bool verbose)
{
    assert (self);
    zstr_sendm (self->pipe, "VERBOSE");
    zstr_sendf (self->pipe, "%d", verbose);
}


//  --------------------------------------------------------------------------
//  Selftest of this class

static bool
s_check_event (zmonitor_t *self, int expected_event)
{
    zmsg_t *msg = zmonitor_recv (self);
    assert (msg);
    char *string = zmsg_popstr (msg);
    int event = atoi (string);
    zstr_free (&string);
    zmsg_destroy (&msg);
    return event == expected_event;
}

void
zmonitor_test (bool verbose)
{
    printf (" * zmonitor: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    bool result;

    void *sink = zsocket_new (ctx, ZMQ_PULL);
    zmonitor_t *sinkmon = zmonitor_new (ctx,
        sink, ZMQ_EVENT_LISTENING | ZMQ_EVENT_ACCEPTED);
    zmonitor_set_verbose (sinkmon, verbose);

    //  Check sink is now listening
    zsocket_bind (sink, "tcp://*:5555");
    result = s_check_event (sinkmon, ZMQ_EVENT_LISTENING);
    assert (result);

    void *source = zsocket_new (ctx, ZMQ_PUSH);
    zmonitor_t *sourcemon = zmonitor_new (ctx,
        source, ZMQ_EVENT_CONNECTED | ZMQ_EVENT_DISCONNECTED);
    zmonitor_set_verbose (sourcemon, verbose);
    zsocket_connect (source, "tcp://localhost:5555");

    //  Check source connected to sink
    result = s_check_event (sourcemon, ZMQ_EVENT_CONNECTED);
    assert (result);

    //  Check sink accepted connection
    result = s_check_event (sinkmon, ZMQ_EVENT_ACCEPTED);
    assert (result);

    //  Destroy sink to trigger a disconnect event on the source
    //  PH: disabled since this causes an access violation in
    //  zmonitor_destroy as the socket is no longer valid.
//     zsocket_destroy (ctx, sink);
//     result = s_check_event (sourcemon, ZMQ_EVENT_DISCONNECTED);
//     assert (result);

    zmonitor_destroy (&sinkmon);
    zmonitor_destroy (&sourcemon);
    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
}


//  --------------------------------------------------------------------------
//  Backend agent implementation

//  Agent instance

typedef struct {
    zctx_t *ctx;
    void *pipe;             //  Socket back to application
    void *socket;           //  Monitored socket
    char *endpoint;         //  Monitored endpoint
    bool verbose;           //  Trace activity to stdout
    bool terminated;
} agent_t;

//  Prototypes for local functions we use in the agent

static agent_t *
    s_agent_new (zctx_t *ctx, void *pipe, char *endpoint);
static void
    s_api_command (agent_t *self);
static void
    s_socket_event (agent_t *self);
static void
    s_agent_destroy (agent_t **self_p);


//  This is the background task that monitors socket events

static void
s_agent_task (void *args, zctx_t *ctx, void *pipe)
{
    char *endpoint = zstr_recv (pipe);
    assert (endpoint);

    agent_t *self = s_agent_new (ctx, pipe, endpoint);
    zpoller_t *poller = zpoller_new (self->pipe, self->socket, NULL);

    while (!self->terminated) {
        //  Poll on API pipe and on monitor socket
        void *result = zpoller_wait (poller, -1);
        if (result == NULL)
            break;              // Interrupted
        else
        if (result == self->pipe)
            s_api_command (self);
        else
        if (result == self->socket)
            s_socket_event (self);
    }
    zpoller_destroy (&poller);
    s_agent_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Create and initialize new agent instance

static agent_t *
s_agent_new (zctx_t *ctx, void *pipe, char *endpoint)
{
    agent_t *self = (agent_t *) malloc (sizeof (agent_t));
    assert (self);

    self->ctx = ctx;
    self->pipe = pipe;
    self->endpoint = endpoint;

    // connect to the socket monitor inproc endpoint
    self->socket = zsocket_new (self->ctx, ZMQ_PAIR);
    assert (self->socket);

    if (zsocket_connect (self->socket, "%s", self->endpoint) == 0)
        zstr_send (self->pipe, "OK");
    else
        zstr_send (self->pipe, "ERROR");

    return self;
}


//  --------------------------------------------------------------------------
//  Handle command from API

static void
s_api_command (agent_t *self)
{
    char *command = zstr_recv (self->pipe);
    if (streq (command, "TERMINATE")) {
        self->terminated = true;
        zstr_send (self->pipe, "OK");
    }
    else
    if (streq (command, "VERBOSE")) {
        char *verbose = zstr_recv (self->pipe);
        self->verbose = *verbose == '1';
        free (verbose);
    }
    else
        printf ("E: zmonitor unexpected API command '%s'\n", command);

    free (command);
}


//  --------------------------------------------------------------------------
// Handle event from socket monitor

static void
s_socket_event (agent_t *self)
{
    zframe_t *frame;
    zmq_event_t event;
    char *description = "Unknown";
    char address [1025];

    //  Copy event data into event struct
    frame = zframe_recv (self->socket);

    //  Extract id of the event as bitfield
    memcpy (&(event.event), zframe_data (frame), sizeof (event.event));

    //  Extract value which is either error code, fd, or reconnect interval
    memcpy (&(event.value), zframe_data (frame) + sizeof (event.event),
           sizeof (event.value));
    zframe_destroy (&frame);

    //  Copy address part
    frame = zframe_recv (self->socket);
    memcpy (address, zframe_data (frame), zframe_size (frame));
    address [zframe_size (frame)] = 0;  // Terminate address string
    zframe_destroy (&frame);

    switch (event.event) {
        case ZMQ_EVENT_ACCEPTED:
            description = "Accepted";
            break;
        case ZMQ_EVENT_ACCEPT_FAILED:
            description = "Accept failed";
            break;
        case ZMQ_EVENT_BIND_FAILED:
            description = "Bind failed";
            break;
        case ZMQ_EVENT_CLOSED:
            description = "Closed";
            break;
        case ZMQ_EVENT_CLOSE_FAILED:
            description = "Close failed";
            break;
        case ZMQ_EVENT_DISCONNECTED:
            description = "Disconnected";
            break;
        case ZMQ_EVENT_CONNECTED:
            description = "Connected";
            break;
        case ZMQ_EVENT_CONNECT_DELAYED:
            description = "Connect delayed";
            break;
        case ZMQ_EVENT_CONNECT_RETRIED:
            description = "Connect retried";
            break;
        case ZMQ_EVENT_LISTENING:
            description = "Listening";
            break;
        case ZMQ_EVENT_MONITOR_STOPPED:
            description = "Monitor stopped";
            break;
        default:
            if (self->verbose)
                printf ("Unknown socket monitor event: %d", event.event);
            break;
    }
    if (self->verbose)
        printf ("I: zmonitor: %s - %s\n", description, address);

    zmsg_t *msg = zmsg_new();
    zmsg_addstrf (msg, "%d", (int) event.event);
    zmsg_addstrf (msg, "%d", (int) event.value);
    zmsg_addstrf (msg, "%s", address);
    zmsg_addstrf (msg, "%s", description);
    zmsg_send (&msg, self->pipe);
}


//  --------------------------------------------------------------------------
//  Destroy agent instance

static void
s_agent_destroy (agent_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        agent_t *self = *self_p;
        free (self->endpoint);
        free (self);
        *self_p = NULL;
    }
}

#endif          //  ZeroMQ 4.0 or later
