//  The Ironhouse Pattern
//
//  This is exactly the same example but broken into two threads
//  so you can better see what client and server do, separately.

#include <czmq.h>

void await_term (zsock_t *pipe)
{
    bool terminated = false;
    while (!terminated) {
        zmsg_t *msg = zmsg_recv (pipe);
        if (!msg)
            break;              //  Interrupted
        char *command = zmsg_popstr (msg);
        //  All actors must handle $TERM in this way
        if (streq (command, "$TERM"))
            terminated = true;
        else {
            puts ("E: invalid message to actor");
            assert (false);
        }
        free (command);
        zmsg_destroy (&msg);
    }
}

//  The client task runs in its own context, and receives the
//  server public key as an argument.

static void
client_task (zsock_t *pipe, void *args)
{
    //  Load our persistent certificate from disk
    zcert_t *client_cert = zcert_load ("client_cert.txt");
    assert (client_cert);

    //  Create client socket and configure it to use full encryption
    zsock_t *client = zsock_new (ZMQ_PULL);
    assert (client);
    zcert_apply (client_cert, client);
    zsock_set_curve_serverkey (client, (char *) args);
    int rc = zsock_connect (client, "tcp://127.0.0.1:9000");
    assert (rc == 0);

    zsock_signal (pipe, 0);

    //  Wait for our message, that signals the test was successful
    char *message = zstr_recv (client);
    assert (streq (message, "Hello"));
    free (message);
    puts ("Ironhouse test OK");

    await_term (pipe);

    //  Free all memory we used
    zsock_destroy (&client);
    zcert_destroy (&client_cert);
}

static void
server_task (zsock_t *pipe, void *args)
{
    //  Start the authenticator and tell it do authenticate clients
    //  via the certificates stored in the .curve directory.
    zactor_t *auth = zactor_new (zauth, NULL);
    assert (auth);
    assert (zstr_send (auth, "VERBOSE") == 0);
    assert (zsock_wait (auth) >= 0);
    assert (zstr_sendx (auth, "ALLOW", "127.0.0.1", NULL) == 0);
    assert (zsock_wait (auth) >= 0);
    assert (zstr_sendx (auth, "CURVE", ".curve", NULL) == 0);
    assert (zsock_wait (auth) >= 0);

    //  Create server socket and configure it to use full encryption
    zsock_t *server = zsock_new (ZMQ_PUSH);
    assert (server);
    zcert_apply ((zcert_t *) args, server);
    zsock_set_curve_server (server, 1);
    int rc = zsock_bind (server, "tcp://*:9000");
    assert (rc != -1);

    zsock_signal (pipe, 0);

    //  Send our test message, just once
    assert (zstr_send (server, "Hello") == 0);

    await_term (pipe);

    //  Free all memory we used
    zsock_destroy (&server);
    zactor_destroy (&auth);
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
    zactor_t *server_actor = zactor_new (server_task, server_cert);
    assert (server_actor);
    zactor_t *client_actor = zactor_new (client_task, (void *) zcert_public_txt (server_cert));
    assert (client_actor);

    //  Free the memory we used
    //  and coordinate process termination with actor termination
    zactor_destroy (&client_actor);
    zactor_destroy (&server_actor);

    return 0;
}
