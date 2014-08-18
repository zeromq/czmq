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
//  Create new zgossip server instance, passing logging prefix:
//
//      zactor_t *zgossip_server = zactor_new (zgossip, "myname");
//  
//  Destroy zgossip server instance
//
//      zactor_destroy (&zgossip_server);
//  
//  Bind zgossip server to specified endpoint. TCP endpoints may specify
//  the port number as "*" to aquire an ephemeral port:
//
//      zstr_sendx (zgossip_server, "BIND", endpoint, NULL);
//
//  Return assigned port number, specifically when BIND was done using an
//  an ephemeral port:
//
//      zstr_sendx (zgossip_server, "PORT", NULL);
//      char *command, *port_str;
//      zstr_recvx (zgossip_server, &command, &port_str, NULL);
//      assert (streq (command, "PORT"));
//
//  Specify configuration file to load, overwriting any previous loaded
//  configuration file or options:
//
//      zstr_sendx (zgossip_server, "CONFIGURE", filename, NULL);
//
//  Set configuration path valuu:
//
//      zstr_sendx (zgossip_server, "SET", path, value, NULL);
//    
//  Send zmsg_t instance to zgossip server:
//
//      zactor_send (zgossip_server, &msg);
//
//  Receive zmsg_t instance from zgossip server:
//
//      zmsg_t *msg = zactor_recv (zgossip_server);
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
