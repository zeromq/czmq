/*  =========================================================================
    zhashx - extended generic hash container

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zhashx is an extended hash table container with more functionality than
    zhash, its simpler cousin.
@discuss
    The hash table always has a size that is prime and roughly doubles its
    size when 75% full. In case of hash collisions items are chained in a
    linked list. The hash table size is increased slightly (up to 5 times
    before roughly doubling the size) when an overly long chain (between 1
    and 63 items depending on table size) is detected.
@end
*/

#include "../include/czmq.h"

//  Hash table performance parameters

#define INITIAL_PRIME    0    //  Initial size in items (index into primes)
#define GROWTH_FACTOR    5    //  Increase after splitting (index into primes)
#define LOAD_FACTOR     75    //  Percent loading before splitting
#define INITIAL_CHAIN    1    //  Initial chaining limit
#define CHAIN_GROWS      1    //  Increase after splitting (chaining limit)

#include "zhash_primes.inc"


//  Hash item, used internally only

typedef struct _item_t {
    void *value;                //  Opaque item value
    struct _item_t *next;       //  Next item in the hash slot
    qbyte index;                //  Index of item in table
    const void *key;            //  Item's original key
    //  Supporting deprecated v2 functionality; we can't quite replace
    //  this with strdup/zstr_free as zhashx_insert also uses autofree.
    zhashx_free_fn *free_fn;     //  Value free function if any
} item_t;


//  ---------------------------------------------------------------------
//  Structure of our class

struct _zhashx_t {
    size_t size;                //  Current size of hash table
    uint prime_index;           //  Current prime number used as limit
    uint chain_limit;           //  Current limit on chain length
    item_t **items;             //  Array of items
    size_t cached_index;        //  Avoids duplicate hash calculations
    size_t cursor_index;        //  For first/next iteration
    item_t *cursor_item;        //  For first/next iteration
    const void *cursor_key;     //  After first/next call, points to key
    zlistx_t *comments;         //  File comments, if any
    time_t modified;            //  Set during zhashx_load
    char *filename;             //  Set during zhashx_load
    //  Function callbacks for duplicating and destroying items, if any
    czmq_duplicator *duplicator;
    czmq_destructor *destructor;
    //  Function callbacks for duplicating and destroying keys, if any
    czmq_duplicator *key_duplicator;
    czmq_destructor *key_destructor;
    czmq_comparator *key_comparator;
    //  Custom hash function
    zhashx_hash_fn *hasher;
};

//  Local helper functions
static item_t *s_item_lookup (zhashx_t *self, const void *key);
static item_t *s_item_insert (zhashx_t *self, const void *key, void *value);
static void s_item_destroy (zhashx_t *self, item_t *item, bool hard);


//  --------------------------------------------------------------------------
//  Modified Bernstein hashing function

static size_t
s_bernstein_hash (const void *key)
{
    const char *pointer = (const char *) key;
    size_t key_hash = 0;
    while (*pointer)
        key_hash = 33 * key_hash ^ *pointer++;
    return key_hash;
}


//  --------------------------------------------------------------------------
//  Hash table constructor

zhashx_t *
zhashx_new (void)
{
    zhashx_t *self = (zhashx_t *) zmalloc (sizeof (zhashx_t));
    if (self) {
        self->prime_index = INITIAL_PRIME;
        self->chain_limit = INITIAL_CHAIN;
        size_t limit = primes [self->prime_index];
        self->items = (item_t **) zmalloc (sizeof (item_t *) * limit);
        if (self->items) {
            self->hasher = s_bernstein_hash;
            self->key_destructor = (czmq_destructor *) zstr_free;
            self->key_duplicator = (czmq_duplicator *) strdup;
            self->key_comparator = (czmq_comparator *) strcmp;
        }
        else
            zhashx_destroy (&self);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Purge all items from a hash table

static void
s_purge (zhashx_t *self)
{
    uint index;
    size_t limit = primes [self->prime_index];

    for (index = 0; index < limit; index++) {
        //  Destroy all items in this hash bucket
        item_t *cur_item = self->items [index];
        while (cur_item) {
            item_t *next_item = cur_item->next;
            s_item_destroy (self, cur_item, true);
            cur_item = next_item;
        }
        self->items [index] = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Hash table destructor

void
zhashx_destroy (zhashx_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zhashx_t *self = *self_p;
        if (self->items) {
            s_purge (self);
            free (self->items);
        }
        zlistx_destroy (&self->comments);
        free (self->filename);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Destroy item in hash table, item must exist in table

static void
s_item_destroy (zhashx_t *self, item_t *item, bool hard)
{
    //  Find previous item since it's a singly-linked list
    item_t *cur_item = self->items [item->index];
    item_t **prev_item = &(self->items [item->index]);
    while (cur_item) {
        if (cur_item == item)
            break;
        prev_item = &(cur_item->next);
        cur_item = cur_item->next;
    }
    assert (cur_item);
    *prev_item = item->next;
    self->size--;
    if (hard) {
        if (self->destructor)
            (self->destructor)(&item->value);
        else
        if (item->free_fn)
            (item->free_fn)(item->value);

        self->cursor_item = NULL;
        self->cursor_key = NULL;

        if (self->key_destructor)
            (self->key_destructor)((void **) &item->key);
        free (item);
    }
}


//  --------------------------------------------------------------------------
//  Rehash hash table with specified new prime index
//  Returns 0 on success, or -1 on failure (insufficient memory)

static int
s_zhashx_rehash (zhashx_t *self, uint new_prime_index)
{
    assert (self);
    assert (new_prime_index < sizeof (primes));

    size_t limit = primes [self->prime_index];
    size_t new_limit = primes [new_prime_index];
    item_t **new_items = (item_t **) zmalloc (sizeof (item_t *) * new_limit);
    if (!new_items)
        return -1;

    //  Move all items to the new hash table, rehashing to
    //  take into account new hash table limit
    size_t index;
    for (index = 0; index < limit; index++) {
        item_t *cur_item = self->items [index];
        while (cur_item) {
            item_t *next_item = cur_item->next;
            size_t new_index = self->hasher (cur_item->key);
            new_index %= new_limit;
            cur_item->index = new_index;
            cur_item->next = new_items [new_index];
            new_items [new_index] = cur_item;
            cur_item = next_item;
        }
    }
    //  Destroy old hash table
    free (self->items);
    self->items = new_items;
    self->prime_index = new_prime_index;
    return 0;
}


//  --------------------------------------------------------------------------
//  Insert item into hash table with specified key and item. Returns 0 on
//  success. If the key is already present, or the process heap memory ran
//  out, returns -1 and leaves existing item unchanged. Sets the hash cursor
//  to the item, if found.

int
zhashx_insert (zhashx_t *self, const void *key, void *value)
{
    assert (self);
    assert (key);

    //  If we're exceeding the load factor of the hash table,
    //  resize it according to the growth factor
    size_t limit = primes [self->prime_index];
    if (self->size >= limit * LOAD_FACTOR / 100) {
        //  Create new hash table
        uint new_prime_index = self->prime_index + GROWTH_FACTOR;
        if (s_zhashx_rehash (self, new_prime_index))
            return -1;
        self->chain_limit += CHAIN_GROWS;
    }
    return s_item_insert (self, key, value) ? 0 : -1;
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Insert new item into hash table, returns item
//  If item already existed, returns NULL
//  Sets the hash cursor to the item, if found.

static item_t *
s_item_insert (zhashx_t *self, const void *key, void *value)
{
    //  Check that item does not already exist in hash table
    //  Leaves self->cached_index with calculated hash item
    item_t *item = s_item_lookup (self, key);
    if (item == NULL) {
        item = (item_t *) zmalloc (sizeof (item_t));
        if (!item)
            return NULL;

        //  If necessary, take duplicate of item key
        if (self->key_duplicator)
            item->key = (self->key_duplicator)((void *) key);
        else
            item->key = key;

        //  If necessary, take duplicate of item value
        if (self->duplicator)
            item->value = (self->duplicator)(value);
        else
            item->value = value;

        item->index = self->cached_index;

        //  Insert into start of bucket list
        item->next = self->items [self->cached_index];
        self->items [self->cached_index] = item;
        self->size++;
        self->cursor_item = item;
        self->cursor_key = item->key;
    }
    else
        item = NULL;            //  Signal duplicate insertion

    return item;
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Lookup item in hash table, returns item or NULL

static item_t *
s_item_lookup (zhashx_t *self, const void *key)
{
    //  Look in bucket list for item by key
    size_t limit = primes [self->prime_index];
    self->cached_index = self->hasher (key) % limit;
    item_t *item = self->items [self->cached_index];
    uint len = 0;
    while (item) {
        if ((self->key_comparator)(item->key, key) == 0)
            break;
        item = item->next;
        ++len;
    }
    if (len > self->chain_limit) {
        //  Create new hash table
        uint new_prime_index = self->prime_index + GROWTH_FACTOR;
        if (s_zhashx_rehash (self, new_prime_index))
            return NULL;
        limit = primes [self->prime_index];
        self->cached_index = self->hasher (key) % limit;
    }
    return item;
}


//  --------------------------------------------------------------------------
//  Update or insert item into hash table with specified key and item. If the
//  key is already present, destroys old item and inserts new one. If you set
//  a container item destructor, this is called on the old value. If the key
//  was not already present, inserts a new item. Sets the hash cursor to the
//  new item.

void
zhashx_update (zhashx_t *self, const void *key, void *value)
{
    assert (self);
    assert (key);

    item_t *item = s_item_lookup (self, key);
    if (item) {
        if (self->destructor)
            (self->destructor)(&item->value);
        else
        if (item->free_fn)
            (item->free_fn)(item->value);

        //  If necessary, take duplicate of item value
        if (self->duplicator)
            item->value = (self->duplicator)(value);
        else
            item->value = value;
    }
    else
        zhashx_insert (self, key, value);
}


//  --------------------------------------------------------------------------
//  Remove an item specified by key from the hash table. If there was no such
//  item, this function does nothing.

void
zhashx_delete (zhashx_t *self, const void *key)
{
    assert (self);
    assert (key);

    item_t *item = s_item_lookup (self, key);
    if (item)
        s_item_destroy (self, item, true);
}


//  --------------------------------------------------------------------------
//  Delete all items from the hash table. If the key destructor is
//  set, calls it on every key. If the item destructor is set, calls
//  it on every item.
void
zhashx_purge (zhashx_t *self)
{
    assert (self);
    s_purge (self);

    if (self->prime_index > INITIAL_PRIME) {
        // Try to shrink hash table
        size_t limit = primes [INITIAL_PRIME];
        item_t **items =
            (item_t **) zmalloc (sizeof (item_t *) * limit);
        if (items) {
            free (self->items);
            self->prime_index = INITIAL_PRIME;
            self->chain_limit = INITIAL_CHAIN;
            self->items = items;
        }
    }
}


//  --------------------------------------------------------------------------
//  Look for item in hash table and return its item, or NULL. Sets the hash
//  cursor to the item, if found.

void *
zhashx_lookup (zhashx_t *self, const void *key)
{
    assert (self);
    assert (key);

    item_t *item = s_item_lookup (self, key);
    if (item) {
        self->cursor_item = item;
        self->cursor_key = item->key;
        return item->value;
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Reindexes an item from an old key to a new key. If there was no such
//  item, does nothing. If the new key already exists, deletes old item.
//  Sets the item cursor to the renamed item.

int
zhashx_rename (zhashx_t *self, const void *old_key, const void *new_key)
{
    item_t *old_item = s_item_lookup (self, old_key);
    item_t *new_item = s_item_lookup (self, new_key);
    if (old_item && !new_item) {
        s_item_destroy (self, old_item, false);
        if (self->key_destructor)
            (self->key_destructor)((void **) &old_item->key);

        if (self->key_duplicator)
            old_item->key = (self->key_duplicator)(new_key);
        else
            old_item->key = new_key;

        old_item->index = self->cached_index;
        old_item->next = self->items [self->cached_index];
        self->items [self->cached_index] = old_item;
        self->size++;
        self->cursor_item = old_item;
        self->cursor_key = old_item->key;
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Set a free function for the specified hash table item. When the item is
//  destroyed, the free function, if any, is called on that item.
//  Use this when hash items are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.

void *
zhashx_freefn (zhashx_t *self, const void *key, zhashx_free_fn *free_fn)
{
    assert (self);
    assert (key);

    item_t *item = s_item_lookup (self, key);
    if (item) {
        item->free_fn = free_fn;
        return item->value;
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return size of hash table

size_t
zhashx_size (zhashx_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Return a zlistx_t containing the keys for the items in the
//  table. Uses the key_duplicator to duplicate all keys and sets the
//  key_destructor as destructor for the list.

zlistx_t *
zhashx_keys (zhashx_t *self)
{
    assert (self);
    zlistx_t *keys = zlistx_new ();
    if (!keys)
        return NULL;
    zlistx_set_destructor (keys, self->key_destructor);
    zlistx_set_duplicator (keys, self->key_duplicator);

    uint index;
    size_t limit = primes [self->prime_index];
    for (index = 0; index < limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            if (zlistx_add_end (keys, (void *) item->key) == NULL) {
                zlistx_destroy (&keys);
                return NULL;
            }
            item = item->next;
        }
    }
    return keys;
}


//  --------------------------------------------------------------------------
//  Simple iterator; returns first item in hash table, in no given order,
//  or NULL if the table is empty. This method is simpler to use than the
//  foreach() method, which is deprecated. NOTE: do NOT modify the table
//  while iterating.

void *
zhashx_first (zhashx_t *self)
{
    assert (self);
    //  Point to before or at first item
    self->cursor_index = 0;
    self->cursor_item = self->items [self->cursor_index];
    //  Now scan forwards to find it, leave cursor after item
    return zhashx_next (self);
}


//  --------------------------------------------------------------------------
//  Simple iterator; returns next item in hash table, in no given order,
//  or NULL if the last item was already returned. Use this together with
//  zhashx_first() to process all items in a hash table. If you need the
//  items in sorted order, use zhashx_keys() and then zlistx_sort(). NOTE:
//  do NOT modify the table while iterating.

void *
zhashx_next (zhashx_t *self)
{
    assert (self);
    //  Scan forward from cursor until we find an item
    size_t limit = primes [self->prime_index];
    while (self->cursor_item == NULL) {
        if (self->cursor_index < limit - 1)
            self->cursor_index++;
        else
            return NULL;        //  At end of table

        //  Get first item in next bucket
        self->cursor_item = self->items [self->cursor_index];
    }
    //  We have an item, so return it, and bump past it
    assert (self->cursor_item);
    item_t *item = self->cursor_item;
    self->cursor_key = item->key;
    self->cursor_item = self->cursor_item->next;
    return item->value;
}


//  --------------------------------------------------------------------------
//  After a successful insert, update, or first/next method, returns the key
//  for the item that was returned. You may not modify or deallocate
//  the key, and it lasts as long as the item in the hash.
//  After an unsuccessful first/next, returns NULL.

const void *
zhashx_cursor (zhashx_t *self)
{
    assert (self);
    return self->cursor_key;
}


//  --------------------------------------------------------------------------
//  Add a comment to hash table before saving to disk. You can add as many
//  comment lines as you like. These comment lines are discarded when loading
//  the file. If you use a null format, all comments are deleted.
//  FIXME: return 0 on success, -1 on error

void
zhashx_comment (zhashx_t *self, const char *format, ...)
{
    if (format) {
        if (!self->comments) {
            self->comments = zlistx_new ();
            if (!self->comments)
                return;
            zlistx_set_destructor (self->comments, (czmq_destructor *) zstr_free);
        }
        va_list argptr;
        va_start (argptr, format);
        char *string = zsys_vprintf (format, argptr);
        va_end (argptr);
        if (string)
            zlistx_add_end (self->comments, string);
    }
    else
        zlistx_destroy (&self->comments);
}


//  --------------------------------------------------------------------------
//  Save hash table to a text file in name=value format
//  Hash values must be printable strings.
//  Returns 0 if OK, else -1 if a file error occurred

int
zhashx_save (zhashx_t *self, const char *filename)
{
    assert (self);

    FILE *handle = fopen (filename, "w");
    if (!handle)
        return -1;              //  Failed to create file

    if (self->comments) {
        char *comment = (char *) zlistx_first (self->comments);
        while (comment) {
            fprintf (handle, "#   %s\n", comment);
            comment = (char *) zlistx_next (self->comments);
        }
        fprintf (handle, "\n");
    }
    uint index;
    size_t limit = primes [self->prime_index];
    for (index = 0; index < limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            fprintf (handle, "%s=%s\n", (char *) item->key, (char *) item->value);
            item = item->next;
        }
    }
    fclose (handle);
    return 0;
}


//  --------------------------------------------------------------------------
//  Load hash table from a text file in name=value format; hash table must
//  already exist. Hash values must printable strings.
//  Returns 0 if OK, else -1 if a file was not readable.

int
zhashx_load (zhashx_t *self, const char *filename)
{
    assert (self);
    zhashx_autofree (self);

    //  Whether or not file exists, we'll track the filename and last
    //  modification date (0 for unknown files), so that zhashx_refresh ()
    //  will always work after zhashx_load (), to load a newly-created
    //  file.

    //  Take copy of filename in case self->filename is same string.
    char *filename_copy = strdup (filename);
    if (filename_copy) {
        free (self->filename);
        self->filename = filename_copy;
        self->modified = zsys_file_modified (self->filename);
        FILE *handle = fopen (self->filename, "r");
        if (handle) {
            char *buffer = (char *) zmalloc (1024);
            if (buffer) {
                while (fgets (buffer, 1024, handle)) {
                    //  Skip lines starting with "#" or that do not look like
                    //  name=value data.
                    char *equals = strchr (buffer, '=');
                    if (buffer [0] == '#' || equals == buffer || !equals)
                        continue;

                    //  Buffer may end in newline, which we don't want
                    if (buffer [strlen (buffer) - 1] == '\n')
                        buffer [strlen (buffer) - 1] = 0;
                    *equals++ = 0;
                    zhashx_update (self, buffer, equals);
                }
                free (buffer);
            }
            else {
                fclose (handle);
                return -1; //  Out of memory
            }
            fclose (handle);
        }
        else
            return -1; //  Failed to open file for reading
    }
    else
        return -1; //  Out of memory

    return 0;
}


//  --------------------------------------------------------------------------
//  When a hash table was loaded from a file by zhashx_load, this method will
//  reload the file if it has been modified since, and is "stable", i.e. not
//  still changing. Returns 0 if OK, -1 if there was an error reloading the
//  file.

int
zhashx_refresh (zhashx_t *self)
{
    assert (self);

    if (self->filename) {
        if (  zsys_file_modified (self->filename) > self->modified
           && zsys_file_stable (self->filename)) {
            //  Empty the hash table; code is copied from zhashx_destroy
            uint index;
            size_t limit = primes [self->prime_index];
            for (index = 0; index < limit; index++) {
                //  Destroy all items in this hash bucket
                item_t *cur_item = self->items [index];
                while (cur_item) {
                    item_t *next_item = cur_item->next;
                    s_item_destroy (self, cur_item, true);
                    cur_item = next_item;
                }
            }
            zhashx_load (self, self->filename);
        }
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Serialize hash table to a binary frame that can be sent in a message.
//  The packed format is compatible with the 'dictionary' type defined in
//  http://rfc.zeromq.org/spec:35/FILEMQ, and implemented by zproto:
//
//     ; A list of name/value pairs
//     dictionary      = dict-count *( dict-name dict-value )
//     dict-count      = number-4
//     dict-value      = longstr
//     dict-name       = string
//
//     ; Strings are always length + text contents
//     longstr         = number-4 *VCHAR
//     string          = number-1 *VCHAR
//
//     ; Numbers are unsigned integers in network byte order
//     number-1        = 1OCTET
//     number-4        = 4OCTET
//
//  Comments are not included in the packed data. Item values MUST be
//  strings.

zframe_t *
zhashx_pack (zhashx_t *self)
{
    assert (self);

    //  First, calculate packed data size
    size_t frame_size = 4;      //  Dictionary size, number-4
    uint index;
    size_t limit = primes [self->prime_index];
    for (index = 0; index < limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            //  We store key as short string
            frame_size += 1 + strlen ((char *) item->key);
            //  We store value as long string
            frame_size += 4 + strlen ((char *) item->value);
            item = item->next;
        }
    }
    //  Now serialize items into the frame
    zframe_t *frame = zframe_new (NULL, frame_size);
    if (!frame)
        return NULL;
    byte *needle = zframe_data (frame);
    //  Store size as number-4
    *(uint32_t *) needle = htonl ((uint32_t) self->size);
    needle += 4;
    for (index = 0; index < limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            //  Store key as string
            *needle++ = (byte) strlen ((char *) item->key);
            memcpy (needle, item->key, strlen ((char *) item->key));
            needle += strlen ((char *) item->key);

            //  Store value as longstr
            *(uint32_t *) needle = htonl (strlen ((char *) item->value));
            needle += 4;
            memcpy (needle, (char *) item->value, strlen ((char *) item->value));
            needle += strlen ((char *) item->value);
            item = item->next;
        }
    }
    return frame;
}


//  --------------------------------------------------------------------------
//  Unpack binary frame into a new hash table. Packed data must follow format
//  defined by zhashx_pack. Hash table is set to autofree. An empty frame
//  unpacks to an empty hash table.

zhashx_t *
zhashx_unpack (zframe_t *frame)
{
    zhashx_t *self = zhashx_new ();
    if (!self)
        return NULL;
    assert (frame);
    if (zframe_size (frame) < 4)
        return self;            //  Arguable...

    byte *needle = zframe_data (frame);
    byte *ceiling = needle + zframe_size (frame);
    size_t nbr_items = ntohl (*(uint32_t *) needle);
    needle += 4;
    while (nbr_items && needle < ceiling) {
        //  Get key as string
        size_t key_size = *needle++;
        if (needle + key_size <= ceiling) {
            char key [256];
            memcpy (key, needle, key_size);
            key [key_size] = 0;
            needle += key_size;

            //  Get value as longstr
            if (needle + 4 <= ceiling) {
                size_t value_size = ntohl (*(uint32_t *) needle);
                needle += 4;
                //  Be wary of malformed frames
                if (needle + value_size <= ceiling) {
                    char *value = (char *) zmalloc (value_size + 1);
                    if (!value) {
                        zhashx_destroy (&self);
                        return NULL;
                    }
                    memcpy (value, needle, value_size);
                    value [value_size] = 0;
                    needle += value_size;

                    //  Hash takes ownership of value
                    if (zhashx_insert (self, key, value)) {
                        zhashx_destroy (&self);
                        break;
                    }
                }
            }
        }
    }
    //  Hash will free values in destructor
    if (self)
        zhashx_autofree (self);
    return self;
}


//  --------------------------------------------------------------------------
//  Make a copy of the list; items are duplicated if you set a duplicator
//  for the list, otherwise not. Copying a null reference returns a null
//  reference. Note that this method's behavior changed slightly for CZMQ
//  v3.x, as it does not set nor respect autofree. It does however let you
//  duplicate any hash table safely. The old behavior is in zhashx_dup_v2.

zhashx_t *
zhashx_dup (zhashx_t *self)
{
    if (!self)
        return NULL;

    zhashx_t *copy = zhashx_new ();
    if (copy) {
        copy->destructor = self->destructor;
        copy->duplicator = self->duplicator;
        uint index;
        size_t limit = primes [self->prime_index];
        for (index = 0; index < limit; index++) {
            item_t *item = self->items [index];
            while (item) {
                if (zhashx_insert (copy, item->key, item->value)) {
                    zhashx_destroy (&copy);
                    break;
                }
                item = item->next;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Set a user-defined deallocator for hash items; by default items are not
//  freed when the hash is destroyed.

void
zhashx_set_destructor (zhashx_t *self, czmq_destructor destructor)
{
    assert (self);
    self->destructor = destructor;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for hash items; by default items are not
//  copied when the hash is duplicated.

void
zhashx_set_duplicator (zhashx_t *self, czmq_duplicator duplicator)
{
    assert (self);
    self->duplicator = duplicator;
}


//  --------------------------------------------------------------------------
//  Set a user-defined deallocator for keyss; by default keys are
//  freed when the hash is destroyed by calling free().

void
zhashx_set_key_destructor (zhashx_t *self, czmq_destructor destructor)
{
    assert (self);
    self->key_destructor = destructor;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for keys; by default keys are
//  duplicated by calling strdup().

void
zhashx_set_key_duplicator (zhashx_t *self, czmq_duplicator duplicator)
{
    assert (self);
    self->key_duplicator = duplicator;
}


//  --------------------------------------------------------------------------
//  Set a user-defined comparator for keys; by default keys are
//  compared using streq.

void
zhashx_set_key_comparator (zhashx_t *self, czmq_comparator comparator)
{
    assert (self);
    assert (comparator != NULL);
    self->key_comparator = comparator;
}


//  --------------------------------------------------------------------------
//  Set a user-defined hash function for keys; by default keys are
//  hashed by a modified Bernstein hashing function.

void
zhashx_set_key_hasher (zhashx_t *self, zhashx_hash_fn hasher)
{
    assert (self);
    self->hasher = hasher;
}


//  --------------------------------------------------------------------------
//  DEPRECATED by zhashx_dup
//  Make copy of hash table; if supplied table is null, returns null.
//  Does not copy items themselves. Rebuilds new table so may be slow on
//  very large tables. NOTE: only works with item values that are strings
//  since there's no other way to know how to duplicate the item value.

zhashx_t *
zhashx_dup_v2 (zhashx_t *self)
{
    if (!self)
        return NULL;

    zhashx_t *copy = zhashx_new ();
    if (copy) {
        zhashx_autofree (copy);
        uint index;
        size_t limit = primes [self->prime_index];
        for (index = 0; index < limit; index++) {
            item_t *item = self->items [index];
            while (item) {
                if (zhashx_insert (copy, item->key, item->value)) {
                    zhashx_destroy (&copy);
                    break;
                }
                item = item->next;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as clumsy -- use set_destructor instead
//  Set hash for automatic value destruction

void
zhashx_autofree (zhashx_t *self)
{
    assert (self);
    zhashx_set_destructor (self, (czmq_destructor *) zstr_free);
    zhashx_set_duplicator (self, (czmq_duplicator *) strdup);
}


//  --------------------------------------------------------------------------
//  DEPRECATED as clumsy -- use zhashx_first/_next instead
//  Apply function to each item in the hash table. Items are iterated in no
//  defined order.  Stops if callback function returns non-zero and returns
//  final return code from callback function (zero = success).

int
zhashx_foreach (zhashx_t *self, zhashx_foreach_fn *callback, void *argument)
{
    assert (self);

    uint index;
    size_t limit = primes [self->prime_index];
    for (index = 0; index < limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            //  Invoke callback, passing item properties and argument
            item_t *next = item->next;
            int rc = callback ((const char *) item->key, item->value, argument);
            if (rc)
                return rc;      //  End if non-zero return code
            item = next;
        }
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class
//

void
zhashx_test (int verbose)
{
    printf (" * zhashx: ");

    //  @selftest
    zhashx_t *hash = zhashx_new ();
    assert (hash);
    assert (zhashx_size (hash) == 0);
    assert (zhashx_first (hash) == NULL);
    assert (zhashx_cursor (hash) == NULL);

    //  Insert some items
    int rc;
    rc = zhashx_insert (hash, "DEADBEEF", "dead beef");
    char *item = (char *) zhashx_first (hash);
    assert (streq ((char *) zhashx_cursor (hash), "DEADBEEF"));
    assert (streq (item, "dead beef"));
    assert (rc == 0);
    rc = zhashx_insert (hash, "ABADCAFE", "a bad cafe");
    assert (rc == 0);
    rc = zhashx_insert (hash, "C0DEDBAD", "coded bad");
    assert (rc == 0);
    rc = zhashx_insert (hash, "DEADF00D", "dead food");
    assert (rc == 0);
    assert (zhashx_size (hash) == 4);

    //  Look for existing items
    item = (char *) zhashx_lookup (hash, "DEADBEEF");
    assert (streq (item, "dead beef"));
    item = (char *) zhashx_lookup (hash, "ABADCAFE");
    assert (streq (item, "a bad cafe"));
    item = (char *) zhashx_lookup (hash, "C0DEDBAD");
    assert (streq (item, "coded bad"));
    item = (char *) zhashx_lookup (hash, "DEADF00D");
    assert (streq (item, "dead food"));

    //  Look for non-existent items
    item = (char *) zhashx_lookup (hash, "foo");
    assert (item == NULL);

    //  Try to insert duplicate items
    rc = zhashx_insert (hash, "DEADBEEF", "foo");
    assert (rc == -1);
    item = (char *) zhashx_lookup (hash, "DEADBEEF");
    assert (streq (item, "dead beef"));

    //  Some rename tests

    //  Valid rename, key is now LIVEBEEF
    rc = zhashx_rename (hash, "DEADBEEF", "LIVEBEEF");
    assert (rc == 0);
    item = (char *) zhashx_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));

    //  Trying to rename an unknown item to a non-existent key
    rc = zhashx_rename (hash, "WHATBEEF", "NONESUCH");
    assert (rc == -1);

    //  Trying to rename an unknown item to an existing key
    rc = zhashx_rename (hash, "WHATBEEF", "LIVEBEEF");
    assert (rc == -1);
    item = (char *) zhashx_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));

    //  Trying to rename an existing item to another existing item
    rc = zhashx_rename (hash, "LIVEBEEF", "ABADCAFE");
    assert (rc == -1);
    item = (char *) zhashx_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));
    item = (char *) zhashx_lookup (hash, "ABADCAFE");
    assert (streq (item, "a bad cafe"));

    //  Test keys method
    zlistx_t *keys = zhashx_keys (hash);
    assert (zlistx_size (keys) == 4);
    zlistx_destroy (&keys);

    //  Test dup method
    zhashx_t *copy = zhashx_dup (hash);
    assert (zhashx_size (copy) == 4);
    item = (char *) zhashx_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhashx_destroy (&copy);

    //  Test pack/unpack methods
    zframe_t *frame = zhashx_pack (hash);
    copy = zhashx_unpack (frame);
    zframe_destroy (&frame);
    assert (zhashx_size (copy) == 4);
    item = (char *) zhashx_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhashx_destroy (&copy);

    //  Test save and load
    zhashx_comment (hash, "This is a test file");
    zhashx_comment (hash, "Created by %s", "czmq_selftest");
    zhashx_save (hash, ".cache");
    copy = zhashx_new ();
    assert (copy);
    zhashx_load (copy, ".cache");
    item = (char *) zhashx_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhashx_destroy (&copy);
    zsys_file_delete (".cache");

    //  Delete a item
    zhashx_delete (hash, "LIVEBEEF");
    item = (char *) zhashx_lookup (hash, "LIVEBEEF");
    assert (item == NULL);
    assert (zhashx_size (hash) == 3);

    //  Check that the queue is robust against random usage
    struct {
        char name [100];
        bool exists;
    } testset [200];
    memset (testset, 0, sizeof (testset));
    int testmax = 200, testnbr, iteration;

    srandom ((unsigned) time (NULL));
    for (iteration = 0; iteration < 25000; iteration++) {
        testnbr = randof (testmax);
        if (testset [testnbr].exists) {
            item = (char *) zhashx_lookup (hash, testset [testnbr].name);
            assert (item);
            zhashx_delete (hash, testset [testnbr].name);
            testset [testnbr].exists = false;
        }
        else {
            sprintf (testset [testnbr].name, "%x-%x", rand (), rand ());
            if (zhashx_insert (hash, testset [testnbr].name, "") == 0)
                testset [testnbr].exists = true;
        }
    }
    //  Test 10K lookups
    for (iteration = 0; iteration < 10000; iteration++)
        item = (char *) zhashx_lookup (hash, "DEADBEEFABADCAFE");

    //  Destructor should be safe to call twice
    zhashx_destroy (&hash);
    zhashx_destroy (&hash);
    assert (hash == NULL);

    //  Test autofree; automatically copies and frees string values
    hash = zhashx_new ();
    assert (hash);
    zhashx_autofree (hash);
    char value [255];
    strcpy (value, "This is a string");
    rc = zhashx_insert (hash, "key1", value);
    assert (rc == 0);
    strcpy (value, "Ring a ding ding");
    rc = zhashx_insert (hash, "key2", value);
    assert (rc == 0);
    assert (streq ((char *) zhashx_lookup (hash, "key1"), "This is a string"));
    assert (streq ((char *) zhashx_lookup (hash, "key2"), "Ring a ding ding"));
    zhashx_destroy (&hash);
    //  @end

    printf ("OK\n");
}
