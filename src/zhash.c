/*  =========================================================================
    zhash - generic type-free hash container

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
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
    =========================================================================
*/

/*
@header
    Expandable hash table container
@discuss
    Note that it's relatively slow (~50K insertions/deletes per second), so
    don't do inserts/updates on the critical path for message I/O. It can
    do ~2.5M lookups per second for 16-char keys. Timed on a 1.6GHz CPU.
@end
*/

#include "../include/czmq.h"

//  Hash table performance parameters

#define INITIAL_SIZE    255     //  Initial size in items
#define LOAD_FACTOR     75      //  Percent loading before splitting
#define GROWTH_FACTOR   200     //  Increase in % after splitting


//  Hash item, used internally only

typedef struct _item_t {
    void *value;                //  Opaque item value
    struct _item_t *next;       //  Next item in the hash slot
    qbyte index;                //  Index of item in table
    char *key;                  //  Item's original key
    zhash_free_fn *free_fn;     //  Value free function if any
} item_t;


//  ---------------------------------------------------------------------
//  Structure of our class

struct _zhash_t {
    size_t size;                //  Current size of hash table
    size_t limit;               //  Current hash table limit
    item_t **items;             //  Array of items
    uint cached_index;          //  Avoids duplicate hash calculations
    bool autofree;              //  If true, free values in destructor
    zlist_t *comments;          //  File comments, if any
    time_t modified;            //  Set during zhash_load
    char *filename;             //  Set during zhash_load
};

//  Local helper functions
static uint s_item_hash (const char *key, size_t limit);
static item_t *s_item_lookup (zhash_t *self, const char *key);
static item_t *s_item_insert (zhash_t *self, const char *key, void *value);
static void s_item_destroy (zhash_t *self, item_t *item, bool hard);



//  --------------------------------------------------------------------------
//  Hash table constructor

zhash_t *
zhash_new (void)
{
    zhash_t *self = (zhash_t *) zmalloc (sizeof (zhash_t));
    if (self) {
        self->limit = INITIAL_SIZE;
        self->items = (item_t **) zmalloc (sizeof (item_t *) * self->limit);
        if (!self->items)
            zhash_destroy (&self);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Hash table destructor

void
zhash_destroy (zhash_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zhash_t *self = *self_p;
        uint index;
        for (index = 0; index < self->limit; index++) {
            //  Destroy all items in this hash bucket
            item_t *cur_item = self->items [index];
            while (cur_item) {
                item_t *next_item = cur_item->next;
                s_item_destroy (self, cur_item, true);
                cur_item = next_item;
            }
        }
        if (self->items)
            free (self->items);

        zlist_destroy (&self->comments);
        free (self->filename);
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Local helper function
//  Destroy item in hash table, item must exist in table

static void
s_item_destroy (zhash_t *self, item_t *item, bool hard)
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
        if (item->free_fn)
            (item->free_fn) (item->value);
        else
        if (self->autofree)
            free (item->value);

        free (item->key);
        free (item);
    }
}


//  --------------------------------------------------------------------------
//  Insert item into hash table with specified key and item
//  If key is already present returns -1 and leaves existing item unchanged
//  Returns 0 on success.

int
zhash_insert (zhash_t *self, const char *key, void *value)
{
    assert (self);
    assert (key);

    //  If we're exceeding the load factor of the hash table,
    //  resize it according to the growth factor
    if (self->size >= self->limit * LOAD_FACTOR / 100) {
        //  Create new hash table
        size_t new_limit = self->limit * GROWTH_FACTOR / 100;
        item_t **new_items = (item_t **) zmalloc (sizeof (item_t *) * new_limit);
        if (!new_items)
            return ENOMEM;

        //  Move all items to the new hash table, rehashing to
        //  take into account new hash table limit
        uint index;
        for (index = 0; index != self->limit; index++) {
            item_t *cur_item = self->items [index];
            while (cur_item) {
                item_t *next_item = cur_item->next;
                uint new_index = s_item_hash (cur_item->key, new_limit);
                cur_item->index = new_index;
                cur_item->next = new_items [new_index];
                new_items [new_index] = cur_item;
                cur_item = next_item;
            }
        }
        //  Destroy old hash table
        free (self->items);
        self->items = new_items;
        self->limit = new_limit;
    }
    //  If necessary, take duplicate of item (string) value
    if (self->autofree)
        value = strdup ((char *) value);

    return s_item_insert (self, key, value)? 0: -1;
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Compute hash for key string

static uint
s_item_hash (const char *key, size_t limit)
{
    //  Modified Bernstein hashing function
    uint key_hash = 0;
    while (*key)
        key_hash = 33 * key_hash ^ *key++;
    key_hash %= limit;
    return key_hash;
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Insert new item into hash table, returns item
//  If item already existed, returns NULL

static item_t *
s_item_insert (zhash_t *self, const char *key, void *value)
{
    //  Check that item does not already exist in hash table
    //  Leaves self->cached_index with calculated hash item
    item_t *item = s_item_lookup (self, key);
    if (item == NULL) {
        item = (item_t *) zmalloc (sizeof (item_t));
        if (!item)
            return NULL;
        item->value = value;
        item->key = strdup (key);
        item->index = self->cached_index;
        //  Insert into start of bucket list
        item->next = self->items [self->cached_index];
        self->items [self->cached_index] = item;
        self->size++;
    }
    else
        item = NULL;            //  Signal duplicate insertion
    return item;
}


//  --------------------------------------------------------------------------
//  Local helper function
//  Lookup item in hash table, returns item or NULL

static item_t *
s_item_lookup (zhash_t *self, const char *key)
{
    //  Look in bucket list for item by key
    self->cached_index = s_item_hash (key, self->limit);
    item_t *item = self->items [self->cached_index];
    while (item) {
        if (streq (item->key, key))
            break;
        item = item->next;
    }
    return item;
}


//  --------------------------------------------------------------------------
//  Update item into hash table with specified key and item.
//  If key is already present, destroys old item and inserts new one.
//  Use free_fn method to ensure deallocator is properly called on item.

void
zhash_update (zhash_t *self, const char *key, void *value)
{
    assert (self);
    assert (key);

    item_t *item = s_item_lookup (self, key);
    if (item) {
        if (item->free_fn)
            (item->free_fn) (item->value);
        else
        if (self->autofree)
            free (item->value);

        //  If necessary, take duplicate of item (string) value
        if (self->autofree)
            value = strdup ((char *) value);
        item->value = value;
    }
    else
        zhash_insert (self, key, value);
}


//  --------------------------------------------------------------------------
//  Remove an item specified by key from the hash table. If there was no such
//  item, this function does nothing.

void
zhash_delete (zhash_t *self, const char *key)
{
    assert (self);
    assert (key);

    item_t *item = s_item_lookup (self, key);
    if (item)
        s_item_destroy (self, item, true);
}


//  --------------------------------------------------------------------------
//  Look for item in hash table and return its item, or NULL

void *
zhash_lookup (zhash_t *self, const char *key)
{
    assert (self);
    assert (key);

    item_t *item = s_item_lookup (self, key);
    if (item)
        return item->value;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Reindexes an item from an old key to a new key. If there was no such
//  item, does nothing. If the new key already exists, deletes old item.

int
zhash_rename (zhash_t *self, const char *old_key, const char *new_key)
{
    item_t *old_item = s_item_lookup (self, old_key);
    item_t *new_item = s_item_lookup (self, new_key);
    if (old_item && !new_item) {
        s_item_destroy (self, old_item, false);
        free (old_item->key);
        old_item->key = strdup (new_key);
        old_item->index = self->cached_index;
        old_item->next = self->items [self->cached_index];
        self->items [self->cached_index] = old_item;
        self->size++;
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
zhash_freefn (zhash_t *self, const char *key, zhash_free_fn *free_fn)
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
zhash_size (zhash_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Make copy of hash table
//  Does not copy items themselves. Rebuilds new table so may be slow on
//  very large tables. NOTE: only works with item values that are strings
//  since there's no other way to know how to duplicate the item value.

zhash_t *
zhash_dup (zhash_t *self)
{
    if (!self)
        return NULL;

    zhash_t *copy = zhash_new ();
    zhash_autofree (copy);
    if (copy) {
        uint index;
        for (index = 0; index != self->limit; index++) {
            item_t *item = self->items [index];
            while (item) {
                zhash_insert (copy, item->key, item->value);
                item = item->next;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Return keys for items in table

zlist_t *
zhash_keys (zhash_t *self)
{
    assert (self);
    zlist_t *keys = zlist_new ();
    zlist_autofree (keys);

    uint index;
    for (index = 0; index != self->limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            zlist_append (keys, item->key);
            item = item->next;
        }
    }
    return keys;
}


//  --------------------------------------------------------------------------
//  Apply function to each item in the hash table. Items are iterated in no
//  defined order.  Stops if callback function returns non-zero and returns
//  final return code from callback function (zero = success).

int
zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument)
{
    assert (self);
    int rc = 0;
    uint index;
    for (index = 0; index != self->limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            //  Invoke callback, passing item properties and argument
            item_t *next = item->next;
            rc = callback (item->key, item->value, argument);
            if (rc)
                return rc;      //  End if non-zero return code
            item = next;
        }
    }
    return rc;
}


//  --------------------------------------------------------------------------
//  Add comment to hash table before saving to disk. You can add as many
//  comment lines as you like. These comment lines are discarded when loading
//  the file. If you use a null format, all comments are deleted.

void
zhash_comment (zhash_t *self, char *format, ...)
{
    if (format) {
        if (!self->comments) {
            self->comments = zlist_new ();
            zlist_autofree (self->comments);
        }
        va_list argptr;
        va_start (argptr, format);
        char *string = zsys_vprintf (format, argptr);
        va_end (argptr);
        zlist_append (self->comments, string);
        free (string);
    }
    else
        zlist_destroy (&self->comments);
}


//  --------------------------------------------------------------------------
//  Save hash table to a text file in name=value format
//  Hash values must be printable strings.
//  Returns 0 if OK, else -1 if a file error occurred

int
zhash_save (zhash_t *self, char *filename)
{
    assert (self);

    FILE *handle = fopen (filename, "w");
    if (!handle)
        return -1;              //  Failed to create file

    if (self->comments) {
        char *comment = (char *) zlist_first (self->comments);
        while (comment) {
            fprintf (handle, "#   %s\n", comment);
            comment = (char *) zlist_next (self->comments);
        }
        fprintf (handle, "\n");
    }
    uint index;
    for (index = 0; index != self->limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            fprintf (handle, "%s=%s\n", item->key, (char *) item->value);
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
zhash_load (zhash_t *self, char *filename)
{
    assert (self);
    zhash_autofree (self);

    //  Whether or not file exists, we'll track the filename and last
    //  modification date (0 for unknown files), so that zhash_refresh ()
    //  will always work after zhash_load (), to load a newly-created
    //  file.
    
    //  Take copy of filename in case self->filename is same string.
    filename = strdup (filename);
    free (self->filename);
    self->filename = filename;
    self->modified = zsys_file_modified (self->filename);
    FILE *handle = fopen (self->filename, "r");
    if (!handle)
        return -1;              //  Failed to open file for reading

    char *buffer = (char *) zmalloc (1024);
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
        zhash_update (self, buffer, equals);
    }
    free (buffer);
    fclose (handle);
    return 0;
}


//  --------------------------------------------------------------------------
//  When a hash table was loaded from a file by zhash_load, this method will
//  reload the file if it has been modified since, and is "stable", i.e. not
//  still changing. Returns 0 if OK, -1 if there was an error reloading the 
//  file.

int
zhash_refresh (zhash_t *self)
{
    assert (self);
    
    if (self->filename) {
        if (zsys_file_modified (self->filename) > self->modified
        &&  zsys_file_stable (self->filename)) {
            //  Empty the hash table; code is copied from zhash_destroy
            uint index;
            for (index = 0; index < self->limit; index++) {
                //  Destroy all items in this hash bucket
                item_t *cur_item = self->items [index];
                while (cur_item) {
                    item_t *next_item = cur_item->next;
                    s_item_destroy (self, cur_item, true);
                    cur_item = next_item;
                }
            }
            zhash_load (self, self->filename);
        }
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Set hash for automatic value destruction

void
zhash_autofree (zhash_t *self)
{
    assert (self);
    self->autofree = true;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class
//

static int
test_foreach (const char *key, void *item, void *arg)
{
    assert (NULL != zhash_lookup ((zhash_t*) arg, key));
    return 0;
}

static int
test_foreach_error (const char *key, void *item, void *arg)
{
    return -1;
}

void
zhash_test (int verbose)
{
    printf (" * zhash: ");

    //  @selftest
    zhash_t *hash = zhash_new ();
    assert (hash);
    assert (zhash_size (hash) == 0);

    //  Insert some items
    int rc;
    rc = zhash_insert (hash, "DEADBEEF", "dead beef");
    assert (rc == 0);
    rc = zhash_insert (hash, "ABADCAFE", "a bad cafe");
    assert (rc == 0);
    rc = zhash_insert (hash, "C0DEDBAD", "coded bad");
    assert (rc == 0);
    rc = zhash_insert (hash, "DEADF00D", "dead food");
    assert (rc == 0);
    assert (zhash_size (hash) == 4);

    //  Look for existing items
    char *item;
    item = (char *) zhash_lookup (hash, "DEADBEEF");
    assert (streq (item, "dead beef"));
    item = (char *) zhash_lookup (hash, "ABADCAFE");
    assert (streq (item, "a bad cafe"));
    item = (char *) zhash_lookup (hash, "C0DEDBAD");
    assert (streq (item, "coded bad"));
    item = (char *) zhash_lookup (hash, "DEADF00D");
    assert (streq (item, "dead food"));

    //  Look for non-existent items
    item = (char *) zhash_lookup (hash, "foo");
    assert (item == NULL);

    //  Try to insert duplicate items
    rc = zhash_insert (hash, "DEADBEEF", "foo");
    assert (rc == -1);
    item = (char *) zhash_lookup (hash, "DEADBEEF");
    assert (streq (item, "dead beef"));

    //  Some rename tests

    //  Valid rename, key is now LIVEBEEF
    rc = zhash_rename (hash, "DEADBEEF", "LIVEBEEF");
    assert (rc == 0);
    item = (char *) zhash_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));

    //  Trying to rename an unknown item to a non-existent key
    rc = zhash_rename (hash, "WHATBEEF", "NONESUCH");
    assert (rc == -1);

    //  Trying to rename an unknown item to an existing key
    rc = zhash_rename (hash, "WHATBEEF", "LIVEBEEF");
    assert (rc == -1);
    item = (char *) zhash_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));

    //  Trying to rename an existing item to another existing item
    rc = zhash_rename (hash, "LIVEBEEF", "ABADCAFE");
    assert (rc == -1);
    item = (char *) zhash_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));
    item = (char *) zhash_lookup (hash, "ABADCAFE");
    assert (streq (item, "a bad cafe"));

    //  Test keys method
    zlist_t *keys = zhash_keys (hash);
    assert (zlist_size (keys) == 4);
    zlist_destroy (&keys);

    //  Test dup method
    zhash_t *copy = zhash_dup (hash);
    assert (zhash_size (copy) == 4);
    item = (char *) zhash_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhash_destroy (&copy);

    // Test foreach
    assert (0 == zhash_foreach (hash, test_foreach, hash));
    assert (-1 == zhash_foreach (hash, test_foreach_error, hash));

    //  Test save and load
    zhash_comment (hash, "This is a test file");
    zhash_comment (hash, "Created by %s", "czmq_selftest");
    zhash_save (hash, ".cache");
    copy = zhash_new ();
    zhash_load (copy, ".cache");
    item = (char *) zhash_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhash_destroy (&copy);
    zsys_file_delete (".cache");

    //  Delete a item
    zhash_delete (hash, "LIVEBEEF");
    item = (char *) zhash_lookup (hash, "LIVEBEEF");
    assert (item == NULL);
    assert (zhash_size (hash) == 3);

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
            item = (char *) zhash_lookup (hash, testset [testnbr].name);
            assert (item);
            zhash_delete (hash, testset [testnbr].name);
            testset [testnbr].exists = false;
        }
        else {
            sprintf (testset [testnbr].name, "%x-%x", rand (), rand ());
            if (zhash_insert (hash, testset [testnbr].name, "") == 0)
                testset [testnbr].exists = true;
        }
    }
    //  Test 10K lookups
    for (iteration = 0; iteration < 10000; iteration++)
        item = (char *) zhash_lookup (hash, "DEADBEEFABADCAFE");

    //  Destructor should be safe to call twice
    zhash_destroy (&hash);
    zhash_destroy (&hash);
    assert (hash == NULL);

    // Test autofree; automatically copies and frees string values
    hash = zhash_new ();
    zhash_autofree (hash);
    char value [255];
    strcpy (value, "This is a string");
    rc = zhash_insert (hash, "key1", value);
    assert (rc == 0);
    strcpy (value, "Ring a ding ding");
    rc = zhash_insert (hash, "key2", value);
    assert (rc == 0);
    assert (streq ((char *) zhash_lookup (hash, "key1"), "This is a string"));
    assert (streq ((char *) zhash_lookup (hash, "key2"), "Ring a ding ding"));
    zhash_destroy (&hash);
    //  @end

    printf ("OK\n");
}
