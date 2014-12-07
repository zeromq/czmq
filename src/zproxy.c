/*  =========================================================================
    zproxy - run a steerable proxy in the background

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
    A zproxy actor switches messages between a frontend and a backend socket.
    It acts much like the zmq_proxy_steerable method, though it makes benefit
    of CZMQ's facilities, to be somewhat simpler to set-up.
@discuss
    This class replaces zproxy_v2, and is meant for applications that use the
    CZMQ v3 API (meaning, zsock).
@end
*/

#include "../include/czmq.h"

//  --------------------------------------------------------------------------
//  The self_t structure holds the state for one actor instance

typedef struct {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    zsock_t *frontend;          //  Frontend socket
    zsock_t *backend;           //  Backend socket
    zsock_t *capture;           //  Capture socket
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
} self_t;

static void
s_self_destroy (self_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        self_t *self = *self_p;
        zsock_destroy (&self->frontend);
        zsock_destroy (&self->backend);
        zsock_destroy (&self->capture);
        zpoller_destroy (&self->poller);
        free (self);
        *self_p = NULL;
    }
}

static self_t *
s_self_new (zsock_t *pipe)
{
    self_t *self = (self_t *) zmalloc (sizeof (self_t));
    if (self) {
        self->pipe = pipe;
        self->poller = zpoller_new (self->pipe, NULL);
        if (!self->poller)
            s_self_destroy (&self);
    }
    return self;
}


static zsock_t *
s_create_socket (char *type_name, char *endpoints)
{
    //  This array matches ZMQ_XXX type definitions
    assert (ZMQ_PAIR == 0);
    char *type_names [] = {
        "PAIR", "PUB", "SUB", "REQ", "REP",
        "DEALER", "ROUTER", "PULL", "PUSH",
        "XPUB", "XSUB", type_name
    };
    //  We always match type at least at end of table
    int index;
    for (index = 0; strneq (type_name, type_names [index]); index++) ;
    if (index > ZMQ_XSUB) {
        zsys_error ("zproxy: invalid socket type '%s'", type_name);
        return NULL;
    }
    zsock_t *sock = zsock_new (index);
    if (sock) {
        if (zsock_attach (sock, endpoints, true)) {
            zsys_error ("zproxy: invalid endpoints '%s'", endpoints);
            zsock_destroy (&sock);
        }
    }
    return sock;
}

static void
s_self_configure (self_t *self, zsock_t **sock_p, zmsg_t *request, char *name)
{
    char *type_name = zmsg_popstr (request);
    assert (type_name);
    char *endpoints = zmsg_popstr (request);
    assert (endpoints);
    if (self->verbose)
        zsys_info ("zmonitor: - %s type=%s attach=%s", name, type_name, endpoints);
    assert (*sock_p == NULL);
    *sock_p = s_create_socket (type_name, endpoints);
    assert (*sock_p);
    zpoller_add (self->poller, *sock_p);
    zstr_free (&type_name);
    zstr_free (&endpoints);
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
    assert (command);
    if (self->verbose)
        zsys_info ("zproxy: API command=%s", command);

    if (streq (command, "FRONTEND")) {
        s_self_configure (self, &self->frontend, request, "frontend");
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "BACKEND")) {
        s_self_configure (self, &self->backend, request, "backend");
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "CAPTURE")) {
        self->capture = zsock_new (ZMQ_PUSH);
        assert (self->capture);
        char *endpoint = zmsg_popstr (request);
        assert (endpoint);
        int rc = zsock_connect (self->capture, "%s", endpoint);
        assert (rc == 0);
        zstr_free (&endpoint);
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "PAUSE")) {
        zpoller_destroy (&self->poller);
        self->poller = zpoller_new (self->pipe, NULL);
        assert (self->poller);
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "RESUME")) {
        zpoller_destroy (&self->poller);
        self->poller = zpoller_new (self->pipe, self->frontend, self->backend, NULL);
        assert (self->poller);
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "VERBOSE")) {
        self->verbose = true;
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "$TERM"))
        self->terminated = true;
    else {
        zsys_error ("zproxy: - invalid command: %s", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
    return 0;
}


//  --------------------------------------------------------------------------
//  Switch messages from an input socket to an output socket until there are
//  no messages left waiting. We use this loop rather than zmq_poll, to reduce
//  the cost of polling, which is non-trivial on some boxes (OS/X, mainly).

static void
s_self_switch (self_t *self, zsock_t *input, zsock_t *output)
{
    //  We use the low-level libzmq API for best performance
    void *zmq_input = zsock_resolve (input);
    void *zmq_output = zsock_resolve (output);
    void *zmq_capture = self->capture ? zsock_resolve (self->capture) : NULL;

    zmq_msg_t msg;
    zmq_msg_init (&msg);
    while (true) {
        if (zmq_recvmsg (zmq_input, &msg, ZMQ_DONTWAIT) == -1)
            break;      //  Presumably EAGAIN
        int send_flags = zsock_rcvmore (zmq_input) ? ZMQ_SNDMORE : 0;
        if (zmq_capture) {
            zmq_msg_t dup;
            zmq_msg_init (&dup);
            zmq_msg_copy (&dup, &msg);
            if (zmq_sendmsg (zmq_capture, &dup, send_flags) == -1)
                zmq_msg_close (&dup);
        }
        if (zmq_sendmsg (zmq_output, &msg, send_flags) == -1) {
            zmq_msg_close (&msg);
            break;
        }
    }
}


//  --------------------------------------------------------------------------
//  zproxy() implements the zproxy actor interface

void
zproxy (zsock_t *pipe, void *unused)
{
    self_t *self = s_self_new (pipe);
    assert (self);
    //  Signal successful initialization
    zsock_signal (pipe, 0);

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, -1);
        if (zpoller_terminated (self->poller))
            break;          //  Interrupted
        else
        if (which == self->pipe)
            s_self_handle_pipe (self);
        else
        if (which == self->frontend)
            s_self_switch (self, self->frontend, self->backend);
        else
        if (which == self->backend)
            s_self_switch (self, self->backend, self->frontend);
    }
    s_self_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Selftest

void
zproxy_test (bool verbose)
{
    printf (" * zproxy: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    //  Create and configure our proxy
    zactor_t *proxy = zactor_new (zproxy, NULL);
    assert (proxy);
    if (verbose) {
        zstr_sendx (proxy, "VERBOSE", NULL);
        zsock_wait (proxy);
    }
    zstr_sendx (proxy, "FRONTEND", "PULL", "inproc://frontend", NULL);
    zsock_wait (proxy);
    zstr_sendx (proxy, "BACKEND", "PUSH", "inproc://backend", NULL);
    zsock_wait (proxy);

    //  Connect application sockets to proxy
    zsock_t *faucet = zsock_new_push (">inproc://frontend");
    assert (faucet);
    zsock_t *sink = zsock_new_pull (">inproc://backend");
    assert (sink);

    //  Send some messages and check they arrived
    char *hello, *world;
    zstr_sendx (faucet, "Hello", "World", NULL);
    zstr_recvx (sink, &hello, &world, NULL);
    assert (streq (hello, "Hello"));
    assert (streq (world, "World"));
    zstr_free (&hello);
    zstr_free (&world);

    //  Test pause/resume functionality
    zstr_sendx (proxy, "PAUSE", NULL);
    zsock_wait (proxy);
    zstr_sendx (faucet, "Hello", "World", NULL);
    zsock_set_rcvtimeo (sink, 100);
    zstr_recvx (sink, &hello, &world, NULL);
    assert (!hello && !world);

    zstr_sendx (proxy, "RESUME", NULL);
    zsock_wait (proxy);
    zstr_recvx (sink, &hello, &world, NULL);
    assert (streq (hello, "Hello"));
    assert (streq (world, "World"));
    zstr_free (&hello);
    zstr_free (&world);

    //  Test capture functionality
    zsock_t *capture = zsock_new_pull ("inproc://capture");
    assert (capture);

    //  Switch on capturing, check that it works
    zstr_sendx (proxy, "CAPTURE", "inproc://capture", NULL);
    zsock_wait (proxy);
    zstr_sendx (faucet, "Hello", "World", NULL);
    zstr_recvx (sink, &hello, &world, NULL);
    assert (streq (hello, "Hello"));
    assert (streq (world, "World"));
    zstr_free (&hello);
    zstr_free (&world);

    zstr_recvx (capture, &hello, &world, NULL);
    assert (streq (hello, "Hello"));
    assert (streq (world, "World"));
    zstr_free (&hello);
    zstr_free (&world);

    zsock_destroy (&faucet);
    zsock_destroy (&sink);
    zsock_destroy (&capture);
    zactor_destroy (&proxy);
    //  @end
    printf ("OK\n");
}
