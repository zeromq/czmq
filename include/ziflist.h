/*  =========================================================================
    ziflist - List of network interfaces available on system
    
    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZIFLIST_H_INCLUDED__
#define __ZIFLIST_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Get a list of network interfaces currently defined on the system
CZMQ_EXPORT ziflist_t *
    ziflist_new (void);

//  Destroy a ziflist instance
CZMQ_EXPORT void
    ziflist_destroy (ziflist_t **self_p);

//  Reload network interfaces from system
CZMQ_EXPORT void
    ziflist_reload (ziflist_t *self);

//  Return the number of network interfaces on system
CZMQ_EXPORT size_t
    ziflist_size (ziflist_t *self);

//  Get first network interface, return NULL if there are none
CZMQ_EXPORT const char *
    ziflist_first (ziflist_t *self);

//  Get next network interface, return NULL if we hit the last one
CZMQ_EXPORT const char *
    ziflist_next (ziflist_t *self);

//  Return the current interface IP address as a printable string
CZMQ_EXPORT const char *
    ziflist_address (ziflist_t *self);

//  Return the current interface broadcast address as a printable string
CZMQ_EXPORT const char *
    ziflist_broadcast (ziflist_t *self);

//  Return the current interface network mask as a printable string
CZMQ_EXPORT const char *
    ziflist_netmask (ziflist_t *self);

//  Selftest for this class
CZMQ_EXPORT void
    ziflist_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
