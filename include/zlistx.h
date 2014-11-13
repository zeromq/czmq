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
//  on the item. Leaves cursor at newly inserted item. Returns 0 on success,
//  -1 if failed due to lack of memory.
CZMQ_EXPORT int
    zlistx_add_start (zlistx_t *self, void *item);
    
//  Add an item to the tail of the list. Calls the item duplicator, if any,
//  on the item. Leaves cursor at newly inserted item. Returns 0 on success,
//  -1 if failed due to lack of memory.
CZMQ_EXPORT int
    zlistx_add_end (zlistx_t *self, void *item);
    
//  Add an item before the current cursor, if any. If no cursor is set, adds
//  to the start of the list. Calls the item duplicator, if any, on the item.
//  Leaves cursor at newly inserted item. Returns 0 on success, -1 if failed
//  due to lack of memory.
CZMQ_EXPORT int
    zlistx_add_before (zlistx_t *self, void *item);

//  Add an item after the current cursor, if any. If no cursor is set, adds
//  to the end of the list. Calls the item duplicator, if any, on the item.
//  Leaves cursor at newly inserted item. Returns 0 on success, -1 if failed
//  due to lack of memory.
CZMQ_EXPORT int
    zlistx_add_after (zlistx_t *self, void *item);

//  Return the number of items in the list
CZMQ_EXPORT size_t
    zlistx_size (zlistx_t *self);
    
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zlistx_first (zlistx_t *self);
    
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.
CZMQ_EXPORT void *
    zlistx_last (zlistx_t *self);
    
//  Return the next item. At the end of the list (or in an empty list),
//  returns NULL. Use repeated zlistx_next () calls to work through the list
//  from zlistx_first ().
CZMQ_EXPORT void *
    zlistx_next (zlistx_t *self);
    
//  Return the previous item. At the start of the list (or in an empty list),
//  returns NULL. Use repeated zlistx_prev () calls to work through the list
//  backwards from zlistx_last ().
CZMQ_EXPORT void *
    zlistx_prev (zlistx_t *self);
    
//  Find an item in the list, looking first at the cursor, and then from
//  the first to last item. If an item comparator was set, calls this to
//  compare each item in the list with the supplied target item. If none
//  was set, compares the two item pointers for equality. If the item is
//  found, leaves the cursor at the found item. Returns the item if found,
//  else null.
CZMQ_EXPORT void *
    zlistx_find (zlistx_t *self, void *item);
    
//  Remove an item from the list, and destroy it, if the item destructor is
//  set. Searches the list for the item, always starting with the cursor, if
//  any is set, and then from the start of the list. If item is null, removes
//  the first item, if any. If the item was found and detached, returns the
//  0, else returns -1.
CZMQ_EXPORT int
    zlistx_delete (zlistx_t *self, void *item);
    
//  Detach an item from the list, without destroying the item. Searches the
//  list for the item, always starting with the cursor, if any is set, and
//  then from the start of the list. If item is null, detaches the first item
//  in the list, if any. If the item was found and detached, returns the item.
//  Else, returns null.
CZMQ_EXPORT void *
    zlistx_detach (zlistx_t *self, void *item);
    
//  Remove all items from the list, and destroy them if the item destructor
//  is set.
CZMQ_EXPORT void
    zlistx_purge (zlistx_t *self);
    
//  Return current item in the list. If the list is empty, or the cursor
//  passed the end of the list, returns NULL. Does not change the cursor.
CZMQ_EXPORT void *
    zlistx_item (zlistx_t *self);
    
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
