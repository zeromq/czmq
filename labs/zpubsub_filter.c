/*  =========================================================================
    zpubsub_filter - zpubsub protocol

    Codec class for zpubsub_filter.

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

/*
@header
    zpubsub_filter - zpubsub protocol
@discuss
@end
*/

#include "../include/czmq.h"
#include "./zpubsub_filter.h"

//  Structure of our class

struct _zpubsub_filter_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  zpubsub_filter message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    uint16_t magic;                     //  Magic number
    uint16_t version;                   //  Version
    char *partition;                    //  Partition name
    char *topic;                        //  Topic name
};

//  --------------------------------------------------------------------------
//  Network data encoding macros

//  Put a block of octets to the frame
#define PUT_OCTETS(host,size) { \
    memcpy (self->needle, (host), size); \
    self->needle += size; \
}

//  Get a block of octets from the frame
#define GET_OCTETS(host,size) { \
    if (self->needle + size > self->ceiling) \
        goto malformed; \
    memcpy ((host), self->needle, size); \
    self->needle += size; \
}

//  Put a 1-byte number to the frame
#define PUT_NUMBER1(host) { \
    *(byte *) self->needle = (host); \
    self->needle++; \
}

//  Put a 2-byte number to the frame
#define PUT_NUMBER2(host) { \
    self->needle [0] = (byte) (((host) >> 8)  & 255); \
    self->needle [1] = (byte) (((host))       & 255); \
    self->needle += 2; \
}

//  Put a 4-byte number to the frame
#define PUT_NUMBER4(host) { \
    self->needle [0] = (byte) (((host) >> 24) & 255); \
    self->needle [1] = (byte) (((host) >> 16) & 255); \
    self->needle [2] = (byte) (((host) >> 8)  & 255); \
    self->needle [3] = (byte) (((host))       & 255); \
    self->needle += 4; \
}

//  Put a 8-byte number to the frame
#define PUT_NUMBER8(host) { \
    self->needle [0] = (byte) (((host) >> 56) & 255); \
    self->needle [1] = (byte) (((host) >> 48) & 255); \
    self->needle [2] = (byte) (((host) >> 40) & 255); \
    self->needle [3] = (byte) (((host) >> 32) & 255); \
    self->needle [4] = (byte) (((host) >> 24) & 255); \
    self->needle [5] = (byte) (((host) >> 16) & 255); \
    self->needle [6] = (byte) (((host) >> 8)  & 255); \
    self->needle [7] = (byte) (((host))       & 255); \
    self->needle += 8; \
}

//  Get a 1-byte number from the frame
#define GET_NUMBER1(host) { \
    if (self->needle + 1 > self->ceiling) \
        goto malformed; \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) \
        goto malformed; \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) \
        goto malformed; \
    (host) = ((uint32_t) (self->needle [0]) << 24) \
           + ((uint32_t) (self->needle [1]) << 16) \
           + ((uint32_t) (self->needle [2]) << 8) \
           +  (uint32_t) (self->needle [3]); \
    self->needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (self->needle + 8 > self->ceiling) \
        goto malformed; \
    (host) = ((uint64_t) (self->needle [0]) << 56) \
           + ((uint64_t) (self->needle [1]) << 48) \
           + ((uint64_t) (self->needle [2]) << 40) \
           + ((uint64_t) (self->needle [3]) << 32) \
           + ((uint64_t) (self->needle [4]) << 24) \
           + ((uint64_t) (self->needle [5]) << 16) \
           + ((uint64_t) (self->needle [6]) << 8) \
           +  (uint64_t) (self->needle [7]); \
    self->needle += 8; \
}

//  Put a string to the frame
#define PUT_STRING(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER1 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a string from the frame
#define GET_STRING(host) { \
    size_t string_size; \
    GET_NUMBER1 (string_size); \
    if (self->needle + string_size > (self->ceiling)) \
        goto malformed; \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}

//  Put a long string to the frame
#define PUT_LONGSTR(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER4 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a long string from the frame
#define GET_LONGSTR(host) { \
    size_t string_size; \
    GET_NUMBER4 (string_size); \
    if (self->needle + string_size > (self->ceiling)) \
        goto malformed; \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}


//  --------------------------------------------------------------------------
//  Create a new zpubsub_filter

zpubsub_filter_t *
zpubsub_filter_new (int id)
{
    zpubsub_filter_t *self = (zpubsub_filter_t *) zmalloc (sizeof (zpubsub_filter_t));
    self->id = id;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zpubsub_filter

void
zpubsub_filter_destroy (zpubsub_filter_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zpubsub_filter_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        free (self->partition);
        free (self->topic);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Parse a zpubsub_filter from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. Destroys msg and 
//  nullifies the msg reference.

zpubsub_filter_t *
zpubsub_filter_decode (zmsg_t **msg_p)
{
    assert (msg_p);
    zmsg_t *msg = *msg_p;
    if (msg == NULL)
        return NULL;
        
    zpubsub_filter_t *self = zpubsub_filter_new (0);
    //  Read and parse command in frame
    zframe_t *frame = zmsg_pop (msg);
    if (!frame) 
        goto empty;             //  Malformed or empty

    //  Get and check protocol signature
    self->needle = zframe_data (frame);
    self->ceiling = self->needle + zframe_size (frame);
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 7))
        goto empty;             //  Invalid signature

    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case ZPUBSUB_FILTER_FILTER:
            GET_NUMBER2 (self->magic);
            if (self->magic != ZPUBSUB_FILTER_MAGIC_NUMBER)
                goto malformed;
            GET_NUMBER2 (self->version);
            if (self->version != ZPUBSUB_FILTER_VERSION)
                goto malformed;
            GET_STRING (self->partition);
            GET_STRING (self->topic);
            break;

        default:
            goto malformed;
    }
    //  Successful return
    zframe_destroy (&frame);
    zmsg_destroy (msg_p);
    return self;

    //  Error returns
    malformed:
        zsys_error ("malformed message '%d'\n", self->id);
    empty:
        zframe_destroy (&frame);
        zmsg_destroy (msg_p);
        zpubsub_filter_destroy (&self);
        return (NULL);
}


//  --------------------------------------------------------------------------
//  Encode zpubsub_filter into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.

zmsg_t *
zpubsub_filter_encode (zpubsub_filter_t **self_p)
{
    assert (self_p);
    assert (*self_p);
    
    zpubsub_filter_t *self = *self_p;
    zmsg_t *msg = zmsg_new ();

    size_t frame_size = 2 + 1;          //  Signature and message ID
    switch (self->id) {
        case ZPUBSUB_FILTER_FILTER:
            //  magic is a 2-byte integer
            frame_size += 2;
            //  version is a 2-byte integer
            frame_size += 2;
            //  partition is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->partition)
                frame_size += strlen (self->partition);
            //  topic is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->topic)
                frame_size += strlen (self->topic);
            break;
            
        default:
            zsys_error ("bad message type '%d', not sent\n", self->id);
            //  No recovery, this is a fatal application error
            assert (false);
    }
    //  Now serialize message into the frame
    zframe_t *frame = zframe_new (NULL, frame_size);
    self->needle = zframe_data (frame);
    PUT_NUMBER2 (0xAAA0 | 7);
    PUT_NUMBER1 (self->id);

    switch (self->id) {
        case ZPUBSUB_FILTER_FILTER:
            PUT_NUMBER2 (ZPUBSUB_FILTER_MAGIC_NUMBER);
            PUT_NUMBER2 (ZPUBSUB_FILTER_VERSION);
            if (self->partition) {
                PUT_STRING (self->partition);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->topic) {
                PUT_STRING (self->topic);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            break;

    }
    //  Now send the data frame
    if (zmsg_append (msg, &frame)) {
        zmsg_destroy (&msg);
        zpubsub_filter_destroy (self_p);
        return NULL;
    }
    //  Destroy zpubsub_filter object
    zpubsub_filter_destroy (self_p);
    return msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a zpubsub_filter from the socket. Returns new object or
//  NULL if error. Will block if there's no message waiting.

zpubsub_filter_t *
zpubsub_filter_recv (void *input)
{
    assert (input);
    zmsg_t *msg = zmsg_recv (input);
    if (!msg)
        return NULL;            //  Interrupted
    //  If message came from a router socket, first frame is routing_id
    zframe_t *routing_id = NULL;
    if (zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER) {
        routing_id = zmsg_pop (msg);
        //  If message was not valid, forget about it
        if (!routing_id || !zmsg_next (msg))
            return NULL;        //  Malformed or empty
    }
    zpubsub_filter_t *zpubsub_filter = zpubsub_filter_decode (&msg);
    if (zpubsub_filter && zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER)
        zpubsub_filter->routing_id = routing_id;

    return zpubsub_filter;
}


//  --------------------------------------------------------------------------
//  Receive and parse a zpubsub_filter from the socket. Returns new object,
//  or NULL either if there was no input waiting, or the recv was interrupted.

zpubsub_filter_t *
zpubsub_filter_recv_nowait (void *input)
{
    assert (input);
    zmsg_t *msg = zmsg_recv_nowait (input);
    if (!msg)
        return NULL;            //  Interrupted
    //  If message came from a router socket, first frame is routing_id
    zframe_t *routing_id = NULL;
    if (zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER) {
        routing_id = zmsg_pop (msg);
        //  If message was not valid, forget about it
        if (!routing_id || !zmsg_next (msg))
            return NULL;        //  Malformed or empty
    }
    zpubsub_filter_t *zpubsub_filter = zpubsub_filter_decode (&msg);
    if (zpubsub_filter && zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER)
        zpubsub_filter->routing_id = routing_id;

    return zpubsub_filter;
}


//  --------------------------------------------------------------------------
//  Send the zpubsub_filter to the socket, and destroy it
//  Returns 0 if OK, else -1

int
zpubsub_filter_send (zpubsub_filter_t **self_p, void *output)
{
    assert (self_p);
    assert (*self_p);
    assert (output);

    //  Save routing_id if any, as encode will destroy it
    zpubsub_filter_t *self = *self_p;
    zframe_t *routing_id = self->routing_id;
    self->routing_id = NULL;

    //  Encode zpubsub_filter message to a single zmsg
    zmsg_t *msg = zpubsub_filter_encode (self_p);
    
    //  If we're sending to a ROUTER, send the routing_id first
    if (zsocket_type (zsock_resolve (output)) == ZMQ_ROUTER) {
        assert (routing_id);
        zmsg_prepend (msg, &routing_id);
    }
    else
        zframe_destroy (&routing_id);
        
    if (msg && zmsg_send (&msg, output) == 0)
        return 0;
    else
        return -1;              //  Failed to encode, or send
}


//  --------------------------------------------------------------------------
//  Send the zpubsub_filter to the output, and do not destroy it

int
zpubsub_filter_send_again (zpubsub_filter_t *self, void *output)
{
    assert (self);
    assert (output);
    self = zpubsub_filter_dup (self);
    return zpubsub_filter_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Encode FILTER message

zmsg_t * 
zpubsub_filter_encode_filter (
    const char *partition,
    const char *topic)
{
    zpubsub_filter_t *self = zpubsub_filter_new (ZPUBSUB_FILTER_FILTER);
    zpubsub_filter_set_partition (self, partition);
    zpubsub_filter_set_topic (self, topic);
    return zpubsub_filter_encode (&self);
}


//  --------------------------------------------------------------------------
//  Send the FILTER to the socket in one step

int
zpubsub_filter_send_filter (
    void *output,
    const char *partition,
    const char *topic)
{
    zpubsub_filter_t *self = zpubsub_filter_new (ZPUBSUB_FILTER_FILTER);
    zpubsub_filter_set_partition (self, partition);
    zpubsub_filter_set_topic (self, topic);
    return zpubsub_filter_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Duplicate the zpubsub_filter message

zpubsub_filter_t *
zpubsub_filter_dup (zpubsub_filter_t *self)
{
    if (!self)
        return NULL;
        
    zpubsub_filter_t *copy = zpubsub_filter_new (self->id);
    if (self->routing_id)
        copy->routing_id = zframe_dup (self->routing_id);
    switch (self->id) {
        case ZPUBSUB_FILTER_FILTER:
            copy->magic = self->magic;
            copy->version = self->version;
            copy->partition = self->partition? strdup (self->partition): NULL;
            copy->topic = self->topic? strdup (self->topic): NULL;
            break;

    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
zpubsub_filter_print (zpubsub_filter_t *self)
{
    assert (self);
    switch (self->id) {
        case ZPUBSUB_FILTER_FILTER:
            zsys_debug ("ZPUBSUB_FILTER_FILTER:");
            zsys_debug ("    magic=zpubsub_filter_magic_number");
            zsys_debug ("    version=zpubsub_filter_version");
            if (self->partition)
                zsys_debug ("    partition='%s'", self->partition);
            else
                zsys_debug ("    partition=");
            if (self->topic)
                zsys_debug ("    topic='%s'", self->topic);
            else
                zsys_debug ("    topic=");
            break;
            
    }
}


//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
zpubsub_filter_routing_id (zpubsub_filter_t *self)
{
    assert (self);
    return self->routing_id;
}

void
zpubsub_filter_set_routing_id (zpubsub_filter_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the zpubsub_filter id

int
zpubsub_filter_id (zpubsub_filter_t *self)
{
    assert (self);
    return self->id;
}

void
zpubsub_filter_set_id (zpubsub_filter_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
zpubsub_filter_command (zpubsub_filter_t *self)
{
    assert (self);
    switch (self->id) {
        case ZPUBSUB_FILTER_FILTER:
            return ("FILTER");
            break;
    }
    return "?";
}

//  --------------------------------------------------------------------------
//  Get/set the partition field

const char *
zpubsub_filter_partition (zpubsub_filter_t *self)
{
    assert (self);
    return self->partition;
}

void
zpubsub_filter_set_partition (zpubsub_filter_t *self, const char *format, ...)
{
    //  Format partition from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->partition);
    self->partition = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the topic field

const char *
zpubsub_filter_topic (zpubsub_filter_t *self)
{
    assert (self);
    return self->topic;
}

void
zpubsub_filter_set_topic (zpubsub_filter_t *self, const char *format, ...)
{
    //  Format topic from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->topic);
    self->topic = zsys_vprintf (format, argptr);
    va_end (argptr);
}



//  --------------------------------------------------------------------------
//  Selftest

int
zpubsub_filter_test (bool verbose)
{
    printf (" * zpubsub_filter: ");

    //  @selftest
    //  Simple create/destroy test
    zpubsub_filter_t *self = zpubsub_filter_new (0);
    assert (self);
    zpubsub_filter_destroy (&self);

    //  Create pair of sockets we can send through
    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    zsock_connect (input, "inproc://selftest-zpubsub_filter");

    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    zsock_bind (output, "inproc://selftest-zpubsub_filter");

    //  Encode/send/decode and verify each message type
    int instance;
    zpubsub_filter_t *copy;
    self = zpubsub_filter_new (ZPUBSUB_FILTER_FILTER);
    
    //  Check that _dup works on empty message
    copy = zpubsub_filter_dup (self);
    assert (copy);
    zpubsub_filter_destroy (&copy);

    zpubsub_filter_set_partition (self, "Life is short but Now lasts for ever");
    zpubsub_filter_set_topic (self, "Life is short but Now lasts for ever");
    //  Send twice from same object
    zpubsub_filter_send_again (self, output);
    zpubsub_filter_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = zpubsub_filter_recv (input);
        assert (self);
        assert (zpubsub_filter_routing_id (self));
        
        assert (streq (zpubsub_filter_partition (self), "Life is short but Now lasts for ever"));
        assert (streq (zpubsub_filter_topic (self), "Life is short but Now lasts for ever"));
        zpubsub_filter_destroy (&self);
    }

    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
    return 0;
}
