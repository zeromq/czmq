//  The Ironhouse Pattern
//
//  Security doesn't get any stronger than this. An attacker is going to
//  have to break into your systems to see data before/after encryption.
//
//  CZMQ APIv3
//
//
//  For more info see: http://hintjens.com/blog:49#toc6
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

    //  Tell the authenticator to use the certificate store in .curve
    zstr_sendx (auth,"CURVE",".curve",NULL);

    //  We'll generate a new client certificate and save the public part
    //  in the certificate store (in practice this would be done by hand
    //  or some out-of-band process).
    zcert_t *client_cert = zcert_new ();
    zsys_dir_create (".curve");
    zcert_set_meta (client_cert, "name", "Client test certificate");
    zcert_save_public (client_cert, ".curve/testcert.pub");
    
    //  Prepare the server certificate as we did in Stonehouse
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
    puts ("Ironhouse test OK");

    zcert_destroy (&client_cert);
    zcert_destroy (&server_cert);
    zactor_destroy (&auth);
    zsock_destroy (&client);
    zsock_destroy (&server);

    return 0;
}
