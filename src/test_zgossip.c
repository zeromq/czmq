#include "../include/czmq.h"

int main (void)
{
    //  Test case: cluster of central servers and cloud of mobile clients
    //  We'll start two central servers, and 10 mobile clients, and then
    //  connect each client to both servers.

    zactor_t *server1 = zactor_new (zgossip, "server1");
    assert (server1);
    zstr_sendx (server1, "SET", "server/animate", "0", NULL);
    zstr_sendx (server1, "BIND", "inproc://server1", NULL);
    free (zstr_recv (server1));
    
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
    zstr_sendx (client3, "CONNECT", "inproc://client2", NULL);
    
    zactor_t *client4 = zactor_new (zgossip, "client4");
    assert (client4);
    zstr_sendx (client4, "CONNECT", "inproc://client2", NULL);
    
    zclock_sleep (100);
    
    zstr_sendx (server1, "DUMP", "server1", NULL);
    zstr_sendx (client1, "DUMP", "client1", NULL);
    zstr_sendx (client2, "DUMP", "client2", NULL);
    zstr_sendx (client3, "DUMP", "client3", NULL);
    
//     int client_max = 2;
//     zactor_t *clients [client_max];
//     int client_nbr;
//     for (client_nbr = 0; client_nbr < client_max; client_nbr++) {
//         char client_name [15];
//         sprintf (client_name, "client-%d", client_nbr);
//         zactor_t *client = zactor_new (zgossip, client_name);
//         assert (client);
//         zstr_sendx (client, "SET", "server/animate", "0", NULL);
//         zstr_sendx (client, "CONNECT", "inproc://server1", NULL);
// //         zstr_sendx (client, "CONNECT", "inproc://server2", NULL);
//         zstr_sendx (client, "PUBLISH", client_name, "yes", NULL);
//         clients [client_nbr] = client;
//     }
//     //  Give the system time to settle
//     zclock_sleep (200);
    
//     //  Now get every node's status
//     for (client_nbr = 0; client_nbr < client_max; client_nbr++) {
//         char client_name [15];
//         sprintf (client_name, "client-%d", client_nbr);
//         zstr_sendx (clients [client_nbr], "DUMP", client_name, NULL);
//     }
    
    //  Give the system time to settle
//     zclock_sleep (100);
    
    zactor_destroy (&server1);
    zactor_destroy (&client1);
    zactor_destroy (&client2);
    zactor_destroy (&client3);
    zactor_destroy (&client4);

    return 0;
}