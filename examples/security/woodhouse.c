//  The Woodhouse Pattern
//
//  It may keep some malicious people out but all it takes is a bit
//  of network sniffing, and they'll be able to fake their way in.
//
//  CZMQ APIv3
//
//
//  For more info see: http://hintjens.com/blog:49#toc4
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
    //  Tell the authenticator how to handle PLAIN requests
    zstr_sendx(auth,"PLAIN","passwords",NULL);
    zsock_wait(auth); 
    //  Create and bind server socket
    zsock_t *server = zsock_new (ZMQ_PUSH);
    zsock_set_plain_server (server, 1);
    zsock_bind (server, "tcp://*:9000");

    //  Create and connect client socket
    zsock_t *client = zsock_new (ZMQ_PULL);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "secret");
    zsock_connect (client, "tcp://127.0.0.1:9000");
    
    //  Send a single message from server to client
    zstr_send (server, "Hello");
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Woodhouse test OK");

    zactor_destroy (&auth);
    zsock_destroy (&server);
    zsock_destroy (&client);
    return 0;
}
