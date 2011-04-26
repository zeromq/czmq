/*  =========================================================================
    zhash.h - ZFL singly-linked hash class

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of czmq, the high-level C binding for 0MQ:
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

#ifndef __ZFL_HASH_H_INCLUDED__
#define __ZFL_HASH_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zhash zhash_t;

//  @interface
//  Callback function for zhash_foreach method
typedef int (zhash_foreach_fn) (char *key, void *item, void *argument);
//  Callback function for zhash_freefn method
typedef void (zhash_free_fn) (void *data);

//  Create a new, empty hash container
zhash_t *
    zhash_new (void);

//  Destroy a hash container and all items in it
void
    zhash_destroy (zhash_t **self_p);

//  Insert an item into the hash container using the specified key
int
    zhash_insert (zhash_t *self, char *key, void *item);

//  Insert or update the item for the specified key
void
    zhash_update (zhash_t *self, char *key, void *item);

//  Destroy the item at the specified key, if any
void
    zhash_delete (zhash_t *self, char *key);

//  Return the item at the specified key, or null
void *
    zhash_lookup (zhash_t *self, char *key);

//  Set a free function for the item at the specified key
void *
    zhash_freefn (zhash_t *self, char *key, zhash_free_fn *free_fn);

//  Return the number of keys/items in the hash table
size_t
    zhash_size (zhash_t *self);

//  Iterate over the hash table and apply the function to each item
int
    zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument);

//  Self test of this class
void
    zhash_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
