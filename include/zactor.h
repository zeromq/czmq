/*  =========================================================================
    zactor - actor 

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZACTOR_H_INCLUDED__
#define __ZACTOR_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Actors get arguments from caller
typedef void (zactor_fn) (void *args);

//  Create a new actor passing arbitrary arguments reference.
CZMQ_EXPORT zactor_t *
    zactor_new (zactor_fn *task, void *args);

//  Destroy an actor.
CZMQ_EXPORT void
    zactor_destroy (zactor_t **p_self);

//  Probe the supplied object, and report if it looks like a zactor_t.
CZMQ_EXPORT bool
    zactor_is (void *self);

//  Probe the supplied reference. If it looks like a zactor_t instance,
//  return the underlying libzmq actor handle; else if it looks like
//  a libzmq actor handle, return the supplied value.
CZMQ_EXPORT void *
    zactor_resolve (void *self);

//  Self test of this class
CZMQ_EXPORT int
    zactor_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
