/*  =========================================================================
    zinterface - Information about network interfaces
    
    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZINTERFACE_H_INCLUDED__
#define __ZINTERFACE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Return a list of available network interfaces. The interfaces remain
//  valid while the list is not destroyed. If one interface is to be valid
//  after destroyed, it must be duplicated, and then destroyed.
CZMQ_EXPORT zlist_t *
    zinterface_list ();

//  Duplicate an interface
CZMQ_EXPORT zinterface_t *
    zinterface_dup (zinterface_t *self);

//  Destroy an interface
CZMQ_EXPORT void
    zinterface_destroy (zinterface_t **self_p);

//  Return the interface's name as a printable string
CZMQ_EXPORT char *
    zinterface_name (zinterface_t *self);

//  Return the interface's IP address as a printable string
CZMQ_EXPORT char *
    zinterface_address (zinterface_t *self);

//  Return the interface's broadcast address as a printable string
CZMQ_EXPORT char *
    zinterface_broadcast (zinterface_t *self);

//  Return the interface's network mask as a printable string
CZMQ_EXPORT char *
    zinterface_netmask (zinterface_t *self);

// Prints all detected interfaces on the screen
CZMQ_EXPORT void
    zinterface_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
