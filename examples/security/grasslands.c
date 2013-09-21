//  The Grasslands Pattern
//
//  The Classic ZeroMQ model, plain text with no protection at all.

#include <czmq.h>

int main (void) 
{
    //  Create context
    zctx_t *ctx = zctx_new ();
    
    //  Create and bind server socket
    void *server = zsocket_new (ctx, ZMQ_PUSH);
    zsocket_bind (server, "tcp://*:9000");

    //  Create and connect client socket
    void *client = zsocket_new (ctx, ZMQ_PULL);
    zsocket_connect (client, "tcp://127.0.0.1:9000");
    
    //  Send a single message from server to client
    zstr_send (server, "Hello");
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Grasslands test OK");
    
    zctx_destroy (&ctx);
    return 0;
}
