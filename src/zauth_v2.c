/*  =========================================================================
    zauth_v2 - authentication for ZeroMQ servers (deprecated)

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
    A zauth object takes over authentication for all incoming connections in
    its context.
@discuss
    This class is deprecated in CZMQ v3; it works together with zctx, zsocket,
    and other deprecated V2 classes. New applications should use the V3 zauth
    interface, based on zactor, together with the zsock class for sockets.
@end
*/

#include "../include/czmq.h"
#define ZAP_ENDPOINT  "inproc://zeromq.zap.01"

//  Structure of our class
//  All work is done by a background thread, the "agent", which we talk
//  to over a pipe (created by zthread_fork). This lets the agent do work
//  asynchronously in the background while our application does other
//  things. This is invisible to the caller, who sees a classic API.

struct _zauth_t {
    void *pipe;                 //  Pipe through to background agent
};

//  The background agent task does all the real work
static void
s_agent_task (void *args, zctx_t *ctx, void *pipe);


//  --------------------------------------------------------------------------
//  Constructor
//
//  Install authentication for the specified context. Returns a new zauth
//  object that you can use to configure authentication. Note that until you
//  add policies, all incoming NULL connections are allowed (classic ZeroMQ
//  behavior), and all PLAIN and CURVE connections are denied. If there was
//  an error during initialization, returns NULL.

zauth_t *
zauth_new (zctx_t *ctx)
{
    zauth_t *self = (zauth_t *) zmalloc (sizeof (zauth_t));
    if (!self)
        return NULL;

    //  Start background agent and wait for it to initialize
    assert (ctx);
    self->pipe = zthread_fork (ctx, s_agent_task, NULL);
    if (self->pipe) {
        char *status = zstr_recv (self->pipe);
        if (strneq (status, "OK"))
            zauth_destroy (&self);
        zstr_free (&status);
    }
    else {
        zauth_destroy (&self);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zauth_destroy (zauth_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zauth_t *self = *self_p;
        zstr_send (self->pipe, "TERMINATE");
        char *reply = zstr_recv (self->pipe);
        zstr_free (&reply);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Allow (whitelist) a single IP address. For NULL, all clients from this
//  address will be accepted. For PLAIN and CURVE, they will be allowed to
//  continue with authentication. You can call this method multiple times
//  to whitelist multiple IP addresses. If you whitelist a single address,
//  any non-whitelisted addresses are treated as blacklisted.

void
zauth_allow (zauth_t *self, const char *address)
{
    zstr_sendx (self->pipe, "ALLOW", address, NULL);
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Deny (blacklist) a single IP address. For all security mechanisms, this
//  rejects the connection without any further authentication. Use either a
//  whitelist, or a blacklist, not not both. If you define both a whitelist
//  and a blacklist, only the whitelist takes effect.

void
zauth_deny (zauth_t *self, const char *address)
{
    zstr_sendx (self->pipe, "DENY", address, NULL);
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Configure PLAIN authentication for a given domain. PLAIN authentication
//  uses a plain-text password file. To cover all domains, use "*". You can
//  modify the password file at any time; it is reloaded automatically.

void
zauth_configure_plain (zauth_t *self, const char *domain, const char *filename)
{
    assert (self);
    assert (domain);
    assert (filename);
    zstr_sendx (self->pipe, "PLAIN", domain, filename, NULL);
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Configure CURVE authentication for a given domain. CURVE authentication
//  uses a directory that holds all public client certificates, i.e. their
//  public keys. The certificates must be in zcert_save () format. To cover
//  all domains, use "*". You can add and remove certificates in that
//  directory at any time. To allow all client keys without checking, specify
//  CURVE_ALLOW_ANY for the location.

void
zauth_configure_curve (zauth_t *self, const char *domain, const char *location)
{
    assert (self);
    assert (domain);
    assert (location);
    zstr_sendx (self->pipe, "CURVE", domain, location, NULL);
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Configure GSSAPI authentication for a given domain. GSSAPI authentication
//  uses an underlying mechanism (usually Kerberos) to establish a secure
//  context and perform mutual authentication.  To cover all domains, use "*".

void
zauth_configure_gssapi (zauth_t *self, char *domain)
{
    assert (self);
    assert (domain);
    zstr_sendx (self->pipe, "GSSAPI", domain, NULL);
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Enable verbose tracing of commands and activity

void
zauth_set_verbose (zauth_t *self, bool verbose)
{
    assert (self);
    zstr_sendm (self->pipe, "VERBOSE");
    zstr_sendf (self->pipe, "%d", verbose);
    zsocket_wait (self->pipe);
}


//  *************************     ZAP REQUESTS     *************************

//  A small class for working with ZAP requests and replies
//  This isn't exported in the CZMQ API just used internally in zauth
//  to simplify working with RFC 27 requests and replies.

//  Structure of a ZAP request

typedef struct {
    void *handler;              //  Socket we're talking to
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
zap_request_destroy (zap_request_t **self_p)
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
zap_request_new (void *handler)
{
#if (ZMQ_VERSION_MAJOR == 4)
    zap_request_t *self = (zap_request_t *) zmalloc (sizeof (zap_request_t));
    if (!self)
        return NULL;

    //  Store handler socket so we can send a reply easily
    self->handler = handler;
    zmsg_t *request = zmsg_recv (handler);
    if (!request) {
        zap_request_destroy (&self);
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
        zmq_z85_encode (self->client_key, zframe_data (frame), 32);
        zframe_destroy (&frame);
    }
    else
    if (streq (self->mechanism, "GSSAPI"))
        self->principal = zmsg_popstr (request);

    zmsg_destroy (&request);
    return self;
#else
    return NULL;
#endif
}


//  Send a ZAP reply to the handler socket

static int
zap_request_reply (zap_request_t *self, char *status_code, char *status_text)
{
    zstr_sendx (self->handler,
                "1.0", self->sequence, status_code, status_text, "", "",
                NULL);
    return 0;
}


//  *************************    BACK END AGENT    *************************

//  This structure holds the context for our agent, so we can
//  pass that around cleanly to methods which need it

typedef struct {
    zctx_t *ctx;                //  CZMQ context we're working for
    void *pipe;                 //  Pipe back to application API
    void *handler;              //  ZAP handler socket
    bool verbose;               //  Trace activity to stdout
    zhash_t *whitelist;         //  Whitelisted addresses
    zhash_t *blacklist;         //  Blacklisted addresses
    zhash_t *passwords;         //  PLAIN passwords, if loaded
    zcertstore_t *certstore;    //  CURVE certificate store, if loaded
    bool allow_any;             //  CURVE allows arbitrary clients
    bool terminated;            //  Did API ask us to quit?
} agent_t;

static void
s_agent_destroy (agent_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        agent_t *self = *self_p;
        zhash_destroy (&self->passwords);
        zhash_destroy (&self->whitelist);
        zhash_destroy (&self->blacklist);
        zcertstore_destroy (&self->certstore);
        zsocket_unbind (self->handler, ZAP_ENDPOINT);
        zsocket_destroy (self->ctx, self->handler);
        free (self);
        *self_p = NULL;
    }
}

static agent_t *
s_agent_new (zctx_t *ctx, void *pipe)
{
    agent_t *self = (agent_t *) zmalloc (sizeof (agent_t));
    if (!self)
        return NULL;

    self->ctx = ctx;
    self->pipe = pipe;
    self->whitelist = zhash_new ();
    if (self->whitelist)
        self->blacklist = zhash_new ();

    //  Create ZAP handler and get ready for requests
    if (self->blacklist)
        self->handler = zsocket_new (self->ctx, ZMQ_REP);
    if (self->handler) {
        if (zsocket_bind (self->handler, ZAP_ENDPOINT) == 0)
            zstr_send (self->pipe, "OK");
        else
            zstr_send (self->pipe, "ERROR");
    }
    else
        s_agent_destroy (&self);

    return self;
}

//  Handle a message from front-end API

static int
s_agent_handle_pipe (agent_t *self)
{
    //  Get the whole message off the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    if (!request)
        return -1;                  //  Interrupted
    char *command = zmsg_popstr (request);
    if (!command) {
        zmsg_destroy (&request);
        return -1;                  //  Interrupted
    }

    if (streq (command, "ALLOW")) {
        char *address = zmsg_popstr (request);
        zhash_insert (self->whitelist, address, "OK");
        zstr_free (&address);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "DENY")) {
        char *address = zmsg_popstr (request);
        zhash_insert (self->blacklist, address, "OK");
        zstr_free (&address);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "PLAIN")) {
        //  For now we don't do anything with domains
        char *domain = zmsg_popstr (request);
        zstr_free (&domain);
        //  Get password file and load into zhash table
        //  If the file doesn't exist we'll get an empty table
        char *filename = zmsg_popstr (request);
        zhash_destroy (&self->passwords);
        self->passwords = zhash_new ();
        zhash_load (self->passwords, filename);
        zstr_free (&filename);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "CURVE")) {
        char *domain = zmsg_popstr (request);
        //  For now we don't do anything with domains
        zstr_free (&domain);
        //  If location is CURVE_ALLOW_ANY, allow all clients. Otherwise
        //  treat location as a directory that holds the certificates.
        char *location = zmsg_popstr (request);
        if (streq (location, CURVE_ALLOW_ANY))
            self->allow_any = true;
        else {
            zcertstore_destroy (&self->certstore);
            self->certstore = zcertstore_new (location);
            self->allow_any = false;
        }
        zstr_free (&location);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "GSSAPI")) {
        char *domain = zmsg_popstr (request);
        //  For now we don't do anything with domains
        zstr_free (&domain);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "VERBOSE")) {
        char *verbose = zmsg_popstr (request);
        self->verbose = *verbose == '1';
        zstr_free (&verbose);
        zsocket_signal (self->pipe);
    }
    else
    if (streq (command, "TERMINATE")) {
        self->terminated = true;
        zsocket_signal (self->pipe);
    }
    else {
        zsys_error ("invalid command from API: %s\n", command);
        assert (false);
    }
    zstr_free (&command);
    zmsg_destroy (&request);
    return 0;
}


//  Handle an authentication request from libzmq core

static bool s_authenticate_plain (agent_t *self, zap_request_t *request);
static bool s_authenticate_curve (agent_t *self, zap_request_t *request);
static bool s_authenticate_gssapi (agent_t *self, zap_request_t *request);

static int
s_agent_authenticate (agent_t *self)
{
    zap_request_t *request = zap_request_new (self->handler);
    if (request) {
        //  Is address explicitly whitelisted or blacklisted?
        bool allowed = false;
        bool denied = false;

        if (zhash_size (self->whitelist)) {
            if (zhash_lookup (self->whitelist, request->address)) {
                allowed = true;
                if (self->verbose)
                    printf ("ZAUTH I: PASSED (whitelist) address=%s\n", request->address);
            }
            else {
                denied = true;
                if (self->verbose)
                    printf ("ZAUTH I: DENIED (not in whitelist) address=%s\n", request->address);
            }
        }
        else
        if (zhash_size (self->blacklist)) {
            if (zhash_lookup (self->blacklist, request->address)) {
                denied = true;
                if (self->verbose)
                    printf ("ZAUTH I: DENIED (blacklist) address=%s\n", request->address);
            }
            else {
                allowed = true;
                if (self->verbose)
                    printf ("ZAUTH I: PASSED (not in blacklist) address=%s\n", request->address);
            }
        }
        //  Mechanism-specific checks
        if (!denied) {
            if (streq (request->mechanism, "NULL") && !allowed) {
                //  For NULL, we allow if the address wasn't blacklisted
                if (self->verbose)
                    printf ("ZAUTH I: ALLOWED (NULL)\n");
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
            zap_request_reply (request, "200", "OK");
        else
            zap_request_reply (request, "400", "NO ACCESS");

        zap_request_destroy (&request);
    }
    else
        zap_request_reply (request, "500", "Internal error");
    return 0;
}


static bool
s_authenticate_plain (agent_t *self, zap_request_t *request)
{
    if (self->passwords) {
        zhash_refresh (self->passwords);
        char *password = (char *) zhash_lookup (self->passwords, request->username);
        if (password && streq (password, request->password)) {
            if (self->verbose)
                printf ("ZAUTH I: ALLOWED (PLAIN) username=%s password=%s\n",
                        request->username, request->password);
            return true;
        }
        else {
            if (self->verbose)
                printf ("ZAUTH I: DENIED (PLAIN) username=%s password=%s\n",
                        request->username, request->password);
            return false;
        }
    }
    else {
        if (self->verbose)
            printf ("ZAUTH I: DENIED (PLAIN) no password file defined\n");
        return false;
    }
}


static bool
s_authenticate_curve (agent_t *self, zap_request_t *request)
{
    //  TODO: load metadata from certificate and return via ZAP response
    if (self->allow_any) {
        if (self->verbose)
            printf ("ZAUTH I: ALLOWED (CURVE allow any client)\n");
        return true;
    }
    else
    if (  self->certstore
       && zcertstore_lookup (self->certstore, request->client_key)) {
        if (self->verbose)
            printf ("ZAUTH I: ALLOWED (CURVE) client_key=%s\n", request->client_key);
        return true;
    }
    else {
        if (self->verbose)
            printf ("ZAUTH I: DENIED (CURVE) client_key=%s\n", request->client_key);
        return false;
    }
}

static bool
s_authenticate_gssapi (agent_t *self, zap_request_t *request)
{
    if (self->verbose)
        printf ("I: ALLOWED (GSSAPI) principal=%s identity=%s\n", request->principal, request->identity);
    return true;
}

//  Handle a message from front-end API

static void
s_agent_task (void *args, zctx_t *ctx, void *pipe)
{
    //  Create agent instance as we start this task
    agent_t *self = s_agent_new (ctx, pipe);
    if (!self)                  //  Interrupted
        return;

    zpoller_t *poller = zpoller_new (self->pipe, self->handler, NULL);
    if (poller)
        while (!zpoller_terminated (poller) && !self->terminated) {
            void *which = zpoller_wait (poller, -1);
            if (which == self->pipe)
                s_agent_handle_pipe (self);
            else
            if (which == self->handler)
                s_agent_authenticate (self);
        }
    //  Done, free all agent resources
    zpoller_destroy (&poller);
    s_agent_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Selftest

#if (ZMQ_VERSION_MAJOR == 4)
//  Checks whether client can connect to server
static bool
s_can_connect (zctx_t *ctx, void **server, void **client)
{
    int port_nbr = zsocket_bind (*server, "tcp://127.0.0.1:*");
    assert (port_nbr > 0);
    int rc = zsocket_connect (*client, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc == 0);
    zstr_send (*server, "Hello, World");
    zpoller_t *poller = zpoller_new (*client, NULL);
    bool success = (zpoller_wait (poller, 200) == *client);
    zpoller_destroy (&poller);
    zsocket_destroy (ctx, *client);
    zsocket_destroy (ctx, *server);
    *server = zsocket_new (ctx, ZMQ_PUSH);
    assert (*server);
    *client = zsocket_new (ctx, ZMQ_PULL);
    assert (*client);
    return success;
}
#endif

//  --------------------------------------------------------------------------
//  Selftest

void
zauth_v2_test (bool verbose)
{
    printf (" * zauth (deprecated): ");
#if (ZMQ_VERSION_MAJOR == 4)
    if (verbose)
        printf ("\n");

    //  @selftest
    //  Create temporary directory for test files
#   define TESTDIR ".test_zauth"
    zsys_dir_create (TESTDIR);

    //  Install the authenticator
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    zauth_t *auth = zauth_new (ctx);
    assert (auth);
    zauth_set_verbose (auth, verbose);

    //  A default NULL connection should always success, and not
    //  go through our authentication infrastructure at all.
    void *server = zsocket_new (ctx, ZMQ_PUSH);
    assert (server);
    void *client = zsocket_new (ctx, ZMQ_PULL);
    assert (client);
    bool success = s_can_connect (ctx, &server, &client);
    assert (success);

    //  When we set a domain on the server, we switch on authentication
    //  for NULL sockets, but with no policies, the client connection
    //  will be allowed.
    zsocket_set_zap_domain (server, "global");
    success = s_can_connect (ctx, &server, &client);
    assert (success);

    //  Blacklist 127.0.0.1, connection should fail
    zsocket_set_zap_domain (server, "global");
    zauth_deny (auth, "127.0.0.1");
    success = s_can_connect (ctx, &server, &client);
    assert (!success);

    //  Whitelist our address, which overrides the blacklist
    zsocket_set_zap_domain (server, "global");
    zauth_allow (auth, "127.0.0.1");
    success = s_can_connect (ctx, &server, &client);
    assert (success);

    //  Try PLAIN authentication
    zsocket_set_plain_server (server, 1);
    zsocket_set_plain_username (client, "admin");
    zsocket_set_plain_password (client, "Password");
    success = s_can_connect (ctx, &server, &client);
    assert (!success);

    FILE *password = fopen (TESTDIR "/password-file", "w");
    assert (password);
    fprintf (password, "admin=Password\n");
    fclose (password);
    zsocket_set_plain_server (server, 1);
    zsocket_set_plain_username (client, "admin");
    zsocket_set_plain_password (client, "Password");
    zauth_configure_plain (auth, "*", TESTDIR "/password-file");
    success = s_can_connect (ctx, &server, &client);
    assert (success);

    zsocket_set_plain_server (server, 1);
    zsocket_set_plain_username (client, "admin");
    zsocket_set_plain_password (client, "Bogus");
    success = s_can_connect (ctx, &server, &client);
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
        zsocket_set_curve_server (server, 1);
        zsocket_set_curve_serverkey (client, server_key);
        success = s_can_connect (ctx, &server, &client);
        assert (!success);

        //  Test CURVE_ALLOW_ANY
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsocket_set_curve_server (server, 1);
        zsocket_set_curve_serverkey (client, server_key);
        zauth_configure_curve (auth, "*", CURVE_ALLOW_ANY);
        success = s_can_connect (ctx, &server, &client);
        assert (success);

        //  Test full client authentication using certificates
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsocket_set_curve_server (server, 1);
        zsocket_set_curve_serverkey (client, server_key);
        zcert_save_public (client_cert, TESTDIR "/mycert.txt");
        zauth_configure_curve (auth, "*", TESTDIR);
        success = s_can_connect (ctx, &server, &client);
        assert (success);

        zcert_destroy (&server_cert);
        zcert_destroy (&client_cert);
    }
    //  Remove the authenticator and check a normal connection works
    zauth_destroy (&auth);
    success = s_can_connect (ctx, &server, &client);
    assert (success);

    zctx_destroy (&ctx);

    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
    //  @end
#endif
    printf ("OK\n");
}
