//  The Strawhouse Pattern
//
//  We allow or deny clients according to their IP address. It may keep 
//  spammers and idiots away, but won't stop a real attacker for more 
//  than a heartbeat.
//
//  CZMQ APIv3
//
//  For more info see: http://hintjens.com/blog:49#toc3
//
#include <czmq.h>

int main (void) 
{
    //  Start an authentication engine for this context. This engine
    //  allows or denies incoming connections (talking to the libzmq
    //  core over a protocol called ZAP).
    zactor_t *auth = zactor_new(zauth,NULL);
    
    //  Get some indication of what the authenticator is deciding
    zstr_send (auth,"VERBOSE");
    zsock_wait (auth);
    
    //  Allow our address; any other address will be rejected
    //  Add as much address as argument as you like before the NULL-Argument
    //  e.g. zstr_sendx (auth,"ALLOW","127.0.0.1","192.168.1.20",NULL);
    zstr_sendx (auth,"ALLOW","127.0.0.1",NULL);    
    zsock_wait (auth);
    
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
    puts ("Strawhouse test OK");

    zactor_destroy (&auth);
    zsock_destroy (&client);
    zsock_destroy (&server);    
    return 0;
}
