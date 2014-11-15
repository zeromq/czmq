/*  =========================================================================
    zlistx - extended generic list container

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZLISTX_H_INCLUDED__
#define __ZLISTX_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface

CZMQ_EXPORT

//  Create a new, empty list.
CZMQ_EXPORT zlistx_t *
    zlistx_new (void);
    
//  Destroy a list. If an item destructor was specified, all items in the
//  list are automatically destroyed as well.
CZMQ_EXPORT void
    zlistx_destroy (zlistx_t **self_p);
    
//  Add an item to the head of the list. Calls the item duplicator, if any,
//  on the item. Resets cursor to list head. Returns an item handle on
//  success, NULL if memory was exhausted.
CZMQ_EXPORT void *
    zlistx_add_start (zlistx_t *self, void *item);
    
//  Add an item to the tail of the list. Calls the item duplicator, if any,
//  on the item. Resets cursor to list head. Returns an item handle on
//  success, NULL if memory was exhausted.
CZMQ_EXPORT void *
    zlistx_add_end (zlistx_t *self, void *item);
    
//  Return the number of items in the list
CZMQ_EXPORT size_t
    zlistx_size (zlistx_t *self);
    
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zlistx_first (zlistx_t *self);
    
//  Return the next item. At the end of the list (or in an empty list),
//  returns NULL. Use repeated zlistx_next () calls to work through the list
//  from zlistx_first (). First time, acts as zlistx_first().
CZMQ_EXPORT void *
    zlistx_next (zlistx_t *self);

//  Return the previous item. At the start of the list (or in an empty list),
//  returns NULL. Use repeated zlistx_prev () calls to work through the list
//  backwards from zlistx_last (). First time, acts as zlistx_last().
CZMQ_EXPORT void *
    zlistx_prev (zlistx_t *self);
    
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zlistx_last (zlistx_t *self);

//  Returns the value of the item at the cursor, or NULL if the cursor is
//  not pointing to an item.
CZMQ_EXPORT void *
    zlistx_item (zlistx_t *self);

//  Returns the handle of the item at the cursor, or NULL if the cursor is
//  not pointing to an item.
CZMQ_EXPORT void *
    zlistx_handle (zlistx_t *self);

//  Find an item in the list, searching from the start. Uses the item
//  comparator, if any, else compares item values directly. Returns the
//  item handle found, or NULL. Sets the cursor to the found item, if any.
CZMQ_EXPORT void *
    zlistx_find (zlistx_t *self, void *item);

//  Detach an item from the list, using its handle. The item is not modified,
//  and the caller is responsible for destroying it if necessary. If handle is
//  null, detaches the first item on the list. Returns item that was detached,
//  or null if none was.
CZMQ_EXPORT void *
    zlistx_detach (zlistx_t *self, void *handle);

//  Delete an item, using its handle. Calls the item destructor is any is
//  set. If handle is null, deletes the first item on the list.
CZMQ_EXPORT int
    zlistx_delete (zlistx_t *self, void *handle);
    
//  Move an item to the start of the list, via its handle. If sorted is true,
//  the item is inserted before the first item with an equal or higher value,
//  determined by the item comparator (or item value if no comparator is set).
CZMQ_EXPORT void
    zlistx_move_start (zlistx_t *self, void *handle, bool sorted);

//  Move an item to the end of the list, via its handle. If sorted is true,
//  the item is inserted after the last item with an equal or lower value,
//  determined by the item comparator (or item value if no comparator is set).
CZMQ_EXPORT void
    zlistx_move_end (zlistx_t *self, void *handle, bool sorted);

//  Remove all items from the list, and destroy them if the item destructor
//  is set.
CZMQ_EXPORT void
    zlistx_purge (zlistx_t *self);
    
//  Sort the list. If an item comparator was set, calls that to compare
//  items, otherwise compares on item value. The sort is not stable, so may
//  reorder equal items.
CZMQ_EXPORT void
    zlistx_sort (zlistx_t *self);
    
//  Make a copy of the list; items are duplicated if you set a duplicator
//  for the list, otherwise not. Copying a null reference returns a null
//  reference.
CZMQ_EXPORT zlistx_t *
    zlistx_dup (zlistx_t *self);

//  Set a user-defined deallocator for list items; by default items are not
//  freed when the list is destroyed.
CZMQ_EXPORT void
    zlistx_set_destructor (zlistx_t *self, czmq_destructor destructor);
    
//  Set a user-defined duplicator for list items; by default items are not
//  copied when the list is duplicated.
CZMQ_EXPORT void
    zlistx_set_duplicator (zlistx_t *self, czmq_duplicator duplicator);

//  Set a user-defined comparator for zlistx_find and zlistx_sort; the method
//  must return -1, 0, or 1 depending on whether item1 is less than, equal to,
//  or greater than, item2.
CZMQ_EXPORT void
    zlistx_set_comparator (zlistx_t *self, czmq_comparator comparator);

//  Runs selftest of class
CZMQ_EXPORT void
    zlistx_test (int verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
