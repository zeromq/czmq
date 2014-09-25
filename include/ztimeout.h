/*  =========================================================================
    ztimeout - spezialized priority queue for timeout handling

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZTIMEOUT_H_INCLUDED__
#define __ZTIMEOUT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Trigger function for ztimeout items
typedef void (ztimeout_trigger_fn) (void **item);

//  Create a new timeout queue
CZMQ_EXPORT ztimeout_t *
    ztimeout_new (ztimeout_trigger_fn trigger_fn);

//  Destroy a timeout queue
CZMQ_EXPORT void
    ztimeout_destroy (ztimeout_t **self_p);

// Return remaining time till first timeout in ms. Processes timeout
// callbacks as a side effect.
CZMQ_EXPORT int64_t
    ztimeout_first (ztimeout_t *self);

//  Add a timeout at time <time> to the queue, return timeout handle if
//  OK or NULL if this failed for some reason (out of memory).
CZMQ_EXPORT ztimeout_node_t *
    ztimeout_add (ztimeout_t *self, void *item, int64_t time);

//  Cancel a timeout by handle.
CZMQ_EXPORT void
    ztimeout_cancel (ztimeout_t *self, ztimeout_node_t *node);

//  Restart a timeout by handle.
CZMQ_EXPORT void
    ztimeout_restart (ztimeout_t *self, ztimeout_node_t *node, int64_t time);

//  Return number of timeouts in the queue
CZMQ_EXPORT size_t
    ztimeout_size (ztimeout_t *self);

//  Set a user-defined destructor for items; by default items are not
//  freed when the timeout is destroyed.
CZMQ_EXPORT void
    ztimeout_set_destructor (ztimeout_t *self, czmq_destructor destructor);

//  Set a user-defined duplicator for items; by default items are not
//  copied when the timeout is duplicated.
CZMQ_EXPORT void
    ztimeout_set_duplicator (ztimeout_t *self, czmq_duplicator duplicator);

//  Self test of this class
CZMQ_EXPORT void
    ztimeout_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
