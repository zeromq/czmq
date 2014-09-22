#include <czmq.h>

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

static int
s_deserialize_filter (byte *data, int size, zpubsub_filter_t **filter)
{
	assert (data);
	assert (size > 0);
	assert (filter);
	
	*filter = NULL;
	
	int min_size = 5 * sizeof (dbyte) + 2;
	
	if (size < min_size)
		return -1;

	zpubsub_filter_t *f = (zpubsub_filter_t *) zmalloc (sizeof (zpubsub_filter_t));
	assert (f);
	
	byte *ptr = data;
	memcpy (&(f->magic_number), ptr, sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	memcpy (&(f->major_version), ptr, sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	memcpy (&(f->minor_version), ptr, sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	dbyte part_len = 0;
	memcpy (&part_len, ptr, sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	assert (part_len > 0);
	f->partition = (char *) zmalloc ((part_len + 1) * sizeof (char));
	memcpy (f->partition, ptr, part_len);
	
	f->partition[part_len] = '\0';
	ptr += part_len;
	dbyte topic_len = 0;
	memcpy (&topic_len, ptr, sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	assert (topic_len > 0);
	f->topic = (char *) zmalloc ((topic_len + 1) * sizeof (char));
	memcpy (f->topic, ptr, topic_len);
	
	f->topic[topic_len] = '\0';
	
	*filter = f;
	return 0;
}

static int
s_serialize_filter (zpubsub_filter_t *filter, byte *data, int size)
{
	assert (filter);
	
	dbyte part_len = (dbyte) strlen (filter->partition);
	dbyte topic_len = (dbyte) strlen (filter->topic);
	int required_size = 5 * sizeof (dbyte) + part_len + topic_len;
	
	if (size < required_size)
		return required_size;
	
	byte *ptr = data;
	memcpy (ptr, &(filter->magic_number), sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	memcpy (ptr, &(filter->major_version), sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	memcpy (ptr, &(filter->minor_version), sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	memcpy (ptr, &part_len, sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	memcpy (ptr, filter->partition, part_len);
	
	ptr += part_len;
	memcpy (ptr, &topic_len, sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	memcpy (ptr, filter->topic, topic_len);
	
	return required_size;
}

static int
s_serialize_message (const char *topic, const char *partition, void *message, byte *data, int size)
{
	assert (topic);
	assert (partition);
	assert (message);
	
	test_message_t *msg = (test_message_t *) message;
	
	dbyte message_len = (dbyte) strlen (msg->message);
	int required_size = sizeof (uint64_t) + sizeof (dbyte) + message_len + sizeof (int);
	
	if (size < required_size)
		return required_size;
	
	byte *ptr = data;
	memcpy (ptr, &(msg->id), sizeof (uint64_t));
	
	ptr += sizeof (uint64_t);
	memcpy (ptr, &message_len, sizeof (dbyte));
	
	ptr += sizeof (dbyte);
	memcpy (ptr, msg->message, message_len);
	
	ptr += message_len;
	memcpy (ptr, &(msg->test_int), sizeof (int));
	
	return required_size;
}

static int
s_deserialize_message (const char *topic, const char *partition, void *args, byte *data, int size, void **message)
{
	assert (topic);
	assert (partition);
	assert (data);
	assert (size > 0);
	assert (message);
	
	*message = NULL;
	
	int min_size = sizeof (dbyte) + 1 + sizeof (int);
	
	if (size < min_size)
		return -1;

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
	
	*message = (void *) m;
	return 0;
}

static void
s_sample (const char *topic, const char *partition, void *message)
{
	assert (topic);
	assert (partition);
	assert (message);
	
	assert (streq (topic, "test_message_t"));
	assert (streq (partition, "<default>"));
	
	test_message_t *msg = (test_message_t *) message;
	assert (msg);
	
	printf ("\nReceived message:\n");
	print_msg (msg);
}

int
main (int argc, char* argv[])
{
	zpubsub_t *pubsub = zpubsub_new (25, NULL, s_serialize_filter, s_deserialize_filter);
	
	while (true)
	{
		print_menu();
		
		char input[256];
		scanf ("%s", input);
				
		if (streq (input, "1"))
		{
			zpubsub_subscribe (pubsub, "test_message_t", NULL, NULL, s_deserialize_message, s_sample);
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
