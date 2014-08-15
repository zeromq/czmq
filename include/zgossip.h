/*  =========================================================================
    zgossip - zgossip server

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

    * The XML model used for this code generation: zgossip.xml
    * The code generation script that built this file: zproto_server_c
    ************************************************************************

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

#ifndef __ZGOSSIP_H_INCLUDED__
#define __ZGOSSIP_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  To work with zgossip, use the CZMQ zactor API:
//
//  Create new zgossip instance, passing logging prefix:
//      zactor_t *gossip = zactor_new (zgossip, "myname");
//  
//  Destroy zgossip instance
//      zactor_destroy (&gossip);
//  
//  Send zmsg instance message to zgossip instance:
//      zactor_send (gossip, &msg);
//  
//  Send command to zgossip instance:
//      zstr_sendx (gossip, "COMMAND", "ARG1", "ARG2", NULL);
//  
//  Receive zmsg message from zgossip instance:
//      zmsg_t *msg = zactor_recv (gossip);
//
//  This is the zgossip constructor as a zactor_fn:
//
CZMQ_EXPORT void
    zgossip (zsock_t *pipe, void *args);

//  Self test of this class
CZMQ_EXPORT void
    zgossip_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
