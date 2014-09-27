/*  =========================================================================
    zpubsub_filter - zpubsub protocol
    
    Codec header for zpubsub_filter.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

    * The XML model used for this code generation: zpubsub_filter.xml
    * The code generation script that built this file: zproto_codec_c
    ************************************************************************
    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef __ZPUBSUB_FILTER_H_INCLUDED__
#define __ZPUBSUB_FILTER_H_INCLUDED__

/*  These are the zpubsub_filter messages:

    FILTER - A zpubsub filter.
        magic               number 2    Magic number
        version             number 2    Version
        partition           string      Partition name
        topic               string      Topic name
*/

#define ZPUBSUB_FILTER_MAGIC_NUMBER         666
#define ZPUBSUB_FILTER_VERSION              2

#define ZPUBSUB_FILTER_FILTER               1

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zpubsub_filter_t zpubsub_filter_t;

//  @interface
//  Create a new zpubsub_filter
CZMQ_EXPORT zpubsub_filter_t *
    zpubsub_filter_new (int id);

//  Destroy the zpubsub_filter
CZMQ_EXPORT void
    zpubsub_filter_destroy (zpubsub_filter_t **self_p);

//  Parse a zpubsub_filter from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. Destroys msg and 
//  nullifies the msg reference.
CZMQ_EXPORT zpubsub_filter_t *
    zpubsub_filter_decode (zmsg_t **msg_p);

//  Encode zpubsub_filter into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.
CZMQ_EXPORT zmsg_t *
    zpubsub_filter_encode (zpubsub_filter_t **self_p);

//  Receive and parse a zpubsub_filter from the socket. Returns new object, 
//  or NULL if error. Will block if there's no message waiting.
CZMQ_EXPORT zpubsub_filter_t *
    zpubsub_filter_recv (void *input);

//  Receive and parse a zpubsub_filter from the socket. Returns new object, 
//  or NULL either if there was no input waiting, or the recv was interrupted.
CZMQ_EXPORT zpubsub_filter_t *
    zpubsub_filter_recv_nowait (void *input);

//  Send the zpubsub_filter to the output, and destroy it
CZMQ_EXPORT int
    zpubsub_filter_send (zpubsub_filter_t **self_p, void *output);

//  Send the zpubsub_filter to the output, and do not destroy it
CZMQ_EXPORT int
    zpubsub_filter_send_again (zpubsub_filter_t *self, void *output);

//  Encode the FILTER 
CZMQ_EXPORT zmsg_t *
    zpubsub_filter_encode_filter (
        const char *partition,
        const char *topic);


//  Send the FILTER to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    zpubsub_filter_send_filter (void *output,
        const char *partition,
        const char *topic);
    
//  Duplicate the zpubsub_filter message
CZMQ_EXPORT zpubsub_filter_t *
    zpubsub_filter_dup (zpubsub_filter_t *self);

//  Print contents of message to stdout
CZMQ_EXPORT void
    zpubsub_filter_print (zpubsub_filter_t *self);

//  Get/set the message routing id
CZMQ_EXPORT zframe_t *
    zpubsub_filter_routing_id (zpubsub_filter_t *self);
CZMQ_EXPORT void
    zpubsub_filter_set_routing_id (zpubsub_filter_t *self, zframe_t *routing_id);

//  Get the zpubsub_filter id and printable command
CZMQ_EXPORT int
    zpubsub_filter_id (zpubsub_filter_t *self);
CZMQ_EXPORT void
    zpubsub_filter_set_id (zpubsub_filter_t *self, int id);
CZMQ_EXPORT const char *
    zpubsub_filter_command (zpubsub_filter_t *self);

//  Get/set the partition field
CZMQ_EXPORT const char *
    zpubsub_filter_partition (zpubsub_filter_t *self);
CZMQ_EXPORT void
    zpubsub_filter_set_partition (zpubsub_filter_t *self, const char *format, ...);

//  Get/set the topic field
CZMQ_EXPORT const char *
    zpubsub_filter_topic (zpubsub_filter_t *self);
CZMQ_EXPORT void
    zpubsub_filter_set_topic (zpubsub_filter_t *self, const char *format, ...);

//  Self test of this class
CZMQ_EXPORT int
    zpubsub_filter_test (bool verbose);
//  @end

//  For backwards compatibility with old codecs
#define zpubsub_filter_dump  zpubsub_filter_print

#ifdef __cplusplus
}
#endif

#endif
