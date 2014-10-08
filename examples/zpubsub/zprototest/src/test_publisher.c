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
    printf (" 1. Publish test message\n");
    printf (" q  quit\n");
    printf ("====================================\n");
    printf (" Enter choice: ");
}

static byte *
s_serialize_message (zprototest_msg_t **msg, size_t *size)
{
    assert (msg);
    assert (size);

    zmsg_t *encoded_msg = zprototest_msg_encode (msg);
    assert (encoded_msg);

    zframe_t *frame = zmsg_pop (encoded_msg);
    *size = zframe_size (frame);
    byte *data = zmalloc (*size);
    memcpy (data, zframe_data (frame), *size);

    zframe_destroy (&frame);
    zmsg_destroy (&encoded_msg);

    return data;
}

int
main (int argc, char* argv[])
{
    zpubsub_t *pubsub = zpubsub_new (25, NULL);

    uint64_t id = 1;
    char message[256];

    while (true)
    {
        print_menu();

        char input[256];
        scanf ("%s", input);

        if (streq (input, "1"))
        {
            snprintf(message, 256, "This is message number %ld", id);

            zprototest_msg_t *msg = zprototest_msg_new (ZPROTOTEST_MSG_TESTMESSAGE);
            zprototest_msg_set_msgid (msg, id++);
            zprototest_msg_set_message (msg, message);
            zprototest_msg_set_testint (msg, (int) (zclock_time() & 0xffffffff));

            printf ("\nPublishing message:\n");
            print_msg (msg);

            size_t size;
            byte *data = s_serialize_message (&msg, &size);
            assert (data);

            zpubsub_publish (pubsub, "zprototest_msg_t", NULL, data, size);

            free (data);
        }
        else
        if (streq (input, "q") || streq (input, "Q"))
        {
            break;
        }
    }

    zpubsub_destroy (&pubsub);
}
