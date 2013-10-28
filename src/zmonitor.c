/*  =========================================================================
    zmonitor - socket event monitor

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
@header
    The zmonitor class provides an API for obtaining socket events such as
    connected, listen, disconnected, etc. Socket events are only available
    for ipc and tcp socket types.
@discuss
@end
*/


#include "../include/czmq.h"


//  Structure of our class
struct _zmonitor_t {
    zctx_t *ctx;                //  Private 0MQ context
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

    // register a monitor endpoint on the socket
    char *monitor_endpoint = (char *) zmalloc (100);
    sprintf (monitor_endpoint, "inproc://zmonitor-%p", socket);
    int rc = zmq_socket_monitor (socket, monitor_endpoint, events);
    assert (rc == 0);

    //  Start background agent to connect to the inproc monitor socket
    self->pipe = zthread_fork (ctx, s_agent_task, NULL);

    // Configure backend agent with monitor endpoint
    zstr_send (self->pipe, "%s", monitor_endpoint);
    free (monitor_endpoint);

    char *status = zstr_recv (self->pipe);
    if (strneq (status, "OK"))
        zmonitor_destroy (&self);
    free (status);
    return self;
}

//  --------------------------------------------------------------------------
//  Destructor

void
zmonitor_destroy (zmonitor_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmonitor_t *self = *self_p;
        zstr_send (self->pipe, "TERMINATE");
        free (zstr_recv (self->pipe));
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Get the ZeroMQ socket, for polling or receiving socket
//  event messages from the backend agent on.

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
    zstr_send (self->pipe, "%d", verbose);
}


//  --------------------------------------------------------------------------
//  Self test of this class

static bool
s_check_event(void *s, int expected_event)
{
    zpoller_t *poller = zpoller_new (s, NULL);
    void *result = zpoller_wait (poller, 500);
    assert (result);
    assert (result == s);
    zmsg_t *msg = zmsg_recv (s);
    char *evstr = zmsg_popstr (msg);
    int actual_event = atoi (evstr);
    free (evstr);
    zmsg_destroy (&msg);
    zpoller_destroy (&poller);
    return actual_event == expected_event;
}

void
zmonitor_test (bool verbose)
{
    printf (" * zmonitor: ");

    if (verbose)
        printf("\n");
    bool result;
    //  @selftest
    zctx_t *ctx = zctx_new ();

    void *sink = zsocket_new (ctx, ZMQ_PULL);
    zmonitor_t *sinkmon = zmonitor_new (ctx,
        sink, ZMQ_EVENT_LISTENING | ZMQ_EVENT_ACCEPTED);
    zmonitor_set_verbose (sinkmon, verbose);
    void *sinkmon_sock = zmonitor_socket (sinkmon);

    // check sink is now listening
    zsocket_bind (sink, "tcp://*:5555");
    result = s_check_event (sinkmon_sock, ZMQ_EVENT_LISTENING);
    assert (result);

    void *source = zsocket_new (ctx, ZMQ_PUSH);
    zmonitor_t *sourcemon = zmonitor_new (ctx,
        source, ZMQ_EVENT_CONNECTED | ZMQ_EVENT_DISCONNECTED);
    zmonitor_set_verbose (sourcemon, verbose);
    void *sourcemon_sock = zmonitor_socket (sourcemon);
    zsocket_connect (source, "tcp://localhost:5555");

    // check source connected to sink
    result = s_check_event (sourcemon_sock, ZMQ_EVENT_CONNECTED);
    assert (result);

    // check sink accepted connection
    result = s_check_event (sinkmon_sock, ZMQ_EVENT_ACCEPTED);
    assert (result);

    // destroy sink to trigger a disconnect event on the source
    zsocket_destroy (ctx, sink);
    result = s_check_event (sourcemon_sock, ZMQ_EVENT_DISCONNECTED);
    assert (result);

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
    void *pipe;           // Socket back to application
    void *mon;            // monitor socket
    char *endpoint;       // monitor endpoint
    bool verbose;         // Trace activity to stdout
    bool terminated;

} agent_t;

//  Prototypes for local functions we use in the agent

static agent_t *
    s_agent_new (zctx_t *ctx, void *pipe, char *endpoint);
static void
    s_api_command (agent_t *self);
static void
    s_event_recv (agent_t *self);
static void
    s_agent_destroy (agent_t **self_p);


//  This is the background task that monitors socket events

static void
s_agent_task (void *args, zctx_t *ctx, void *pipe)
{

    char *endpoint = zstr_recv (pipe);
    assert (endpoint);

    //  Create agent instance
    agent_t *self = s_agent_new (ctx, pipe, endpoint);

    zpoller_t *poller = zpoller_new (self->pipe, self->mon, NULL);

    while (!zctx_interrupted) {

        //  Poll on API pipe and on monitor socket
        void *result = zpoller_wait (poller, -1);

        if (result == NULL)
            break; // Interrupted

        if (result == self->pipe)
            s_api_command (self);
        if (result == self->mon)
            s_event_recv (self);

        if (self->terminated)
            break;
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
    self->verbose = false;
    self->terminated = false;

    // connect to the socket monitor inproc endpoint
    self->mon = zsocket_new (self->ctx, ZMQ_PAIR);
    assert (self->mon);

    if (zsocket_connect (self->mon, self->endpoint) == 0)
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
s_event_recv (agent_t *self)
{

    zframe_t *frame;
    zmq_event_t event;
    char *description = "Unknown";
    char addr[1025];

    // copy event data into event struct
    frame = zframe_recv (self->mon);
    // extract id of the event as bitfield
    memcpy(&(event.event),
           zframe_data (frame),
           sizeof (event.event));
    // extract value which is either error code, fd or reconnect interval
    memcpy(&(event.value),
           zframe_data (frame) + sizeof (event.event),
           sizeof (event.value));
    zframe_destroy (&frame);

    // copy address part
    frame = zframe_recv (self->mon);
    memcpy(addr, zframe_data (frame), zframe_size (frame));
    *(addr + zframe_size (frame)) = '\0'; // add null terminator to address string
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
        printf ("I: zmonitor: %s - %s\n", description, addr);

    zmsg_t *msg = zmsg_new();
    zmsg_addstr (msg, "%d", (int) event.event );
    zmsg_addstr (msg, "%d", (int) event.value );
    zmsg_addstr (msg, "%s", addr);
    zmsg_addstr (msg, "%s", description);
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