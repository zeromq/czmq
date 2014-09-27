/*  =========================================================================
    zring - generic type-free doubly linked ring container

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
//  Create a new ring container (a ring is a doubly-linked cyclic
//  list). Produce a guard element if item is NULL. See zring_first/next.
CZMQ_EXPORT zring_t *
    zring_new (void *item);

//  Destroy a ring container
CZMQ_EXPORT void
    zring_destroy (zring_t **self_p);

//  Insert an item after an element, return new element if OK, else
//  NULL. Creates a new ring if self is NULL. Insert a guard element
//  if item is NULL. See zring_first/next.
CZMQ_EXPORT zring_t *
    zring_insert_after (zring_t *self, void *item);

//  Insert an item before an element, return new element if OK, else
//  NULL. Creates a new ring if self is NULL. Insert a guard element
//  if item is NULL. See zring_first/next.
CZMQ_EXPORT zring_t *
    zring_insert_before (zring_t *self, void *item);

//  Detach first element from the ring and return it. Advances self_p to
//  the next element in the ring or NULL if none left.
CZMQ_EXPORT zring_t *
    zring_detach (zring_t **self_p);

//  Remove first element from the ring and return its item. Advances
//  self_p to the next element in the ring or NULL if none left.
CZMQ_EXPORT void *
    zring_remove (zring_t **self_p);

//  Remove first element from the ring and destroy it. Advances
//  self_p to the next element in the ring or NULL if none left.
CZMQ_EXPORT void
    zring_destroy_one (zring_t **self_p);

//  Move first element from the ring to after the destination. Destination may
//  be another ring.
CZMQ_EXPORT void
    zring_move_after (zring_t *self, zring_t *destination);

//  Move first element from the ring to before the destination. Destination
//  may be another ring.
CZMQ_EXPORT void
    zring_move_before (zring_t *self, zring_t *destination);

//  Return first element with item from the ring. If self is NULL
//  return NULL. If terminator is NULL return NULL when there is no
//  element before the next guard. Otherwise return NULL if there is
//  no element that is not a guard before the terminator.
CZMQ_EXPORT zring_t *
    zring_first (zring_t *self, zring_t *terminator);

//  Return next element with item from the ring. If terminator is NULL
//  return NULL when the next element is a guard. Otherwise return
//  NULL if the next element that is no a guard is the terminator.
CZMQ_EXPORT zring_t *
    zring_next (zring_t *last, zring_t *terminator);

//  Return the item of an element of the ring. If self is NULL return
//  NULL. Returns NULL on guard elements.
CZMQ_EXPORT void *
    zring_item (zring_t *self);

//  Set a user-defined deallocator for ring items; by default items are not
//  freed when the ring is destroyed. This is an O(n) operation.
CZMQ_EXPORT czmq_destructor *
    zring_set_destructor (zring_t *self, czmq_destructor destructor);

//  Set a user-defined duplicator for ring items; by default items are not
//  copied when inserted into the ring. This is an O(n) operation.
CZMQ_EXPORT czmq_duplicator *
    zring_set_duplicator (zring_t *self, czmq_duplicator duplicator);

//  Self test of this class
CZMQ_EXPORT void
    zring_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
