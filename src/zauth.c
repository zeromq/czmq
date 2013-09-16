/*  =========================================================================
    zauth - authentication for ZeroMQ security mechanisms

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
    A zauth object takes over authentication for all incoming connections in 
    its context. Note that libzmq provides four levels of security: default 
    NULL (which zauth does not see), and authenticated NULL, PLAIN, and CURVE,
    which zauth can see.
@discuss

@end
*/

#include "../include/czmq.h"

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
//  behaviour), and all PLAIN and CURVE connections are denied. If there was
//  an error during initialization, returns NULL.

zauth_t *
zauth_new (zctx_t *ctx)
{    
    zauth_t *self = (zauth_t *) zmalloc (sizeof (zauth_t));
    assert (self);
    
    //  Start background agent and wait for it to initialize
    self->pipe = zthread_fork (ctx, s_agent_task, NULL);
    char *status = zstr_recv (self->pipe);
    if (strneq (status, "OK"))
        zauth_destroy (&self);
    free (status);
    return self;
}


//  --------------------------------------------------------------------------
//  Set the global whitelist. The whitelist is a list of IP addresses that are
//  always allowed, and is a text file containing one IP address per line, in
//  IPv4 or IPv6 text format. The filename is treated as a printf format. You 
//  may create or modify the file at any time; it is reloaded for each connect
//  request.

void
zauth_set_whitelist (zauth_t *self, char *filename, ...)
{
    va_list argptr;
    va_start (argptr, filename);
    char *formatted = zsys_vprintf (filename, argptr);
    va_end (argptr);
    zstr_sendx (self->pipe, "WHITELIST", formatted, NULL);
    free (formatted);
}


//  --------------------------------------------------------------------------
//  Set the global blacklist. The blacklist is a list of IP addresses that are
//  never allowed, and is a text file containing one IP address per line, in
//  IPv4 or IPv6 text format. The filename is treated as a printf format. You 
//  may create or modify the file at any time; it is reloaded for each connect
//  request.

void
zauth_set_blacklist (zauth_t *self, char *filename, ...)
{
    va_list argptr;
    va_start (argptr, filename);
    char *formatted = zsys_vprintf (filename, argptr);
    va_end (argptr);
    zstr_sendx (self->pipe, "BLACKLIST", formatted, NULL);
    free (formatted);
}


//  --------------------------------------------------------------------------
//  Configure PLAIN authentication for a given domain. PLAIN authentication
//  uses a plain-text password file. The filename is treated as a printf 
//  format. To cover all domains, use "*". You can modify the password file
//  at any time; it is reloaded automatically.

void
zauth_configure_plain (zauth_t *self, char *domain, char *filename, ...)
{
    assert (self);
    assert (domain);
    va_list argptr;
    va_start (argptr, filename);
    char *formatted = zsys_vprintf (filename, argptr);
    va_end (argptr);
    zstr_sendx (self->pipe, "PLAIN", domain, formatted, NULL);
    free (formatted);
}


//  --------------------------------------------------------------------------
//  Configure CURVE authentication for a given domain. CURVE authentication
//  uses a directory that holds all public client certificates, i.e. their
//  public keys. The certificates must be in zcert_save () format. The 
//  directory is treated as a printf format. To cover all domains, use "*". 
//  You can add and remove certificates in that directory at any time. 

void
zauth_configure_curve (zauth_t *self, char *domain, char *directory, ...)
{
    assert (self);
    assert (domain);
    va_list argptr;
    va_start (argptr, directory);
    char *formatted = zsys_vprintf (directory, argptr);
    va_end (argptr);
    zstr_sendx (self->pipe, "CURVE", domain, formatted, NULL);
    free (formatted);
}


//  --------------------------------------------------------------------------
//  Enable verbose tracing of commands and activity

void
zauth_set_verbose (zauth_t *self, bool verbose)
{
    assert (self);
    zstr_sendm (self->pipe, "VERBOSE");
    zstr_send  (self->pipe, "%d", verbose);
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
        free (zstr_recv (self->pipe));
        free (self);
        *self_p = NULL;
    }
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
} zap_request_t;


//  Receive a valid ZAP request from the handler socket
//  If the request was not valid, returns NULL.

static zap_request_t *
zap_request_new (void *handler)
{
    zap_request_t *self = (zap_request_t *) zmalloc (sizeof (zap_request_t));
    assert (self);
    
    //  Store handler socket so we can send a reply easily
    self->handler = handler;
    zmsg_t *request = zmsg_recv (handler);
    
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
        self->client_key = zmalloc (41);
        zmq_z85_encode (self->client_key, zframe_data (frame), 32);
    }
    zmsg_destroy (&request);
    return self;
}


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
        free (self);
        *self_p = NULL;
    }
}

//  Send a ZAP reply to the handler socket

int
zap_request_reply (zap_request_t *self, char *status_code, char *status_text)
{
    zstr_sendx (self->handler, 
        "1.0", self->sequence, status_code, status_text, "", "",
        NULL);
    return 0;
}

void
zap_request_dump (zap_request_t *self)
{
    printf ("Mechanism=%s domain=%s address=%s\n", 
        self->mechanism, self->domain, self->address);
}

        
//  *************************    BACK END AGENT    *************************

//  This structure holds the context for our agent, so we can
//  pass that around cleanly to methods which need it

typedef struct {
    zctx_t *ctx;                //  CZMQ context we're working for
    void *pipe;                 //  Pipe back to application API
    void *handler;              //  ZAP handler socket
    bool verbose;               //  Trace activity to stdout
    char *whitelist;            //  Default whitelist
    char *blacklist;            //  Default blacklist
    zhash_t *plain_passwd;      //  PLAIN passwords, if loaded
    char *curve_directory;      //  CURVE certificate directory
    zlist_t *curve_cert_list;   //  List of loaded certificates
    zhash_t *curve_certs;       //  Hash of loaded certificates
    time_t curve_modified;      //  Date/time directory changed
    bool terminated;            //  Did API ask us to quit?
} agent_t;

static agent_t *
s_agent_new (zctx_t *ctx, void *pipe)
{
    agent_t *self = (agent_t *) zmalloc (sizeof (agent_t));
    self->ctx = ctx;
    self->pipe = pipe;
    self->curve_cert_list = zlist_new ();

    //  Create ZAP handler and get ready for requests
    self->handler = zsocket_new (self->ctx, ZMQ_REP);
    if (zsocket_bind (self->handler, "inproc://zeromq.zap.01") == 0)
        zstr_send (self->pipe, "OK");
    else
        zstr_send (self->pipe, "ERROR");
        
    return self;
}

static void
s_agent_destroy (agent_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        agent_t *self = *self_p;
        free (self->blacklist);
        free (self->whitelist);
        zhash_destroy (&self->plain_passwd);
        
        zcert_t *cert = (zcert_t *) zlist_pop (self->curve_cert_list);
        while (cert) {
            zcert_destroy (&cert);
            cert = (zcert_t *) zlist_pop (self->curve_cert_list);
        }
        zlist_destroy (&self->curve_cert_list);
        zhash_destroy (&self->curve_certs);
        free (self->curve_directory);
        
        free (self);
        *self_p = NULL;
    }
}

//  Handle a message from front-end API
static void s_load_curve_certificates (agent_t *self);

static int
s_agent_handle_pipe (agent_t *self)
{
    //  Get the whole message off the pipe in one go
    zmsg_t *request = zmsg_recv (self->pipe);
    char *command = zmsg_popstr (request);
    if (!command)
        return -1;                  //  Interrupted

    if (streq (command, "WHITELIST")) {
        free (self->whitelist);
        self->whitelist = zmsg_popstr (request);
    }
    else
    if (streq (command, "BLACKLIST")) {
        free (self->blacklist);
        self->blacklist = zmsg_popstr (request);
    }
    else
    if (streq (command, "PLAIN")) {
        //  For now we don't do anything with domains
        char *domain = zmsg_popstr (request);
        free (domain);
        //  Get password file and load into zhash table
        //  If the file doesn't exist we'll get an empty table
        char *filename = zmsg_popstr (request);
        zhash_destroy (&self->plain_passwd);
        self->plain_passwd = zhash_new ();
        zhash_load (self->plain_passwd, filename);
        free (filename);
    }
    else
    if (streq (command, "CURVE")) {
        //  For now we don't do anything with domains
        char *domain = zmsg_popstr (request);
        free (domain);
        //  Get the directory name and load all certificates
        free (self->curve_directory);
        self->curve_directory = zmsg_popstr (request);
        s_load_curve_certificates (self);
    }
    else
    if (streq (command, "VERBOSE")) {
        char *verbose = zmsg_popstr (request);
        self->verbose = *verbose == '1';
        free (verbose);
    }
    else
    if (streq (command, "TERMINATE")) {
        self->terminated = true;
        zstr_send (self->pipe, "OK");
    }
    else {
        printf ("E: invalid command from API: %s\n", command);
        assert (false);
    }
    free (command);
    zmsg_destroy (&request);
    return 0;
}

static void
s_load_curve_certificates (agent_t *self)
{
    //  Want a zkeystore class to cover this
    
    zcert_t *cert = (zcert_t *) zlist_pop (self->curve_cert_list);
    while (cert) {
        zcert_destroy (&cert);
        cert = (zcert_t *) zlist_pop (self->curve_cert_list);
    }
    zlist_destroy (&self->curve_cert_list);
    zhash_destroy (&self->curve_certs);

    self->curve_cert_list = zlist_new ();
    self->curve_certs = zhash_new ();
    self->curve_modified = zsys_file_modified (self->curve_directory);
    
    zdir_t *dir = zdir_new (self->curve_directory, NULL);
    if (dir) {
        zfile_t **files = zdir_flatten (dir);
        uint index;
        for (index = 0;; index++) {
            zfile_t *file = files [index];
            if (!file)
                break;      //  End of list
            if (zfile_is_regular (file)) {
                zcert_t *cert = zcert_load (zfile_filename (file, NULL));
                if (cert) {
                    zlist_push (self->curve_cert_list, cert);
                    zhash_insert (self->curve_certs, zcert_public_txt (cert), cert);
                }
            }
        }
        free (files);
    }
}


//  Handle an authentication request from libzmq core

static bool s_authenticate_plain (agent_t *self, zap_request_t *request);
static bool s_authenticate_curve (agent_t *self, zap_request_t *request);

static int
s_agent_authenticate (agent_t *self)
{
    zap_request_t *request = zap_request_new (self->handler);
    if (request) {
        if (self->verbose)
            zap_request_dump (request);
        
        bool allowed = false;       //  Not allowed yet
        bool denied = false;        //  Not denied yet
        
        //  Check whitelist, and then blacklist
        if (self->whitelist) {
            FILE *whitelist = fopen (self->whitelist, "r");
            if (whitelist) {
                char line [256];
                while (fgets (line, 256, whitelist)) {
                    if (line [strlen (line) - 1] == '\n')
                        line [strlen (line) - 1] = 0;
                    if (streq (line, request->address)) {
                        allowed = true;
                        break;
                    }
                }
                fclose (whitelist);
            }
        }
        if (!allowed && self->blacklist) {
            FILE *blacklist = fopen (self->blacklist, "r");
            if (blacklist) {
                char line [256];
                while (fgets (line, 256, blacklist)) {
                    if (line [strlen (line) - 1] == '\n')
                        line [strlen (line) - 1] = 0;
                    if (streq (line, request->address)) {
                        denied = true;
                        break;
                    }
                }
                fclose (blacklist);
            }
        }
        //  Mechanism-specific checks
        if (!denied) {
            if (streq (request->mechanism, "NULL"))
                allowed = true;
            else
            if (streq (request->mechanism, "PLAIN"))
                allowed = s_authenticate_plain (self, request);
            else
            if (streq (request->mechanism, "CURVE"))
                allowed = s_authenticate_curve (self, request);
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
    zhash_refresh (self->plain_passwd);
    char *password = (char *) zhash_lookup (self->plain_passwd, request->username);
    if (password && streq (password, request->password))
        return true;
    else
        return false;
}


static bool 
s_authenticate_curve (agent_t *self, zap_request_t *request)
{
    //s_load_curve_certificates (self);
    if (self->curve_certs) {
        zcert_t *cert = (zcert_t *) zhash_lookup (self->curve_certs, request->client_key);
        if (cert)
            return true;
    }
    return false;
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
    while (!zpoller_terminated (poller) && !self->terminated) {
        void *which = zpoller_wait (poller, -1);
        if (which == self->pipe)
            s_agent_handle_pipe (self);
        else
        if (which == self->handler) {
            s_agent_authenticate (self);
        }
    }
    //  Done, free all agent resources
    zpoller_destroy (&poller);
    s_agent_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Selftest

//  Checks whether client can connect to server, and if so, returns
//  true; else returns false after a short timeout.

static bool
s_can_connect (void *server, void *client)
{
    //  We'll do each test on a new port number since otherwise we have to
    //  destroy and recreate the sockets each time.
    static int port_nbr = 9000;
    int rc = zsocket_bind (server, "tcp://*:%d", port_nbr);
    assert (rc == port_nbr);
    rc = zsocket_connect (client, "tcp://localhost:%d", port_nbr);
    assert (rc == 0);
    
    zpoller_t *poller = zpoller_new (client, NULL);
    zstr_send (server, "Hello, World");
    //  Need up to half a second if running under Valgrind
    bool success = zpoller_wait (poller, 500) == client;
    if (success)
        free (zstr_recv (client));
    zpoller_destroy (&poller);
    rc = zsocket_unbind (server, "tcp://*:%d", port_nbr);
    assert (rc != -1);
    rc = zsocket_disconnect (client, "tcp://localhost:%d", port_nbr);
    assert (rc != -1);
    port_nbr++;
    return success;
}

int
zauth_test (bool verbose)
{
    printf (" * zauth: ");

    //  @selftest
    
    //  Install the authenticator
    zctx_t *ctx = zctx_new ();
    zauth_t *auth = zauth_new (ctx);
    assert (auth);
    zauth_set_verbose (auth, verbose);
    
    //  A default NULL connection should always success, and not go through
    //  our authentication infrastructure at all.
    void *server = zsocket_new (ctx, ZMQ_PUSH);
    void *client = zsocket_new (ctx, ZMQ_PULL);
    bool success = s_can_connect (server, client);
    assert (success);
    
    //  When we set a domain on the server, we switch on authentication 
    //  for NULL sockets, but with no policies, the client connection will 
    //  be allowed.
    //
    //  TODO: libzmq should accept new security options after unbind/bind
    //  but for now we have to create a new server socket each time.
    server = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_set_zap_domain (server, "global");
    success = s_can_connect (server, client);
    assert (success);
        
    //  Let's try blacklisting on NULL connections; our address is 127.0.0.1
    //  Connection should now fail
    FILE *blacklist = fopen ("banned-addresses", "w");
    assert (blacklist);
    fprintf (blacklist, "127.0.0.1\n");
    fclose (blacklist);
    zauth_set_blacklist (auth, "banned-addresses");
    success = s_can_connect (server, client);
    assert (!success);
    
    //  Add a whitelist, which takes precedence
    //  Connection should now succeed again
    FILE *whitelist = fopen ("allowed-addresses", "w");
    assert (whitelist);
    fprintf (whitelist, "127.0.0.1\n");
    fclose (whitelist);
    zauth_set_whitelist (auth, "allowed-addresses");
    success = s_can_connect (server, client);
    assert (success);

    //  Try PLAIN authentication
    FILE *password = fopen ("password-file", "w");
    assert (password);
    fprintf (password, "admin=Password\n");
    fclose (password);
    zauth_configure_plain (auth, "*", "password-file");
    
    zsocket_set_plain_server (server, 1);
    zsocket_set_plain_username (client, "admin");
    zsocket_set_plain_password (client, "1234");
    success = s_can_connect (server, client);
    assert (!success);

    zsocket_set_plain_password (client, "Password");
    success = s_can_connect (server, client);
    assert (success);

    //  Try CURVE authentication
    zsocket_set_curve_server (server, 1);
    zsocket_set_curve_publickey (server, "rq:rM>}U?@Lns47E1%kR.o@n%FcmmsL/@{H8]yf7");
    zsocket_set_curve_secretkey (server, "JTKVSB%%)wK0E.X)V>+}o?pNmC{O&4W4b!Ni{Lh6");

    zsocket_set_curve_serverkey (client, "rq:rM>}U?@Lns47E1%kR.o@n%FcmmsL/@{H8]yf7");
    zsocket_set_curve_publickey (client, "Yne@$w-vo<fVvi]a<NY6T1ed:M$fCG*[IaLV{hID");
    zsocket_set_curve_secretkey (client, "D:)Q[IlAW!ahhC2ac:9*A}h:p?([4%wOTJ%JR%cs");

    success = s_can_connect (server, client);
    assert (!success);

    zsys_dir_create ("certificates");
    FILE *certificate = fopen ("certificates/mycert.txt", "w");
    assert (certificate);
    fprintf (certificate, "curve\n");
    fprintf (certificate, "    public-key = \"Yne@$w-vo<fVvi]a<NY6T1ed:M$fCG*[IaLV{hID\"\n");
    fclose (certificate);
    zauth_configure_curve (auth, "*", "certificates");
    success = s_can_connect (server, client);
    assert (success);
    
    //  Remove the authenticator and check a normal connection works
    zauth_destroy (&auth);
    success = s_can_connect (server, client);
    assert (success);
    
    zctx_destroy (&ctx);
    
    //  Delete all test files
    zsys_file_delete ("allowed-addresses");
    zsys_file_delete ("banned-addresses");
    zsys_file_delete ("password-file");
    zsys_file_delete ("certificates/mycert.txt");
    zsys_dir_delete ("certificates");
    
    //  @end
    printf ("OK\n");
    return 0;
}
