/*  =========================================================================
    zpubsub - simple pubsub framework

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The zpubsub class provides a simple pubsub framework.

@discuss
    zpubsub allows publishing of a "topic", being a message of
    a certain type, known to both the publisher of said
    message, and its subscribers. Publishers and subscribers
    operate on a specific domain (an integer id) which, in the
    context of a CZMQ zpubsub instance, translates into a specific
    TCP port on which they communicate. Messages are further filtered
    on "partition", which is a string passed when publishing a message
    or when subscribing to specific message types.

    zpubsub borrows some terminology from the Object Management Group
    Data Distribution Service (http://www.omg.org/hot-topics/dds.htm),
    but is in no way associated with the efforts of the OMG, neither
    does it claim to be in any way compliant with the DDS specification.
    It is, however, implemented as a simple alternative to using a full
    fledge DDS implementation to achieve the purposes intended for the
    zpubsub class.

    zpubsub relies heavily on the zactor and zloop implementations to
    achieve asynchronous behaviour, and uses the zbeacon to broadcast
    the presence of publishers. The basic mechanisms for publishing
    and subscribing are of course implemented by way of 0MQ PUB and
    SUB sockets.

    The message serialization protocol is fully up to the application.
    zpubsub accepts a blob of data for publishing and delivers the
    same blob unchanged to the subscribers. Currently, the filter
    data is serialized in a non-portable way, so that zpubsub in
    its current form will not be able to communicate across little/big-
    endian platforms. This will be amended by using zproto in a near-
    future update.

    Two examples are included in the examples/zpubsub directory.
    The simple example is implemented in pure C, with the simplest
    form of (platform-dependent) binary serialization. The protobuf
    example wraps zpubsub in a Participant class, which uses
    Google Protocol Buffers to serialize data in a portable manner.
@end
*/

#include "../include/czmq.h"
#include "./zpubsub_filter.h"


//  zpubsub_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZPUBSUB_TAG          0x02808508

//  Structure of our class

struct _zpubsub_t {
    uint32_t tag;          //  Object tag for runtime detection
    int domain;            //  Domain id; basis for port selection
    char *partition;       //  Default partition to use; defaults to "<default>"
    zsock_t *pub_socket;   //  The publisher socket
    zactor_t *pub_beacon;  //  The publisher beacon
    zactor_t *sub_actor;   //  Actor for asynchronous subscriber management
};


//  Data for sample callback

typedef struct {
    zpubsub_sample_fn *sample_fn;
} sample_function_t;


//  Data for a subscriber

typedef struct {
    char *topic;
    char *partition;
    char *key;
    void *args;
    sample_function_t *sample_function;
} sub_data_t;


//  Data for the subscriber actor

typedef struct {
    dbyte port;
    char *default_partition;
    dbyte beacon_port;
} sub_actor_data_t;


//  Data for the subscriber loop handlers

typedef struct {
    zsock_t *sub_socket;
    dbyte port;
    char *default_partition;
    zhash_t *connections;
    zhash_t *subscribers;
} sub_reactor_data_t;


//  --------------------------------------------------------------------------
//  Buffer destructor function (zhash_set_destructor will not accept zstr_free)

static void
s_free_buffer (void **buffer)
{
    assert (buffer);
    free (*buffer);
    *buffer = NULL;
}


//  --------------------------------------------------------------------------
//  Subscriber destructor function

static void
s_free_subscriber (void **subscriber)
{
    assert (subscriber);

    if (*subscriber) {
        sub_data_t *sub = (sub_data_t *) *subscriber;

        free (sub->partition);
        free (sub->topic);
        free (sub->key);
        free (sub->sample_function);
        free (sub);

        *subscriber = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Get pointer value from buffer

static void *
s_buffer_to_ptr (byte *buffer, size_t size)
{
    assert (buffer);
    assert (size >= sizeof (void *));

    void *ptr;
    memcpy (&ptr, buffer, sizeof (void *));
    return ptr;
}


//  --------------------------------------------------------------------------
//  Store pointer value in buffer

static void
s_ptr_to_buffer (void *ptr, byte *buffer, size_t size)
{
    assert (buffer);
    assert (size >= sizeof (void *));

    memcpy (buffer, &ptr, sizeof (void *));
}


//  --------------------------------------------------------------------------
//  Get pointer value from message, popping the topmost frame

static void *
s_msg_pop_frame_to_ptr (zmsg_t *msg)
{
    assert (msg);

    zframe_t *ptr_frame = zmsg_pop (msg);
    assert (ptr_frame);
    void *ptr =
        s_buffer_to_ptr (zframe_data (ptr_frame), zframe_size (ptr_frame));
    zframe_destroy (&ptr_frame);

    return ptr;
}


//  --------------------------------------------------------------------------
//  Generate subscriber key

static char *
s_generate_key (const char *topic, const char *partition)
{
    assert (topic);
    assert (partition);

    size_t required_len = strlen (partition) + strlen (topic) + 2;
    char *key = (char *) zmalloc (required_len * sizeof (char));
    assert (key);

    snprintf (key, required_len, "%s:%s", partition, topic);
    return key;
}


//  --------------------------------------------------------------------------
//  Encode filter

static zframe_t *
s_encode_filter (const char *topic, const char *partition)
{
    zmsg_t *msg = zpubsub_filter_encode_filter (partition, topic);
    assert (msg);

    zframe_t *encoded_filter = zmsg_pop (msg);
    zmsg_destroy (&msg);

    return encoded_filter;
}


//  --------------------------------------------------------------------------
//  Decode filter

static zpubsub_filter_t *
s_decode_filter (zframe_t **encoded_filter)
{
    assert (encoded_filter);

    zmsg_t *msg = zmsg_new ();
    assert (msg);

    zmsg_append (msg, encoded_filter);
    zpubsub_filter_t *filter = zpubsub_filter_decode (&msg);

    return filter;
}


//  --------------------------------------------------------------------------
//  Calculate port number from domain id

dbyte
s_get_port (int domain)
{
    assert (domain >= 0 && domain < 100);

    return (dbyte) (23013 + 13 * domain);
}


//  --------------------------------------------------------------------------
//  Beacon handler

static int
s_sub_beacon_fn (zloop_t *loop, zmq_pollitem_t *item, void *args)
{
    sub_reactor_data_t *reactor_data = (sub_reactor_data_t *) args;

    char *address;
    int rc = zsock_recv (item->socket, "s", &address);
    assert (rc == 0);

    if (address) {
        if (zhash_lookup (reactor_data->connections, address) == NULL) {
            int rc =
                zsock_connect(reactor_data->sub_socket, "tcp://%s:%d", address,
                              reactor_data->port);

            if (rc == 0)
                zhash_insert (reactor_data->connections, address,
                              strdup (address));
        }

        free (address);
    }

    return 0;
}


//  --------------------------------------------------------------------------
//  Control message handler

static int
s_sub_ctrl_fn (zloop_t *loop, zsock_t *reader, void *args)
{
    int rc = 0;
    sub_reactor_data_t *reactor_data = (sub_reactor_data_t *) args;

    zmsg_t *msg = zmsg_recv(reader);
    assert (msg);

    char *command = zmsg_popstr (msg);
    assert (command);

    if (streq(command, "$TERM")) {
        rc = -1;
    }
    else
    if (streq(command, "SUB")) {
        sub_data_t * sub = (sub_data_t *) zmalloc (sizeof (sub_data_t));

        sub->topic = zmsg_popstr (msg);
        sub->partition = zmsg_popstr (msg);
        sub->key = s_generate_key (sub->topic, sub->partition);
        sub->args = s_msg_pop_frame_to_ptr (msg);
        sub->sample_function =
            (sample_function_t *) s_msg_pop_frame_to_ptr (msg);

        assert (sub->topic);
        assert (sub->partition);
        assert (sub->key);
        assert (sub->sample_function);
        assert (sub->sample_function->sample_fn);

        zhash_update (reactor_data->subscribers, sub->key, sub);

        zframe_t *encoded_filter = s_encode_filter (sub->topic, sub->partition);
        assert (encoded_filter);

        rc = zmq_setsockopt (zsock_resolve (reactor_data->sub_socket),
                             ZMQ_SUBSCRIBE,
                             zframe_data (encoded_filter),
                             zframe_size (encoded_filter));
        assert (rc == 0 || zmq_errno () == ETERM);

        zframe_destroy (&encoded_filter);
    }
    else
    if (streq(command, "UNSUB")) {
        char *topic = zmsg_popstr (msg);
        char *partition = zmsg_popstr (msg);
        char *key = s_generate_key (topic, partition);

        assert (topic);
        assert (partition);
        assert (key);

        sub_data_t * sub = (sub_data_t *) zhash_lookup (reactor_data->subscribers, key);

        if (sub) {
            zframe_t *encoded_filter = s_encode_filter (topic, partition);
            assert (encoded_filter);

            rc = zmq_setsockopt (zsock_resolve (reactor_data->sub_socket),
                                 ZMQ_UNSUBSCRIBE,
                                 zframe_data (encoded_filter),
                                 zframe_size (encoded_filter));
            assert (rc == 0 || zmq_errno () == ETERM);

            zframe_destroy (&encoded_filter);
            zhash_delete (reactor_data->subscribers, key);
        }

        free (topic);
        free (partition);
        free (key);
    }

    free (command);
    zmsg_destroy (&msg);

    return rc;
}


//  --------------------------------------------------------------------------
//  Sub socket handler

static int
s_sub_sample_fn (zloop_t *loop, zsock_t *reader, void *args)
{
    sub_reactor_data_t *reactor_data = (sub_reactor_data_t *) args;

    zframe_t *filter_frame, *message_frame;
    int rc = zsock_recv (reader, "ff", &filter_frame, &message_frame);
    assert (rc == 0);

    zpubsub_filter_t *filter = s_decode_filter (&filter_frame);
    assert (filter);

    const char *topic = zpubsub_filter_topic (filter);
    assert (topic);

    const char *partition = zpubsub_filter_partition (filter);
    assert (partition);

    char *key = s_generate_key (topic, partition);
    assert (key);

    sub_data_t *sub = (sub_data_t *) zhash_lookup (reactor_data->subscribers, key);
    free (key);

    if (sub)
        (sub->sample_function->sample_fn) (topic,
                                           partition,
                                           sub->args,
                                           zframe_data (message_frame),
                                           zframe_size (message_frame));

    zpubsub_filter_destroy (&filter);
    zframe_destroy (&message_frame);

    return 0;
}


//  --------------------------------------------------------------------------
//  Asynchronous subscriber management actor function

static void
s_sub_actor (zsock_t *pipe, void *args)
{
    assert (pipe);
    assert (args);

    zsock_signal (pipe, 0);

    sub_actor_data_t *actor_data = (sub_actor_data_t *) args;

    zsock_t *sub_socket = zsock_new (ZMQ_SUB);

    zactor_t *sub_beacon = zactor_new (zbeacon, NULL);
    zsock_send (sub_beacon, "si", "CONFIGURE", actor_data->beacon_port);
    zsock_send (sub_beacon, "sb", "SUBSCRIBE", NULL, 0);

    sub_reactor_data_t reactor_data;
    reactor_data.sub_socket = sub_socket;
    reactor_data.port = actor_data->port;
    reactor_data.default_partition = strdup (actor_data->default_partition);
    reactor_data.connections = zhash_new ();
    assert (reactor_data.connections);
    zhash_set_destructor (reactor_data.connections, s_free_buffer);
    reactor_data.subscribers = zhash_new ();
    assert (reactor_data.subscribers);
    zhash_set_destructor (reactor_data.subscribers, s_free_subscriber);

    zloop_t *sub_reactor = zloop_new ();
    zloop_reader (sub_reactor, pipe, s_sub_ctrl_fn, &reactor_data);
    zloop_reader_set_tolerant (sub_reactor, pipe);
    zloop_reader (sub_reactor, sub_socket, s_sub_sample_fn, &reactor_data);
    zloop_reader_set_tolerant (sub_reactor, sub_socket);

    //  Must use low level poller for beacon, since zloop_reader does not
    //  accept a zactor_t * for its socket parameter.
    zmq_pollitem_t poll_item;
    memset(&poll_item, 0, sizeof (zmq_pollitem_t));
    poll_item.socket = zactor_resolve (sub_beacon);
    poll_item.events = ZMQ_POLLIN;
    zloop_poller (sub_reactor, &poll_item, s_sub_beacon_fn, &reactor_data);
    zloop_poller_set_tolerant (sub_reactor, &poll_item);

    zloop_start (sub_reactor);

    //  Reactor loop ended, clean up
    zloop_poller_end (sub_reactor, &poll_item);
    zloop_reader_end (sub_reactor, sub_socket);
    zloop_reader_end (sub_reactor, pipe);
    zloop_destroy (&sub_reactor);
    zactor_destroy (&sub_beacon);
    zsock_destroy (&sub_socket);
    zhash_destroy (&(reactor_data.connections));
    zhash_destroy (&(reactor_data.subscribers));
    free (reactor_data.default_partition);
    free (actor_data->default_partition);
    free (actor_data);
}


//  --------------------------------------------------------------------------
//  Create a new pubsub instance.

zpubsub_t *
zpubsub_new (int domain, const char* partition)
{
    assert (domain >= 0 && domain < 100);

    zpubsub_t *self = (zpubsub_t *) zmalloc (sizeof (zpubsub_t));
    if (!self)
        return NULL;
    self->tag = ZPUBSUB_TAG;

    self->domain = domain;
    self->partition = partition? strdup (partition): strdup ("<default>");
    assert (self->partition);

    //  Get port function
    zpubsub_port_fn *port_fn = zpubsub_port_function ();
    if (port_fn == NULL)
        port_fn = s_get_port;

    //  Bind pub socket
    self->pub_socket = zsock_new (ZMQ_PUB);
    assert (self->pub_socket);
    zsock_bind (self->pub_socket, "tcp://*:%d", (port_fn) (self->domain));

    //  Start actor for subscriptions
    sub_actor_data_t *actor_data =
        (sub_actor_data_t *) zmalloc (sizeof (sub_actor_data_t));
    assert (actor_data);
    actor_data->port = (port_fn) (self->domain);
    actor_data->default_partition = strdup (self->partition);
    actor_data->beacon_port = zpubsub_beacon_port ();
    self->sub_actor = zactor_new (s_sub_actor, actor_data);

    //  Pub beacon
    self->pub_beacon = zactor_new (zbeacon, NULL);
    zsock_send (self->pub_beacon, "si", "CONFIGURE", zpubsub_beacon_port ());
    zsock_send (self->pub_beacon, "sbi", "PUBLISH", NULL, 0, zpubsub_beacon_interval ());

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the pubsub.

void
zpubsub_destroy (zpubsub_t **self_p)
{
    assert (self_p);

    if (*self_p) {
        zpubsub_t *self = *self_p;
        assert (zpubsub_is (self));

        zsock_send (self->pub_beacon, "s", "SILENCE");
        zactor_destroy (&self->pub_beacon);
        zactor_destroy (&self->sub_actor);
        zsock_destroy (&self->pub_socket);

        free (self->partition);
        self->tag = 0xDeadBeef;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Publish a message.

void
zpubsub_publish (zpubsub_t *self, const char *topic, const char *partition,
                 byte *message, size_t size)
{
    assert (self);
    assert (topic);
    assert (message);

    zmsg_t *msg = zmsg_new ();
    assert (msg);

    zframe_t *filter = s_encode_filter (topic, partition? partition: self->partition);
    assert (filter);

    zmsg_prepend (msg, &filter);
    zmsg_addmem (msg, message, size);
    zmsg_send (&msg, self->pub_socket);
}


//  --------------------------------------------------------------------------
//  Subscribe to a topic.

void
zpubsub_subscribe (zpubsub_t *self, const char *topic, const char *partition,
                   void *args, zpubsub_sample_fn *sample_fn)
{
    assert (self);
    assert (topic);
    assert (sample_fn);

    //  Wrapping function pointers inside structs. -Werror=pedantic does not
    //  allow casting function pointers to void *

    sample_function_t *sample_function =
        (sample_function_t *) zmalloc (sizeof (sample_function_t));
    sample_function->sample_fn = sample_fn;

    zsock_send (self->sub_actor, "ssspp", "SUB", topic,
                partition? partition: self->partition, args, sample_function);
}


//  --------------------------------------------------------------------------
//  Unsubscribe from a topic.

void
zpubsub_unsubscribe (zpubsub_t *self, const char *topic, const char *partition)
{
    assert (self);
    assert (topic);

    zsock_send (self->sub_actor, "sss", "UNSUB", topic,
                partition? partition: self->partition);
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zpubsub_t.

bool
zpubsub_is (void *self)
{
    assert (self);
    return ((zpubsub_t *) self)->tag == ZPUBSUB_TAG;
}


//  Test message struct

typedef struct {
    char* hello;
    int world;
} test_msg_t;


//  --------------------------------------------------------------------------
//  Deserialize test message; simple binary serialization

static test_msg_t *
s_deserialize_message (byte *data, int size)
{
    assert (data);
    assert (size > 0);

    test_msg_t *message = (test_msg_t *) zmalloc (sizeof (test_msg_t));

    int min_size = sizeof (dbyte) + 1 + sizeof (int);
    if (size < min_size)
        return NULL;

    byte *ptr = data;
    dbyte hello_len;
    memcpy (&hello_len, ptr, sizeof (dbyte));
    assert (hello_len > 0);

    message->hello = (char *) zmalloc ((hello_len + 1) * sizeof (char));
    ptr += sizeof (dbyte);
    memcpy (message->hello, ptr, hello_len);
    message->hello [hello_len] = '\0';

    ptr += hello_len;
    memcpy (&(message->world), ptr, sizeof (int));

    return message;
}


//  --------------------------------------------------------------------------
//  Serialize test message; simple binary serialization

static byte *
s_serialize_message (test_msg_t* msg, size_t *size)
{
    assert (msg);
    assert (size);

    dbyte hello_len = (dbyte) strlen (msg->hello);
    *size = sizeof (dbyte) + hello_len + sizeof (int);

    byte *data = (byte *) zmalloc (*size);
    assert (data);

    byte *ptr = data;
    memcpy (ptr, &hello_len, sizeof (dbyte));

    ptr += sizeof (dbyte);
    memcpy (ptr, msg->hello, hello_len);

    ptr += hello_len;
    memcpy (ptr, &(msg->world), sizeof (int));

    return data;
}


//  Flag set by test sample function to indicate message received

bool s_test_msg_recvd = false;


//  --------------------------------------------------------------------------
//  Test sample function

static void
s_sample_fn (const char *topic, const char *partition, void *args,
             byte *sample, size_t size)
{
    assert (topic);
    assert (partition);
    assert (sample);

    assert (streq (topic, "TestTopic"));
    assert (streq (partition, "<default>"));

    test_msg_t *msg = s_deserialize_message (sample, size);
    assert (msg);

    assert (streq (msg->hello, "Hello"));
    assert (msg->world == 30116);
    free (msg->hello);
    free (msg);

    s_test_msg_recvd = true;
}


//  --------------------------------------------------------------------------
//  Selftest

void
zpubsub_test (bool verbose)
{
    printf (" * zpubsub: ");
    if (verbose)
        printf ("\n");


    //  s_free_subscriber unit test
    if (verbose)
        zsys_info ("s_free_subscriber...");
    sub_data_t *sub = (sub_data_t *) zmalloc (sizeof (sub_data_t));
    assert (sub);
    sub->topic = strdup ("TestTopic");
    sub->partition = strdup ("TestPartition");
    sub->key = strdup ("TestKey");
    sub->sample_function =
        (sample_function_t *) zmalloc (sizeof (sample_function_t));
    s_free_subscriber ((void **) &sub);
    assert (sub == NULL);
    if (verbose)
        zsys_info ("s_free_subscriber OK");


    //  s_ptr_to_buffer / s_buffer_to_ptr unit test
    if (verbose)
        zsys_info ("s_ptr_to_buffer / s_buffer_to_ptr...");
    int test_int = 23101;
    byte buffer2 [8];
    s_ptr_to_buffer (&test_int, buffer2, 8);
    int *int_ptr = (int *) s_buffer_to_ptr (buffer2, 8);
    assert (*int_ptr == test_int);
    if (verbose)
        zsys_info ("s_ptr_to_buffer / s_buffer_to_ptr OK");


    //  s_msg_pop_frame_to_ptr unit test
    if (verbose)
        zsys_info ("s_msg_pop_frame_to_ptr...");
    zmsg_t *msg = zmsg_new ();
    zmsg_pushmem (msg, buffer2, 8);
    int_ptr = (int *) s_msg_pop_frame_to_ptr (msg);
    assert (int_ptr);
    assert (*int_ptr == test_int);
    zmsg_destroy (&msg);
    assert (msg == NULL);
    if (verbose)
        zsys_info ("s_msg_pop_frame_to_ptr OK");


    //  s_generate_key unit test
    if (verbose)
        zsys_info ("s_generate_key...");
    char *key = s_generate_key ("TestTopic", "TestPartition");
    assert (key);
    assert (streq (key, "TestPartition:TestTopic"));
    free (key);
    if (verbose)
        zsys_info ("s_generate_key OK");


    //  s_encode_filter unit test
    if (verbose)
        zsys_info ("s_encode_filter...");
    zframe_t *filter_frame = s_encode_filter ("TestTopic", "TestPartition");
    assert (filter_frame);
    assert (zframe_data (filter_frame));
    assert (zframe_size (filter_frame) > 0);
    if (verbose)
        zsys_info ("s_encode_filter OK");


    //  s_decode_filter unit test
    if (verbose)
        zsys_info ("s_decode_filter...");
    zpubsub_filter_t *filter = s_decode_filter (&filter_frame);
    assert (filter);
    assert (streq (zpubsub_filter_partition (filter), "TestPartition"));
    assert (streq (zpubsub_filter_topic (filter), "TestTopic"));
    zpubsub_filter_destroy (&filter);
    if (verbose)
        zsys_info ("s_decode_filter OK");


    //  zpubsub system test
    if (verbose)
        zsys_info ("zpubsub system test...");

    //  Test for broadcast interface
    zactor_t *test_beacon = zactor_new (zbeacon, NULL);
    zsock_send (test_beacon, "si", "CONFIGURE", 9999);
    char *hostname = zstr_recv (test_beacon);
    if (!*hostname) {
        printf ("OK (skipping test, no UDP broadcasting)\n");
        zactor_destroy (&test_beacon);
        free (hostname);
        return;
    }
    zactor_destroy (&test_beacon);
    free (hostname);

    //  @selftest
    //  Create pubsub instance
    zpubsub_t * pubsub = zpubsub_new (25, NULL);
    assert (pubsub);
    s_test_msg_recvd = false;

    //  Subscribe to test topic
    zpubsub_subscribe (pubsub, "TestTopic", NULL, NULL, s_sample_fn);

    //  Sleep for 6 seconds; should have received a publisher
    //  beacon by then and connected
    zclock_sleep (6000);

    //  Create a test message
    test_msg_t *tmsg = (test_msg_t *) zmalloc (sizeof (test_msg_t));
    assert (tmsg);
    tmsg->hello = strdup ("Hello");
    tmsg->world = 30116;

    //  Serialize message
    size_t sample_size;
    byte *sample = s_serialize_message (tmsg, &sample_size);

    //  Publish on the test topic
    zpubsub_publish (pubsub, "TestTopic", NULL, sample, sample_size);
    zclock_sleep (1000); //  Sleeping one second after publish to make sure
                         //  subscriber processing is finished

    //  Should have received the message now
    assert (s_test_msg_recvd);
    s_test_msg_recvd = false;

    //  Publish on different partition
    zpubsub_publish (pubsub, "TestTopic", "TestPartition", sample, sample_size);
    zclock_sleep (1000);

    //  Should not receive this message
    assert (!s_test_msg_recvd);
    s_test_msg_recvd = false;

    //  Publish on another topic
    zpubsub_publish (pubsub, "TestTopic2", NULL, sample, sample_size);
    zclock_sleep (1000);

    //  Should not receive this message
    assert (!s_test_msg_recvd);
    s_test_msg_recvd = false;

    //  Publish on the test topic again
    zpubsub_publish (pubsub, "TestTopic", NULL, sample, sample_size);
    zclock_sleep (1000);

    //  Should receive this message
    assert (s_test_msg_recvd);
    s_test_msg_recvd = false;

    //  Unsubscribe and publish again
    zpubsub_unsubscribe (pubsub, "TestTopic", NULL);
    zpubsub_publish (pubsub, "TestTopic", NULL, sample, sample_size);
    zclock_sleep (1000);

    //  Should not receive this message
    assert (!s_test_msg_recvd);

    //  Destroy pubsub instance
    zpubsub_destroy (&pubsub);
    assert (pubsub == NULL);

    free (tmsg->hello);
    free (tmsg);
    free (sample);
    //  @end

    if (verbose)
        zsys_info ("zpubsub system test OK");

    printf ("OK\n");
}
