/*  =========================================================================
    zdispatcher - run a steerable dispatcher in the background

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
    A zdispatcher actor forwards messages based on a tokenizable route, for
    example a URL, URI or URN. A message can be forwarded to a handler that
    must register itself under a route.
@discuss
    Dispatching is a one way process, there is currently no support for a
    client to get a answer to a dispatched message.
@end
*/

#include "../include/czmq.h"

//  Structure of our actor

struct _zdispatcher_t {
    zsock_t *pipe;              //  Actor command pipe
    zsock_t *endpoint;          //  Dispatching endpoint
    bool terminated;            //  Did caller ask us to quit?
    zpoller_t *poller;          //  Socket poller
    bool verbose;               //  Verbose logging enabled?
    //  Declare properties
    ztrie_t *router;
};

//  Internal helper functions

static void
s_signal_caller (zdispatcher_t *self, zframe_t *caller_id,  int code)
{
    zmsg_t *signal = zmsg_new_signal (code);
    zmsg_push (signal, caller_id);
    zmsg_send (&signal, self->endpoint);
}

static void
s_destroy_ztrie_data (void **data_p)
{
    assert (data_p);
    zframe_destroy ((zframe_t **) data_p);
}

//  --------------------------------------------------------------------------
//  Create a new zdispatcher.

static zdispatcher_t *
zdispatcher_new (zsock_t *pipe, void *args)
{
    zdispatcher_t *self = (zdispatcher_t *) zmalloc (sizeof (zdispatcher_t));
    assert (self);

    self->pipe = pipe;
    self->terminated = false;
    self->poller = zpoller_new (self->pipe, NULL);
    self->verbose = 0;

    //  Initialize properties
    if (args)
        self->router = ztrie_new (*((char *) args));
    else
        self->router = ztrie_new ('/');

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zdispatcher.

static void
zdispatcher_destroy (zdispatcher_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdispatcher_t *self = *self_p;

        //  Free actor properties
        ztrie_destroy (&self->router);
        if (self->endpoint)
            zsock_destroy (&self->endpoint);

        //  Free object itself
        zpoller_destroy (&self->poller);
        free (self);
        *self_p = NULL;
    }
}


//  Create and attach the dispatching endpoint

static int
s_configure_endpoint (zdispatcher_t *self, zmsg_t *request)
{
    int ret = -1;
    char *endpoints = zmsg_popstr (request);
    assert (endpoints);
    if (self->endpoint)
        zsys_warning ("zdispatcher: [ENDPOINT] Endpoint already attached");
    else {
        self->endpoint = zsock_new (ZMQ_ROUTER);
        if (self->endpoint) {
            if (zsock_attach (self->endpoint, endpoints, true)) {
                zsys_error ("zdispatcher: [ENDPOINT] Invalid endpoint '%s'", endpoints);
                zsock_destroy (&self->endpoint);
            }
            else {
                if (self->verbose)
                    zsys_info ("zdispatcher: [ENDPOINT] Attach endpoint to '%s'", endpoints);
                zpoller_add (self->poller, self->endpoint);
                ret = 0;
            }
        }
    }
    zstr_free (&endpoints);
    return ret;
}


//  Here we handle incoming messages from the pipe

static void
zdispatcher_recv_api (zdispatcher_t *self)
{
    //  Get the whole message of the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
       return;        //  Interrupted

    char *command = zmsg_popstr (request);
    if (streq (command, "ENDPOINT"))
        zsock_signal (self->pipe, s_configure_endpoint (self, request));
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
        self->poller = zpoller_new (self->pipe, self->endpoint, NULL);
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
        //  The $TERM command is send by zactor_destroy() method
        self->terminated = true;
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
}


static int
s_register_handler (zdispatcher_t *self, zmsg_t *request, zframe_t *handler_id)
{
    int rc = 0;
    char *route = zmsg_popstr (request);
    assert (route);
    if (ztrie_insert_route (self->router, route, zframe_dup (handler_id), s_destroy_ztrie_data)) {
        if (self->verbose)
            zsys_info ("zdispatcher: [REGISTER] Route already exists '%s'", route);
        rc = 1;
    }
    else
    if (self->verbose)
        zsys_info ("zdispatcher: [REGISTER] Route inserted '%s'", route);
    zstr_free (&route);
    return rc;
}


static int
s_unregister_handler (zdispatcher_t *self, zmsg_t *request)
{
    int rc = 0;
    char *route = zmsg_popstr (request);
    assert (route);
    if (ztrie_remove_route (self->router, route)) {
        if (self->verbose)
            zsys_info ("zdispatcher: [UNREGISTER] Route did not exists '%s'", route);
        rc = 1;
    }
    else
    if (self->verbose)
        zsys_info ("zdispatcher: [UNREGISTER] Route removed '%s'", route);
    zstr_free (&route);
    return rc;
}


static int
s_dispatch_message (zdispatcher_t *self, zmsg_t *message)
{
    int rc = 0;
    char *route = zmsg_popstr (message);
    assert (route);
    if (!ztrie_matches (self->router, route)) {
        if (self->verbose)
            zsys_info ("zdispatcher: [DISPATCH] Mismatch for route '%s'", route);
        rc = 1;
    }
    else {
        if (self->verbose)
            zsys_info ("zdispatcher: [DISPATCH] Match for route '%s'", route);
        //  Dispatch request
        //  If there are parameters they are send as first frame
        if (ztrie_hit_parameter_count (self->router) > 0) {
            zhashx_t *parameters = ztrie_hit_parameters (self->router);
            zmsg_push (message, zhashx_pack (parameters));
            zhashx_destroy (&parameters);
        }
        zframe_t *handler_id = (zframe_t *) ztrie_hit_data (self->router);
        zmsg_push (message, zframe_dup (handler_id));
        zmsg_send (&message, self->endpoint);
    }
    zstr_free (&route);
    return rc;
}


//  Here we handle incomming message from the endpoint

static void
zdispatcher_recv_endpoint (zdispatcher_t *self)
{
    //  Obtain the routing frame bevor processing the actual message
    zframe_t *routing_id;
    if (zsock_type (self->endpoint) == ZMQ_ROUTER) {
        routing_id = zframe_recv (self->endpoint);
        if (!routing_id || !zsock_rcvmore (self->endpoint)) {
            zsys_warning ("zdispatcher: no routing ID");
            return;          //  Interrupted or malformed
        }
    }

    //  Get the whole message of the endpoint in one go
    zmsg_t *request = zmsg_recv (self->endpoint);
    if (!request)
        return;       //  Interrupted

    char *command = zmsg_popstr (request);
    assert (command);
    if (streq (command, "REGISTER"))
        s_signal_caller (self, routing_id, s_register_handler (self, request, routing_id));
    else
    if (streq (command, "UNREGISTER"))
        s_signal_caller (self, routing_id, s_unregister_handler (self, request));
    else
    if (streq (command, "DISPATCH"))
        s_signal_caller (self, routing_id, s_dispatch_message (self, request));
    else {
        zsys_error ("invalid command '%s'", command);
        assert (false);
    }
    zstr_free (&command);
    if (zframe_is (routing_id))
        zframe_destroy (&routing_id);
    if (zmsg_is (request))
        zmsg_destroy (&request);
}


//  --------------------------------------------------------------------------
//  This is the zdispatcher constructor as a zactor_fn.

void
zdispatcher (zsock_t *pipe, void *args)
{
    zdispatcher_t * self = zdispatcher_new (pipe, args);
    if (!self)
        return;          //  Interrupted

    //  Signal actor successfully initiated
    zsock_signal (self->pipe, 0);

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, -1);
        if (zpoller_terminated (self->poller))
            break;          //  Interrupted
        else
        if (which == self->pipe)
            zdispatcher_recv_api (self);
        else
        if (which == self->endpoint)
            zdispatcher_recv_endpoint (self);
    }

    zdispatcher_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Self test of this actor.

void
zdispatcher_test (bool verbose)
{
    printf (" * zdispatcher: ");

    int rc = 0;
    //  @selftest
    zactor_t *dispatcher = zactor_new (zdispatcher, NULL);

    if (verbose) {
        zstr_sendx (dispatcher, "VERBOSE", NULL);
        zsock_wait (dispatcher);
    }

    //  Setup endpoint (will succeed)
    zstr_sendx (dispatcher, "ENDPOINT", "inproc://dispatching", NULL);
    rc = zsock_wait (dispatcher);                 //  Wait until endpoint bound
    assert (rc == 0);

    //  Setup endpoint (will fail)
    zstr_sendx (dispatcher, "ENDPOINT", "inproc://something", NULL);
    rc = zsock_wait (dispatcher);                 //  Wait until endpoint bound
    assert (rc != 0);

    //  Connect handler and client sockets
    zsock_t *handler = zsock_new_dealer (">inproc://dispatching");
    zsock_t *client = zsock_new_dealer (">inproc://dispatching");

    //  Handler: register (will succeed)
    zstr_sendx (handler, "REGISTER", "/foo/{[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc == 0);

    //  Handler: register (will fail)
    zstr_sendx (handler, "REGISTER", "/foo/{[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc != 0);

    //  Client: send request (will succeed)
    zstr_sendx (client, "DISPATCH", "/foo/bar", "Hello Handler", NULL);
    rc = zsock_wait (client);
    assert (rc == 0);

    //  Client: send request (will fail)
    zstr_sendx (client, "DISPATCH", "/bar/foo", "Hello Bar", NULL);
    rc = zsock_wait (client);
    assert (rc != 0);

    //  Handler: receive dispatched message
    char *content = zstr_recv (handler);
    assert (streq ("Hello Handler", content));
    free (content);

    //  Handler: register with parameters
    zstr_sendx (handler, "REGISTER", "/bar/{name:[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc == 0);

    //  Client: send request with parameters
    zstr_sendx (client, "DISPATCH", "/bar/foo", "Hello Bar", NULL);
    rc = zsock_wait (client);
    assert (rc == 0);

    //  Handler: receive dispatched message with parameters
    zframe_t *parameter_frame = zframe_recv (handler);
    zhashx_t *parameters = zhashx_unpack (parameter_frame);
    assert (1 == zhashx_size (parameters));
    assert (streq ("foo", zhashx_lookup (parameters, "name")));
    content = zstr_recv (handler);
    assert (streq ("Hello Bar", content));
    free (content);

    //  Handler: unregister with parameters
    zstr_sendx (handler, "UNREGISTER", "/bar/{name:[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc == 0);

    //  Handler: unregister (will succeed)
    zstr_sendx (handler, "UNREGISTER", "/foo/{[^/]+}", NULL);
    rc = zsock_wait (handler);
    assert (rc == 0);

    //  Handler: unregister (will fail)
    zstr_sendx (handler, "UNREGISTER", "/foo/bar", NULL);
    rc = zsock_wait (handler);
    assert (rc != 0);

    //  Cleanup
    zsock_destroy (&handler);
    zsock_destroy (&client);
    zactor_destroy (&dispatcher);
    //  @end

    printf ("OK\n");
}
