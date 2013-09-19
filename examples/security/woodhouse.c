//  The Woodhouse Pattern
//
//  It may keep some malicious people out but all it takes is a bit
//  of network sniffing, and they'll be able to fake their way in.

#include <czmq.h>

int main (void) 
{
    //  Create context and start authentication engine
    zctx_t *ctx = zctx_new ();
    zauth_t *auth = zauth_new (ctx);
    zauth_set_verbose (auth, true);
    zauth_allow (auth, "127.0.0.1");
    
    //  Tell the authenticator how to handle PLAIN requests
    zauth_configure_plain (auth, "*", "passwords");
        
    //  Create and bind server socket
    void *server = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_set_plain_server (server, 1);
    zsocket_bind (server, "tcp://*:9000");

    //  Create and connect client socket
    void *client = zsocket_new (ctx, ZMQ_PULL);
    zsocket_set_plain_username (client, "admin");
    zsocket_set_plain_password (client, "secret");
    zsocket_connect (client, "tcp://127.0.0.1:9000");
    
    //  Send a single message from server to client
    zstr_send (server, "Hello");
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Woodhouse test OK");

    zauth_destroy (&auth);
    zctx_destroy (&ctx);
    return 0;
}
