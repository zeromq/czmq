//  The Ironhouse Pattern
//
//  This is exactly the same example but broken into two threads
//  so you can better see what client and server do, separately.

#include <czmq.h>

//  The client task runs in its own context, and receives the 
//  server public key as an argument.

static void *
client_task (void *args)
{
    //  Load our persistent certificate from disk
    zcert_t *client_cert = zcert_load ("client_cert.txt");
    assert (client_cert);

    //  Create client socket and configure it to use full encryption
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *client = zsocket_new (ctx, ZMQ_PULL);
    assert (client);
    zcert_apply (client_cert, client);
    zsocket_set_curve_serverkey (client, (char *) args);
    int rc = zsocket_connect (client, "tcp://127.0.0.1:9000");
    assert (rc == 0);

    //  Wait for our message, that signals the test was successful
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Ironhouse test OK");

    //  Free all memory we used
    zcert_destroy (&client_cert);
    zctx_destroy (&ctx);
    return NULL;
}

static void *
server_task (void *args)
{
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    
    //  Start the authenticator and tell it do authenticate clients
    //  via the certificates stored in the .curve directory.
    zauth_t *auth = zauth_new (ctx);
    assert (auth);
    zauth_set_verbose (auth, true);
    zauth_allow (auth, "127.0.0.1");
    zauth_configure_curve (auth, "*", ".curve");
    
    //  Create server socket and configure it to use full encryption
    void *server = zsocket_new (ctx, ZMQ_PUSH);
    assert (server);
    zcert_apply ((zcert_t *) args, server);
    zsocket_set_curve_server (server, 1);
    int rc = zsocket_bind (server, "tcp://*:9000");
    assert (rc != -1);

    //  Send our test message, just once
    zstr_send (server, "Hello");
    zclock_sleep (200);
    
    //  Free all memory we used
    zauth_destroy (&auth);
    zctx_destroy (&ctx);
    return NULL;
}

int main (void) 
{
    //  Create the certificate store directory and client certs
    zcert_t *client_cert = zcert_new ();
    int rc = zsys_dir_create (".curve");
    assert (rc == 0);
    zcert_set_meta (client_cert, "name", "Client test certificate");
    zcert_save_public (client_cert, ".curve/testcert.pub");
    rc = zcert_save (client_cert, "client_cert.txt");
    assert (rc == 0);
    zcert_destroy (&client_cert);

    //  Create the server certificate
    zcert_t *server_cert = zcert_new ();
    
    //  Now start the two detached threads; each of these has their
    //  own ZeroMQ context.
    zthread_new (server_task, server_cert);
    zthread_new (client_task, zcert_public_txt (server_cert));
    
    //  As we're using detached threads this is the simplest way 
    //  to ensure they both end, before we exit the process. 200
    //  milliseconds should be enough for anyone. In real code,
    //  you would use messages to synchronize threads.
    zclock_sleep (200);

    //  Free the memory we used
    zcert_destroy (&server_cert);
    return 0;
}
