/*  =========================================================================
    zprototest_msg - zprototest example protocol
    
    Codec header for zprototest_msg.

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

#ifndef __ZPROTOTEST_MSG_H_INCLUDED__
#define __ZPROTOTEST_MSG_H_INCLUDED__

/*  These are the zprototest_msg messages:

    TESTMESSAGE - Test message.
        msgid               number 4    Message id
        message             string      Message data
        testint             number 4    Test member
*/


#define ZPROTOTEST_MSG_TESTMESSAGE          1

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zprototest_msg_t zprototest_msg_t;

//  @interface
//  Create a new zprototest_msg
zprototest_msg_t *
    zprototest_msg_new (int id);

//  Destroy the zprototest_msg
void
    zprototest_msg_destroy (zprototest_msg_t **self_p);

//  Parse a zprototest_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. Destroys msg and 
//  nullifies the msg reference.
zprototest_msg_t *
    zprototest_msg_decode (zmsg_t **msg_p);

//  Encode zprototest_msg into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.
zmsg_t *
    zprototest_msg_encode (zprototest_msg_t **self_p);

//  Receive and parse a zprototest_msg from the socket. Returns new object, 
//  or NULL if error. Will block if there's no message waiting.
zprototest_msg_t *
    zprototest_msg_recv (void *input);

//  Receive and parse a zprototest_msg from the socket. Returns new object, 
//  or NULL either if there was no input waiting, or the recv was interrupted.
zprototest_msg_t *
    zprototest_msg_recv_nowait (void *input);

//  Send the zprototest_msg to the output, and destroy it
int
    zprototest_msg_send (zprototest_msg_t **self_p, void *output);

//  Send the zprototest_msg to the output, and do not destroy it
int
    zprototest_msg_send_again (zprototest_msg_t *self, void *output);

//  Encode the TESTMESSAGE 
zmsg_t *
    zprototest_msg_encode_testmessage (
        uint32_t msgid,
        const char *message,
        uint32_t testint);


//  Send the TESTMESSAGE to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
int
    zprototest_msg_send_testmessage (void *output,
        uint32_t msgid,
        const char *message,
        uint32_t testint);
    
//  Duplicate the zprototest_msg message
zprototest_msg_t *
    zprototest_msg_dup (zprototest_msg_t *self);

//  Print contents of message to stdout
void
    zprototest_msg_print (zprototest_msg_t *self);

//  Get/set the message routing id
zframe_t *
    zprototest_msg_routing_id (zprototest_msg_t *self);
void
    zprototest_msg_set_routing_id (zprototest_msg_t *self, zframe_t *routing_id);

//  Get the zprototest_msg id and printable command
int
    zprototest_msg_id (zprototest_msg_t *self);
void
    zprototest_msg_set_id (zprototest_msg_t *self, int id);
const char *
    zprototest_msg_command (zprototest_msg_t *self);

//  Get/set the msgid field
uint32_t
    zprototest_msg_msgid (zprototest_msg_t *self);
void
    zprototest_msg_set_msgid (zprototest_msg_t *self, uint32_t msgid);

//  Get/set the message field
const char *
    zprototest_msg_message (zprototest_msg_t *self);
void
    zprototest_msg_set_message (zprototest_msg_t *self, const char *format, ...);

//  Get/set the testint field
uint32_t
    zprototest_msg_testint (zprototest_msg_t *self);
void
    zprototest_msg_set_testint (zprototest_msg_t *self, uint32_t testint);

//  Self test of this class
int
    zprototest_msg_test (bool verbose);
//  @end

//  For backwards compatibility with old codecs
#define zprototest_msg_dump  zprototest_msg_print

#ifdef __cplusplus
}
#endif

#endif
