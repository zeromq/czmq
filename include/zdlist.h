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

//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zdlist_first (zdlist_t *self);

//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zdlist_last (zdlist_t *self);
    
//  Return the next item. At the end of the list (or in an empty list),
//  returns NULL. Use repeated zdlist_next () calls to work through the list
//  from zdlist_first ().
CZMQ_EXPORT void *
    zdlist_next (zdlist_t *self);
    
//  Return the previous item. At the start of the list (or in an empty list),
//  returns NULL. Use repeated zdlist_prev () calls to work through the list
//  backwards from zdlist_last ().
CZMQ_EXPORT void *
    zdlist_prev (zdlist_t *self);

//  Append an item to the end of the list, return 0 if OK, else -1.
CZMQ_EXPORT int
    zdlist_append (zdlist_t *self, void *item);

//  Push an item to the start of the list, return 0 if OK, else -1.
CZMQ_EXPORT int
    zdlist_push (zdlist_t *self, void *item);

//  Pop and destroy item off the start of the list, if any. Sets cursor to
//  first remaining item in list.
CZMQ_EXPORT void
    zdlist_pop (zdlist_t *self);

//  Insert an item after cursor, return 0 if OK, else -1.
CZMQ_EXPORT int
    zdlist_insert (zdlist_t *self, void *item);

//  Remove the current item from the list (as set by first/last/next/prev
//  calls). Sets the cursor to the next item.
CZMQ_EXPORT void
    zdlist_remove (zdlist_t *self);

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
