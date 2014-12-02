/*  =========================================================================
    zmonitor_v2 - socket event monitor (deprecated)

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
    This class wraps the ZMQ socket monitor API, see zmq_socket_monitor for
    details. Currently this class requires libzmq v4.0 or later and is empty
    on older versions of libzmq.
@discuss
    This class is deprecated in CZMQ v3; it works together with zctx, zsocket,
    and other deprecated V2 classes. New applications should use the V3 zmonitor
    interface, based on zactor, together with the zsock class for sockets.
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
    if (!self)
        return NULL;

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
    else
        zmonitor_destroy (&self);
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
        if (self->socket)
            zmq_socket_monitor (self->socket, NULL, 0);
        if (self->pipe) {
            zstr_send (self->pipe, "TERMINATE");
            char *reply = zstr_recv (self->pipe);
            zstr_free (&reply);
        }
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
    if (!self)
        return;
    zpoller_t *poller = zpoller_new (self->pipe, self->socket, NULL);
    if (poller)
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


//  --------------------------------------------------------------------------
//  Create and initialize new agent instance

static agent_t *
s_agent_new (zctx_t *ctx, void *pipe, char *endpoint)
{
    agent_t *self = (agent_t *) zmalloc (sizeof (agent_t));
    if (!self)
        return NULL;

    self->ctx = ctx;
    self->pipe = pipe;
    self->endpoint = endpoint;

    // connect to the socket monitor inproc endpoint
    self->socket = zsocket_new (self->ctx, ZMQ_PAIR);
    if (self->socket) {
        if (zsocket_connect (self->socket, "%s", self->endpoint) == 0)
            zstr_send (self->pipe, "OK");
        else
            zstr_send (self->pipe, "ERROR");
    }
    else
        s_agent_destroy (&self);
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
        zsys_error ("zmonitor unexpected API command '%s'", command);

    free (command);
}


//  --------------------------------------------------------------------------
// Handle event from socket monitor

static void
s_socket_event (agent_t *self)
{
    //  First frame is event number and value
    zframe_t *frame = zframe_recv (self->socket);
    int event = *(uint16_t *) (zframe_data (frame));
    int value = *(uint32_t *) (zframe_data (frame) + 2);
    zframe_destroy (&frame);

    //  Second frame is address
    char *address = zstr_recv (self->socket);
    char *description = "Unknown";
    switch (event) {
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
            zsys_error ("illegal socket monitor event: %d", event);
            break;
    }
    if (self->verbose)
        zsys_info ("zmonitor: %s - %s", description, address);

    zstr_sendfm (self->pipe, "%d", event);
    zstr_sendfm (self->pipe, "%d", value);
    zstr_sendm (self->pipe, address);
    zstr_send (self->pipe, description);
    free (address);
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
#endif          //  ZeroMQ 4.0 or later

void
zmonitor_v2_test (bool verbose)
{
    printf (" * zmonitor (deprecated): ");
    if (verbose)
        printf ("\n");

#if (ZMQ_VERSION_MAJOR == 4)
    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    bool result;

    void *sink = zsocket_new (ctx, ZMQ_PULL);
    assert (sink);
    zmonitor_t *sinkmon = zmonitor_new (ctx,
                                        sink, ZMQ_EVENT_LISTENING | ZMQ_EVENT_ACCEPTED);
    assert (sinkmon);
    zmonitor_set_verbose (sinkmon, verbose);

    //  Check sink is now listening
    int port_nbr = zsocket_bind (sink, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    result = s_check_event (sinkmon, ZMQ_EVENT_LISTENING);
    assert (result);

    void *source = zsocket_new (ctx, ZMQ_PUSH);
    assert (source);
    zmonitor_t *sourcemon = zmonitor_new (ctx,
                                          source, ZMQ_EVENT_CONNECTED | ZMQ_EVENT_DISCONNECTED);
    assert (sourcemon);
    zmonitor_set_verbose (sourcemon, verbose);
    zsocket_connect (source, "tcp://127.0.0.1:%d", port_nbr);

    //  Check source connected to sink
    result = s_check_event (sourcemon, ZMQ_EVENT_CONNECTED);
    assert (result);

    //  Check sink accepted connection
    result = s_check_event (sinkmon, ZMQ_EVENT_ACCEPTED);
    assert (result);

    zmonitor_destroy (&sinkmon);
    zmonitor_destroy (&sourcemon);
    zctx_destroy (&ctx);
    //  @end
#endif          //  ZeroMQ 4.0 or later

    printf ("OK\n");
}

