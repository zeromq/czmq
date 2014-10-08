/*  =========================================================================
    zprototest_msg - zprototest example protocol

    Codec class for zprototest_msg.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

    * The XML model used for this code generation: zprototest_msg.xml
    * The code generation script that built this file: zproto_codec_c
    ************************************************************************

    =========================================================================
*/

/*
@header
    zprototest_msg - zprototest example protocol
@discuss
@end
*/

#include "czmq.h"
#include "../include/zprototest_msg.h"

//  Structure of our class

struct _zprototest_msg_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  zprototest_msg message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    uint32_t msgid;                     //  Message id
    char *message;                      //  Message data
    uint32_t testint;                   //  Test member
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
//  Create a new zprototest_msg

zprototest_msg_t *
zprototest_msg_new (int id)
{
    zprototest_msg_t *self = (zprototest_msg_t *) zmalloc (sizeof (zprototest_msg_t));
    self->id = id;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zprototest_msg

void
zprototest_msg_destroy (zprototest_msg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zprototest_msg_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        free (self->message);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Parse a zprototest_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. Destroys msg and
//  nullifies the msg reference.

zprototest_msg_t *
zprototest_msg_decode (zmsg_t **msg_p)
{
    assert (msg_p);
    zmsg_t *msg = *msg_p;
    if (msg == NULL)
        return NULL;

    zprototest_msg_t *self = zprototest_msg_new (0);
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
        case ZPROTOTEST_MSG_TESTMESSAGE:
            GET_NUMBER4 (self->msgid);
            GET_STRING (self->message);
            GET_NUMBER4 (self->testint);
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
        zprototest_msg_destroy (&self);
        return (NULL);
}


//  --------------------------------------------------------------------------
//  Encode zprototest_msg into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.

zmsg_t *
zprototest_msg_encode (zprototest_msg_t **self_p)
{
    assert (self_p);
    assert (*self_p);

    zprototest_msg_t *self = *self_p;
    zmsg_t *msg = zmsg_new ();

    size_t frame_size = 2 + 1;          //  Signature and message ID
    switch (self->id) {
        case ZPROTOTEST_MSG_TESTMESSAGE:
            //  msgid is a 4-byte integer
            frame_size += 4;
            //  message is a string with 1-byte length
            frame_size++;       //  Size is one octet
            if (self->message)
                frame_size += strlen (self->message);
            //  testint is a 4-byte integer
            frame_size += 4;
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
        case ZPROTOTEST_MSG_TESTMESSAGE:
            PUT_NUMBER4 (self->msgid);
            if (self->message) {
                PUT_STRING (self->message);
            }
            else
                PUT_NUMBER1 (0);    //  Empty string
            PUT_NUMBER4 (self->testint);
            break;

    }
    //  Now send the data frame
    if (zmsg_append (msg, &frame)) {
        zmsg_destroy (&msg);
        zprototest_msg_destroy (self_p);
        return NULL;
    }
    //  Destroy zprototest_msg object
    zprototest_msg_destroy (self_p);
    return msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a zprototest_msg from the socket. Returns new object or
//  NULL if error. Will block if there's no message waiting.

zprototest_msg_t *
zprototest_msg_recv (void *input)
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
    zprototest_msg_t *zprototest_msg = zprototest_msg_decode (&msg);
    if (zprototest_msg && zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER)
        zprototest_msg->routing_id = routing_id;

    return zprototest_msg;
}


//  --------------------------------------------------------------------------
//  Receive and parse a zprototest_msg from the socket. Returns new object,
//  or NULL either if there was no input waiting, or the recv was interrupted.

zprototest_msg_t *
zprototest_msg_recv_nowait (void *input)
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
    zprototest_msg_t *zprototest_msg = zprototest_msg_decode (&msg);
    if (zprototest_msg && zsocket_type (zsock_resolve (input)) == ZMQ_ROUTER)
        zprototest_msg->routing_id = routing_id;

    return zprototest_msg;
}


//  --------------------------------------------------------------------------
//  Send the zprototest_msg to the socket, and destroy it
//  Returns 0 if OK, else -1

int
zprototest_msg_send (zprototest_msg_t **self_p, void *output)
{
    assert (self_p);
    assert (*self_p);
    assert (output);

    //  Save routing_id if any, as encode will destroy it
    zprototest_msg_t *self = *self_p;
    zframe_t *routing_id = self->routing_id;
    self->routing_id = NULL;

    //  Encode zprototest_msg message to a single zmsg
    zmsg_t *msg = zprototest_msg_encode (self_p);

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
//  Send the zprototest_msg to the output, and do not destroy it

int
zprototest_msg_send_again (zprototest_msg_t *self, void *output)
{
    assert (self);
    assert (output);
    self = zprototest_msg_dup (self);
    return zprototest_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Encode TESTMESSAGE message

zmsg_t *
zprototest_msg_encode_testmessage (
    uint32_t msgid,
    const char *message,
    uint32_t testint)
{
    zprototest_msg_t *self = zprototest_msg_new (ZPROTOTEST_MSG_TESTMESSAGE);
    zprototest_msg_set_msgid (self, msgid);
    zprototest_msg_set_message (self, message);
    zprototest_msg_set_testint (self, testint);
    return zprototest_msg_encode (&self);
}


//  --------------------------------------------------------------------------
//  Send the TESTMESSAGE to the socket in one step

int
zprototest_msg_send_testmessage (
    void *output,
    uint32_t msgid,
    const char *message,
    uint32_t testint)
{
    zprototest_msg_t *self = zprototest_msg_new (ZPROTOTEST_MSG_TESTMESSAGE);
    zprototest_msg_set_msgid (self, msgid);
    zprototest_msg_set_message (self, message);
    zprototest_msg_set_testint (self, testint);
    return zprototest_msg_send (&self, output);
}


//  --------------------------------------------------------------------------
//  Duplicate the zprototest_msg message

zprototest_msg_t *
zprototest_msg_dup (zprototest_msg_t *self)
{
    if (!self)
        return NULL;

    zprototest_msg_t *copy = zprototest_msg_new (self->id);
    if (self->routing_id)
        copy->routing_id = zframe_dup (self->routing_id);
    switch (self->id) {
        case ZPROTOTEST_MSG_TESTMESSAGE:
            copy->msgid = self->msgid;
            copy->message = self->message? strdup (self->message): NULL;
            copy->testint = self->testint;
            break;

    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
zprototest_msg_print (zprototest_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case ZPROTOTEST_MSG_TESTMESSAGE:
            zsys_debug ("ZPROTOTEST_MSG_TESTMESSAGE:");
            zsys_debug ("    msgid=%ld", (long) self->msgid);
            if (self->message)
                zsys_debug ("    message='%s'", self->message);
            else
                zsys_debug ("    message=");
            zsys_debug ("    testint=%ld", (long) self->testint);
            break;

    }
}


//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
zprototest_msg_routing_id (zprototest_msg_t *self)
{
    assert (self);
    return self->routing_id;
}

void
zprototest_msg_set_routing_id (zprototest_msg_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the zprototest_msg id

int
zprototest_msg_id (zprototest_msg_t *self)
{
    assert (self);
    return self->id;
}

void
zprototest_msg_set_id (zprototest_msg_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
zprototest_msg_command (zprototest_msg_t *self)
{
    assert (self);
    switch (self->id) {
        case ZPROTOTEST_MSG_TESTMESSAGE:
            return ("TESTMESSAGE");
            break;
    }
    return "?";
}

//  --------------------------------------------------------------------------
//  Get/set the msgid field

uint32_t
zprototest_msg_msgid (zprototest_msg_t *self)
{
    assert (self);
    return self->msgid;
}

void
zprototest_msg_set_msgid (zprototest_msg_t *self, uint32_t msgid)
{
    assert (self);
    self->msgid = msgid;
}


//  --------------------------------------------------------------------------
//  Get/set the message field

const char *
zprototest_msg_message (zprototest_msg_t *self)
{
    assert (self);
    return self->message;
}

void
zprototest_msg_set_message (zprototest_msg_t *self, const char *format, ...)
{
    //  Format message from provided arguments
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    free (self->message);
    self->message = zsys_vprintf (format, argptr);
    va_end (argptr);
}


//  --------------------------------------------------------------------------
//  Get/set the testint field

uint32_t
zprototest_msg_testint (zprototest_msg_t *self)
{
    assert (self);
    return self->testint;
}

void
zprototest_msg_set_testint (zprototest_msg_t *self, uint32_t testint)
{
    assert (self);
    self->testint = testint;
}



//  --------------------------------------------------------------------------
//  Selftest

int
zprototest_msg_test (bool verbose)
{
    printf (" * zprototest_msg: ");

    //  @selftest
    //  Simple create/destroy test
    zprototest_msg_t *self = zprototest_msg_new (0);
    assert (self);
    zprototest_msg_destroy (&self);

    //  Create pair of sockets we can send through
    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    zsock_connect (input, "inproc://selftest-zprototest_msg");

    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    zsock_bind (output, "inproc://selftest-zprototest_msg");

    //  Encode/send/decode and verify each message type
    int instance;
    zprototest_msg_t *copy;
    self = zprototest_msg_new (ZPROTOTEST_MSG_TESTMESSAGE);

    //  Check that _dup works on empty message
    copy = zprototest_msg_dup (self);
    assert (copy);
    zprototest_msg_destroy (&copy);

    zprototest_msg_set_msgid (self, 123);
    zprototest_msg_set_message (self, "Life is short but Now lasts for ever");
    zprototest_msg_set_testint (self, 123);
    //  Send twice from same object
    zprototest_msg_send_again (self, output);
    zprototest_msg_send (&self, output);

    for (instance = 0; instance < 2; instance++) {
        self = zprototest_msg_recv (input);
        assert (self);
        assert (zprototest_msg_routing_id (self));

        assert (zprototest_msg_msgid (self) == 123);
        assert (streq (zprototest_msg_message (self), "Life is short but Now lasts for ever"));
        assert (zprototest_msg_testint (self) == 123);
        zprototest_msg_destroy (&self);
    }

    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end

    printf ("OK\n");
    return 0;
}
