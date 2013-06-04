/*  =========================================================================
    ztree - generic red-black tree container

    -------------------------------------------------------------------------
    Copyright (c) 2013 Stephen Hemminger <stephen@networkplumber.org>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

    This code is derived from Redblack balanced tree algorithm
    Copyright (C) Damian Ivereigh 2000
    Which was licensed under GPL version 2 (or later) license.

    =========================================================================
*/

#ifndef __ZTREE_H_INCLUDED__
#define __ZTREE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _ztree_t ztree_t;

//  @interface
//  Callback function for ztee_walk method
typedef int (ztree_walk_fn) (const char *key, void *value, void *argument);

//  Callback function for ztree_freefn method
typedef void (ztree_free_fn) (void *data);

//  Comparison function for ztree ordering
//  returns -1 for key1 < key2, 0 if key1 == key 2, 1 for key1 > key2
//  if key's are strings okay to use strcmp as function
typedef int (ztree_compare_fn) (const char *key1, const char *key2);

//  Create a new tree container
CZMQ_EXPORT ztree_t *
    ztree_new (ztree_compare_fn *compare_func);

//  Destroy a tree container
CZMQ_EXPORT void
    ztree_destroy (ztree_t **self_p);

//  Insert node into tree with specified key and value
//  If key is already present returns -1 and leaves existing node unchanged
//  Returns 0 on success.
CZMQ_EXPORT int
    ztree_insert (ztree_t *self, const char *key, void *value);

//  Update node in tree with specified key and value.
//  If key is already present, destroys old value and inserts new one.
//  Use free_fn method to ensure deallocator is properly called on value.
CZMQ_EXPORT void
    ztree_update (ztree_t *self, const char *key, void *value);

//  Remove a node specified by key from the tree. If there was no such
//  node, this function does nothing.
CZMQ_EXPORT void
    ztree_delete (ztree_t *self, const char *key);

//  Return the value at the specified key, or null
CZMQ_EXPORT void *
    ztree_lookup (ztree_t *self, const char *key);

//  Set a free function for the specified tree node. When the value is
//  destroyed, the free function, if any, is called on that node.
//  Use this when tree values are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.
CZMQ_EXPORT void *
    ztree_freefn (ztree_t *self, const char *key, ztree_free_fn *free_fn);

//  Return the number of keys/values in the tree
CZMQ_EXPORT size_t
    ztree_size (ztree_t *self);

//  Return keys for nodes in tree
CZMQ_EXPORT zlist_t *
    ztree_keys (ztree_t *self);

//  Copy the entire tree, return the copy
CZMQ_EXPORT ztree_t *
    ztree_dup (ztree_t *self);

// Walk the tree depth-first, left-to-right order.
// Stops if callback function returns non-zero and returns
// final return code from callback function (zero = success).
CZMQ_EXPORT int
    ztree_walk (ztree_t *self, ztree_walk_fn *callback, void *argument);

//  Save tree to a text file in name=value format. Values must be
//  printable strings; keys may not contain '=' character. Returns 0 if OK,
//  else -1 if a file error occurred.
CZMQ_EXPORT int
    ztree_save (ztree_t *self, const char *filename);

//  Load tree from a text file in name=value format; tree must
//  already exist. Tree values must printable strings; keys may not contain
//  '=' character. Returns 0 if OK, else -1 if a file was not readable.
CZMQ_EXPORT int
    ztree_load (ztree_t *self, const char *filename);

//  Set tree for automatic value destruction
CZMQ_EXPORT void
    ztree_autofree (ztree_t *self);

//  Self test of this class
CZMQ_EXPORT void
    ztree_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
