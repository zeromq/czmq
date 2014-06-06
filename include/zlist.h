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

//  Return first item in the list, or null
CZMQ_EXPORT void *
    zlist_first (zlist_t *self);

//  Return next item in the list, or null
CZMQ_EXPORT void *
    zlist_next (zlist_t *self);

//  Return last item in the list, or null
CZMQ_EXPORT void *
    zlist_last (zlist_t *self);

//  Return first item in the list, or null, leaves the cursor
CZMQ_EXPORT void *
    zlist_head (zlist_t *self);

//  Return last item in the list, or null, leaves the cursor
CZMQ_EXPORT void *
    zlist_tail (zlist_t *self);

//  Append an item to the end of the list, return 0 if OK
//  or -1 if this failed for some reason (out of memory).
CZMQ_EXPORT int
    zlist_append (zlist_t *self, void *item);

//  Push an item to the start of the list, return 0 if OK
//  or -1 if this failed for some reason (out of memory).
CZMQ_EXPORT int
    zlist_push (zlist_t *self, void *item);

//  Pop the item off the start of the list, if any
CZMQ_EXPORT void *
    zlist_pop (zlist_t *self);

//  Remove the specified item from the list if present
CZMQ_EXPORT void
    zlist_remove (zlist_t *self, void *item);

// Add an explicit free function to the item including a hint as to
// whether it can be found at the tail
CZMQ_EXPORT void *
    zlist_freefn (zlist_t *self, void *item, zlist_free_fn *fn, bool at_tail);

//  Make a copy of list. If the list has autofree set, the copied list will
//  duplicate all items, which must be strings. Otherwise, the list will hold
//  pointers back to the items in the original list.
CZMQ_EXPORT zlist_t *
    zlist_dup (zlist_t *self);

//  Return number of items in the list
CZMQ_EXPORT size_t
    zlist_size (zlist_t *self);

//  Sort the list by ascending key value using a straight ASCII comparison.
//  The sort is not stable, so may reorder items with the same keys.
CZMQ_EXPORT void
    zlist_sort (zlist_t *self, zlist_compare_fn *compare);

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

//  Self test of this class
CZMQ_EXPORT void
    zlist_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
