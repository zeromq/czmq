/*  =========================================================================
    zgossip_msg - ZeroMQ Gossip Protocol

    Codec class for zgossip_msg.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zgossip_msg.xml, or
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
    char key [256];                     //  Tuple key, globally unique
    char *value;                        //  Tuple value, as printable string
    uint32_t ttl;                       //  Time to live, msecs
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
    if (self->needle + size > self->ceiling) { \
        zsys_warning ("zgossip_msg: GET_OCTETS failed"); \
        goto malformed; \
    } \
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
    if (self->needle + 1 > self->ceiling) { \
        zsys_warning ("zgossip_msg: GET_NUMBER1 failed"); \
        goto malformed; \
    } \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) { \
        zsys_warning ("zgossip_msg: GET_NUMBER2 failed"); \
        goto malformed; \
    } \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) { \
        zsys_warning ("zgossip_msg: GET_NUMBER4 failed"); \
        goto malformed; \
    } \
    (host) = ((uint32_t) (self->needle [0]) << 24) \
           + ((uint32_t) (self->needle [1]) << 16) \
           + ((uint32_t) (self->needle [2]) << 8) \
           +  (uint32_t) (self->needle [3]); \
    self->needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (self->needle + 8 > self->ceiling) { \
        zsys_warning ("zgossip_msg: GET_NUMBER8 failed"); \
        goto malformed; \
    } \
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
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("zgossip_msg: GET_STRING failed"); \
        goto malformed; \
    } \
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
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("zgossip_msg: GET_LONGSTR failed"); \
        goto malformed; \
    } \
    free ((host)); \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}


//  --------------------------------------------------------------------------
//  Create a new zgossip_msg

zgossip_msg_t *
zgossip_msg_new (void)
{
    zgossip_msg_t *self = (zgossip_msg_t *) zmalloc (sizeof (zgossip_msg_t));
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
        free (self->value);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive a zgossip_msg from the socket. Returns 0 if OK, -1 if
//  there was an error. Blocks if there is no message waiting.

int
zgossip_msg_recv (zgossip_msg_t *self, zsock_t *input)
{
    assert (input);
    
    if (zsock_type (input) == ZMQ_ROUTER) {
        zframe_destroy (&self->routing_id);
        self->routing_id = zframe_recv (input);
        if (!self->routing_id || !zsock_rcvmore (input)) {
            zsys_warning ("zgossip_msg: no routing ID");
            return -1;          //  Interrupted or malformed
        }
    }
    zmq_msg_t frame;
    zmq_msg_init (&frame);
    int size = zmq_msg_recv (&frame, zsock_resolve (input), 0);
    if (size == -1) {
        zsys_warning ("zgossip_msg: interrupted");
        goto malformed;         //  Interrupted
    }
    //  Get and check protocol signature
    self->needle = (byte *) zmq_msg_data (&frame);
    self->ceiling = self->needle + zmq_msg_size (&frame);
    
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 0)) {
        zsys_warning ("zgossip_msg: invalid signature");
        //  TODO: discard invalid messages and loop, and return
        //  -1 only on interrupt
        goto malformed;         //  Interrupted
    }
    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            {
                byte version;
                GET_NUMBER1 (version);
                if (version != 1) {
                    zsys_warning ("zgossip_msg: version is invalid");
                    goto malformed;
                }
            }
            break;

        case ZGOSSIP_MSG_PUBLISH:
            {
                byte version;
                GET_NUMBER1 (version);
                if (version != 1) {
                    zsys_warning ("zgossip_msg: version is invalid");
                    goto malformed;
                }
            }
            GET_STRING (self->key);
            GET_LONGSTR (self->value);
            GET_NUMBER4 (self->ttl);
            break;

        case ZGOSSIP_MSG_PING:
            {
                byte version;
                GET_NUMBER1 (version);
                if (version != 1) {
                    zsys_warning ("zgossip_msg: version is invalid");
                    goto malformed;
                }
            }
            break;

        case ZGOSSIP_MSG_PONG:
            {
                byte version;
                GET_NUMBER1 (version);
                if (version != 1) {
                    zsys_warning ("zgossip_msg: version is invalid");
                    goto malformed;
                }
            }
            break;

        case ZGOSSIP_MSG_INVALID:
            {
                byte version;
                GET_NUMBER1 (version);
                if (version != 1) {
                    zsys_warning ("zgossip_msg: version is invalid");
                    goto malformed;
                }
            }
            break;

        default:
            zsys_warning ("zgossip_msg: bad message ID");
            goto malformed;
    }
    //  Successful return
    zmq_msg_close (&frame);
    return 0;

    //  Error returns
    malformed:
        zsys_warning ("zgossip_msg: zgossip_msg malformed message, fail");
        zmq_msg_close (&frame);
        return -1;              //  Invalid message
}


//  --------------------------------------------------------------------------
//  Send the zgossip_msg to the socket. Does not destroy it. Returns 0 if
//  OK, else -1.

int
zgossip_msg_send (zgossip_msg_t *self, zsock_t *output)
{
    assert (self);
    assert (output);

    if (zsock_type (output) == ZMQ_ROUTER)
        zframe_send (&self->routing_id, output, ZFRAME_MORE + ZFRAME_REUSE);

    size_t frame_size = 2 + 1;          //  Signature and message ID
    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            frame_size += 1;            //  version
            break;
        case ZGOSSIP_MSG_PUBLISH:
            frame_size += 1;            //  version
            frame_size += 1 + strlen (self->key);
            frame_size += 4;
            if (self->value)
                frame_size += strlen (self->value);
            frame_size += 4;            //  ttl
            break;
        case ZGOSSIP_MSG_PING:
            frame_size += 1;            //  version
            break;
        case ZGOSSIP_MSG_PONG:
            frame_size += 1;            //  version
            break;
        case ZGOSSIP_MSG_INVALID:
            frame_size += 1;            //  version
            break;
    }
    //  Now serialize message into the frame
    zmq_msg_t frame;
    zmq_msg_init_size (&frame, frame_size);
    self->needle = (byte *) zmq_msg_data (&frame);
    PUT_NUMBER2 (0xAAA0 | 0);
    PUT_NUMBER1 (self->id);
    size_t nbr_frames = 1;              //  Total number of frames to send
    
    switch (self->id) {
        case ZGOSSIP_MSG_HELLO:
            PUT_NUMBER1 (1);
            break;

        case ZGOSSIP_MSG_PUBLISH:
            PUT_NUMBER1 (1);
            PUT_STRING (self->key);
            if (self->value) {
                PUT_LONGSTR (self->value);
            }
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
    zmq_msg_send (&frame, zsock_resolve (output), --nbr_frames? ZMQ_SNDMORE: 0);
    
    return 0;
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
zgossip_msg_set_key (zgossip_msg_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->key)
        return;
    strncpy (self->key, value, 255);
    self->key [255] = 0;
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
zgossip_msg_set_value (zgossip_msg_t *self, const char *value)
{
    assert (self);
    assert (value);
    free (self->value);
    self->value = strdup (value);
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
    zgossip_msg_t *self = zgossip_msg_new ();
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
    self = zgossip_msg_new ();
    zgossip_msg_set_id (self, ZGOSSIP_MSG_HELLO);

    //  Send twice
    zgossip_msg_send (self, output);
    zgossip_msg_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        zgossip_msg_recv (self, input);
        assert (zgossip_msg_routing_id (self));
    }
    zgossip_msg_set_id (self, ZGOSSIP_MSG_PUBLISH);

    zgossip_msg_set_key (self, "Life is short but Now lasts for ever");
    zgossip_msg_set_value (self, "Life is short but Now lasts for ever");
    zgossip_msg_set_ttl (self, 123);
    //  Send twice
    zgossip_msg_send (self, output);
    zgossip_msg_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        zgossip_msg_recv (self, input);
        assert (zgossip_msg_routing_id (self));
        assert (streq (zgossip_msg_key (self), "Life is short but Now lasts for ever"));
        assert (streq (zgossip_msg_value (self), "Life is short but Now lasts for ever"));
        assert (zgossip_msg_ttl (self) == 123);
    }
    zgossip_msg_set_id (self, ZGOSSIP_MSG_PING);

    //  Send twice
    zgossip_msg_send (self, output);
    zgossip_msg_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        zgossip_msg_recv (self, input);
        assert (zgossip_msg_routing_id (self));
    }
    zgossip_msg_set_id (self, ZGOSSIP_MSG_PONG);

    //  Send twice
    zgossip_msg_send (self, output);
    zgossip_msg_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        zgossip_msg_recv (self, input);
        assert (zgossip_msg_routing_id (self));
    }
    zgossip_msg_set_id (self, ZGOSSIP_MSG_INVALID);

    //  Send twice
    zgossip_msg_send (self, output);
    zgossip_msg_send (self, output);

    for (instance = 0; instance < 2; instance++) {
        zgossip_msg_recv (self, input);
        assert (zgossip_msg_routing_id (self));
    }

    zgossip_msg_destroy (&self);
    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
    return 0;
}
