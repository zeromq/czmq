//  The Grasslands Pattern
//
//  The Classic ZeroMQ model, plain text with no protection at all.
//
//  CZMQ APIv3
//
//  More info: http://hintjens.com/blog:49#toc2
//

#include <czmq.h>

int main (void) 
{
    //  Create and bind server socket
    zsock_t *server = zsock_new (ZMQ_PUSH);
    zsock_bind (server, "tcp://*:9000");

    //  Create and connect client socket
    zsock_t *client = zsock_new (ZMQ_PULL);
    zsock_connect (client, "tcp://127.0.0.1:9000");
    
    //  Send a single message from server to client
    zstr_send (server, "Hello");
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Grasslands test OK");
    
    zsock_destroy (&client);
    zsock_destroy (&server);
    return 0;
}
