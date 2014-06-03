/*  =========================================================================
    zgossip_msg - zgossip protocol
    
    Codec header for zgossip_msg.

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

#ifndef __ZGOSSIP_MSG_H_INCLUDED__
#define __ZGOSSIP_MSG_H_INCLUDED__

/*  These are the zgossip_msg messages:

    HELLO - Client says hello to server

    ANNOUNCE - Client or server announces a new service
        endpoint            string      ZeroMQ endpoint
        service             string      Service name

    PING - Client signals liveness

    PONG - Server responds to ping

    INVALID - Server rejects command as invalid
*/


#define ZGOSSIP_MSG_HELLO                   1
#define ZGOSSIP_MSG_ANNOUNCE                2
#define ZGOSSIP_MSG_PING                    3
#define ZGOSSIP_MSG_PONG                    4
#define ZGOSSIP_MSG_INVALID                 5

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zgossip_msg_t zgossip_msg_t;

//  @interface
//  Create a new zgossip_msg
CZMQ_EXPORT zgossip_msg_t *
    zgossip_msg_new (int id);

//  Destroy the zgossip_msg
CZMQ_EXPORT void
    zgossip_msg_destroy (zgossip_msg_t **self_p);

//  Parse a zgossip_msg from zmsg_t. Returns a new object, or NULL if
//  the message could not be parsed, or was NULL. If the socket type is
//  ZMQ_ROUTER, then parses the first frame as a routing_id. Destroys msg
//  and nullifies the msg refernce.
CZMQ_EXPORT zgossip_msg_t *
    zgossip_msg_decode (zmsg_t **msg_p);

//  Encode zgossip_msg into zmsg and destroy it. Returns a newly created
//  object or NULL if error. Use when not in control of sending the message.
//  If the socket_type is ZMQ_ROUTER, then stores the routing_id as the
//  first frame of the resulting message.
CZMQ_EXPORT zmsg_t *
    zgossip_msg_encode (zgossip_msg_t **self_p);

//  Receive and parse a zgossip_msg from the socket. Returns new object, 
//  or NULL if error. Will block if there's no message waiting.
CZMQ_EXPORT zgossip_msg_t *
    zgossip_msg_recv (void *input);

//  Receive and parse a zgossip_msg from the socket. Returns new object, 
//  or NULL either if there was no input waiting, or the recv was interrupted.
CZMQ_EXPORT zgossip_msg_t *
    zgossip_msg_recv_nowait (void *input);

//  Send the zgossip_msg to the output, and destroy it
CZMQ_EXPORT int
    zgossip_msg_send (zgossip_msg_t **self_p, void *output);

//  Send the zgossip_msg to the output, and do not destroy it
CZMQ_EXPORT int
    zgossip_msg_send_again (zgossip_msg_t *self, void *output);

//  Encode the HELLO 
CZMQ_EXPORT zmsg_t *
    zgossip_msg_encode_hello (
);

//  Encode the ANNOUNCE 
CZMQ_EXPORT zmsg_t *
    zgossip_msg_encode_announce (
        const char *endpoint,
        const char *service);

//  Encode the PING 
CZMQ_EXPORT zmsg_t *
    zgossip_msg_encode_ping (
);

//  Encode the PONG 
CZMQ_EXPORT zmsg_t *
    zgossip_msg_encode_pong (
);

//  Encode the INVALID 
CZMQ_EXPORT zmsg_t *
    zgossip_msg_encode_invalid (
);


//  Send the HELLO to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    zgossip_msg_send_hello (void *output);
    
//  Send the ANNOUNCE to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    zgossip_msg_send_announce (void *output,
        const char *endpoint,
        const char *service);
    
//  Send the PING to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    zgossip_msg_send_ping (void *output);
    
//  Send the PONG to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    zgossip_msg_send_pong (void *output);
    
//  Send the INVALID to the output in one step
//  WARNING, this call will fail if output is of type ZMQ_ROUTER.
CZMQ_EXPORT int
    zgossip_msg_send_invalid (void *output);
    
//  Duplicate the zgossip_msg message
CZMQ_EXPORT zgossip_msg_t *
    zgossip_msg_dup (zgossip_msg_t *self);

//  Print contents of message to stdout
CZMQ_EXPORT void
    zgossip_msg_print (zgossip_msg_t *self);

//  Get/set the message routing id
CZMQ_EXPORT zframe_t *
    zgossip_msg_routing_id (zgossip_msg_t *self);
CZMQ_EXPORT void
    zgossip_msg_set_routing_id (zgossip_msg_t *self, zframe_t *routing_id);

//  Get the zgossip_msg id and printable command
CZMQ_EXPORT int
    zgossip_msg_id (zgossip_msg_t *self);
CZMQ_EXPORT void
    zgossip_msg_set_id (zgossip_msg_t *self, int id);
CZMQ_EXPORT const char *
    zgossip_msg_command (zgossip_msg_t *self);

//  Get/set the endpoint field
CZMQ_EXPORT const char *
    zgossip_msg_endpoint (zgossip_msg_t *self);
CZMQ_EXPORT void
    zgossip_msg_set_endpoint (zgossip_msg_t *self, const char *format, ...);

//  Get/set the service field
CZMQ_EXPORT const char *
    zgossip_msg_service (zgossip_msg_t *self);
CZMQ_EXPORT void
    zgossip_msg_set_service (zgossip_msg_t *self, const char *format, ...);

//  Self test of this class
CZMQ_EXPORT int
    zgossip_msg_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
