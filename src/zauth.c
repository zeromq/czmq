/*  =========================================================================
    zauth - authentication for ZeroMQ security mechanisms

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
    A zauth actor takes over authentication for all incoming connections in
    its context. You can whitelist or blacklist peers based on IP address,
    and define policies for securing PLAIN, CURVE, and GSSAPI connections.
@discuss
    This class replaces zauth_v2, and is meant for applications that use the
    CZMQ v3 API (meaning, zsock).
@end
*/

#include "../include/czmq.h"
#define ZAP_ENDPOINT  "inproc://zeromq.zap.01"

//  --------------------------------------------------------------------------
//  The self_t structure holds the state for one actor instance

typedef struct {
    zsock_t *pipe;              //  Actor command pipe
    zsock_t *handler;           //  ZAP handler socket
    zhashx_t *whitelist;        //  Whitelisted addresses
    zhashx_t *blacklist;        //  Blacklisted addresses
    zhashx_t *passwords;        //  PLAIN passwords, if loaded
    zpoller_t *poller;          //  Socket poller
    zcertstore_t *certstore;    //  CURVE certificate store, if loaded
    bool allow_any;             //  CURVE allows arbitrary clients
    bool terminated;            //  Did caller ask us to quit?
    bool verbose;               //  Verbose logging enabled?
} self_t;

static void
s_self_destroy (self_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        self_t *self = *self_p;
        zhashx_destroy (&self->passwords);
        zhashx_destroy (&self->whitelist);
        zhashx_destroy (&self->blacklist);
        zcertstore_destroy (&self->certstore);
        zpoller_destroy (&self->poller);
        if (self->handler) {
            zsock_unbind (self->handler, ZAP_ENDPOINT);
            zsock_destroy (&self->handler);
        }
        free (self);
        *self_p = NULL;
    }
}

static self_t *
s_self_new (zsock_t *pipe)
{
    self_t *self = (self_t *) zmalloc (sizeof (self_t));
    int rc = -1;
    if (self) {
        self->pipe = pipe;
        self->whitelist = zhashx_new ();
        if (self->whitelist)
            self->blacklist = zhashx_new ();

        //  Create ZAP handler and get ready for requests
        if (self->blacklist)
            self->handler = zsock_new (ZMQ_REP);
        if (self->handler)
            rc = zsock_bind (self->handler, ZAP_ENDPOINT);
        if (rc == 0)
            self->poller = zpoller_new (self->pipe, self->handler, NULL);
        if (!self->poller)
            s_self_destroy (&self);
    }
    return self;
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
    if (self->verbose)
        zsys_info ("zauth: API command=%s", command);

    if (streq (command, "ALLOW")) {
        char *address = zmsg_popstr (request);
        while (address) {
            if (self->verbose)
                zsys_info ("zauth: - whitelisting ipaddress=%s", address);
            zhashx_insert (self->whitelist, address, "OK");
            zstr_free (&address);
            address = zmsg_popstr (request);
        }
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "DENY")) {
        char *address = zmsg_popstr (request);
        while (address) {
            if (self->verbose)
                zsys_info ("zauth: - blacklisting ipaddress=%s", address);
            zhashx_insert (self->blacklist, address, "OK");
            zstr_free (&address);
            address = zmsg_popstr (request);
        }
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "PLAIN")) {
        //  Get password file and load into zhash table
        //  If the file doesn't exist we'll get an empty table
        char *filename = zmsg_popstr (request);
        zhashx_destroy (&self->passwords);
        self->passwords = zhashx_new ();
        if (zhashx_load (self->passwords, filename) && self->verbose)
            zsys_info ("zauth: could not load file=%s", filename);
        zstr_free (&filename);
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "CURVE")) {
        //  If location is CURVE_ALLOW_ANY, allow all clients. Otherwise
        //  treat location as a directory that holds the certificates.
        char *location = zmsg_popstr (request);
        if (streq (location, CURVE_ALLOW_ANY))
            self->allow_any = true;
        else {
            zcertstore_destroy (&self->certstore);
            // FIXME: what if this fails?
            self->certstore = zcertstore_new (location);
            self->allow_any = false;
        }
        zstr_free (&location);
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "GSSAPI"))
        //  GSSAPI authentication is not yet implemented here
        zsock_signal (self->pipe, 0);
    else
    if (streq (command, "VERBOSE")) {
        self->verbose = true;
        zsock_signal (self->pipe, 0);
    }
    else
    if (streq (command, "$TERM"))
        self->terminated = true;
    else {
        zsys_error ("zauth: - invalid command: %s", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
    return 0;
}


//  --------------------------------------------------------------------------
//  A small class for working with ZAP requests and replies.
//  Used internally in zauth to simplify working with RFC 27 messages.

//  Structure of a ZAP request

typedef struct {
    zsock_t *handler;           //  Socket we're talking to
    bool verbose;               //  Log ZAP requests and replies?
    char *version;              //  Version number, must be "1.0"
    char *sequence;             //  Sequence number of request
    char *domain;               //  Server socket domain
    char *address;              //  Client IP address
    char *identity;             //  Server socket idenntity
    char *mechanism;            //  Security mechansim
    char *username;             //  PLAIN user name
    char *password;             //  PLAIN password, in clear text
    char *client_key;           //  CURVE client public key in ASCII
    char *principal;            //  GSSAPI client principal
} zap_request_t;


static void
s_zap_request_destroy (zap_request_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zap_request_t *self = *self_p;
        free (self->version);
        free (self->sequence);
        free (self->domain);
        free (self->address);
        free (self->identity);
        free (self->mechanism);
        free (self->username);
        free (self->password);
        free (self->client_key);
        free (self->principal);
        free (self);
        *self_p = NULL;
    }
}

//  Receive a valid ZAP request from the handler socket
//  If the request was not valid, returns NULL.

static zap_request_t *
s_zap_request_new (zsock_t *handler, bool verbose)
{
    zap_request_t *self = (zap_request_t *) zmalloc (sizeof (zap_request_t));
    if (!self)
        return NULL;

    //  Store handler socket so we can send a reply easily
    self->handler = handler;
    self->verbose = verbose;
    zmsg_t *request = zmsg_recv (handler);
    if (!request) { // interrupted
        s_zap_request_destroy (&self);
        return NULL;
    }

    //  Get all standard frames off the handler socket
    self->version = zmsg_popstr (request);
    self->sequence = zmsg_popstr (request);
    self->domain = zmsg_popstr (request);
    self->address = zmsg_popstr (request);
    self->identity = zmsg_popstr (request);
    self->mechanism = zmsg_popstr (request);

    //  If the version is wrong, we're linked with a bogus libzmq, so die
    assert (streq (self->version, "1.0"));

    //  Get mechanism-specific frames
    if (streq (self->mechanism, "PLAIN")) {
        self->username = zmsg_popstr (request);
        self->password = zmsg_popstr (request);
    }
    else
    if (streq (self->mechanism, "CURVE")) {
        zframe_t *frame = zmsg_pop (request);
        assert (zframe_size (frame) == 32);
        self->client_key = (char *) zmalloc (41);
#if (ZMQ_VERSION_MAJOR == 4)
        zmq_z85_encode (self->client_key, zframe_data (frame), 32);
#endif
        zframe_destroy (&frame);
    }
    else
    if (streq (self->mechanism, "GSSAPI"))
        self->principal = zmsg_popstr (request);

    if (self->verbose)
        zsys_info ("zauth: ZAP request mechanism=%s ipaddress=%s",
                   self->mechanism, self->address);
    zmsg_destroy (&request);
    return self;
}

//  Send a ZAP reply to the handler socket

static int
s_zap_request_reply (zap_request_t *self, char *status_code, char *status_text)
{
    if (self->verbose)
        zsys_info ("zauth: - ZAP reply status_code=%s status_text=%s",
                   status_code, status_text);

    zstr_sendx (self->handler,
                "1.0", self->sequence, status_code, status_text, "", "",
                NULL);
    return 0;
}


//  --------------------------------------------------------------------------
//  Handle an authentication request from libzmq core

static bool
s_authenticate_plain (self_t *self, zap_request_t *request)
{
    if (self->passwords) {
        zhashx_refresh (self->passwords);
        char *password = (char *) zhashx_lookup (self->passwords, request->username);
        if (password && streq (password, request->password)) {
            if (self->verbose)
                zsys_info ("zauth: - allowed (PLAIN) username=%s password=%s",
                           request->username, request->password);
            return true;
        }
        else {
            if (self->verbose)
                zsys_info ("zauth: - denied (PLAIN) username=%s password=%s",
                           request->username, request->password);
            return false;
        }
    }
    else {
        if (self->verbose)
            zsys_info ("zauth: - denied (PLAIN) no password file defined");
        return false;
    }
}


static bool
s_authenticate_curve (self_t *self, zap_request_t *request)
{
    //  TODO: load metadata from certificate and return via ZAP response
    if (self->allow_any) {
        if (self->verbose)
            zsys_info ("zauth: - allowed (CURVE allow any client)");
        return true;
    }
    else
    if (  self->certstore
       && zcertstore_lookup (self->certstore, request->client_key)) {
        if (self->verbose)
            zsys_info ("zauth: - allowed (CURVE) client_key=%s", request->client_key);
        return true;
    }
    else {
        if (self->verbose)
            zsys_info ("zauth: - denied (CURVE) client_key=%s", request->client_key);
        return false;
    }
}

static bool
s_authenticate_gssapi (self_t *self, zap_request_t *request)
{
    if (self->verbose)
        zsys_info ("zauth: - allowed (GSSAPI) principal=%s identity=%s",
                   request->principal, request->identity);
    return true;
}

//  TODO: allow regular expressions in addresses
static int
s_self_authenticate (self_t *self)
{
    zap_request_t *request = s_zap_request_new (self->handler, self->verbose);
    if (request) {
        //  Is address explicitly whitelisted or blacklisted?
        bool allowed = false;
        bool denied = false;

        if (zhashx_size (self->whitelist)) {
            if (zhashx_lookup (self->whitelist, request->address)) {
                allowed = true;
                if (self->verbose)
                    zsys_info ("zauth: - passed (whitelist) address=%s", request->address);
            }
            else {
                denied = true;
                if (self->verbose)
                    zsys_info ("zauth: - denied (not in whitelist) address=%s", request->address);
            }
        }
        else
        if (zhashx_size (self->blacklist)) {
            if (zhashx_lookup (self->blacklist, request->address)) {
                denied = true;
                if (self->verbose)
                    zsys_info ("zauth: - denied (blacklist) address=%s", request->address);
            }
            else {
                allowed = true;
                if (self->verbose)
                    zsys_info ("zauth: - passed (not in blacklist) address=%s", request->address);
            }
        }
        //  Mechanism-specific checks
        if (!denied) {
            if (streq (request->mechanism, "NULL") && !allowed) {
                //  For NULL, we allow if the address wasn't blacklisted
                if (self->verbose)
                    zsys_info ("zauth: - allowed (NULL)");
                allowed = true;
            }
            else
            if (streq (request->mechanism, "PLAIN"))
                //  For PLAIN, even a whitelisted address must authenticate
                allowed = s_authenticate_plain (self, request);
            else
            if (streq (request->mechanism, "CURVE"))
                //  For CURVE, even a whitelisted address must authenticate
                allowed = s_authenticate_curve (self, request);
            else
            if (streq (request->mechanism, "GSSAPI"))
                //  For GSSAPI, even a whitelisted address must authenticate
                allowed = s_authenticate_gssapi (self, request);
        }
        if (allowed)
            s_zap_request_reply (request, "200", "OK");
        else
            s_zap_request_reply (request, "400", "No access");

        s_zap_request_destroy (&request);
    }
    else
        s_zap_request_reply (request, "500", "Internal error");

    return 0;
}


//  --------------------------------------------------------------------------
//  zauth() implements the zauth actor interface

void
zauth (zsock_t *pipe, void *unused)
{
    self_t *self = s_self_new (pipe);
    if (!self)
        return;

    //  Signal successful initialization
    zsock_signal (pipe, 0);

    while (!self->terminated) {
        zsock_t *which = (zsock_t *) zpoller_wait (self->poller, -1);
        if (which == self->pipe)
            s_self_handle_pipe (self);
        else
        if (which == self->handler)
            s_self_authenticate (self);
        else
        if (zpoller_terminated (self->poller))
            break;          //  Interrupted
    }
    s_self_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Selftest

#if (ZMQ_VERSION_MAJOR == 4)
//  Checks whether client can connect to server
static bool
s_can_connect (zsock_t **server, zsock_t **client)
{
    int port_nbr = zsock_bind (*server, "tcp://127.0.0.1:*");
    assert (port_nbr > 0);
    int rc = zsock_connect (*client, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc == 0);

    zstr_send (*server, "Hello, World");
    zpoller_t *poller = zpoller_new (*client, NULL);
    assert (poller);
    bool success = (zpoller_wait (poller, 200) == *client);
    zpoller_destroy (&poller);
    zsock_destroy (client);
    zsock_destroy (server);
    *server = zsock_new (ZMQ_PUSH);
    assert (*server);
    *client = zsock_new (ZMQ_PULL);
    assert (*client);
    return success;
}
#endif

void
zauth_test (bool verbose)
{
    printf (" * zauth: ");
#if (ZMQ_VERSION_MAJOR == 4)
    if (verbose)
        printf ("\n");

    //  @selftest
    //  Create temporary directory for test files
#   define TESTDIR ".test_zauth"
    zsys_dir_create (TESTDIR);

    //  Check there's no authentication
    zsock_t *server = zsock_new (ZMQ_PUSH);
    assert (server);
    zsock_t *client = zsock_new (ZMQ_PULL);
    assert (client);
    bool success = s_can_connect (&server, &client);
    assert (success);

    //  Install the authenticator
    zactor_t *auth = zactor_new (zauth, NULL);
    assert (auth);
    if (verbose) {
        zstr_sendx (auth, "VERBOSE", NULL);
        zsock_wait (auth);
    }
    //  Check there's no authentication on a default NULL server
    success = s_can_connect (&server, &client);
    assert (success);

    //  When we set a domain on the server, we switch on authentication
    //  for NULL sockets, but with no policies, the client connection
    //  will be allowed.
    zsock_set_zap_domain (server, "global");
    success = s_can_connect (&server, &client);
    assert (success);

    //  Blacklist 127.0.0.1, connection should fail
    zsock_set_zap_domain (server, "global");
    zstr_sendx (auth, "DENY", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&server, &client);
    assert (!success);

    //  Whitelist our address, which overrides the blacklist
    zsock_set_zap_domain (server, "global");
    zstr_sendx (auth, "ALLOW", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&server, &client);
    assert (success);

    //  Try PLAIN authentication
    zsock_set_plain_server (server, 1);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "Password");
    success = s_can_connect (&server, &client);
    assert (!success);

    FILE *password = fopen (TESTDIR "/password-file", "w");
    assert (password);
    fprintf (password, "admin=Password\n");
    fclose (password);
    zsock_set_plain_server (server, 1);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "Password");
    zstr_sendx (auth, "PLAIN", TESTDIR "/password-file", NULL);
    zsock_wait (auth);
    success = s_can_connect (&server, &client);
    assert (success);

    zsock_set_plain_server (server, 1);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "Bogus");
    success = s_can_connect (&server, &client);
    assert (!success);

    if (zsys_has_curve ()) {
        //  Try CURVE authentication
        //  We'll create two new certificates and save the client public
        //  certificate on disk; in a real case we'd transfer this securely
        //  from the client machine to the server machine.
        zcert_t *server_cert = zcert_new ();
        assert (server_cert);
        zcert_t *client_cert = zcert_new ();
        assert (client_cert);
        char *server_key = zcert_public_txt (server_cert);

        //  Test without setting-up any authentication
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        success = s_can_connect (&server, &client);
        assert (!success);

        //  Test CURVE_ALLOW_ANY
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        zstr_sendx (auth, "CURVE", CURVE_ALLOW_ANY, NULL);
        zsock_wait (auth);
        success = s_can_connect (&server, &client);
        assert (success);

        //  Test full client authentication using certificates
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        zcert_save_public (client_cert, TESTDIR "/mycert.txt");
        zstr_sendx (auth, "CURVE", TESTDIR, NULL);
        zsock_wait (auth);
        success = s_can_connect (&server, &client);
        assert (success);

        zcert_destroy (&server_cert);
        zcert_destroy (&client_cert);
    }
    //  Remove the authenticator and check a normal connection works
    zactor_destroy (&auth);
    success = s_can_connect (&server, &client);
    assert (success);

    zsock_destroy (&client);
    zsock_destroy (&server);

    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
    //  @end
#endif
    printf ("OK\n");
}
