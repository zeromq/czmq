/*  =========================================================================
    zgossip_msg - ZeroMQ Gossip Protocol

    Codec class for zgossip_msg.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

    * The XML model used for this code generation: zgossip_msg.xml
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
    zgossip_msg - ZeroMQ Gossip Protocol
@discuss
@end
*/

#include "../include/czmq.h"
#include "./zgossip_msg.h"

//  Structure of our class

struct _zgossip_msg_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  zgossip_msg message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    byte version;                       //  Version = 1
    char *key;                          //  Tuple key, globally unique
    char *value;                        //  Tuple value, as printable string
    uint32_t ttl;                       //  Time to live, msecs
};

//  --------------------------------------------------------------------------
//  Network data encoding macros

//  Put a block of octets to the frame
#define PUT_OCTETS(host, size) { \
        memcpy (self->needle, (host), size); \
        self->needle += size; \
}

//  Get a block of octets from the frame
#define GET_OCTETS(host, size) { \
        if (self->needle + size > self->ceiling) \
            goto malformed; \
        memcpy ((host), self->needle, size); \
        self->needle += size; \
}

//  Put a 1-byte number to the frame
#define PUT_NUMBER1(host) { \
        *(byte *) self->needle = (host); \
        self->needle ++; \
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
        self->needle ++; \
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
//  Create a new zgossip_msg

zgossip_msg_t *
zgossip_msg_new (int id)
{
    zgossip_msg_t *self = (zgossip_msg_t *) zmalloc (sizeof (zgossip_msg_t));
    self->id = id;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zgossip_msg

void
zgossip_msg_destroy (zgossip_msg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zgossip_msg_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        free (self->key);
        free (self->value);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Parse a zgossip_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. Destroys msg and
//  nullifies the msg reference.

zgossip_msg_t *
zgossip_msg_decode (zmsg_t **msg_p)
{
    assert (msg_p);
    zmsg_t *msg = *msg_p;
    if (msg == NULL)
        return NULL;

    zgossip_msg_t *self = zgossip_msg_new (0);
    //  Read and parse command in frame
    zframe_t *frame = zmsg_pop (msg);
    if (!frame)
        goto empty;             //  Malformed or empty

    //  Get and check protocol signature
    self->needle = zframe_data (frame);
    self->ceiling = self->needle + zframe_size (frame);
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 0))
        goto empty;             //  Invalid signature

    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            GET_NUMBER1 (self->version);
            if (self->version != 1)
                goto malformed;
            break;

        case ZGOSSIP_MSG_PUBLISH:
            GET_NUMBER1 (self->version);
            if (self->version != 1)
                goto malformed;
            GET_STRING (self->key);
            GET_LONGSTR (self->value);
            GET_NUMBER4 (self->ttl);
            break;

        case ZGOSSIP_MSG_PING:
            GET_NUMBER1 (self->version);
            if (self->version != 1)
                goto malformed;
            break;

        case ZGOSSIP_MSG_PONG:
            GET_NUMBER1 (self->version);
            if (self->version != 1)
                goto malformed;
            break;

        case ZGOSSIP_MSG_INVALID:
            GET_NUMBER1 (self->version);
            if (self->version != 1)
                goto malformed;
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
    zgossip_msg_destroy (&self);
    return (NULL);
}


//  --------------------------------------------------------------------------
//  Encode zgossip_msg into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.

zmsg_t *
zgossip_msg_encode (zgossip_msg_t **self_p)
{
    assert (self_p);
    assert (*self_p);

    zgossip_msg_t *self = *self_p;
    zmsg_t *msg = zmsg_new ();

    size_t frame_size = 2 + 1;          //  Signature and message ID
    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            //  version is a 1-byte integer
            frame_size += 1;
            break;

        case ZGOSSIP_MSG_PUBLISH:
            //  version is a 1-byte integer
            frame_size += 1;
            //  key is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->key)
                frame_size += strlen (self->key);
            //  value is a string with 4-byte length
            frame_size += 4;
            if (self->value)
                frame_size += strlen (self->value);
            //  ttl is a 4-byte integer
            frame_size += 4;
            break;

        case ZGOSSIP_MSG_PING:
            //  version is a 1-byte integer
            frame_size += 1;
            break;

        case ZGOSSIP_MSG_PONG:
            //  version is a 1-byte integer
            frame_size += 1;
            break;

        case ZGOSSIP_MSG_INVALID:
            //  version is a 1-byte integer
            frame_size += 1;
            break;

        default:
            zsys_error ("bad message type '%d', not sent\n", self->id);
            //  No recovery, this is a fatal application error
            assert (false);
    }
    //  Now serialize message into the frame
    zframe_t *frame = zframe_new (NULL, frame_size);
    self->needle = zframe_data (frame);
    PUT_NUMBER2 (0xAAA0 | 0);
    PUT_NUMBER1 (self->id);

    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            PUT_NUMBER1 (1);
            break;

        case ZGOSSIP_MSG_PUBLISH:
            PUT_NUMBER1 (1);
            if (self->key)
                PUT_STRING (self->key);
            else
                PUT_NUMBER1 (0);    //  Empty string
            if (self->value)
                PUT_LONGSTR (self->value);
            else
                PUT_NUMBER4 (0);    //  Empty string
            PUT_NUMBER4 (self->ttl);
            break;

        case ZGOSSIP_MSG_PING:
            PUT_NUMBER1 (1);
            break;

        case ZGOSSIP_MSG_PONG:
            PUT_NUMBER1 (1);
            break;

        case ZGOSSIP_MSG_INVALID:
            PUT_NUMBER1 (1);
            break;

    }
    //  Now send the data frame
    if (zmsg_append (msg, &frame)) {
        zmsg_destroy (&msg);
        zgossip_msg_destroy (self_p);
        return NULL;
    }
    //  Destroy zgossip_msg object
    zgossip_msg_destroy (self_p);
    return msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a zgossip_msg from the socket. Returns new object or
//  NULL if error. Will block if there's no message waiting.

zgossip_msg_t *
zgossip_msg_recv (void *input)
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
    zgossip_msg_t *zgossip_msg = zgossip_msg_decode (&msg);
    if (zgossip_msg && zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER)
        zgossip_msg->routing_id = routing_id;

    return zgossip_msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a zgossip_msg from the socket. Returns new object,
//  or NULL either if there was no input waiting, or the recv was interrupted.

zgossip_msg_t *
zgossip_msg_recv_nowait (void *input)
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
    zgossip_msg_t *zgossip_msg = zgossip_msg_decode (&msg);
    if (zgossip_msg && zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER)
        zgossip_msg->routing_id = routing_id;

    return zgossip_msg;
}


//  --------------------------------------------------------------------------
//  Send the zgossip_msg to the socket, and destroy it
//  Returns 0 if OK, else -1

int
zgossip_msg_send (zgossip_msg_t **self_p, void *output)
{
    assert (self_p);
    assert (*self_p);
    assert (output);

    //  Save routing_id if any, as encode will destroy it
    zgossip_msg_t *self = *self_p;
    zframe_t *routing_id = self->routing_id;
    self->routing_id = NULL;

    //  Encode zgossip_msg message to a single zmsg
    zmsg_t *msg = zgossip_msg_encode (self_p);

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
//  Send the zgossip_msg to the output, and do not destroy it

int
zgossip_msg_send_again (zgossip_msg_t *self, void *output)
{
    assert (self);
    assert (output);
    self = zgossip_msg_dup (self);
    return zgossip_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Encode HELLO message

zmsg_t *
zgossip_msg_encode_hello (
    )
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_HELLO);
    return zgossip_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode PUBLISH message

zmsg_t *
zgossip_msg_encode_publish (
    const char *key,
    const char *value,
    uint32_t ttl)
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_PUBLISH);
    zgossip_msg_set_key (self, key);
    zgossip_msg_set_value (self, value);
    zgossip_msg_set_ttl (self, ttl);
    return zgossip_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode PING message

zmsg_t *
zgossip_msg_encode_ping (
    )
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_PING);
    return zgossip_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode PONG message

zmsg_t *
zgossip_msg_encode_pong (
    )
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_PONG);
    return zgossip_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Encode INVALID message

zmsg_t *
zgossip_msg_encode_invalid (
    )
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_INVALID);
    return zgossip_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Send the HELLO to the socket in one step

int
zgossip_msg_send_hello (
    void *output)
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_HELLO);
    return zgossip_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the PUBLISH to the socket in one step

int
zgossip_msg_send_publish (
    void *output,
    const char *key,
    const char *value,
    uint32_t ttl)
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_PUBLISH);
    zgossip_msg_set_key (self, key);
    zgossip_msg_set_value (self, value);
    zgossip_msg_set_ttl (self, ttl);
    return zgossip_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the PING to the socket in one step

int
zgossip_msg_send_ping (
    void *output)
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_PING);
    return zgossip_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the PONG to the socket in one step

int
zgossip_msg_send_pong (
    void *output)
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_PONG);
    return zgossip_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Send the INVALID to the socket in one step

int
zgossip_msg_send_invalid (
    void *output)
{
    zgossip_msg_t *self = zgossip_msg_new (ZGOSSIP_MSG_INVALID);
    return zgossip_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Duplicate the zgossip_msg message

zgossip_msg_t *
zgossip_msg_dup (zgossip_msg_t *self)
{
    if (!self)
        return NULL;

    zgossip_msg_t *copy = zgossip_msg_new (self->id);
    if (self->routing_id)
        copy->routing_id = zframe_dup (self->routing_id);
    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            copy->version = self->version;
            break;

        case ZGOSSIP_MSG_PUBLISH:
            copy->version = self->version;
            copy->key = self->key ? strdup (self->key) : NULL;
            copy->value = self->value ? strdup (self->value) : NULL;
            copy->ttl = self->ttl;
            break;

        case ZGOSSIP_MSG_PING:
            copy->version = self->version;
            break;

        case ZGOSSIP_MSG_PONG:
            copy->version = self->version;
            break;

        case ZGOSSIP_MSG_INVALID:
            copy->version = self->version;
            break;

    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
zgossip_msg_print (zgossip_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            zsys_debug ("ZGOSSIP_MSG_HELLO:");
            zsys_debug ("    version=1");
            break;

        case ZGOSSIP_MSG_PUBLISH:
            zsys_debug ("ZGOSSIP_MSG_PUBLISH:");
            zsys_debug ("    version=1");
            if (self->key)
                zsys_debug ("    key='%s'", self->key);
            else
                zsys_debug ("    key=");
            if (self->value)
                zsys_debug ("    value='%s'", self->value);
            else
                zsys_debug ("    value=");
            zsys_debug ("    ttl=%ld", (long) self->ttl);
            break;

        case ZGOSSIP_MSG_PING:
            zsys_debug ("ZGOSSIP_MSG_PING:");
            zsys_debug ("    version=1");
            break;

        case ZGOSSIP_MSG_PONG:
            zsys_debug ("ZGOSSIP_MSG_PONG:");
            zsys_debug ("    version=1");
            break;

        case ZGOSSIP_MSG_INVALID:
            zsys_debug ("ZGOSSIP_MSG_INVALID:");
            zsys_debug ("    version=1");
            break;

    }
}


//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
zgossip_msg_routing_id (zgossip_msg_t *self)
{
    assert (self);
    return self->routing_id;
}

void
zgossip_msg_set_routing_id (zgossip_msg_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the zgossip_msg id

int
zgossip_msg_id (zgossip_msg_t *self)
{
    assert (self);
    return self->id;
}

void
zgossip_msg_set_id (zgossip_msg_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
zgossip_msg_command (zgossip_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            return ("HELLO");
            break;
        case ZGOSSIP_MSG_PUBLISH:
            return ("PUBLISH");
            break;
        case ZGOSSIP_MSG_PING:
            return ("PING");
            break;
        case ZGOSSIP_MSG_PONG:
            return ("PONG");
            break;
        case ZGOSSIP_MSG_INVALID:
            return ("INVALID");
            break;
    }
    return "?";
}

//  --------------------------------------------------------------------------
//  Get/set the key field

const char *
zgossip_msg_key (zgossip_msg_t *self)
{
    assert (self);
    return self->key;
}

void
zgossip_msg_set_key (zgossip_msg_t *self, const char *format, ...)
{
    //  Format key from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->key);
    self->key = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the value field

const char *
zgossip_msg_value (zgossip_msg_t *self)
{
    assert (self);
    return self->value;
}

void
zgossip_msg_set_value (zgossip_msg_t *self, const char *format, ...)
{
    //  Format value from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->value);
    self->value = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the ttl field

uint32_t
zgossip_msg_ttl (zgossip_msg_t *self)
{
    assert (self);
    return self->ttl;
}

void
zgossip_msg_set_ttl (zgossip_msg_t *self, uint32_t ttl)
{
    assert (self);
    self->ttl = ttl;
}



//  --------------------------------------------------------------------------
//  Selftest

int
zgossip_msg_test (bool verbose)
{
    printf (" * zgossip_msg: ");

    //  @selftest
    //  Simple create/destroy test
    zgossip_msg_t *self = zgossip_msg_new (0);
    assert (self);
    zgossip_msg_destroy (&self);

    //  Create pair of sockets we can send through
    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    zsock_connect (input, "inproc://selftest-zgossip_msg");

    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    zsock_bind (output, "inproc://selftest-zgossip_msg");

    //  Encode/send/decode and verify each message type
    int instance;
    zgossip_msg_t *copy;
    self = zgossip_msg_new (ZGOSSIP_MSG_HELLO);

    //  Check that _dup works on empty message
    copy = zgossip_msg_dup (self);
    assert (copy);
    zgossip_msg_destroy (&copy);

    //  Send twice from same object
    zgossip_msg_send_again (self, output);
    zgossip_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = zgossip_msg_recv (input);
        assert (self);
        assert (zgossip_msg_routing_id (self));

        zgossip_msg_destroy (&self);
    }
    self = zgossip_msg_new (ZGOSSIP_MSG_PUBLISH);

    //  Check that _dup works on empty message
    copy = zgossip_msg_dup (self);
    assert (copy);
    zgossip_msg_destroy (&copy);

    zgossip_msg_set_key (self, "Life is short but Now lasts for ever");
    zgossip_msg_set_value (self, "Life is short but Now lasts for ever");
    zgossip_msg_set_ttl (self, 123);
    //  Send twice from same object
    zgossip_msg_send_again (self, output);
    zgossip_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = zgossip_msg_recv (input);
        assert (self);
        assert (zgossip_msg_routing_id (self));

        assert (streq (zgossip_msg_key (self), "Life is short but Now lasts for ever"));
        assert (streq (zgossip_msg_value (self), "Life is short but Now lasts for ever"));
        assert (zgossip_msg_ttl (self) == 123);
        zgossip_msg_destroy (&self);
    }
    self = zgossip_msg_new (ZGOSSIP_MSG_PING);

    //  Check that _dup works on empty message
    copy = zgossip_msg_dup (self);
    assert (copy);
    zgossip_msg_destroy (&copy);

    //  Send twice from same object
    zgossip_msg_send_again (self, output);
    zgossip_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = zgossip_msg_recv (input);
        assert (self);
        assert (zgossip_msg_routing_id (self));

        zgossip_msg_destroy (&self);
    }
    self = zgossip_msg_new (ZGOSSIP_MSG_PONG);

    //  Check that _dup works on empty message
    copy = zgossip_msg_dup (self);
    assert (copy);
    zgossip_msg_destroy (&copy);

    //  Send twice from same object
    zgossip_msg_send_again (self, output);
    zgossip_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = zgossip_msg_recv (input);
        assert (self);
        assert (zgossip_msg_routing_id (self));

        zgossip_msg_destroy (&self);
    }
    self = zgossip_msg_new (ZGOSSIP_MSG_INVALID);

    //  Check that _dup works on empty message
    copy = zgossip_msg_dup (self);
    assert (copy);
    zgossip_msg_destroy (&copy);

    //  Send twice from same object
    zgossip_msg_send_again (self, output);
    zgossip_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = zgossip_msg_recv (input);
        assert (self);
        assert (zgossip_msg_routing_id (self));

        zgossip_msg_destroy (&self);
    }

    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
    return 0;
}
