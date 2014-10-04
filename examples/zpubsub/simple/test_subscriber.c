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
    printf (" 1. Subscribe to test message\n");
    printf (" 2. Unsubscribe from test message\n");
    printf (" q  quit\n");
    printf ("====================================\n");
    printf (" Enter choice: ");
}

static test_message_t *
s_deserialize_message (byte *data, size_t size)
{
    assert (data);
    assert (size > 0);

    int min_size = sizeof (dbyte) + 1 + sizeof (int);
    if (size < min_size)
        return NULL;

    test_message_t *m = (test_message_t *) zmalloc (sizeof (test_message_t));
    assert (m);

    byte *ptr = data;
    memcpy (&(m->id), ptr, sizeof (uint64_t));

    ptr += sizeof (uint64_t);
    dbyte message_len;
    memcpy (&message_len, ptr, sizeof (dbyte));
    assert (message_len > 0);

    m->message = (char *) zmalloc ((message_len + 1) * sizeof (char));
    ptr += sizeof (dbyte);
    memcpy (m->message, ptr, message_len);
    m->message[message_len] = '\0';

    ptr += message_len;
    memcpy (&(m->test_int), ptr, sizeof (int));

    return m;
}

static void
s_sample (const char *topic, const char *partition, void *args, byte *sample, size_t size)
{
    assert (topic);
    assert (partition);
    assert (sample);

    assert (streq (topic, "test_message_t"));
    assert (streq (partition, "<default>"));

    test_message_t *msg = s_deserialize_message (sample, size);
    assert (msg);

    printf ("\nReceived message:\n");
    print_msg (msg);

    free (msg->message);
    free (msg);
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
            zpubsub_subscribe (pubsub, "test_message_t", NULL, NULL, s_sample);
        }
        else
        if (streq (input, "2"))
        {
            zpubsub_unsubscribe (pubsub, "test_message_t", NULL);
        }
        else
        if (streq (input, "q") || streq (input, "Q"))
        {
            break;
        }
    }

    zpubsub_destroy (&pubsub);
}
