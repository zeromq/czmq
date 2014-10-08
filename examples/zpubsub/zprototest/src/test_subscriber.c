#include <czmq.h>
#include <zpubsub.h>
#include <zpubsub_option.h>
#include <zpubsub_filter.h>
#include "../include/zprototest_msg.h"

static void
print_msg (zprototest_msg_t *msg)
{
    zprototest_msg_print (msg);
//     printf ("    Id     : %d\n", zprototest_msg_msgid (msg));
//     printf ("    Message: %s\n", zprototest_msg_message (msg));
//     printf ("    Testint: %d\n\n", zprototest_msg_testint (msg));
}

static void
print_menu ()
{
    printf ("\n");
    printf ("====================================\n");
    printf (" 1. Subscribe to test message\n");
    printf (" 2. Unsubscribe from test message\n");
    printf (" q  quit\n");
    printf ("====================================\n");
    printf (" Enter choice: ");
}

static zprototest_msg_t *
s_deserialize_message (byte *data, size_t size)
{
    assert (data);
    assert (size > 0);

    zmsg_t *msg = zmsg_new ();
    zmsg_addmem (msg, data, size);
    zprototest_msg_t *decoded_msg = zprototest_msg_decode (&msg);

    return decoded_msg;
}

static void
s_sample (const char *topic, const char *partition, void *args, byte *sample, size_t size)
{
    assert (topic);
    assert (partition);
    assert (sample);

    assert (streq (topic, "zprototest_msg_t"));
    assert (streq (partition, "<default>"));

    zprototest_msg_t *msg = s_deserialize_message (sample, size);
    assert (msg);

    printf ("\nReceived message:\n");
    print_msg (msg);

    zprototest_msg_destroy (&msg);
}

int
main (int argc, char* argv[])
{
    zpubsub_t *pubsub = zpubsub_new (25, NULL);

    while (true)
    {
        print_menu();

        char input[256];
        scanf ("%s", input);

        if (streq (input, "1"))
        {
            zpubsub_subscribe (pubsub, "zprototest_msg_t", NULL, NULL, s_sample);
        }
        else
        if (streq (input, "2"))
        {
            zpubsub_unsubscribe (pubsub, "zprototest_msg_t", NULL);
        }
        else
        if (streq (input, "q") || streq (input, "Q"))
        {
            break;
        }
    }

    zpubsub_destroy (&pubsub);
}
