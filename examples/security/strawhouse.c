//  The Strawhouse Pattern
//
//  We allow or deny clients according to their IP address. It may keep 
//  spammers and idiots away, but won't stop a real attacker for more 
//  than a heartbeat.

#include <czmq.h>

int main (void) 
{
    //  Create context
    zctx_t *ctx = zctx_new ();
    
    //  Start an authentication engine for this context. This engine
    //  allows or denies incoming connections (talking to the libzmq
    //  core over a protocol called ZAP).
    zauth_t *auth = zauth_new (ctx);
    
    //  Get some indication of what the authenticator is deciding
    zauth_set_verbose (auth, true);
    
    //  Whitelist our address; any other address will be rejected
    zauth_allow (auth, "127.0.0.1");
        
    //  Create and bind server socket
    void *server = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_set_zap_domain (server, "global");
    zsocket_bind (server, "tcp://*:9000");

    //  Create and connect client socket
    void *client = zsocket_new (ctx, ZMQ_PULL);
    zsocket_connect (client, "tcp://127.0.0.1:9000");
    
    //  Send a single message from server to client
    zstr_send (server, "Hello");
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Strawhouse test OK");

    zauth_destroy (&auth);
    zctx_destroy (&ctx);
    return 0;
}
