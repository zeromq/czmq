/*  =========================================================================
    zgossip - gossip discovery service

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
//  This is the gossip service, implemented as a CZMQ zactor task
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
