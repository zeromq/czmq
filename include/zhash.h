/*  =========================================================================
    zhash - generic type-free hash container

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZHASH_H_INCLUDED__
#define __ZHASH_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zhash_t zhash_t;

//  @interface
//  Callback function for zhash_foreach method
typedef int (zhash_foreach_fn) (const char *key, void *item, void *argument);
//  Callback function for zhash_freefn method
typedef void (zhash_free_fn) (void *data);

//  Create a new, empty hash container
CZMQ_EXPORT zhash_t *
    zhash_new (void);

//  Destroy a hash container and all items in it
CZMQ_EXPORT void
    zhash_destroy (zhash_t **self_p);

//  Insert item into hash table with specified key and item.
//  If key is already present returns -1 and leaves existing item unchanged
//  Returns 0 on success.
CZMQ_EXPORT int
    zhash_insert (zhash_t *self, const char *key, void *item);

//  Update item into hash table with specified key and item.
//  If key is already present, destroys old item and inserts new one.
//  Use free_fn method to ensure deallocator is properly called on item.
CZMQ_EXPORT void
    zhash_update (zhash_t *self, const char *key, void *item);

//  Remove an item specified by key from the hash table. If there was no such
//  item, this function does nothing.
CZMQ_EXPORT void
    zhash_delete (zhash_t *self, const char *key);

//  Return the item at the specified key, or null
CZMQ_EXPORT void *
    zhash_lookup (zhash_t *self, const char *key);

//  Reindexes an item from an old key to a new key. If there was no such
//  item, does nothing. Returns 0 if successful, else -1.
CZMQ_EXPORT int
    zhash_rename (zhash_t *self, const char *old_key, const char *new_key);

//  Set a free function for the specified hash table item. When the item is
//  destroyed, the free function, if any, is called on that item.
//  Use this when hash items are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.
CZMQ_EXPORT void *
    zhash_freefn (zhash_t *self, const char *key, zhash_free_fn *free_fn);

//  Return the number of keys/items in the hash table
CZMQ_EXPORT size_t
    zhash_size (zhash_t *self);

//  Make copy of hash table
CZMQ_EXPORT zhash_t *
    zhash_dup (zhash_t *self);

//  Return keys for items in table
CZMQ_EXPORT zlist_t *
    zhash_keys (zhash_t *self);
    
//  Apply function to each item in the hash table. Items are iterated in no
//  defined order. Stops if callback function returns non-zero and returns
//  final return code from callback function (zero = success).
CZMQ_EXPORT int
    zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument);

//  Add comment to hash table before saving to disk. You can add as many
//  comment lines as you like. These comment lines are discarded when loading
//  the file. If you use a null format, all comments are deleted.
CZMQ_EXPORT void
    zhash_comment (zhash_t *self, const char *format, ...);

//  Save hash table to a text file in name=value format. Hash values must be
//  printable strings; keys may not contain '=' character. Returns 0 if OK,
//  else -1 if a file error occurred.
CZMQ_EXPORT int
    zhash_save (zhash_t *self, const char *filename);

//  Load hash table from a text file in name=value format; hash table must
//  already exist. Hash values must printable strings; keys may not contain
//  '=' character. Returns 0 if OK, else -1 if a file was not readable.
CZMQ_EXPORT int
    zhash_load (zhash_t *self, const char *filename);

//  When a hash table was loaded from a file by zhash_load, this method will
//  reload the file if it has been modified since, and is "stable", i.e. not
//  still changing. Returns 0 if OK, -1 if there was an error reloading the 
//  file.
CZMQ_EXPORT int
    zhash_refresh (zhash_t *self);

//  Set hash for automatic value destruction
CZMQ_EXPORT void
    zhash_autofree (zhash_t *self);
    
//  Serialize hash table to a binary frame that can be sent in a message.
//  The packed format is compatible with the 'dictionary' type defined in
//  rfc.zeromq.org/spec:19/FILEMQ. Comments are not included in the packed
//  data. Item values MUST be strings.
CZMQ_EXPORT zframe_t *
    zhash_pack (zhash_t *self);
    
//  Unpack binary frame into a new hash table. Packed data must follow format
//  defined by zhash_pack. Hash table is set to autofree. An empty frame
//  unpacks to an empty hash table.
CZMQ_EXPORT zhash_t *
    zhash_unpack (zframe_t *frame);

//  Self test of this class
CZMQ_EXPORT void
    zhash_test (int verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
