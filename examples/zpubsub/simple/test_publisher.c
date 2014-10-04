#include <czmq.h>
#include <zpubsub.h>
#include <zpubsub_option.h>
#include <zpubsub_filter.h>

typedef struct {
    uint64_t id;
    char *message;
    int test_int;
} test_message_t;

static void
print_msg (test_message_t *msg)
{
    printf ("    Id     : %ld\n", msg->id);
    printf ("    Message: %s\n", msg->message);
    printf ("    Testint: %d\n\n", msg->test_int);
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
s_serialize_message (test_message_t *msg, size_t *size)
{
    assert (msg);
    assert (size);

    dbyte message_len = (dbyte) strlen (msg->message);
    *size = sizeof (uint64_t) + sizeof (dbyte) + message_len + sizeof (int);
    byte *data = zmalloc (*size);

    byte *ptr = data;
    memcpy (ptr, &(msg->id), sizeof (uint64_t));

    ptr += sizeof (uint64_t);
    memcpy (ptr, &message_len, sizeof (dbyte));

    ptr += sizeof (dbyte);
    memcpy (ptr, msg->message, message_len);

    ptr += message_len;
    memcpy (ptr, &(msg->test_int), sizeof (int));

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

            test_message_t* msg = (test_message_t *) zmalloc (sizeof (test_message_t));
            msg->id = id++;
            msg->message = strdup (message);
            msg->test_int = (int) (zclock_time() & 0xffffffff);

            printf ("\nPublishing message:\n");
            print_msg (msg);

            size_t size;
            byte *data = s_serialize_message (msg, &size);
            assert (data);

            zpubsub_publish (pubsub, "test_message_t", NULL, data, size);

            free (msg->message);
            free (msg);
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
