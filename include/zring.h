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

//  Prepend an item to the start of the ring, return 0 if OK, else -1.
CZMQ_EXPORT int
    zring_prepend (zring_t *self, void *item);

//  Append an item to the end of the ring, return 0 if OK, else -1.
CZMQ_EXPORT int
    zring_append (zring_t *self, void *item);

//  Detach specified item from the ring. If the item is not present in the
//  ring, returns null. Caller must destroy item when finished with it.
//  Returns 0 if an item was detached, else -1.
CZMQ_EXPORT int
    zring_detach (zring_t *self, void *item);

//  Delete specified item from the ring. If the item is not present in the
//  ring, returns null. Calls the ring item destructor if one was set.
//  Returns 0 if an item was deleted, else -1.
CZMQ_EXPORT int
    zring_delete (zring_t *self, void *item);

//  Return number of items in the ring
CZMQ_EXPORT size_t
    zring_size (zring_t *self);

//  Make a copy of the ring; items are duplicated if you set a duplicator
//  for the ring, otherwise not. Copying a null reference returns a null
//  reference.
CZMQ_EXPORT zring_t *
    zring_copy (zring_t *self);

//  Set a user-defined deallocator for ring items; by default items are not
//  freed when the ring is destroyed.
CZMQ_EXPORT void
    zring_set_destructor (zring_t *self, czmq_destructor destructor);

//  Set a user-defined duplicator for ring items; by default items are not
//  copied when the ring is duplicated.
CZMQ_EXPORT void
    zring_set_duplicator (zring_t *self, czmq_duplicator duplicator);

//  Self test of this class
CZMQ_EXPORT void
    zring_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
