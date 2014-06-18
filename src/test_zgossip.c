#include "../include/czmq.h"

int main (void)
{
    //  test case: cluster of central servers and cloud of mobile clients
    //  We'll start two central servers, and 10 mobile clients, and then
    //  connect each client to both servers.
    
    zactor_t *base = zactor_new (zgossip, "base");
    assert (base);
    zstr_sendx (base, "SET", "server/animate", "1", NULL);
    //  Set a 100msec timeout on clients so we can test expiry
    zstr_sendx (base, "SET", "server/timeout", "100", NULL);
    zstr_sendx (base, "BIND", "inproc://base", NULL);
    free (zstr_recv (base));

    zactor_t *alpha = zactor_new (zgossip, "alpha");
    assert (alpha);
    zstr_sendx (alpha, "CONNECT", "inproc://base", NULL);
    zstr_sendx (alpha, "PUBLISH", "inproc://alpha-1", "service1", NULL);
    zstr_sendx (alpha, "PUBLISH", "inproc://alpha-2", "service2", NULL);

    zactor_t *beta = zactor_new (zgossip, "beta");
    assert (beta);
    zstr_sendx (beta, "CONNECT", "inproc://base", NULL);
    zstr_sendx (beta, "PUBLISH", "inproc://beta-1", "service1", NULL);
    zstr_sendx (beta, "PUBLISH", "inproc://beta-2", "service2", NULL);

    //  got nothing
    zclock_sleep (200);

    zactor_destroy (&base);
    zactor_destroy (&alpha);
    zactor_destroy (&beta);

    return 0;
}