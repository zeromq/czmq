/*  =========================================================================
    zgossip_msg - ZeroMQ Gossip Protocol
    
    Codec header for zgossip_msg.

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

#ifndef __ZGOSSIP_MSG_H_INCLUDED__
#define __ZGOSSIP_MSG_H_INCLUDED__

/*  These are the zgossip_msg messages:

    HELLO - Client says hello to server
        version             number 1    Version = 1

    PUBLISH - Client or server announces a new tuple
        version             number 1    Version = 1
        key                 string      Tuple key, globally unique
        value               longstr     Tuple value, as printable string
        ttl                 number 4    Time to live, msecs

    PING - Client signals liveness
        version             number 1    Version = 1

    PONG - Server responds to ping; note that pongs are not correlated with pings,
and may be mixed with other commands, and the client should treat any
incoming traffic as valid activity.
        version             number 1    Version = 1

    INVALID - Server rejects command as invalid
        version             number 1    Version = 1
*/


#define ZGOSSIP_MSG_HELLO                   1
#define ZGOSSIP_MSG_PUBLISH                 2
#define ZGOSSIP_MSG_PING                    3
#define ZGOSSIP_MSG_PONG                    4
#define ZGOSSIP_MSG_INVALID                 5

#include "czmq.h"

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
#ifndef ZGOSSIP_MSG_T_DEFINED
typedef struct _zgossip_msg_t zgossip_msg_t;
#define ZGOSSIP_MSG_T_DEFINED
#endif

//  @interface
//  Create a new empty zgossip_msg
CZMQ_EXPORT zgossip_msg_t *
    zgossip_msg_new (void);

//  Destroy a zgossip_msg instance
CZMQ_EXPORT void
    zgossip_msg_destroy (zgossip_msg_t **self_p);

//  Receive a zgossip_msg from the socket. Returns 0 if OK, -1 if
//  there was an error. Blocks if there is no message waiting.
CZMQ_EXPORT int
    zgossip_msg_recv (zgossip_msg_t *self, zsock_t *input);

//  Send the zgossip_msg to the output socket, does not destroy it
CZMQ_EXPORT int
    zgossip_msg_send (zgossip_msg_t *self, zsock_t *output);
    
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

//  Get/set the key field
CZMQ_EXPORT const char *
    zgossip_msg_key (zgossip_msg_t *self);
CZMQ_EXPORT void
    zgossip_msg_set_key (zgossip_msg_t *self, const char *value);

//  Get/set the value field
CZMQ_EXPORT const char *
    zgossip_msg_value (zgossip_msg_t *self);
CZMQ_EXPORT void
    zgossip_msg_set_value (zgossip_msg_t *self, const char *value);

//  Get/set the ttl field
CZMQ_EXPORT uint32_t
    zgossip_msg_ttl (zgossip_msg_t *self);
CZMQ_EXPORT void
    zgossip_msg_set_ttl (zgossip_msg_t *self, uint32_t ttl);

//  Self test of this class
CZMQ_EXPORT int
    zgossip_msg_test (bool verbose);
//  @end

//  For backwards compatibility with old codecs
#define zgossip_msg_dump    zgossip_msg_print

#ifdef __cplusplus
}
#endif

#endif
