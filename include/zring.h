/*  =========================================================================
    zring - generic type-free doubly linked list container

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZRING_H_INCLUDED__
#define __ZRING_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new ring container (a ring is a doubly-linked list)
CZMQ_EXPORT zring_t *
    zring_new (void);

//  Destroy a ring container
CZMQ_EXPORT void
    zring_destroy (zring_t **self_p);

//  Return the item at the head of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the ring is empty.
CZMQ_EXPORT void *
    zring_first (zring_t *self);

//  Return the item at the tail of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the ring is empty.
CZMQ_EXPORT void *
    zring_last (zring_t *self);
    
//  Return the next item. At the end of the ring (or in an empty ring),
//  returns NULL. Use repeated zring_next () calls to work through the ring
//  from zring_first ().
CZMQ_EXPORT void *
    zring_next (zring_t *self);
    
//  Return the previous item. At the start of the ring (or in an empty ring),
//  returns NULL. Use repeated zring_prev () calls to work through the ring
//  backwards from zring_last ().
CZMQ_EXPORT void *
    zring_prev (zring_t *self);

//  Insert an item after cursor, return 0 if OK, else -1. Cursor is set to
//  inserted item (zring_remove() will remove it again). In any empty ring,
//  inserts after the head.
CZMQ_EXPORT int
    zring_insert (zring_t *self, void *item);

//  Append an item to the end of the ring, return 0 if OK, else -1.
CZMQ_EXPORT int
    zring_append (zring_t *self, void *item);

//  Push an item to the start of the ring, return 0 if OK, else -1.
CZMQ_EXPORT int
    zring_push (zring_t *self, void *item);

//  Remove current item from the ring (as set by first/last/next/prev calls),
//  and return item. Sets the cursor to the next item. If ring was empty,
//  returns null. Caller should destroy item when finished with it.
CZMQ_EXPORT void *
    zring_remove (zring_t *self);

//  Pop item off the start of the ring, and return item. If the ring is empty.
//  returns null. Sets cursor to first remaining item in ring. Caller should
//  destroy item when finished with it.
CZMQ_EXPORT void *
    zring_pop (zring_t *self);

//  Return number of items in the ring
CZMQ_EXPORT size_t
    zring_size (zring_t *self);

//  Set a user-defined deallocator for ring items; by default items are not
//  freed when the ring is destroyed.
typedef void (zring_free_fn) (void **item);
CZMQ_EXPORT void
    zring_set_free_fn (zring_t *self, zring_free_fn free_fn);

//  Set a user-defined duplicator for ring items; by default items are not
//  copied when the ring is duplicated.
typedef void * (zring_dup_fn) (void *item);
CZMQ_EXPORT void
    zring_set_dup_fn (zring_t *self, zring_dup_fn dup_fn);

//  Make a copy of the ring; items are duplicated if you set a duplicator
//  for the ring.
CZMQ_EXPORT zring_t *
    zring_dup (zring_t *self);

//  Self test of this class
CZMQ_EXPORT void
    zring_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
