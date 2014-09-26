/*  =========================================================================
    zlist - generic type-free list container

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZLIST_H_INCLUDED__
#define __ZLIST_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Comparison function for zlist_sort method
typedef bool (zlist_compare_fn) (void *item1, void *item2);

// Callback function for zlist_freefn method
typedef void (zlist_free_fn) (void *data);

//  Create a new list container
CZMQ_EXPORT zlist_t *
    zlist_new (void);

//  Destroy a list container
CZMQ_EXPORT void
    zlist_destroy (zlist_t **self_p);

//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zlist_first (zlist_t *self);

//  Return the next item. If the list is empty, returns NULL. To move to
//  the start of the list call zlist_first (). Advances the cursor.
CZMQ_EXPORT void *
    zlist_next (zlist_t *self);

//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zlist_last (zlist_t *self);

//  Return first item in the list, or null, leaves the cursor
CZMQ_EXPORT void *
    zlist_head (zlist_t *self);

//  Return last item in the list, or null, leaves the cursor
CZMQ_EXPORT void *
    zlist_tail (zlist_t *self);

//  Return the current item of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the current item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zlist_item (zlist_t *self);

//  Append an item to the end of the list, return 0 if OK or -1 if this
//  failed for some reason (out of memory). Note that if a duplicator has 
//  been set, this method will also duplicate the item.
CZMQ_EXPORT int
    zlist_append (zlist_t *self, void *item);

//  Push an item to the start of the list, return 0 if OK or -1 if this
//  failed for some reason (out of memory). Note that if a duplicator has
//  been set, this method will also duplicate the item.
CZMQ_EXPORT int
    zlist_push (zlist_t *self, void *item);

//  Pop the item off the start of the list, if any
CZMQ_EXPORT void *
    zlist_pop (zlist_t *self);

//  Remove the specified item from the list if present
CZMQ_EXPORT void
    zlist_remove (zlist_t *self, void *item);

//  Make a copy of list. If the list has autofree set, the copied list will
//  duplicate all items, which must be strings. Otherwise, the list will hold
//  pointers back to the items in the original list.
CZMQ_EXPORT zlist_t *
    zlist_dup (zlist_t *self);

//  Purge all items from list
CZMQ_EXPORT void
zlist_purge (zlist_t *self);

//  Return number of items in the list
CZMQ_EXPORT size_t
    zlist_size (zlist_t *self);

//  Sort the list by ascending key value using a straight ASCII comparison.
//  The sort is not stable, so may reorder items with the same keys.
CZMQ_EXPORT void
    zlist_sort (zlist_t *self, zlist_compare_fn *compare);

//  Set a user-defined destructor for items; by default items are not
//  freed when the list is destroyed.
CZMQ_EXPORT void
    zlist_set_destructor (zlist_t *self, czmq_destructor destructor);

//  Set a user-defined duplicator for items; by default items are not
//  copied when the list is duplicated.
CZMQ_EXPORT void
    zlist_set_duplicator (zlist_t *self, czmq_duplicator duplicator);

//  DEPRECATED by zlist_set_duplicator/zlist_set_destructor
//  Set list for automatic item destruction; item values MUST be strings.
//  By default a list item refers to a value held elsewhere. When you set
//  this, each time you append or push a list item, zlist will take a copy
//  of the string value. Then, when you destroy the list, it will free all
//  item values automatically. If you use any other technique to allocate
//  list values, you must free them explicitly before destroying the list.
//  The usual technique is to pop list items and destroy them, until the
//  list is empty.
CZMQ_EXPORT void
    zlist_autofree (zlist_t *self);

//  DEPRECATED by zlist_set_duplicator/zlist_set_destructor
//  Set a free function for the specified list item. When the item is
//  destroyed, the free function, if any, is called on that item.
//  Use this when list items are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.
CZMQ_EXPORT void *
    zlist_freefn (zlist_t *self, void *item, zlist_free_fn *fn, bool at_tail);

//  Self test of this class
CZMQ_EXPORT void
    zlist_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
