/*  =========================================================================
    zlist - generic type-free list container

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZLIST_H_INCLUDED__
#define __ZLIST_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zlist_t zlist_t;

//  @interface
//  Comparison function for zlist_sort method
typedef bool (zlist_compare_fn) (void *item1, void *item2);

//  Create a new list container
CZMQ_EXPORT zlist_t *
    zlist_new (void);

//  Destroy a list container
CZMQ_EXPORT void
    zlist_destroy (zlist_t **self_p);

//  Return first item in the list, or null
CZMQ_EXPORT void *
    zlist_first (zlist_t *self);

//  Return last item in the list, or null
CZMQ_EXPORT void *
    zlist_last (zlist_t *self);

//  Return first item in the list, or null, leaves the cursor
CZMQ_EXPORT void *
    zlist_head (zlist_t *self);

//  Return last item in the list, or null, leaves the cursor
CZMQ_EXPORT void *
    zlist_tail (zlist_t *self);

//  Return next item in the list, or null
CZMQ_EXPORT void *
    zlist_next (zlist_t *self);

//  Append an item to the end of the list
CZMQ_EXPORT int
    zlist_append (zlist_t *self, void *item);

//  Push an item to the start of the list
CZMQ_EXPORT int
    zlist_push (zlist_t *self, void *item);

//  Pop the item off the start of the list, if any
CZMQ_EXPORT void *
    zlist_pop (zlist_t *self);

//  Remove the specified item from the list if present
CZMQ_EXPORT void
    zlist_remove (zlist_t *self, void *item);

//  Copy the entire list, return the copy
CZMQ_EXPORT zlist_t *
    zlist_dup (zlist_t *self);

//  Copy the entire list, return the copy (deprecated)
CZMQ_EXPORT zlist_t *
    zlist_copy (zlist_t *self);

//  Return number of items in the list
CZMQ_EXPORT size_t
    zlist_size (zlist_t *self);

//  Sort list
CZMQ_EXPORT void
    zlist_sort (zlist_t *self, zlist_compare_fn *compare);

//  Set list for automatic item destruction
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
