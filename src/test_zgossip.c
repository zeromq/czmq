#include "../include/czmq.h"

void assert_status (zactor_t *actor, int status)
{
    zstr_sendx (actor, "STATUS", NULL);
    char *reply = zstr_recv (actor);
    puts (reply);
    assert (atoi (reply) == status);
    free (reply);
}


int main (void)
{
    //  Test case 1: two servers, bunch of clients.
    printf ("Starting test case 1: ");
    
    zactor_t *server1 = zactor_new (zgossip, "server1");
    assert (server1);
    zstr_sendx (server1, "SET", "server/animate", "0", NULL);
    zstr_sendx (server1, "BIND", "inproc://server1", NULL);
    
    zactor_t *server2 = zactor_new (zgossip, "server2");
    assert (server2);
    zstr_sendx (server2, "SET", "server/animate", "0", NULL);
    zstr_sendx (server2, "BIND", "inproc://server2", NULL);
    zstr_sendx (server2, "CONNECT", "inproc://server1", NULL);
    
    zactor_t *client1 = zactor_new (zgossip, "client1");
    assert (client1);
    zstr_sendx (client1, "BIND", "inproc://client1", NULL);
    zstr_sendx (client1, "PUBLISH", "client1-00", "0000", NULL);
    zstr_sendx (client1, "PUBLISH", "client1-11", "1111", NULL);
    zstr_sendx (client1, "PUBLISH", "client1-22", "2222", NULL);
    zstr_sendx (client1, "CONNECT", "inproc://server1", NULL);
    
    zactor_t *client2 = zactor_new (zgossip, "client2");
    assert (client2);
    zstr_sendx (client2, "BIND", "inproc://client2", NULL);
    zstr_sendx (client2, "CONNECT", "inproc://server1", NULL);
    zstr_sendx (client2, "PUBLISH", "client2-00", "0000", NULL);
    zstr_sendx (client2, "PUBLISH", "client2-11", "1111", NULL);
    zstr_sendx (client2, "PUBLISH", "client2-22", "2222", NULL);
    
    zactor_t *client3 = zactor_new (zgossip, "client3");
    assert (client3);
    zstr_sendx (client3, "CONNECT", "inproc://server2", NULL);
    
    zactor_t *client4 = zactor_new (zgossip, "client4");
    assert (client4);
    zstr_sendx (client4, "CONNECT", "inproc://server2", NULL);
    
    zclock_sleep (100);

    assert_status (server1, 6);
    assert_status (server2, 6);
    assert_status (client1, 6);
    assert_status (client2, 6);
    assert_status (client3, 6);
    assert_status (client4, 6);
        
    zactor_destroy (&server1);
    zactor_destroy (&server2);
    zactor_destroy (&client1);
    zactor_destroy (&client2);
    zactor_destroy (&client3);
    zactor_destroy (&client4);
    printf ("OK\n");

    //  Test case 2: swarm of peers
    printf ("Starting test case 2: ");
    printf ("OK\n");

    return 0;
}