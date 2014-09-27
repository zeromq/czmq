/*  =========================================================================
    zhashring - generic type-free doubly linked ring container

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZHASHRING_H_INCLUDED__
#define __ZHASHRING_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new ring container (a ring is a doubly-linked ring)
CZMQ_EXPORT zhashring_t *
    zhashring_new (void);

//  Destroy a ring container
CZMQ_EXPORT void
    zhashring_destroy (zhashring_t **self_p);

//  Prepend an item to the start of the ring, return 0 if OK, else -1.
//  Leaves cursor at newly inserted item.
CZMQ_EXPORT int
    zhashring_prepend (zhashring_t *self, void *item);

//  Append an item to the end of the ring, return 0 if OK, else -1.
//  Leaves cursor at newly inserted item.
CZMQ_EXPORT int
    zhashring_append (zhashring_t *self, void *item);

//  Append an item to the end of the ring, and insert into the ring
//  dictionary, so that you can find the item rapidly using zhashring_lookup.
//  If you do a lot of item searches, this is faster than zhashring_find,
//  which is at worst an O(N) operation. When items leave the ring, they
//  are always removed from the dictionary. Returns 0 on success, -1 if
//  the key already existed in the dictionary, or heap memory ran out.
CZMQ_EXPORT int
    zhashring_insert (zhashring_t *self, const void *key, void *item);
    
//  Find an item in the ring, looking first at the cursor, and then from the
//  first to last item. If a comparator was set on container, calls this to
//  compare each item in the ring with the supplied target item. If none
//  was set, compares the two item pointers for equality. If the item is
//  found, leaves the cursor at the found item. Returns the item if found,
//  else null.
CZMQ_EXPORT void *
    zhashring_find (zhashring_t *self, void *item);

//  Search the ring dictionary for an item, by key. If the item is in the
//  dictionary (via zhashring_insert), then sets the ring cursor to the item,
//  and returns the item value. If not, leaves the cursor unchanged, and
//  returns NULL.
CZMQ_EXPORT void *
    zhashring_lookup (zhashring_t *self, const void *key);

//  Detach an item from the ring, without destroying the item. Searches the
//  ring for the item, always starting with the cursor, if any is set, and
//  then from the start of the ring. If item is null, detaches the item at the
//  cursor, if set. If the item was found and detached, leaves the cursor at
//  the next item, if any, and returns the item. Else, returns null.
CZMQ_EXPORT void *
    zhashring_detach (zhashring_t *self, void *item);

//  Remove an item from the ring, and destroy it, if the item destructor is
//  set. Searches the ring for the item, always starting with the cursor, if
//  any is set, and then from the start of the ring. If item is null, removes
//  the item at the cursor, if set. If the item was found and removed, leaves
//  the cursor at the next item, if any, and returns 0. Else, returns -1.
CZMQ_EXPORT int
    zhashring_remove (zhashring_t *self, void *item);

//  Search the ring dictionary for an item, by key. If the item is in the
//  dictionary (via zhashring_insert), then removes the item from the ring and
//  calls the item destructor, if any is found. Returns 0 if the item was
//  found and removed, else -1 if not found.
CZMQ_EXPORT int
    zhashring_delete (zhashring_t *self, const void *key);
    
//  Delete all items from the ring. If the item destructor is set, calls it
//  on every item.
CZMQ_EXPORT void
    zhashring_purge (zhashring_t *self);

//  Return number of items in the ring
CZMQ_EXPORT size_t
    zhashring_size (zhashring_t *self);

//  Return the item at the head of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the ring is empty.
CZMQ_EXPORT void *
    zhashring_first (zhashring_t *self);

//  Return the item at the tail of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the ring is empty.
CZMQ_EXPORT void *
    zhashring_last (zhashring_t *self);
    
//  Return the next item. At the end of the ring (or in an empty ring),
//  returns NULL. Use repeated zhashring_next () calls to work through the
//  ring from zhashring_first ().
CZMQ_EXPORT void *
    zhashring_next (zhashring_t *self);
    
//  Return the previous item. At the start of the ring (or in an empty ring),
//  returns NULL. Use repeated zhashring_prev () calls to work through the
//  ring backwards from zhashring_last ().
CZMQ_EXPORT void *
    zhashring_prev (zhashring_t *self);

//  Return current item in the ring. If the ring is empty, or the cursor
//  passed the end of the ring, returns NULL. Does not change the cursor.
CZMQ_EXPORT void *
    zhashring_item (zhashring_t *self);

//  Sort the list using the compare function.
//  The sort is not stable, so may reorder items with the same keys.
CZMQ_EXPORT void
    zlist_sort (zlist_t *self, zlist_compare_fn *compare);

//  Make a copy of the ring; items are duplicated if you set a duplicator
//  for the ring, otherwise not. Copying a null reference returns a null
//  reference.
CZMQ_EXPORT zhashring_t *
    zhashring_dup (zhashring_t *self);
    
//  Set a user-defined deallocator for ring items; by default items are not
//  freed when the ring is destroyed.
CZMQ_EXPORT void
    zhashring_set_destructor (zhashring_t *self, czmq_destructor destructor);

//  Set a user-defined duplicator for ring items; by default items are not
//  copied when the ring is duplicated.
CZMQ_EXPORT void
    zhashring_set_duplicator (zhashring_t *self, czmq_duplicator duplicator);

//  Set a user-defined comparator for zhashring_find and zhashring_sort; the
//  method must return -1, 0, or 1 depending on whether item1 is less than,
//  equal to, or greater than, item2.
CZMQ_EXPORT void
    zhashring_set_comparator (zhashring_t *self, czmq_comparator comparator);

//  Self test of this class
CZMQ_EXPORT void
    zhashring_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
