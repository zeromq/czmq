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

#ifndef __ZPUBSUB_H_INCLUDED__
#define __ZPUBSUB_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  A struct defining contents of a topic filter.
typedef struct {
    dbyte magic_number;
    dbyte major_version;
    dbyte minor_version;
    char *topic;
    char *partition;
} zpubsub_filter_t;

//  Function to calculate port from domain id
typedef dbyte (zpubsub_port_fn) (int domain);

//  Subscribers get a callback when a sample is available
typedef void (zpubsub_sample_fn) (const char *topic, const char *partition, void *sample);

//  Publishers get a callback to serialize the filter data to a byte array. If NULL is passed for data,
//  or size is too small to hold the serialized data, the function shall return the required size. Upon
//  successful serialization, the size of the serialized data shall be returned.
typedef int (zpubsub_serialize_filter_fn) (zpubsub_filter_t *filter, byte *data, int size);

//  Subscribers get a callback to deserialize filter data from a byte array. If deserialization fails, -1
//  shall be returned. Upon successful deserialization, 0 shall be returned.
typedef int (zpubsub_deserialize_filter_fn) (byte *data, int size, zpubsub_filter_t **filter);

//  Publishers get a callback to serialize the message data to a byte array. If NULL is passed for data,
//  or size is too small to hold the serialized data, the function shall return the required size. Upon
//  successful serialization, the size of the serialized data shall be returned.
typedef int (zpubsub_serialize_message_fn) (const char *topic, const char *partition, void *message, byte *data, int size);

//  Subscribers get a callback to deserialize message data from a byte array. If deserialization fails, -1
//  shall be returned. Upon successful deserialization, 0 shall be returned.
typedef int (zpubsub_deserialize_message_fn) (const char *topic, const char *partition, void *args, byte *data, int size, void **message);

//  Create a new pubsub, passing functions to use for serializing and deserializing filters.
CZMQ_EXPORT zpubsub_t *
    zpubsub_new (int domain, const char* partition, zpubsub_serialize_filter_fn *serialize_fn, zpubsub_deserialize_filter_fn *deserialize_fn);

//  Publish a message on a given topic.
CZMQ_EXPORT void
    zpubsub_publish(zpubsub_t *self, char *topic, char *partition, void *message, zpubsub_serialize_message_fn *serialize_fn);

//  Subscribe to messages on a given topic.
CZMQ_EXPORT void
    zpubsub_subscribe(zpubsub_t *self, char *topic, char *partition, void *args, zpubsub_deserialize_message_fn *deserialize_fn, zpubsub_sample_fn *sample_fn);

//  Unsubscribe from messages on a given topic.
CZMQ_EXPORT void
    zpubsub_unsubscribe(zpubsub_t *self, char *topic, char *partition);

//  Destroy a pubsub instance.
CZMQ_EXPORT void
    zpubsub_destroy (zpubsub_t **p_self);

//  Probe the supplied object, and report if it looks like a zpubsub_t.
CZMQ_EXPORT bool
	zpubsub_is (void *self);

//  Self test of this class
CZMQ_EXPORT void
    zpubsub_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
