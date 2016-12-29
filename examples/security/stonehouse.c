//  The Stonehouse Pattern
//
//  Where we allow any clients to connect, but we promise clients
//  that we are who we claim to be, and our conversations won't be
//  tampered with or modified, or spied on.
//
//  CZMQ APIv3
//
//
//  For more info see: http://hintjens.com/blog:49#toc5
//

#include <czmq.h>

int main (void) 
{
    //  Create and start authentication engine
    zactor_t *auth = zactor_new (zauth,NULL);
    zstr_send(auth,"VERBOSE");
    zsock_wait(auth);
    zstr_sendx(auth,"ALLOW","127.0.0.1",NULL);
    zsock_wait(auth);
 
    //  Tell the authenticator how to handle CURVE requests
    zstr_sendx (auth,"CURVE",CURVE_ALLOW_ANY,NULL);

    //  We need two certificates, one for the client and one for
    //  the server. The client must know the server's public key
    //  to make a CURVE connection.
    zcert_t *client_cert = zcert_new ();
    zcert_t *server_cert = zcert_new ();
    const char *server_key = zcert_public_txt (server_cert);
        
    //  Create and bind server socket
    zsock_t *server = zsock_new (ZMQ_PUSH);
    zcert_apply (server_cert, server);
    zsock_set_curve_server (server, 1);
    zsock_bind (server, "tcp://*:9000");

    //  Create and connect client socket
    zsock_t *client = zsock_new (ZMQ_PULL);
    zcert_apply (client_cert, client);
    zsock_set_curve_serverkey (client, server_key);
    zsock_connect (client, "tcp://127.0.0.1:9000");
    
    //  Send a single message from server to client
    zstr_send (server, "Hello");
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Stonehouse test OK");

    zcert_destroy (&client_cert);
    zcert_destroy (&server_cert);
    zactor_destroy (&auth);
    zsock_destroy (&client);
    zsock_destroy (&server);
    return 0;
}
