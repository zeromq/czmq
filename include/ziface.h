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
//  Return a list of available network interfaces.
CZMQ_EXPORT ziface_t *
    ziface_new ();

//  Destroy a list of network interfaces
CZMQ_EXPORT void
    ziface_destroy (ziface_t **self_p);

//  Return the number of interfaces
CZMQ_EXPORT size_t
    ziface_size (ziface_t *self);

//  Set the current interface to be the first one. Returns true if the list is not empty
CZMQ_EXPORT bool
    ziface_first (ziface_t *self);

//  Advances the current interface. Returns false if there's no more items, true otherwise
CZMQ_EXPORT bool
    ziface_next (ziface_t *self);

//  Return the current interface's name as a printable string
CZMQ_EXPORT char *
    ziface_name (ziface_t *self);

//  Return the current interface's IP address as a printable string
CZMQ_EXPORT char *
    ziface_address (ziface_t *self);

//  Return the current interface's broadcast address as a printable string
CZMQ_EXPORT char *
    ziface_broadcast (ziface_t *self);

//  Return the current interface's network mask as a printable string
CZMQ_EXPORT char *
    ziface_netmask (ziface_t *self);

// Prints all detected interfaces on the screen
CZMQ_EXPORT void
    ziface_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
