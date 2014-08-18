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

/*
@header
    Expandable hash table container
@discuss
    Note that it's relatively slow (~50k insertions/deletes per second), so
    don't do inserts/updates on the critical path for message I/O. It can
    do ~2.5M lookups per second for 16-char keys. Timed on a 1.6GHz CPU.
    .
    The hash table always has a size that is prime and roughly doubles it's
    size when 75% full. In case of hash collisions items are chained in a
    linked list. The hash table size is increased slightly (up to 5 times
    before roughly doubling the size) when an overly long chain (between 1
    and 63 items depending on table size) is detected.
@end
*/

#include "../include/czmq.h"

//  Hash table performance parameters

#define INITIAL_PRIME	0	//  Initial size in items (index into primes)
#define GROWTH_FACTOR	5	//  Increase after splitting (index into primes)
#define LOAD_FACTOR     75      //  Percent loading before splitting
#define INITIAL_CHAIN	1	//  Initial chaining limit
#define CHAIN_GROWS	1	//  Increase after splitting (chaining limit)

// 5 largest primes less than 2^n for n = 4...63
size_t primes[] = {
  3LLU, 5LLU, 7LLU, 11LLU, 13LLU, // 2^4
  17LLU, 19LLU, 23LLU, 29LLU, 31LLU, // 2^5
  43LLU, 47LLU, 53LLU, 59LLU, 61LLU, // 2^6
  103LLU, 107LLU, 109LLU, 113LLU, 127LLU, // 2^7
  229LLU, 233LLU, 239LLU, 241LLU, 251LLU, // 2^8
  487LLU, 491LLU, 499LLU, 503LLU, 509LLU, // 2^9
  997LLU, 1009LLU, 1013LLU, 1019LLU, 1021LLU, // 2^10
  2011LLU, 2017LLU, 2027LLU, 2029LLU, 2039LLU, // 2^11
  4057LLU, 4073LLU, 4079LLU, 4091LLU, 4093LLU, // 2^12
  8161LLU, 8167LLU, 8171LLU, 8179LLU, 8191LLU, // 2^13
  16349LLU, 16361LLU, 16363LLU, 16369LLU, 16381LLU, // 2^14
  32707LLU, 32713LLU, 32717LLU, 32719LLU, 32749LLU, // 2^15
  65449LLU, 65479LLU, 65497LLU, 65519LLU, 65521LLU, // 2^16
  131023LLU, 131041LLU, 131059LLU, 131063LLU, 131071LLU, // 2^17
  262111LLU, 262121LLU, 262127LLU, 262133LLU, 262139LLU, // 2^18
  524243LLU, 524257LLU, 524261LLU, 524269LLU, 524287LLU, // 2^19
  1048517LLU, 1048549LLU, 1048559LLU, 1048571LLU, 1048573LLU, // 2^20
  2097091LLU, 2097097LLU, 2097131LLU, 2097133LLU, 2097143LLU, // 2^21
  4194247LLU, 4194271LLU, 4194277LLU, 4194287LLU, 4194301LLU, // 2^22
  8388547LLU, 8388571LLU, 8388581LLU, 8388587LLU, 8388593LLU, // 2^23
  16777141LLU, 16777153LLU, 16777183LLU, 16777199LLU, 16777213LLU, // 2^24
  33554341LLU, 33554347LLU, 33554371LLU, 33554383LLU, 33554393LLU, // 2^25
  67108763LLU, 67108777LLU, 67108819LLU, 67108837LLU, 67108859LLU, // 2^26
  134217593LLU, 134217613LLU, 134217617LLU, 134217649LLU, 134217689LLU, // 2^27
  268435331LLU, 268435337LLU, 268435361LLU, 268435367LLU, 268435399LLU, // 2^28
  536870839LLU, 536870849LLU, 536870869LLU, 536870879LLU, 536870909LLU, // 2^29
  1073741719LLU, 1073741723LLU, 1073741741LLU, 1073741783LLU, 1073741789LLU, // 2^30
  2147483563LLU, 2147483579LLU, 2147483587LLU, 2147483629LLU, 2147483647LLU, // 2^31
  4294967197LLU, 4294967231LLU, 4294967279LLU, 4294967291LLU, 4294967295LLU, // 2^32
#if __WORDSIZE == 64
  8589934581LLU, 8589934585LLU, 8589934587LLU, 8589934589LLU, 8589934591LLU, // 2^33
  17179869175LLU, 17179869177LLU, 17179869179LLU, 17179869181LLU, 17179869183LLU, // 2^34
  34359738359LLU, 34359738361LLU, 34359738363LLU, 34359738365LLU, 34359738367LLU, // 2^35
  68719476725LLU, 68719476727LLU, 68719476729LLU, 68719476733LLU, 68719476735LLU, // 2^36
  137438953463LLU, 137438953465LLU, 137438953467LLU, 137438953469LLU, 137438953471LLU, // 2^37
  274877906935LLU, 274877906937LLU, 274877906939LLU, 274877906941LLU, 274877906943LLU, // 2^38
  549755813877LLU, 549755813879LLU, 549755813883LLU, 549755813885LLU, 549755813887LLU, // 2^39
  1099511627767LLU, 1099511627769LLU, 1099511627771LLU, 1099511627773LLU, 1099511627775LLU, // 2^40
  2199023255543LLU, 2199023255545LLU, 2199023255547LLU, 2199023255549LLU, 2199023255551LLU, // 2^41
  4398046511095LLU, 4398046511097LLU, 4398046511099LLU, 4398046511101LLU, 4398046511103LLU, // 2^42
  8796093022199LLU, 8796093022201LLU, 8796093022203LLU, 8796093022205LLU, 8796093022207LLU, // 2^43
  17592186044407LLU, 17592186044409LLU, 17592186044411LLU, 17592186044413LLU, 17592186044415LLU, // 2^44
  35184372088823LLU, 35184372088825LLU, 35184372088827LLU, 35184372088829LLU, 35184372088831LLU, // 2^45
  70368744177655LLU, 70368744177657LLU, 70368744177659LLU, 70368744177661LLU, 70368744177663LLU, // 2^46
  140737488355319LLU, 140737488355321LLU, 140737488355323LLU, 140737488355325LLU, 140737488355327LLU, // 2^47
  281474976710647LLU, 281474976710649LLU, 281474976710651LLU, 281474976710653LLU, 281474976710655LLU, // 2^48
  562949953421303LLU, 562949953421305LLU, 562949953421307LLU, 562949953421309LLU, 562949953421311LLU, // 2^49
  1125899906842615LLU, 1125899906842617LLU, 1125899906842619LLU, 1125899906842621LLU, 1125899906842623LLU, // 2^50
  2251799813685239LLU, 2251799813685241LLU, 2251799813685243LLU, 2251799813685245LLU, 2251799813685247LLU, // 2^51
  4503599627370487LLU, 4503599627370489LLU, 4503599627370491LLU, 4503599627370493LLU, 4503599627370495LLU, // 2^52
  9007199254740983LLU, 9007199254740985LLU, 9007199254740987LLU, 9007199254740989LLU, 9007199254740991LLU, // 2^53
  18014398509481975LLU, 18014398509481977LLU, 18014398509481979LLU, 18014398509481981LLU, 18014398509481983LLU, // 2^54
  36028797018963959LLU, 36028797018963961LLU, 36028797018963963LLU, 36028797018963965LLU, 36028797018963967LLU, // 2^55
  72057594037927925LLU, 72057594037927927LLU, 72057594037927929LLU, 72057594037927933LLU, 72057594037927935LLU, // 2^56
  144115188075855863LLU, 144115188075855865LLU, 144115188075855867LLU, 144115188075855869LLU, 144115188075855871LLU, // 2^57
  288230376151711735LLU, 288230376151711737LLU, 288230376151711739LLU, 288230376151711741LLU, 288230376151711743LLU, // 2^58
  576460752303423479LLU, 576460752303423481LLU, 576460752303423483LLU, 576460752303423485LLU, 576460752303423487LLU, // 2^59
  1152921504606846967LLU, 1152921504606846969LLU, 1152921504606846971LLU, 1152921504606846973LLU, 1152921504606846975LLU, // 2^60
  2305843009213693941LLU, 2305843009213693943LLU, 2305843009213693945LLU, 2305843009213693947LLU, 2305843009213693949LLU, // 2^61
  4611686018427387895LLU, 4611686018427387897LLU, 4611686018427387899LLU, 4611686018427387901LLU, 4611686018427387903LLU, // 2^62
  9223372036854775799LLU, 9223372036854775801LLU, 9223372036854775803LLU, 9223372036854775805LLU, 9223372036854775807LLU, // 2^63
#endif
};
#define NUM_PRIMES sizeof(primes)

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
    uint prime_index;		//  Current prime number used as limit
    uint chain_limit;		//  Current limit on chain length
    item_t **items;             //  Array of items
    uint cached_index;          //  Avoids duplicate hash calculations
    uint cursor_index;          //  For first/next iteration
    item_t *cursor_item;        //  For first/next iteration
    char *cursor_key;           //  After first/next call, points to key
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
        self->prime_index = INITIAL_PRIME;
	self->chain_limit = INITIAL_CHAIN;
	size_t limit = primes[self->prime_index];
        self->items = (item_t **) zmalloc (sizeof (item_t *) * limit);
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
	size_t limit = primes[self->prime_index];
        for (index = 0; index < limit; index++) {
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
//  Rehash hash table with specified new prime index
//  Returns 0 on success.
static int
s_zhash_rehash (zhash_t *self, uint new_prime_index)
{
    assert (self);
    assert (new_prime_index < NUM_PRIMES);

    size_t limit = primes[self->prime_index];
    size_t new_limit = primes[new_prime_index];
    item_t **new_items = (item_t **) zmalloc (sizeof (item_t *) * new_limit);
    if (!new_items)
	return ENOMEM;

    //  Move all items to the new hash table, rehashing to
    //  take into account new hash table limit
    size_t index;
    for (index = 0; index < limit; index++) {
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
    self->prime_index = new_prime_index;

    return 0;
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
    size_t limit = primes[self->prime_index];
    if (self->size >= limit * LOAD_FACTOR / 100) {
        //  Create new hash table
	uint new_prime_index = self->prime_index + GROWTH_FACTOR;
	int rc = s_zhash_rehash(self, new_prime_index);
	if (rc != 0) return rc;
	self->chain_limit += CHAIN_GROWS;
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
    size_t limit = primes[self->prime_index];
    self->cached_index = s_item_hash (key, limit);
    item_t *item = self->items [self->cached_index];
    uint len = 0;
    while (item) {
        if (streq (item->key, key))
            break;
        item = item->next;
	++len;
    }
    if (len > self->chain_limit) {
        //  Create new hash table
	uint new_prime_index = self->prime_index + GROWTH_FACTOR;
	int rc = s_zhash_rehash(self, new_prime_index);
	assert (rc == 0);
	limit = primes[self->prime_index];
	self->cached_index = s_item_hash (key, limit);
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
//  Make copy of hash table; if supplied table is null, returns null.
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
	size_t limit = primes[self->prime_index];
        for (index = 0; index < limit; index++) {
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
//  Return keys for items in table. If you remove items from this list you
//  must free the key value yourself.

zlist_t *
zhash_keys (zhash_t *self)
{
    assert (self);
    zlist_t *keys = zlist_new ();
    zlist_autofree (keys);

    uint index;
    size_t limit = primes[self->prime_index];
    for (index = 0; index < limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            zlist_append (keys, item->key);
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
zhash_first (zhash_t *self)
{
    assert (self);
    //  Point to before or at first item
    self->cursor_index = 0;
    self->cursor_item = self->items [self->cursor_index];
    //  Now scan forwards to find it, leave cursor after item
    return zhash_next (self);
}


//  --------------------------------------------------------------------------
//  Simple iterator; returns next item in hash table, in no given order,
//  or NULL if the last item was already returned. Use this together with
//  zhash_first() to process all items in a hash table. If you need the
//  items in sorted order, use zhash_keys() and then zlist_sort(). NOTE:
//  do NOT modify the table while iterating.

void *
zhash_next (zhash_t *self)
{
    assert (self);
    //  Scan forward from cursor until we find an item
    size_t limit = primes[self->prime_index];
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
//  After a successful first/next method, returns the key for the item
//  that was returned. After an unsuccessful first/next, returns NULL.

char *
zhash_cursor (zhash_t *self)
{
    assert (self);
    return self->cursor_key;
}


//  --------------------------------------------------------------------------
//  Add a comment to hash table before saving to disk. You can add as many
//  comment lines as you like. These comment lines are discarded when loading
//  the file. If you use a null format, all comments are deleted.

void
zhash_comment (zhash_t *self, const char *format, ...)
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
zhash_save (zhash_t *self, const char *filename)
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
    size_t limit = primes [self->prime_index];
    for (index = 0; index < limit; index++) {
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
zhash_load (zhash_t *self, const char *filename)
{
    assert (self);
    zhash_autofree (self);

    //  Whether or not file exists, we'll track the filename and last
    //  modification date (0 for unknown files), so that zhash_refresh ()
    //  will always work after zhash_load (), to load a newly-created
    //  file.

    //  Take copy of filename in case self->filename is same string.
    char *filename_copy = strdup (filename);
    free (self->filename);
    self->filename = filename_copy;
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
	    size_t limit = primes[self->prime_index];
            for (index = 0; index < limit; index++) {
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
zhash_pack (zhash_t *self)
{
    assert (self);

    //  First, calculate packed data size
    size_t frame_size = 4;      //  Dictionary size, number-4
    uint index;
    size_t limit = primes[self->prime_index];
    for (index = 0; index < limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            //  We store key as short string
            frame_size += 1 + strlen (item->key);
            //  We store value as long string
            frame_size += 4 + strlen ((char *) item->value);
            item = item->next;
        }
    }
    //  Now serialize items into the frame
    zframe_t *frame = zframe_new (NULL, frame_size);
    byte *needle = zframe_data (frame);
    //  Store size as number-4
    *(uint32_t *) needle = htonl (self->size);
    needle += 4;
    for (index = 0; index < limit; index++) {
        item_t *item = self->items [index];
        while (item) {
            //  Store key as string
            *needle++ = (byte) strlen (item->key);
            memcpy (needle, item->key, strlen (item->key));
            needle += strlen (item->key);

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
//  defined by zhash_pack. Hash table is set to autofree. An empty frame
//  unpacks to an empty hash table.

zhash_t *
zhash_unpack (zframe_t *frame)
{
    zhash_t *self = zhash_new ();
    assert (self);
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
                    char *value = (char *) malloc (value_size + 1);
                    memcpy (value, needle, value_size);
                    value [value_size] = 0;
                    needle += value_size;
                    //  Hash takes ownership of value
                    zhash_insert (self, key, value);
                }
            }
        }
    }
    //  Hash will free values in destructor
    zhash_autofree (self);
    return self;
}

            
//  --------------------------------------------------------------------------
//  Apply function to each item in the hash table. Items are iterated in no
//  defined order.  Stops if callback function returns non-zero and returns
//  final return code from callback function (zero = success).
//  NOTE: this is deprecated in favor of zhash_first/next since the callback
//  design is clumsy and over-complex, and unnecessary.

int
zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument)
{
    assert (self);
    int rc = 0;
    uint index;
    size_t limit = primes[self->prime_index];
    for (index = 0; index < limit; index++) {
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
//  Runs selftest of class
//

void
zhash_test (int verbose)
{
    printf (" * zhash: ");

    //  @selftest
    zhash_t *hash = zhash_new ();
    assert (hash);
    assert (zhash_size (hash) == 0);
    assert (zhash_first (hash) == NULL);
    assert (zhash_cursor (hash) == NULL);
    
    //  Insert some items
    int rc;
    rc = zhash_insert (hash, "DEADBEEF", "dead beef");
    char *item = (char *) zhash_first (hash);
    assert (streq (zhash_cursor (hash), "DEADBEEF"));
    assert (streq (item, "dead beef"));
    assert (rc == 0);
    rc = zhash_insert (hash, "ABADCAFE", "a bad cafe");
    assert (rc == 0);
    rc = zhash_insert (hash, "C0DEDBAD", "coded bad");
    assert (rc == 0);
    rc = zhash_insert (hash, "DEADF00D", "dead food");
    assert (rc == 0);
    assert (zhash_size (hash) == 4);

    //  Look for existing items
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

    //  Test pack/unpack methods
    zframe_t *frame = zhash_pack (hash);
    copy = zhash_unpack (frame);
    zframe_destroy (&frame);
    assert (zhash_size (copy) == 4);
    item = (char *) zhash_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhash_destroy (&copy);

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
