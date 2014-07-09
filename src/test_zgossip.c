#include "../include/czmq.h"

void assert_status (zactor_t *actor, int status)
{
    zstr_sendx (actor, "STATUS", NULL);
    char *reply = zstr_recv (actor);
    assert (atoi (reply) == status);
    free (reply);
}


int main (void)
{
    //  Test case 1: two servers, bunch of clients.
    printf ("Starting test case 1: ");
    fflush (stdout);
    
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
    fflush (stdout);

    size_t swarm_size = 200;
    size_t set_size = 8000;
    //  Should be able to set actor HWMs
    zsys_set_sndhwm (set_size * 2);
    zactor_t *nodes [swarm_size];

    //  Create swarm
    uint node_nbr;
    for (node_nbr = 0; node_nbr < swarm_size; node_nbr++)
        nodes [node_nbr] = zactor_new (zgossip, NULL);
    printf (".");
    fflush (stdout);

    //  Interconnect swarm; ever node connects to one arbitrary node to
    //  create a directed graph, then oldest node connects to youngest
    //  node to create a loop, to test we're robust against cycles.
    for (node_nbr = 0; node_nbr < swarm_size; node_nbr++) {
        zstr_sendm (nodes [node_nbr], "BIND");
        zstr_sendf (nodes [node_nbr], "inproc://swarm-%d", node_nbr);
        if (node_nbr > 0) {
            zstr_sendm (nodes [node_nbr], "CONNECT");
            zstr_sendf (nodes [node_nbr], "inproc://swarm-%d", randof (node_nbr));
        }
    }
    zstr_sendm (nodes [0], "CONNECT");
    zstr_sendf (nodes [0], "inproc://swarm-%d", node_nbr - 1);
    printf (".");
    fflush (stdout);

    //  Publish the data set randomly across the swarm
    int item_nbr;
    for (item_nbr = 0; item_nbr < set_size; item_nbr++) {
        node_nbr = randof (swarm_size);
        zstr_sendm  (nodes [node_nbr], "PUBLISH");
        zstr_sendfm (nodes [node_nbr], "key-%d", item_nbr);
        zstr_send   (nodes [node_nbr], "value");
    }
    printf (".");
    fflush (stdout);
    
    //  Allow time for traffic to propagate across whole swarm
    for (node_nbr = 0; node_nbr < swarm_size; node_nbr++) {
        while (true) {
            printf ("%d\n", node_nbr);
            zstr_sendx (nodes [node_nbr], "STATUS", NULL);
            char *reply = zstr_recv (nodes [node_nbr]);
            int status = atoi (reply);
            free (reply);
            if (status == set_size)
                break;
            zclock_sleep (250);
            printf (".");
            fflush (stdout);
        }
    }
        
    //  Destroy swarm
    for (node_nbr = 0; node_nbr < swarm_size; node_nbr++)
        zactor_destroy (&nodes [node_nbr]);
    
    printf ("OK\n");
    
    return 0;
}