//  The Stonehouse Pattern
//
//  Where we allow any clients to connect, but we promise clients
//  that we are who we claim to be, and our conversations won't be
//  tampered with or modified, or spied on.

#include <czmq.h>

int main (void) 
{
    //  Create context and start authentication engine
    zctx_t *ctx = zctx_new ();
    zauth_t *auth = zauth_new (ctx);
    zauth_set_verbose (auth, true);
    zauth_allow (auth, "127.0.0.1");
    
    //  Tell the authenticator how to handle CURVE requests
    zauth_configure_curve (auth, "*", CURVE_ALLOW_ANY);

    //  We need two certificates, one for the client and one for
    //  the server. The client must know the server's public key
    //  to make a CURVE connection.
    zcert_t *client_cert = zcert_new ();
    zcert_t *server_cert = zcert_new ();
    char *server_key = zcert_public_txt (server_cert);
        
    //  Create and bind server socket
    void *server = zsocket_new (ctx, ZMQ_PUSH);
    zcert_apply (server_cert, server);
    zsocket_set_curve_server (server, 1);
    zsocket_bind (server, "tcp://*:9000");

    //  Create and connect client socket
    void *client = zsocket_new (ctx, ZMQ_PULL);
    zcert_apply (client_cert, client);
    zsocket_set_curve_serverkey (client, server_key);
    zsocket_connect (client, "tcp://127.0.0.1:9000");
    
    //  Send a single message from server to client
    zstr_send (server, "Hello");
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Stonehouse test OK");

    zcert_destroy (&client_cert);
    zcert_destroy (&server_cert);
    zauth_destroy (&auth);
    zctx_destroy (&ctx);
    return 0;
}
