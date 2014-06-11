/*  =========================================================================
    zsock_monitor - socket event monitor

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
    The zsock_monitor class provides an API for obtaining socket events such as
    connected, listen, disconnected, etc. Socket events are only available
    for sockets connecting or bound to ipc:// and tcp:// endpoints.
@discuss
    This class wraps the ZMQ socket monitor API, see zmq_socket_monitor for
    details. Currently this class requires libzmq v4.0 or later and is empty
    on older versions of libzmq.
@end
*/

#include "../include/czmq.h"

//  Structure of our class
struct _zsock_monitor_t {
    zctx_t *ctx;                //  Need this until we can use zactor
    zsock_t *sock;              //  Socket being monitored
    void *pipe;                 //  Pipe through to backend agent
};


//  Background task does the real I/O
static void
    s_agent_task (void *args, zctx_t *ctx, void *pipe);


//  --------------------------------------------------------------------------
//  Create a new socket monitor

zsock_monitor_t *
zsock_monitor_new (zsock_t *sock)
{
    zsock_monitor_t *self = (zsock_monitor_t *) zmalloc (sizeof (zsock_monitor_t));
    assert (self);

    //  Start background agent to connect to the inproc monitor socket
    self->ctx = zctx_new ();
    self->pipe = zthread_fork (self->ctx, s_agent_task, sock);
    if (self->pipe) {
        char *status = zstr_recv (self->pipe);
        if (strneq (status, "OK"))
            zsock_monitor_destroy (&self);
        free (status);
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
zsock_monitor_destroy (zsock_monitor_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zsock_monitor_t *self = *self_p;
        zstr_send (self->pipe, "TERMINATE");
        zsocket_wait (self->pipe);
        zctx_destroy (&self->ctx);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Listen to a specified event

void
zsock_monitor_listen (zsock_monitor_t *self, int event)
{
    assert (self);
    zstr_sendm (self->pipe, "LISTEN");
    zstr_sendf (self->pipe, "%d", event);
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Start the monitor; once started, new calls to zsock_monitor_event() will
//  have no effect.

void
zsock_monitor_start (zsock_monitor_t *self)
{
    assert (self);
    zstr_send (self->pipe, "START");
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Receive a status message from the monitor; if no message arrives within
//  500 msec, or the call was interrupted, returns NULL.

zmsg_t *
zsock_monitor_recv (zsock_monitor_t *self)
{
    zsocket_set_rcvtimeo (self->pipe, 500);
    return zmsg_recv (self->pipe);
}


//  --------------------------------------------------------------------------
//  Return the status pipe, to allow polling for monitor data.

zsock_t *
zsock_monitor_handle (zsock_monitor_t *self)
{
    assert (self);
    return (zsock_t *) self->pipe;
}


//  --------------------------------------------------------------------------
//  Enable verbose tracing of commands and activity

void
zsock_monitor_set_verbose (zsock_monitor_t *self, bool verbose)
{
    assert (self);
    zstr_sendm (self->pipe, "VERBOSE");
    zstr_sendf (self->pipe, "%d", verbose);
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Backend agent implementation

//  Agent instance

typedef struct {
    void *pipe;             //  Socket back to application
    zpoller_t *poller;      //  Activity poller
    void *monitored;        //  Monitored socket
    zsock_t *sink;          //  Sink for monitor events
    int events;             //  Monitored event mask
    bool verbose;           //  Trace activity to stdout
    bool terminated;
} agent_t;


static agent_t *
s_agent_new (void *pipe, zsock_t *sock)
{
    agent_t *self = (agent_t *) zmalloc (sizeof (agent_t));
    assert (self);
    self->pipe = pipe;
    self->monitored = zsock_resolve (sock);
    self->poller = zpoller_new (self->pipe, NULL);
    zstr_send (self->pipe, "OK");
    return self;
}

static void
s_agent_destroy (agent_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        agent_t *self = *self_p;
#if defined (ZMQ_EVENT_ALL)
        zmq_socket_monitor (self->monitored, NULL, 0);
#endif
        zpoller_destroy (&self->poller);
        zsock_destroy (&self->sink);
        free (self);
        *self_p = NULL;
    }
}

static void
s_agent_start (agent_t *self)
{
    assert (!self->sink);
    char *endpoint = zsys_sprintf ("inproc://zsock_monitor-%p", self->monitored);
    int rc;
#if defined (ZMQ_EVENT_ALL)
    rc = zmq_socket_monitor (self->monitored, endpoint, self->events);
    assert (rc == 0);
#endif
    self->sink = zsock_new (ZMQ_PAIR);
    assert (self->sink);
    rc = zsock_connect (self->sink, "%s", endpoint);
    assert (rc == 0);
    zpoller_add (self->poller, self->sink);
    free (endpoint);
}

//  Handle command from API

static void
s_api_command (agent_t *self)
{
    char *command = zstr_recv (self->pipe);
    if (streq (command, "LISTEN")) {
        char *event_str = zstr_recv (self->pipe);
        self->events |= atoi (event_str);
        free (event_str);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "START")) {
        s_agent_start (self);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "VERBOSE")) {
        char *verbose = zstr_recv (self->pipe);
        self->verbose = *verbose == '1';
        free (verbose);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "TERMINATE")) {
        self->terminated = true;
        zstr_send (self->pipe, "OK");
    }
    else
        zsys_error ("zsock_monitor unexpected API command '%s'", command);

    free (command);
}

//  Handle event from socket monitor

static void
s_monitor_event (agent_t *self)
{
#if defined (ZMQ_EVENT_ALL)
#if (ZMQ_VERSION_MAJOR == 4)
    //  First frame is event number and value
    zframe_t *frame = zframe_recv (self->sink);
    int event = *(uint16_t *) (zframe_data (frame));
    int value = *(uint32_t *) (zframe_data (frame) + 2);
    //  Address is in second message frame
    char *address = zstr_recv (self->sink);
    zframe_destroy (&frame);

#elif (ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 2)
    //  zmq_event_t is passed as-is in the frame
    zframe_t *frame = zframe_recv (self->sink);
    zmq_event_t *eptr = (zmq_event_t *) zframe_data (frame);
    int event = eptr->event;
    int value = eptr->data.listening.fd;
    char *address = strdup (eptr->data.listening.addr);
    zframe_destroy (&frame);

#else
    //  We can't plausibly be here with other versions of libzmq
    assert (false);
#endif
    
    //  Now map event to something we can send over the pipe
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
#if (ZMQ_VERSION_MAJOR == 4)
        case ZMQ_EVENT_MONITOR_STOPPED:
            description = "Monitor stopped";
            break;
#endif
        default:
            zsys_error ("illegal socket monitor event: %d", event);
            break;
    }
    if (self->verbose)
        zsys_info ("zsock_monitor: %s - %s", description, address);

    zstr_sendfm (self->pipe, "%d", event);
    zstr_sendfm (self->pipe, "%d", value);
    zstr_sendm  (self->pipe, address);
    zstr_send   (self->pipe, description);
    free (address);
#endif
}

//  This is the background task that monitors socket events

static void
s_agent_task (void *args, zctx_t *ctx, void *pipe)
{
    zsock_t *sock = (zsock_t *) args;
    agent_t *self = s_agent_new (pipe, sock);

    while (!self->terminated) {
        //  Poll on API pipe and on monitor socket
        void *result = zpoller_wait (self->poller, -1);
        if (result == NULL)
            break;              // Interrupted
        else
        if (result == self->pipe)
            s_api_command (self);
        else
        if (result == self->sink)
            s_monitor_event (self);
    }
    s_agent_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Selftest of this class

#if defined (ZMQ_EVENT_ALL)
static bool
s_check_event (zsock_monitor_t *self, int expected_event)
{
    zmsg_t *msg = zsock_monitor_recv (self);
    assert (msg);
    char *string = zmsg_popstr (msg);
    int event = atoi (string);
    free (string);
    zmsg_destroy (&msg);
    return event == expected_event;
}
#endif

void
zsock_monitor_test (bool verbose)
{
    printf (" * zsock_monitor: ");
    if (verbose)
        printf ("\n");

    //  @selftest
#if defined (ZMQ_EVENT_ALL)
    zsock_t *sink = zsock_new (ZMQ_PULL);
    zsock_monitor_t *sinkmon = zsock_monitor_new (sink);
    zsock_monitor_set_verbose (sinkmon, verbose);
    zsock_monitor_listen (sinkmon, ZMQ_EVENT_LISTENING);
    zsock_monitor_listen (sinkmon, ZMQ_EVENT_ACCEPTED);
    zsock_monitor_start (sinkmon);

    zsock_t *source = zsock_new (ZMQ_PUSH);
    zsock_monitor_t *sourcemon = zsock_monitor_new (source);
    zsock_monitor_set_verbose (sourcemon, verbose);
    zsock_monitor_listen (sourcemon, ZMQ_EVENT_CONNECTED);
    zsock_monitor_listen (sourcemon, ZMQ_EVENT_DISCONNECTED);
    zsock_monitor_start (sourcemon);

    //  Allow a brief time for the message to get there...
    zmq_poll (NULL, 0, 200);

    //  Check sink is now listening
    int port_nbr = zsock_bind (sink, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    bool result = s_check_event (sinkmon, ZMQ_EVENT_LISTENING);
    assert (result);

    //  Check source connected to sink
    zsock_connect (source, "tcp://127.0.0.1:%d", port_nbr);
    result = s_check_event (sourcemon, ZMQ_EVENT_CONNECTED);
    assert (result);

    //  Check sink accepted connection
    result = s_check_event (sinkmon, ZMQ_EVENT_ACCEPTED);
    assert (result);

    zsock_monitor_destroy (&sinkmon);
    zsock_monitor_destroy (&sourcemon);
    zsock_destroy (&sink);
    zsock_destroy (&source);
#endif
    //  @end
    printf ("OK\n");
}
