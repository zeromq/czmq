/*  =========================================================================
    zface - Information about network interfaces
    
    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZIFACE_H_INCLUDED__
#define __ZIFACE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new ziface instance to take a snapshot of network interfaces
//  currently defined on the system.
CZMQ_EXPORT ziface_t *
    ziface_new (void);

//  Destroy a ziface instance
CZMQ_EXPORT void
    ziface_destroy (ziface_t **self_p);

//  Reload network interfaces from system
CZMQ_EXPORT void
    ziface_reload (ziface_t *self);

//  Return the number of network interfaces on system
CZMQ_EXPORT size_t
    ziface_size (ziface_t *self);

//  Get first network interface, return NULL if there are none
CZMQ_EXPORT const char *
    ziface_first (ziface_t *self);

//  Get next network interface, return NULL if we hit the last one
CZMQ_EXPORT const char *
    ziface_next (ziface_t *self);

//  Return the current interface IP address as a printable string
CZMQ_EXPORT const char *
    ziface_address (ziface_t *self);

//  Return the current interface broadcast address as a printable string
CZMQ_EXPORT const char *
    ziface_broadcast (ziface_t *self);

//  Return the current interface network mask as a printable string
CZMQ_EXPORT const char *
    ziface_netmask (ziface_t *self);

//  Selftest for this class
CZMQ_EXPORT void
    ziface_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
