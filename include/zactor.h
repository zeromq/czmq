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
//  Actors get a pipe and arguments from caller
typedef void (zactor_fn) (zsock_t *pipe, void *args);

//  Create a new actor passing arbitrary arguments reference.
CZMQ_EXPORT zactor_t *
    zactor_new (zactor_fn *task, void *args);

//  Destroy an actor.
CZMQ_EXPORT void
    zactor_destroy (zactor_t **p_self);

//  Send a zmsg message to the actor, take ownership of the message
//  and destroy when it has been sent.
CZMQ_EXPORT int
    zactor_send (zactor_t *self, zmsg_t **msg_p);

//  Receive a zmsg message from the actor. Returns NULL if the actor
//  was interrupted before the message could be received, or if there
//  was a timeout on the actor.
CZMQ_EXPORT zmsg_t *
    zactor_recv (zactor_t *self);

//  Probe the supplied object, and report if it looks like a zactor_t.
CZMQ_EXPORT bool
    zactor_is (void *self);

//  Probe the supplied reference. If it looks like a zactor_t instance,
//  return the underlying libzmq actor handle; else if it looks like
//  a libzmq actor handle, return the supplied value.
CZMQ_EXPORT void *
    zactor_resolve (void *self);

//  Return the actor's zsock handle. Use this when you absolutely need
//  to work with the zsock instance rather than the actor.
CZMQ_EXPORT zsock_t *
    zactor_sock (zactor_t *self);

//  Self test of this class
CZMQ_EXPORT void
    zactor_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
