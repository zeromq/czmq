﻿/*  =========================================================================
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

typedef enum proxy_socket {
    NONE = -1,
    FRONTEND,
    BACKEND,
    SOCKETS
}
proxy_socket;

#define AUTH_NONE  0
#define AUTH_PLAIN 1
#define AUTH_CURVE 2

//  --------------------------------------------------------------------------
//  The self_t structure holds the state for one actor instance

typedef struct {
    zsock_t *pipe;              //  Actor command pipe
    zpoller_t *poller;          //  Socket poller
    zsock_t *frontend;          //  Frontend socket
    zsock_t *backend;           //  Backend socket
    zsock_t *capture;           //  Capture socket
    int auth_type [SOCKETS];    //  Auth type for sockets
    char *domain [SOCKETS];     //  Auth domains for sockets
    char *public_key [SOCKETS]; //  Public keys for sockets
    char *secret_key [SOCKETS]; //  Secret keys for sockets
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
        for (int index = 0; index < SOCKETS; index++) {
            zstr_free (&self->domain [index]);
            zstr_free (&self->public_key [index]);
            zstr_free (&self->secret_key [index]);
        }
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
s_self_create_socket (self_t *self, char *type_name, char *endpoints, proxy_socket selected_socket)
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
#if (ZMQ_VERSION_MAJOR == 4)
        if (self->domain [selected_socket]) {
            // Apply authentication domain
            zsock_set_zap_domain (sock, self->domain [selected_socket]);
        }
        if (self->auth_type [selected_socket] == AUTH_PLAIN) {
            // Enable plain authentication
            zsock_set_plain_server (sock, 1);
        }
        else
        if (self->auth_type [selected_socket] == AUTH_CURVE) {
            // Apply certificate keys
            char *public_key = self->public_key [selected_socket];
            assert(public_key);
            char *secret_key = self->secret_key [selected_socket];
            assert(secret_key);
            zsock_set_curve_publickey (sock, public_key);
            zsock_set_curve_secretkey (sock, secret_key);

            // Enable curve authentication
            zsock_set_curve_server (sock, 1);
        }
#endif
        if (zsock_attach (sock, endpoints, true)) {
            zsys_error ("zproxy: invalid endpoints '%s'", endpoints);
            zsock_destroy (&sock);
        }
    }
    return sock;
}

static const char *
s_self_selected_socket_name (proxy_socket selected_socket)
{
    switch (selected_socket) {
        case FRONTEND:
            return "FRONTEND";
        case BACKEND:
            return "BACKEND";
        default:
            return "UNDEFINED";
    }
}

static const char *
s_self_selected_socket_auth (int auth_type)
{
    switch (auth_type) {
        case AUTH_PLAIN:
            return "PLAIN";
        case AUTH_CURVE:
            return "CURVE";
        default:
            return "NONE";
    }
}

static proxy_socket
s_self_selected_socket (zmsg_t *request)
{
    char *socket_name = zmsg_popstr (request);
    assert (socket_name);

    proxy_socket socket = NONE;

    if (streq (socket_name, "FRONTEND"))
        socket = FRONTEND;
    else
    if (streq (socket_name, "BACKEND"))
        socket = BACKEND;
    else {
        zsys_error ("zproxy: invalid proxy socket selection: %s", socket_name);
        assert (false);
    }

    return socket;
}

static void
s_self_configure (self_t *self, zsock_t **sock_p, zmsg_t *request, proxy_socket selected_socket)
{
    char *type_name = zmsg_popstr (request);
    assert (type_name);
    char *endpoints = zmsg_popstr (request);
    assert (endpoints);
    if (self->verbose)
        zsys_info ("zproxy: - %s type=%s attach=%s authentication=%s",
            s_self_selected_socket_name (selected_socket), type_name, endpoints,
            s_self_selected_socket_auth (self->auth_type [selected_socket]));
    assert (*sock_p == NULL);
    *sock_p = s_self_create_socket (self, type_name, endpoints, selected_socket);
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
        s_self_configure (self, &self->frontend, request, FRONTEND);
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "BACKEND")) {
        s_self_configure (self, &self->backend, request, BACKEND);
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
    if (streq (command, "DOMAIN")) {
        proxy_socket selected_socket = s_self_selected_socket (request);
        char *domain = zmsg_popstr (request);
        assert (domain);
        zstr_free (&self->domain [selected_socket]);
        self->domain [selected_socket] = domain;
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "PLAIN")) {
        proxy_socket selected_socket = s_self_selected_socket (request);
        self->auth_type [selected_socket] = AUTH_PLAIN;
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "CURVE")) {
        proxy_socket selected_socket = s_self_selected_socket (request);
        self->auth_type [selected_socket] = AUTH_CURVE;
        char *public_key = zmsg_popstr (request);
        assert (public_key);
        char *secret_key = zmsg_popstr (request);
        assert (secret_key);
        zstr_free (&self->public_key [selected_socket]);
        zstr_free (&self->secret_key [selected_socket]);
        self->public_key [selected_socket] = public_key;
        self->secret_key [selected_socket] = secret_key;
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
    void *zmq_capture = self->capture? zsock_resolve (self->capture): NULL;

    zmq_msg_t msg;
    zmq_msg_init (&msg);
    while (true) {
        if (zmq_recvmsg (zmq_input, &msg, ZMQ_DONTWAIT) == -1)
            break;      //  Presumably EAGAIN
        int send_flags = zsock_rcvmore (input)? ZMQ_SNDMORE: 0;
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
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, 1000);
        if (zpoller_expired (self->poller))
            continue;       // Timed out
        if (zpoller_terminated (self->poller))
            break;          //  Interrupted
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

#if (ZMQ_VERSION_MAJOR == 4)

#define LOCALENDPOINT "tcp://127.0.0.1:%d"

//  Define flag types for proxy sockets
#define FRONTEND_SOCKET 1 << FRONTEND
#define BACKEND_SOCKET  1 << BACKEND

static void
s_create_test_sockets (zactor_t **proxy, zsock_t **faucet, zsock_t **sink, bool verbose)
{
    if (*faucet)
        zsock_destroy (faucet);
    if (*sink)
        zsock_destroy (sink);
    if (*proxy)
        zactor_destroy (proxy);

    *faucet = zsock_new (ZMQ_PUSH);
    assert (*faucet);
    *sink = zsock_new (ZMQ_PULL);
    assert (*sink);
    *proxy = zactor_new (zproxy, NULL);
    assert (*proxy);
    if (verbose) {
        zstr_sendx (*proxy, "VERBOSE", NULL);
        zsock_wait (*proxy);
    }
}

static int
s_get_available_port ()
{
    int port_nbr = -1;
    int attempts = 0;

    //  Choosing a random port for better results in case multiple tests are running
    //  in parallel on the same machine, such as during CI runs
    while (port_nbr == -1 && attempts++ < 10) {
        port_nbr = 49152 + randof (16383);
        zsock_t *server = zsock_new (ZMQ_PUSH);
        assert (server);
        port_nbr = zsock_bind (server, LOCALENDPOINT, port_nbr);
        zsock_destroy (&server);
    }

    if (port_nbr < 0) {
        zsys_error ("zproxy: failed to find an available port number");
        assert (false);
    }

    return port_nbr;
}

//  Checks whether client can connect to server
static bool
s_can_connect (zactor_t **proxy, zsock_t **faucet, zsock_t **sink, const char *frontend, const char *backend, bool verbose)
{
    assert (*faucet);
    assert (*sink);
    assert (frontend);
    assert (backend);

    int rc = zsock_connect (*faucet, "%s", frontend);
    assert (rc == 0);
    rc = zsock_connect (*sink, "%s", backend);
    assert (rc == 0);
    zstr_send (*faucet, "Hello, World");
    zpoller_t *poller = zpoller_new (*sink, NULL);
    assert (poller);
    bool success = (zpoller_wait (poller, 200) == *sink);
    zpoller_destroy (&poller);
    s_create_test_sockets (proxy, faucet, sink, verbose);

    return success;
}

static void
s_bind_proxy_sockets (zactor_t *proxy, char **frontend, char **backend)
{
    if (*frontend)
        zstr_free (frontend);
    if (*backend)
        zstr_free (backend);
    assert (proxy);

    srandom (time (NULL) ^ *(int *) proxy);
    *frontend = zsys_sprintf (LOCALENDPOINT, s_get_available_port ());
    *backend = zsys_sprintf (LOCALENDPOINT, s_get_available_port ());

    //  Wait a moment to make sure ports have time to unbind...
    zclock_sleep(1000);
    zstr_sendx (proxy, "FRONTEND", "PULL", *frontend, NULL);
    zsock_wait (proxy);
    zstr_sendx (proxy, "BACKEND", "PUSH", *backend, NULL);
    zsock_wait (proxy);
}

static void
s_send_proxy_msg (zactor_t *proxy, const char *command, proxy_socket selected_socket, zmsg_t *msg)
{
    zmsg_pushstr (msg, s_self_selected_socket_name (selected_socket));
    zmsg_pushstr (msg, command);
    assert (zmsg_send (&msg, proxy) == 0);
}

static void
s_send_proxy_command (zactor_t *proxy, const char *command, int selected_sockets, const char *string, ...)
{
    zmsg_t *msg = zmsg_new ();
    if (!msg)
        assert (false);
    va_list args;
    va_start (args, string);
    while (string) {
        zmsg_addstr (msg, string);
        string = va_arg (args, char *);
    }
    va_end (args);
    for (int index = 0; index < SOCKETS; index++) {
        if (selected_sockets & (1 << index)) {
            s_send_proxy_msg (proxy, command, (proxy_socket) index, zmsg_dup (msg));
            zsock_wait (proxy);
        }
    }
    zmsg_destroy (&msg);
}

static void
s_configure_plain_auth (zsock_t *faucet, zsock_t *sink, int selected_sockets, const char *username, const char *password)
{
    assert (username);
    assert (password);
    if (selected_sockets & FRONTEND_SOCKET) {
        assert (faucet);
        zsock_set_plain_username (faucet, username);
        zsock_set_plain_password (faucet, password);
    }
    if (selected_sockets & BACKEND_SOCKET) {
        assert (sink);
        zsock_set_plain_username (sink, username);
        zsock_set_plain_password (sink, password);
    }
}

static void
s_configure_curve_auth (zsock_t *faucet, zsock_t *sink, int selected_sockets, zcert_t *client_cert, const char *public_key)
{
    assert (client_cert);
    assert (public_key);
    if (selected_sockets & FRONTEND_SOCKET) {
        assert (faucet);
        zcert_apply (client_cert, faucet);
        zsock_set_curve_serverkey (faucet, public_key);
    }
    if (selected_sockets & BACKEND_SOCKET) {
        assert (sink);
        zcert_apply (client_cert, sink);
        zsock_set_curve_serverkey (sink, public_key);
    }
}

//  Authentication test procedure for zproxy sockets - matches zauth_test steps
static void
zproxy_test_authentication (int selected_sockets, bool verbose)
{
#   define TESTDIR ".test_zproxy"
#   define TESTPWDS TESTDIR "/password-file"
#   define TESTCERT TESTDIR "/mycert.txt"
#   define TESTFRONTEND (selected_sockets & FRONTEND_SOCKET)
#   define TESTBACKEND (selected_sockets & BACKEND_SOCKET)
#   define UNIQUEDOMAIN "test_domain%d"

    //  Demarcate test boundaries
    zsys_info ("zproxy: TEST authentication type=%s%s%s", 
        TESTFRONTEND? "FRONTEND": "", 
        TESTFRONTEND && TESTBACKEND? "+": "", 
        TESTBACKEND? "BACKEND": "");

    //  Create temporary directory for test files
    zsys_dir_create (TESTDIR);

    // Clear out any test files from previous run
    if (zsys_file_exists (TESTPWDS))
        zsys_file_delete (TESTPWDS);
    if (zsys_file_exists (TESTCERT))
        zsys_file_delete (TESTCERT);

    zactor_t *proxy = NULL;
    zsock_t *faucet = NULL;
    zsock_t *sink = NULL;
    char *frontend = NULL;
    char *backend = NULL;

    //  Check there's no authentication
    s_create_test_sockets (&proxy, &faucet, &sink, verbose);
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    bool success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);

    //  Install the authenticator
    zactor_t *auth = zactor_new (zauth, NULL);
    assert (auth);
    if (verbose) {
        zstr_sendx (auth, "VERBOSE", NULL);
        zsock_wait (auth);
    }

    //  Check there's no authentication on a default NULL server
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);

    //  When we set a domain on the server, we switch on authentication
    //  for NULL sockets, but with no policies, the client connection
    //  will be allowed.
    char *domain = zsys_sprintf (UNIQUEDOMAIN, randof (10000));
    assert (domain);
    s_send_proxy_command (proxy, "DOMAIN", selected_sockets, domain, NULL);
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);

    //  Blacklist 127.0.0.1, connection should fail
    s_send_proxy_command (proxy, "DOMAIN", selected_sockets, domain, NULL);
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    zstr_sendx (auth, "DENY", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (!success);

    //  Whitelist our address, which overrides the blacklist
    s_send_proxy_command (proxy, "DOMAIN", selected_sockets, domain, NULL);
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    zstr_sendx (auth, "ALLOW", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);
    zstr_free (&domain);

    //  Try PLAIN authentication

    //  Test negative case (no server-side passwords defined)
    s_send_proxy_command (proxy, "PLAIN", selected_sockets, NULL);
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    s_configure_plain_auth (faucet, sink, selected_sockets, "admin", "Password");
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (!success);

    //  Test positive case (server-side passwords defined)
    FILE *password = fopen (TESTPWDS, "w");
    assert (password);
    fprintf (password, "admin=Password\n");
    fclose (password);
    s_send_proxy_command (proxy, "PLAIN", selected_sockets, NULL);
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    s_configure_plain_auth (faucet, sink, selected_sockets, "admin", "Password");
    zstr_sendx (auth, "PLAIN", TESTPWDS, NULL);
    zsock_wait (auth);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);

    //  Test negative case (bad client password)
    s_send_proxy_command (proxy, "PLAIN", selected_sockets, NULL);
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    s_configure_plain_auth (faucet, sink, selected_sockets, "admin", "Bogus");
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (!success);

    if (zsys_has_curve ()) {
        //  We'll create two new certificates and save the client public
        //  certificate on disk
        zcert_t *server_cert = zcert_new ();
        assert (server_cert);
        zcert_t *client_cert = zcert_new ();
        assert (client_cert);
        char *public_key = zcert_public_txt (server_cert);
        char *secret_key = zcert_secret_txt (server_cert);

        //  Try CURVE authentication

        //  Test without setting-up any authentication
        s_send_proxy_command (proxy, "CURVE", selected_sockets, public_key, secret_key, NULL);
        s_bind_proxy_sockets (proxy, &frontend, &backend);
        s_configure_curve_auth (faucet, sink, selected_sockets, client_cert, public_key);
        success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
        assert (!success);

        //  Test CURVE_ALLOW_ANY
        s_send_proxy_command (proxy, "CURVE", selected_sockets, public_key, secret_key, NULL);
        s_bind_proxy_sockets (proxy, &frontend, &backend);
        s_configure_curve_auth (faucet, sink, selected_sockets, client_cert, public_key);
        zstr_sendx (auth, "CURVE", CURVE_ALLOW_ANY, NULL);
        zsock_wait (auth);
        success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
        assert (success);

        //  Test with client certificate file in authentication folder
        s_send_proxy_command (proxy, "CURVE", selected_sockets, public_key, secret_key, NULL);
        s_bind_proxy_sockets (proxy, &frontend, &backend);
        s_configure_curve_auth (faucet, sink, selected_sockets, client_cert, public_key);
        zcert_save_public (client_cert, TESTCERT);
        zstr_sendx (auth, "CURVE", TESTDIR, NULL);
        zsock_wait (auth);
        success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
        assert (success);

        zcert_destroy (&server_cert);
        zcert_destroy (&client_cert);
    }

    //  Remove the authenticator and check a normal connection works
    zactor_destroy (&auth);
    s_bind_proxy_sockets (proxy, &frontend, &backend);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);

    zsock_destroy (&faucet);
    zsock_destroy (&sink);
    zactor_destroy (&proxy);
    zstr_free (&frontend);
    zstr_free (&backend);
}
#endif

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

#if (ZMQ_VERSION_MAJOR == 4)
    //  Test authentication on frontend server socket
    zproxy_test_authentication (FRONTEND_SOCKET, verbose);
    //  Test authentication on backend server socket
    zproxy_test_authentication (BACKEND_SOCKET, verbose);
    //  Test authentication on frontend and backend server sockets simultaneously
    zproxy_test_authentication (FRONTEND_SOCKET | BACKEND_SOCKET, verbose);
#endif
    //  @end
    printf ("OK\n");
}
