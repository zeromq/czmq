/*  =========================================================================
    zdlist - generic type-free doubly linked list container

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZDLIST_H_INCLUDED__
#define __ZDLIST_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new list container
CZMQ_EXPORT zdlist_t *
    zdlist_new (void);

//  Destroy a list container
CZMQ_EXPORT void
    zdlist_destroy (zdlist_t **self_p);

//  Return first item in the list, or null
CZMQ_EXPORT void *
    zdlist_first (zdlist_t *self);

//  Return next item in the list, or null
CZMQ_EXPORT void *
    zdlist_next (zdlist_t *self);

//  Return last item in the list, or null
CZMQ_EXPORT void *
    zdlist_last (zdlist_t *self);

//  Return first item in the list, or null, leaves the cursor
CZMQ_EXPORT void *
    zdlist_head (zdlist_t *self);

//  Return last item in the list, or null, leaves the cursor
CZMQ_EXPORT void *
    zdlist_tail (zdlist_t *self);

//  Append an item to the end of the list, return item handle if OK
//  or NULL if this failed for some reason (out of memory).
CZMQ_EXPORT zdlist_node_t *
    zdlist_append (zdlist_t *self, void *item);

//  Push an item to the start of the list, return item handle if OK
//  or NULL if this failed for some reason (out of memory).
CZMQ_EXPORT zdlist_node_t *
    zdlist_push (zdlist_t *self, void *item);

//  Pop the item off the start of the list, if any. Resets the cursor if if
//  points to the first node.
CZMQ_EXPORT void *
    zdlist_pop (zdlist_t *self);

//  Remove the specified item handle from the list. Sets the cursor to the
//  previous node if it points to this node.
CZMQ_EXPORT void
    zdlist_remove (zdlist_t *self, zdlist_node_t **node_p);

//  Return number of items in the list
CZMQ_EXPORT size_t
    zdlist_size (zdlist_t *self);

//  Set a user-defined deallocator for list items; by default items are not
//  freed when the list is destroyed.
typedef void (zdlist_free_fn) (void **item);
CZMQ_EXPORT void
    zdlist_set_free_fn (zdlist_t *self, zdlist_free_fn free_fn);

//  Set a user-defined duplicator for list items; by default items are not
//  copied when the list is duplicated.
typedef void * (zdlist_dup_fn) (void *item);
CZMQ_EXPORT void
    zdlist_set_dup_fn (zdlist_t *self, zdlist_dup_fn dup_fn);

//  Make a copy of the list; items are duplicated if you set a duplicator
//  for the list.
CZMQ_EXPORT zdlist_t *
    zdlist_dup (zdlist_t *self);

//  Self test of this class
CZMQ_EXPORT void
    zdlist_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
