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

    The protocol is fully up to the application. Callbacks are emitted
    to perform serialization and deserialization of the filter and
    message separately. Two examples are included in the
    examples/zpubsub directory. The simple example is implemented in
    pure C, with the simplest form of (platform-dependent) binary
    serialization. The protobuf example wraps the zpubsub in a
    Participant class, which uses Google Protocol Buffers to
    serialize data in a portable manner.
@end
*/

#include "../include/czmq.h"

//  zpubsub_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZPUBSUB_TAG          0x02808508

//  Structure of our class

struct _zpubsub_t {
    uint32_t tag;                                             //  Object tag for runtime detection
    int domain;                                               //  Domain id; basis for port selection
    zpubsub_serialize_filter_fn *serialize_filter_fn;         //  Function to serialize filter data
    zpubsub_deserialize_filter_fn *deserialize_filter_fn;     //  Function to deserialize filter data
    char *partition;                                          //  Default partition to use; defaults to "<default>"
    zsock_t *pub_socket;                                      //  The publisher socket
    zactor_t *pub_beacon;                                     //  The publisher beacon
    zactor_t *sub_actor;                                      //  Actor for asynchronous subscriber management
};


//  Data for filter serialization / deserialization

typedef struct {
    zpubsub_serialize_filter_fn *serialize_fn;
    zpubsub_deserialize_filter_fn * deserialize_fn;
} filter_serialization_t;


//  Data for message serialization / deserialization

typedef struct {
    zpubsub_serialize_message_fn *serialize_fn;
    zpubsub_deserialize_message_fn * deserialize_fn;
} message_serialization_t;


//  Data for sample callback

typedef struct {
    zpubsub_sample_fn *sample_fn;
} sample_function_t;


//  Data for a subscriber

typedef struct {
    char *partition;
    char *topic;
    char *key;
    void *args;
    message_serialization_t *message_serialization;
    sample_function_t *sample_function;
} sub_data_t;


//  Data for the subscriber actor

typedef struct {
    dbyte port;
    char *default_partition;
    filter_serialization_t *filter_serialization;
    dbyte beacon_port;
} sub_actor_data_t;


//  Data for the subscriber loop handlers

typedef struct {
    zsock_t *sub_socket;
    dbyte port;
    char *default_partition;
    zhash_t *connections;
    zhash_t *subscribers;
    filter_serialization_t *filter_serialization;
} sub_reactor_data_t;


//  Version information encoded in filter to make sure incompatible
//  versions don't make life hard for each other

const int ZPUBSUB_MAGIC_NUMBER = 666;
const int ZPUBSUB_MAJOR_VERSION = 1;
const int ZPUBSUB_MINOR_VERSION = 0;


//  --------------------------------------------------------------------------
//  Simple destructor function

static void
s_free_buffer (void **buffer)
{
    assert (buffer);

    if (*buffer)
    {
        free (*buffer);
        *buffer = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Subscriber destructor function

static void
s_free_subscriber (void **subscriber)
{
    assert (subscriber);

    if (*subscriber)
    {
        sub_data_t *sub = (sub_data_t *) *subscriber;

        if (sub->partition)
            free (sub->partition);
        if (sub->topic)
            free (sub->topic);
        if (sub->key)
            free (sub->key);
        if (sub->message_serialization)
            free (sub->message_serialization);
        if (sub->sample_function)
            free (sub->sample_function);
        free (sub);

        *subscriber = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Filter destructor function

static void
s_free_filter (void **filter)
{
    assert (filter);

    if (*filter)
    {
        zpubsub_filter_t *fil = (zpubsub_filter_t *) *filter;

        if (fil->topic)
            free (fil->topic);
        if (fil->partition)
            free (fil->partition);
        free (fil);

        *filter = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Get pointer value from buffer

static void *
s_buffer_to_ptr (byte *buffer, int size)
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
s_ptr_to_buffer (void *ptr, byte *buffer, int size)
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
    void * ptr = s_buffer_to_ptr (zframe_data (ptr_frame), zframe_size (ptr_frame));
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

    int required_len = strlen (partition) + strlen (topic) + 2;
    char *key = zmalloc (required_len * sizeof (char));
    assert (key);

    snprintf (key, required_len, "%s:%s", partition, topic);
    return key;
}


//  --------------------------------------------------------------------------
//  Create filter

static zpubsub_filter_t *
s_create_filter (const char *topic, const char *partition)
{
    assert (topic);
    assert (partition);

    zpubsub_filter_t *filter = (zpubsub_filter_t *) zmalloc (sizeof (zpubsub_filter_t));
    assert (filter);

    filter->magic_number = ZPUBSUB_MAGIC_NUMBER;
    filter->major_version = ZPUBSUB_MAJOR_VERSION;
    filter->minor_version = ZPUBSUB_MINOR_VERSION;
    filter->topic = strdup (topic);
    filter->partition = strdup (partition);

    return filter;
}


//  --------------------------------------------------------------------------
//  Serialize filter

static byte *
s_serialize_filter (zpubsub_filter_t *filter, zpubsub_serialize_filter_fn *serialize_filter_fn, int *size)
{
    assert (filter);
    assert (serialize_filter_fn);
    assert (size);

    *size = (serialize_filter_fn) (filter, NULL, 0);
    byte *data = zmalloc (*size);
    assert (data);

    int rc = (serialize_filter_fn) (filter, data, *size);
    assert (rc == *size);

    return data;
}


//  --------------------------------------------------------------------------
//  Deserialize filter

static zpubsub_filter_t *
s_deserialize_filter (byte *data, int size, zpubsub_deserialize_filter_fn *deserialize_fn)
{
    assert (data);
    assert (size > 0);
    assert (deserialize_fn);

    zpubsub_filter_t *filter = NULL;
    (deserialize_fn) (data, size, &filter);

    return filter;
}


//  --------------------------------------------------------------------------
//  Serialize message

static byte *
s_serialize_message (const char *topic, const char *partition, void *message, zpubsub_serialize_message_fn *serialize_fn, int *size)
{
    assert (message);
    assert (topic);
    assert (partition);
    assert (serialize_fn);
    assert (size);

    *size = (serialize_fn) (topic, partition, message, NULL, 0);
    byte *data = zmalloc (*size);
    assert (data);

    int rc = (serialize_fn) (topic, partition, message, data, *size);
    assert (rc == *size);

    return data;
}


//  --------------------------------------------------------------------------
//  Deserialize message

void *
s_deserialize_message(const char *topic, const char *partition, void *args, byte *data, int size, zpubsub_deserialize_message_fn *deserialize_fn)
{
    assert (topic);
    assert (partition);
    assert (data);
    assert (size > 0);
    assert (deserialize_fn);

    void *message = NULL;
    (deserialize_fn) (topic, partition, args, data, size, &message);

    return message;
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

    if (address)
    {
        if (zhash_lookup (reactor_data->connections, address) == NULL)
        {
            int rc = zsock_connect(reactor_data->sub_socket, "tcp://%s:%d", address, reactor_data->port);
    
            if (rc == 0)
                zhash_insert (reactor_data->connections, address, strdup (address));
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

    if (streq(command, "$TERM"))
    {
        rc = -1;
    }
    else
    if (streq(command, "SUB"))
    {
        sub_data_t * sub = zmalloc (sizeof (sub_data_t));

        sub->topic = zmsg_popstr (msg);
        sub->partition = zmsg_popstr (msg);
        sub->key = s_generate_key (sub->topic, sub->partition);
        sub->args = s_msg_pop_frame_to_ptr (msg);
        sub->message_serialization = (message_serialization_t *) s_msg_pop_frame_to_ptr (msg);
        sub->sample_function = (sample_function_t *) s_msg_pop_frame_to_ptr (msg);

        assert (sub->topic);
        assert (sub->partition);
        assert (sub->key);
        assert (sub->message_serialization);
        assert (sub->sample_function);
        assert (sub->sample_function->sample_fn);

        zhash_update (reactor_data->subscribers, sub->key, sub);

        zpubsub_filter_t *filter = s_create_filter (sub->topic, sub->partition);
        assert (filter);

        int required_size;
        byte *filter_data = s_serialize_filter (filter, reactor_data->filter_serialization->serialize_fn, &required_size);
        assert (filter_data);

        rc = zmq_setsockopt (zsock_resolve (reactor_data->sub_socket), ZMQ_SUBSCRIBE, filter_data, required_size);
        assert (rc == 0 || zmq_errno () == ETERM);

        free (filter_data);
        s_free_filter ((void **) &filter);
    }
    else
    if (streq(command, "UNSUB"))
    {
        char *topic = zmsg_popstr (msg);
        char *partition = zmsg_popstr (msg);
        char *key = s_generate_key (topic, partition);

        assert (topic);
        assert (partition);
        assert (key);

        sub_data_t * sub = (sub_data_t *) zhash_lookup (reactor_data->subscribers, key);

        if (sub)
        {
            zpubsub_filter_t *filter = s_create_filter (topic, partition);
            assert (filter);
    
            int required_size;
            byte *filter_data = s_serialize_filter (filter, reactor_data->filter_serialization->serialize_fn, &required_size);
            assert (filter_data);
    
            rc = zmq_setsockopt (zsock_resolve (reactor_data->sub_socket), ZMQ_UNSUBSCRIBE, filter_data, required_size);
            assert (rc == 0 || zmq_errno () == ETERM);
    
            free (filter_data);
            s_free_filter ((void **) &filter);
    
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

    byte *filter_data, *message_data;
    size_t filter_size, message_size;

    int rc = zsock_recv (reader, "bb", &filter_data, &filter_size, &message_data, &message_size);
    assert (rc == 0);

    zpubsub_filter_t *filter = s_deserialize_filter (filter_data, filter_size, reactor_data->filter_serialization->deserialize_fn);
    assert (filter);
    free (filter_data);

    char *key = s_generate_key (filter->topic, filter->partition);
    assert (key);

    sub_data_t *sub = zhash_lookup (reactor_data->subscribers, key);
    free (key);

    if (sub)
    {
        void *data = s_deserialize_message(filter->topic, filter->partition, sub->args, message_data, message_size, sub->message_serialization->deserialize_fn);
        (sub->sample_function->sample_fn) (filter->topic, filter->partition, data);
    }

    free (message_data);
    s_free_filter ((void **) &filter);

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
    reactor_data.filter_serialization = zmalloc (sizeof (filter_serialization_t));
    reactor_data.filter_serialization->serialize_fn = actor_data->filter_serialization->serialize_fn;
    reactor_data.filter_serialization->deserialize_fn = actor_data->filter_serialization->deserialize_fn;
    reactor_data.connections = zhash_new();
    assert (reactor_data.connections);
    zhash_set_destructor (reactor_data.connections, s_free_buffer);
    reactor_data.subscribers = zhash_new();
    assert (reactor_data.subscribers);
    zhash_set_destructor (reactor_data.subscribers, s_free_subscriber);

    zloop_t *sub_reactor = zloop_new();
    zloop_reader (sub_reactor, pipe, s_sub_ctrl_fn, &reactor_data);
    zloop_reader_set_tolerant (sub_reactor, pipe);
    zloop_reader (sub_reactor, sub_socket, s_sub_sample_fn, &reactor_data);
    zloop_reader_set_tolerant (sub_reactor, sub_socket);

    //  Must use low level poller for beacon, since zloop_reader does not
    //  accept a zactor_t * for its socket parameter.
    zmq_pollitem_t poll_item;
    memset(&poll_item, 0, sizeof(zmq_pollitem_t));
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
    free (reactor_data.filter_serialization);
    free (actor_data->default_partition);
    free (actor_data->filter_serialization);
    free (actor_data);
}


//  --------------------------------------------------------------------------
//  Create a new pubsub instance.

zpubsub_t *
zpubsub_new (int domain, const char* partition, zpubsub_serialize_filter_fn *serialize_fn, zpubsub_deserialize_filter_fn *deserialize_fn)
{
    assert (domain >= 0 && domain < 100);
    assert (serialize_fn);
    assert (deserialize_fn);

    zpubsub_t *self = (zpubsub_t *) zmalloc (sizeof (zpubsub_t));
    if (!self)
        return NULL;
    self->tag = ZPUBSUB_TAG;

    self->domain = domain;
    self->serialize_filter_fn = serialize_fn;
    self->deserialize_filter_fn = deserialize_fn;
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
    sub_actor_data_t *actor_data = (sub_actor_data_t*) zmalloc (sizeof (sub_actor_data_t));
    assert (actor_data);
    actor_data->port = (port_fn) (self->domain);
    actor_data->default_partition = strdup (self->partition);
    actor_data->filter_serialization = zmalloc (sizeof (filter_serialization_t));
    actor_data->filter_serialization->serialize_fn = serialize_fn;
    actor_data->filter_serialization->deserialize_fn = deserialize_fn;
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

        if (self->partition)
            free (self->partition);

        self->tag = 0xDeadBeef;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Publish a message.

void
zpubsub_publish (zpubsub_t *self, char *topic, char *partition, void *message, zpubsub_serialize_message_fn *serialize_fn)
{
    assert (topic);
    assert (self);
    assert (message);
    assert (serialize_fn);

    zpubsub_filter_t *filter = s_create_filter (topic, partition? partition: self->partition);
    assert (filter);

    int required_filter_size;
    byte *filter_data = s_serialize_filter (filter, self->serialize_filter_fn, &required_filter_size);

    int required_message_size;
    byte *message_data = s_serialize_message (topic, partition? partition: self->partition, message, serialize_fn, &required_message_size);

    zmsg_t *msg = zmsg_new();
    assert (msg);

    zmsg_addmem (msg, filter_data, required_filter_size);
    zmsg_addmem (msg, message_data, required_message_size);
    zmsg_send (&msg, self->pub_socket);

    free (filter_data);
    free (message_data);
    s_free_filter ((void **) &filter);
}


//  --------------------------------------------------------------------------
//  Subscribe to a topic.

void
zpubsub_subscribe (zpubsub_t *self, char *topic, char *partition, void *args, zpubsub_deserialize_message_fn *deserialize_fn, zpubsub_sample_fn *sample_fn)
{
    assert (self);
    assert (topic);
    assert (deserialize_fn);
    assert (sample_fn);

    //  Wrapping function pointers inside structs. -Werror=pedantic does not allow
    //  casting function pointers to void *

    message_serialization_t *message_serialization = zmalloc (sizeof (message_serialization_t));
    message_serialization->deserialize_fn = deserialize_fn;

    sample_function_t *sample_function = zmalloc (sizeof (sample_function_t));
    sample_function->sample_fn = sample_fn;

    zsock_send (self->sub_actor, "sssppp", "SUB", topic, partition? partition: self->partition, args, message_serialization, sample_function);
}


//  --------------------------------------------------------------------------
//  Unsubscribe from a topic.

void
zpubsub_unsubscribe (zpubsub_t *self, char *topic, char *partition)
{
    assert (self);
    assert (topic);

    zsock_send (self->sub_actor, "sss", "UNSUB", topic, partition? partition: self->partition);
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zpubsub_t.

bool
zpubsub_is (void *self)
{
    assert (self);
    return ((zpubsub_t *) self)->tag == ZPUBSUB_TAG;
}


//  --------------------------------------------------------------------------
//  Deserialize filter test function; simple binary serialization

static int
s_deserialize_filter_fn (byte *data, int size, zpubsub_filter_t **filter)
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


//  --------------------------------------------------------------------------
//  Serialize filter test function; simple binary serialization

static int
s_serialize_filter_fn (zpubsub_filter_t *filter, byte *data, int size)
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


//  Test message struct

typedef struct {
    char* hello;
    int world;
} test_msg_t;


//  --------------------------------------------------------------------------
//  Deserialize test message; simple binary serialization

static int
s_deserialize_message_fn (const char *topic, const char *partition, void *args, byte *data, int size, void** message)
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

    test_msg_t *m = (test_msg_t *) zmalloc (sizeof (test_msg_t));
    assert (m);

    byte *ptr = data;
    dbyte hello_len;
    memcpy (&hello_len, ptr, sizeof (dbyte));
    assert (hello_len > 0);

    m->hello = (char *) zmalloc ((hello_len + 1) * sizeof (char));
    ptr += sizeof (dbyte);
    memcpy (m->hello, ptr, hello_len);
    m->hello[hello_len] = '\0';

    ptr += hello_len;
    memcpy (&(m->world), ptr, sizeof (int));

    *message = (void *) m;
    return 0;
}


//  --------------------------------------------------------------------------
//  Serialize test message; simple binary serialization

static int
s_serialize_message_fn (const char *topic, const char *partition, void* message, byte *data, int size)
{
    assert (topic);
    assert (partition);
    assert (message);

    test_msg_t *msg = (test_msg_t *) message;

    dbyte hello_len = (dbyte) strlen (msg->hello);
    int required_size = sizeof (dbyte) + hello_len + sizeof (int);

    if (size < required_size)
        return required_size;

    byte *ptr = data;
    memcpy (ptr, &hello_len, sizeof (dbyte));

    ptr += sizeof (dbyte);
    memcpy (ptr, msg->hello, hello_len);

    ptr += hello_len;
    memcpy (ptr, &(msg->world), sizeof (int));

    return required_size;
}


//  Flag set by test sample function to indicate message received

bool s_test_msg_recvd = false;


//  --------------------------------------------------------------------------
//  Test sample function

static void
s_sample_fn (const char *topic, const char *partition, void *message)
{
    assert (topic);
    assert (partition);
    assert (message);

    assert (streq (topic, "TestTopic"));
    assert (streq (partition, "<default>"));

    test_msg_t *msg = (test_msg_t *) message;
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


    //  s_free_buffer unit test
    if (verbose)
        zsys_info ("s_free_buffer...");
    byte *buffer = (byte *) zmalloc (256);
    assert (buffer);
    s_free_buffer ((void **) &buffer);
    assert (buffer == NULL);
    if (verbose)
        zsys_info ("s_free_buffer OK");


    //  s_free_subscriber unit test
    if (verbose)
        zsys_info ("s_free_subscriber...");
    sub_data_t *sub = (sub_data_t *) zmalloc (sizeof (sub_data_t));
    assert (sub);
    sub->partition = strdup ("TestPartition");
    sub->topic = strdup ("TestTopic");
    sub->key = strdup ("TestKey");
    sub->message_serialization = (message_serialization_t *) zmalloc (sizeof (message_serialization_t));
    sub->sample_function = (sample_function_t *) zmalloc (sizeof (sample_function_t));
    s_free_subscriber ((void **) &sub);
    assert (sub == NULL);
    if (verbose)
        zsys_info ("s_free_subscriber OK");


    //  s_free_filter unit test
    if (verbose)
        zsys_info ("s_free_filter...");
    zpubsub_filter_t *filter = (zpubsub_filter_t *) zmalloc (sizeof (zpubsub_filter_t));
    assert (filter);
    filter->topic = strdup ("TestTopic");
    filter->partition = strdup ("TestPartition");
    s_free_filter ((void **) &filter);
    assert (filter == NULL);
    if (verbose)
        zsys_info ("s_free_filter OK");


    //  s_ptr_to_buffer / s_buffer_to_ptr unit test
    if (verbose)
        zsys_info ("s_ptr_to_buffer / s_buffer_to_ptr...");
    int test_int = 23101;
    byte buffer2[8];
    s_ptr_to_buffer (&test_int, buffer2, 8);
    int *int_ptr = (int *) s_buffer_to_ptr (buffer2, 8);
    assert (*int_ptr == test_int);
    if (verbose)
        zsys_info ("s_ptr_to_buffer / s_buffer_to_ptr OK");


    //  s_msg_pop_frame_to_ptr unit test
    if (verbose)
        zsys_info ("s_msg_pop_frame_to_ptr...");
    zmsg_t *msg = zmsg_new();
    zmsg_pushmem (msg, buffer2, 8);
    int_ptr = (int *) s_msg_pop_frame_to_ptr (msg);
    assert (int_ptr);
    assert (*int_ptr == test_int);
    zmsg_destroy (&msg);
    assert (msg == NULL);
    if (verbose)
        zsys_info ("s_msg_pop_frame_to_ptr OK");


    //     s_generate_key unit test
    if (verbose)
        zsys_info ("s_generate_key...");
    char *key = s_generate_key ("TestTopic", "TestPartition");
    assert (key);
    assert (streq (key, "TestPartition:TestTopic"));
    free (key);
    if (verbose)
        zsys_info ("s_generate_key OK");


    //    s_create_filter unit test
    if (verbose)
        zsys_info ("s_create_filter...");
    filter = s_create_filter ("TestTopic", "TestPartition");
    assert (filter);
    assert (filter->magic_number == ZPUBSUB_MAGIC_NUMBER);
    assert (filter->major_version == ZPUBSUB_MAJOR_VERSION);
    assert (filter->minor_version == ZPUBSUB_MINOR_VERSION);
    assert (streq (filter->partition, "TestPartition"));
    assert (streq (filter->topic, "TestTopic"));
    s_free_filter((void **) &filter);
    assert (filter == NULL);
    if (verbose)
        zsys_info ("s_create_filter OK");


    //    s_serialize_filter / s_deserialize_filter unit test
    if (verbose)
        zsys_info ("s_serialize_filter / s_deserialize_filter...");
    filter = s_create_filter ("TestTopic", "TestPartition");
    assert (filter);
    int size;
    byte *filter_data = s_serialize_filter (filter, s_serialize_filter_fn, &size);
    assert (filter_data);
    assert (size > 0);
    zpubsub_filter_t *filter2 = s_deserialize_filter (filter_data, size, s_deserialize_filter_fn);
    assert (filter2);
    assert (filter->magic_number == filter2->magic_number);
    assert (filter->major_version == filter2->major_version);
    assert (filter->minor_version == filter2->minor_version);
    assert (streq (filter->partition, filter2->partition));
    assert (streq (filter->topic, filter2->topic));
    free (filter_data);
    s_free_filter((void **) &filter);
    assert (filter == NULL);
    s_free_filter((void **) &filter2);
    assert (filter2 == NULL);
    if (verbose)
        zsys_info ("s_serialize_filter / s_deserialize_filter OK");


    //    s_serialize_message / s_deserialize_message unit test
    if (verbose)
        zsys_info ("s_serialize_message / s_deserialize_message...");
    test_msg_t *tmsg = (test_msg_t *) zmalloc (sizeof (test_msg_t));
    assert (tmsg);
    tmsg->hello = strdup ("Hello");
    tmsg->world = 30116;
    byte *tmsg_data = s_serialize_message ("TestTopic", "TestPartition", tmsg, s_serialize_message_fn, &size);
    assert (tmsg_data);
    assert (size > 0);
    test_msg_t *tmsg2 = s_deserialize_message ("TestTopic", "TestPartition", NULL, tmsg_data, size, s_deserialize_message_fn);
    assert (tmsg2);
    assert (streq (tmsg->hello, tmsg2->hello));
    assert (tmsg->world == tmsg2->world);
    free (tmsg_data);
    free (tmsg->hello);
    free (tmsg);
    free (tmsg2->hello);
    free (tmsg2);
    if (verbose)
        zsys_info ("s_serialize_message / s_deserialize_message OK");


    //  zpubsub system test
    if (verbose)
        zsys_info ("zpubsub system test...");

    //  @selftest
    //  Create pubsub instance
    zpubsub_t * pubsub = zpubsub_new (25, NULL, s_serialize_filter_fn, s_deserialize_filter_fn);
    assert (pubsub);
    s_test_msg_recvd = false;

    //  Subscribe to test topic
    zpubsub_subscribe (pubsub, "TestTopic", NULL, NULL, s_deserialize_message_fn, s_sample_fn);

    //  Sleep for 6 seconds; should have received a publisher beacon by then and connected
    zclock_sleep (6000);

    //  Create a test message
    tmsg = (test_msg_t *) zmalloc (sizeof (test_msg_t));
    assert (tmsg);
    tmsg->hello = strdup ("Hello");
    tmsg->world = 30116;

    //  Publish on the test topic
    zpubsub_publish (pubsub, "TestTopic", NULL, tmsg, s_serialize_message_fn);
    zclock_sleep (1000); //  Sleeping one second after publish to make sure subscriber processing is finished

    //  Should have received the message now
    assert (s_test_msg_recvd);
    s_test_msg_recvd = false;

    //  Publish on different partition
    zpubsub_publish (pubsub, "TestTopic", "TestPartition", tmsg, s_serialize_message_fn);
    zclock_sleep (1000);

    //  Should not receive this message
    assert (!s_test_msg_recvd);
    s_test_msg_recvd = false;

    //  Publish on another topic
    zpubsub_publish (pubsub, "TestTopic2", NULL, tmsg, s_serialize_message_fn);
    zclock_sleep (1000);

    //  Should not receive this message
    assert (!s_test_msg_recvd);
    s_test_msg_recvd = false;

    //  Publish on the test topic again
    zpubsub_publish (pubsub, "TestTopic", NULL, tmsg, s_serialize_message_fn);
    zclock_sleep (1000);

    //  Should receive this message
    assert (s_test_msg_recvd);
    s_test_msg_recvd = false;

    //  Unsubscribe and publish again
    zpubsub_unsubscribe (pubsub, "TestTopic", NULL);
    zpubsub_publish (pubsub, "TestTopic", NULL, tmsg, s_serialize_message_fn);
    zclock_sleep (1000);

    //  Should not receive this message
    assert (!s_test_msg_recvd);

    //  Destroy pubsub instance
    zpubsub_destroy (&pubsub);
    assert (pubsub == NULL);

    free (tmsg->hello);
    free (tmsg);
    //  @end

    if (verbose)
        zsys_info ("zpubsub system test OK");

    printf ("OK\n");
}
