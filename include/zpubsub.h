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
//  Function to calculate port from domain id
typedef dbyte (zpubsub_port_fn) (int domain);

//  Subscribers get a callback when a sample is available
typedef void (zpubsub_sample_fn) (const char *topic, const char *partition,
                                  void *args, byte *sample, size_t size);

//  Create a new pubsub, passing functions to use for serializing and
//  deserializing filters.
CZMQ_EXPORT zpubsub_t *
    zpubsub_new (int domain, const char* partition);

//  Publish a message on a given topic.
CZMQ_EXPORT void
    zpubsub_publish (zpubsub_t *self, const char *topic, const char *partition,
                     byte *message, size_t size);

//  Subscribe to messages on a given topic.
CZMQ_EXPORT void
    zpubsub_subscribe (zpubsub_t *self,
                       const char *topic, const char *partition,
                       void *args, zpubsub_sample_fn *sample_fn);

//  Unsubscribe from messages on a given topic.
CZMQ_EXPORT void
    zpubsub_unsubscribe (zpubsub_t *self,
                         const char *topic, const char *partition);

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
