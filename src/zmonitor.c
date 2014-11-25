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
    The zmonitor actor provides an API for obtaining socket events such as
    connected, listen, disconnected, etc. Socket events are only available
    for sockets connecting or bound to ipc:// and tcp:// endpoints.
@discuss
    This class wraps the ZMQ socket monitor API, see zmq_socket_monitor for
    details. Works on all versions of libzmq from 3.2 onwards. This class
    replaces zproxy_v2, and is meant for applications that use the CZMQ v3
    API (meaning, zsock).
@end
*/

#include "../include/czmq.h"

//  --------------------------------------------------------------------------
//  The self_t structure holds the state for one actor instance

typedef struct {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    void *monitored;            //  Monitored libzmq socket
    zsock_t *sink;              //  Sink for monitor events
    int events;                 //  Monitored event mask
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
} self_t;

static void
s_self_destroy (self_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        self_t *self = *self_p;
#if defined (ZMQ_EVENT_ALL)
        zmq_socket_monitor (self->monitored, NULL, 0);
#endif
        zpoller_destroy (&self->poller);
        zsock_destroy (&self->sink);
        free (self);
        *self_p = NULL;
    }
}

static self_t *
s_self_new (zsock_t *pipe, void *sock)
{
    self_t *self = (self_t *) zmalloc (sizeof (self_t));
    if (!self)
        return NULL;

    self->pipe = pipe;
    self->monitored = zsock_resolve (sock);
    self->poller = zpoller_new (self->pipe, NULL);
    if (!self->poller)
        s_self_destroy (&self);
    return self;
}


//  --------------------------------------------------------------------------
//  Add listener for specified event

static void
s_self_listen (self_t *self, const char *event)
{
#if defined (ZMQ_EVENT_ALL)
    if (streq (event, "CONNECTED"))
        self->events |= ZMQ_EVENT_CONNECTED;
    else
    if (streq (event, "CONNECT_DELAYED"))
        self->events |= ZMQ_EVENT_CONNECT_DELAYED;
    else
    if (streq (event, "CONNECT_RETRIED"))
        self->events |= ZMQ_EVENT_CONNECT_RETRIED;
    else
    if (streq (event, "LISTENING"))
        self->events |= ZMQ_EVENT_LISTENING;
    else
    if (streq (event, "BIND_FAILED"))
        self->events |= ZMQ_EVENT_BIND_FAILED;
    else
    if (streq (event, "ACCEPTED"))
        self->events |= ZMQ_EVENT_ACCEPTED;
    else
    if (streq (event, "ACCEPT_FAILED"))
        self->events |= ZMQ_EVENT_ACCEPT_FAILED;
    else
    if (streq (event, "CLOSED"))
        self->events |= ZMQ_EVENT_CLOSED;
    else
    if (streq (event, "CLOSE_FAILED"))
        self->events |= ZMQ_EVENT_CLOSE_FAILED;
    else
    if (streq (event, "DISCONNECTED"))
        self->events |= ZMQ_EVENT_DISCONNECTED;
    else
#if (ZMQ_VERSION_MAJOR == 4)
    if (streq (event, "MONITOR_STOPPED"))
        self->events |= ZMQ_EVENT_MONITOR_STOPPED;
    else
#endif
    if (streq (event, "ALL"))
        self->events |= ZMQ_EVENT_ALL;
    else
        zsys_warning ("zmonitor: - invalid listen event=%s", event);
#endif
}


//  --------------------------------------------------------------------------
//  Start monitoring

static void
s_self_start (self_t *self)
{
    assert (!self->sink);
    char *endpoint = zsys_sprintf ("inproc://zmonitor-%p", self->monitored);
    assert (endpoint);
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
    if (!command) {
        s_self_destroy (&self);
        return -1;
    }
    if (self->verbose)
        zsys_info ("zmonitor: API command=%s", command);

    if (streq (command, "LISTEN")) {
        char *event = zmsg_popstr (request);
        while (event) {
            if (self->verbose)
                zsys_info ("zmonitor: - listening to event=%s", event);
            s_self_listen (self, event);
            zstr_free (&event);
            event = zmsg_popstr (request);
        }
    }
    else
    if (streq (command, "START")) {
        s_self_start (self);
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "VERBOSE"))
        self->verbose = true;
    else
    if (streq (command, "$TERM"))
        self->terminated = true;
    else {
        zsys_error ("zmonitor: - invalid command: %s", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
    return 0;
}


//  Handle event from socket monitor

static void
s_self_handle_sink (self_t *self)
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
    assert (address);
    zframe_destroy (&frame);

#else
    //  We can't plausibly be here with other versions of libzmq
    assert (false);
#endif

    //  Now map event to text equivalent
    char *name;
    switch (event) {
        case ZMQ_EVENT_ACCEPTED:
            name = "ACCEPTED";
            break;
        case ZMQ_EVENT_ACCEPT_FAILED:
            name = "ACCEPT_FAILED";
            break;
        case ZMQ_EVENT_BIND_FAILED:
            name = "BIND_FAILED";
            break;
        case ZMQ_EVENT_CLOSED:
            name = "CLOSED";
            break;
        case ZMQ_EVENT_CLOSE_FAILED:
            name = "CLOSE_FAILED";
            break;
        case ZMQ_EVENT_DISCONNECTED:
            name = "DISCONNECTED";
            break;
        case ZMQ_EVENT_CONNECTED:
            name = "CONNECTED";
            break;
        case ZMQ_EVENT_CONNECT_DELAYED:
            name = "CONNECT_DELAYED";
            break;
        case ZMQ_EVENT_CONNECT_RETRIED:
            name = "CONNECT_RETRIED";
            break;
        case ZMQ_EVENT_LISTENING:
            name = "LISTENING";
            break;
#if (ZMQ_VERSION_MAJOR == 4)
        case ZMQ_EVENT_MONITOR_STOPPED:
            name = "MONITOR_STOPPED";
            break;
#endif
        default:
            zsys_error ("illegal socket monitor event: %d", event);
            name = "UNKNOWN";
            break;
    }
    if (self->verbose)
        zsys_info ("zmonitor: %s - %s", name, address);

    zstr_sendfm (self->pipe, "%s", name);
    zstr_sendfm (self->pipe, "%d", value);
    zstr_send (self->pipe, address);
    free (address);
#endif
}


//  --------------------------------------------------------------------------
//  zmonitor() implements the zmonitor actor interface

void
zmonitor (zsock_t *pipe, void *sock)
{
    self_t *self = s_self_new (pipe, sock);
    assert (self);
    //  Signal successful initialization
    zsock_signal (pipe, 0);

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, -1);
        if (which == self->pipe)
            s_self_handle_pipe (self);
        else
        if (which == self->sink)
            s_self_handle_sink (self);
        else
        if (zpoller_terminated (self->poller))
            break;          //  Interrupted
    }
    s_self_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Selftest

#if defined (ZMQ_EVENT_ALL)
static void
s_assert_event (zactor_t *self, char *expected)
{
    zmsg_t *msg = zmsg_recv (self);
    assert (msg);
    char *event = zmsg_popstr (msg);
    assert (streq (event, expected));
    free (event);
    zmsg_destroy (&msg);
}
#endif

void
zmonitor_test (bool verbose)
{
    printf (" * zmonitor: ");
    if (verbose)
        printf ("\n");

#if defined (ZMQ_EVENT_ALL)
    //  @selftest
    zsock_t *client = zsock_new (ZMQ_DEALER);
    assert (client);
    zactor_t *clientmon = zactor_new (zmonitor, client);
    assert (clientmon);
    if (verbose)
        zstr_sendx (clientmon, "VERBOSE", NULL);
    zstr_sendx (clientmon, "LISTEN", "LISTENING", "ACCEPTED", NULL);
    zstr_sendx (clientmon, "START", NULL);
    zsock_wait (clientmon);

    zsock_t *server = zsock_new (ZMQ_DEALER);
    assert (server);
    zactor_t *servermon = zactor_new (zmonitor, server);
    assert (servermon);
    if (verbose)
        zstr_sendx (servermon, "VERBOSE", NULL);
    zstr_sendx (servermon, "LISTEN", "CONNECTED", "DISCONNECTED", NULL);
    zstr_sendx (servermon, "START", NULL);
    zsock_wait (servermon);

    //  Allow a brief time for the message to get there...
    zmq_poll (NULL, 0, 200);

    //  Check client is now listening
    int port_nbr = zsock_bind (client, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    s_assert_event (clientmon, "LISTENING");

    //  Check server connected to client
    zsock_connect (server, "tcp://127.0.0.1:%d", port_nbr);
    s_assert_event (servermon, "CONNECTED");

    //  Check client accepted connection
    s_assert_event (clientmon, "ACCEPTED");

    zactor_destroy (&clientmon);
    zactor_destroy (&servermon);
    zsock_destroy (&client);
    zsock_destroy (&server);
#endif
    //  @end
    printf ("OK\n");
}
