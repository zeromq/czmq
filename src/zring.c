/*  =========================================================================
    zring - generic type-free doubly linked ring container

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
    Provides a generic "ring" container, which mixes a doubly-linked list
    with a hash table to provide both ordered and direct keyed access to
    items. The zring class is more complex and powerful than zlist, which
    is considered a 'basic' list container. This container provides hooks
    for duplicator, comparator, and destructor functions. These tie into
    CZMQ and standard C semantics, so e.g. for string items you can use
    strdup, strcmp, and zstr_free. To store custom objects, define your
    own duplicator and comparator, and use the standard object destructor.
@discuss
@end
*/

#include "../include/czmq.h"

//  Ring node, used internally only

typedef struct _node_t {
    struct _node_t *next;
    struct _node_t *prev;
    void *item;
    const void *key;            //  Dictionary key, if any
} node_t;

//  ---------------------------------------------------------------------
//  Structure of our class

struct _zring_t {
    node_t *head;               //  Dummy node acting as head in ring
    node_t *cursor;             //  Current node for iteration
    size_t size;                //  Number of items in ring
    zhash_t *hash;              //  Dictionary for keyed access
    //  Container-level handlers
    czmq_destructor *destructor;
    czmq_duplicator *duplicator;
    czmq_comparator *comparator;
};


//  --------------------------------------------------------------------------
//  Destroy a ring node. The nodes item must already have been destroyed.

static void
s_node_destroy (node_t **self_p)
{
    assert (self_p);
    node_t *self = *self_p;
    assert (self->item == NULL);
    // unlink node from ring
    self->prev->next = self->next;
    self->next->prev = self->prev;
    // free memory
    free (self);
    self_p = NULL;
}

//  --------------------------------------------------------------------------
//  Initialize a ring node and attach to the prev and next nodes, or itself
//  if these are specified as null. Returns new node, or NULL if process ran
//  out of heap memory.

static node_t *
s_node_new (node_t *prev, node_t *next, void *item)
{
    node_t *self = (node_t *) zmalloc (sizeof (node_t));
    if (self) {
        self->prev = prev ? prev : self;
        self->next = next ? next : self;
        self->item = item;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Create a new ring container (a ring is a doubly-linked ring)

zring_t *
zring_new (void)
{
    zring_t *self = (zring_t *) zmalloc (sizeof (zring_t));
    if (self) {
        self->head = s_node_new (NULL, NULL, NULL);
        self->cursor = self->head;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a ring container

void
zring_destroy (zring_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zring_t *self = *self_p;
        zring_purge (self);
        assert (!self->hash || zhash_size (self->hash) == 0);
        zhash_destroy (&self->hash);
        s_node_destroy (&self->head);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Prepend an item to the start of the ring, return 0 if OK, else -1.
//  Leaves cursor at newly inserted item.

int
zring_prepend (zring_t *self, void *item)
{
    assert (self);
    if (self->duplicator) {
        item = (self->duplicator)(item);
        if (!item)
            return -1;
    }
    node_t *node = s_node_new (self->head, self->head->next, item);
    if (node) {
        self->head->next->prev = node;
        self->head->next = node;
        self->cursor = node;
        self->size++;
        return 0;
    }
    else
        return -1;              //  Ran out of heap memory
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the ring, return 0 if OK, else -1.
//  Leaves cursor at newly inserted item.

int
zring_append (zring_t *self, void *item)
{
    assert (self);
    if (self->duplicator) {
        item = (self->duplicator)(item);
        if (!item)
            return -1;
    }
    node_t *node = s_node_new (self->head->prev, self->head, item);
    if (node) {
        self->head->prev->next = node;
        self->head->prev = node;
        self->cursor = node;
        self->size++;
        return 0;
    }
    else
        return -1;              //  Ran out of heap memory
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the ring, and insert into the ring
//  dictionary, so that you can find the item rapidly using zring_lookup.
//  If you do a lot of item searches, this is faster than zring_find,
//  which is at worst an O(N) operation. When items leave the ring, they
//  are always removed from the dictionary. Returns 0 on success, -1 if
//  the key already existed in the dictionary, or heap memory ran out.

int
zring_insert (zring_t *self, const void *key, void *item)
{
    assert (self);
    assert (key);
    assert (item);

    if (!self->hash)
        self->hash = zhash_new ();
    if (!self->hash)
        return -1;
    if (self->duplicator) {
        item = (self->duplicator)(item);
        if (!item)
            return -1;
    }

    //  If item isn't already in dictionary, append to list and then
    //  store item node (which is in cursor) in dictionary
    if (  !zhash_lookup (self->hash, key)
       && !zring_append (self, item)
       && !zhash_insert (self->hash, key, self->cursor)) {
        self->cursor->key = zhash_cursor (self->hash);
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Find an item in the ring, looking first at the cursor, and then from the
//  first to last item. If a comparator was set on container, calls this to
//  compare each item in the ring with the supplied target item. If none
//  was set, compares the two item pointers for equality. If the item is
//  found, leaves the cursor at the found item. Returns the item if found,
//  else null.

void *
zring_find (zring_t *self, void *item)
{
    assert (self);
    assert (item);

    //  First check item at cursor
    if (self->cursor != self->head) {
        if (self->comparator) {
            if (self->comparator (self->cursor->item, item) == 0)
                return self->cursor->item;
        }
        else
        if (self->cursor->item == item)
            return self->cursor->item;
    }
    //  Now scan ring for item, this is a O(N) operation
    node_t *node = self->head->next;
    while (node != self->head) {
        if (self->comparator) {
            if (self->comparator (node->item, item) == 0)
                return node->item;
        }
        else
        if (node->item == item)
            return node->item;

        node = node->next;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Search the ring dictionary for an item, by key. If the item is in the
//  dictionary (via zring_insert), then sets the ring cursor to the item,
//  and returns the item value. If not, leaves the cursor unchanged, and
//  returns NULL.

void *
zring_lookup (zring_t *self, const void *key)
{
    assert (self);
    assert (key);

    if (self->hash) {
        node_t *node = (node_t *) zhash_lookup (self->hash, key);
        if (node) {
            self->cursor = node;
            return node->item;
        }
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Detach an item from the ring, without destroying the item. Searches the
//  ring for the item, always starting with the cursor, if any is set, and
//  then from the start of the ring. If item is null, detaches the item at the
//  cursor, if set. If the item was found and detached, leaves the cursor at
//  the next item, if any, and returns the item. Else, returns null.

void *
zring_detach (zring_t *self, void *item)
{
    assert (self);

    node_t *found = NULL;
    if (  (item && zring_find (self, item))
       || (!item && self->cursor != self->head))
        found = self->cursor;

    //  Now detach node from list, without destroying it
    if (found) {
        item = found->item;
        self->cursor = found->next;
        self->size--;
        if (found->key)
            zhash_delete (self->hash, found->key);
        found->item = NULL;
        s_node_destroy (&found);
        return item;
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Remove an item from the ring, and destroy it, if the item destructor is
//  set. Searches the ring for the item, always starting with the cursor, if
//  any is set, and then from the start of the ring. If item is null, removes
//  the item at the cursor, if set. If the item was found and removed, leaves
//  the cursor at the next item, if any, and returns 0. Else, returns -1.

int
zring_remove (zring_t *self, void *item)
{
    assert (self);
    if ((item = zring_detach (self, item))) {
        if (self->destructor)
            self->destructor (&item);
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Search the ring dictionary for an item, by key. If the item is in the
//  dictionary (via zring_insert), then removes the item from the ring and
//  calls the item destructor, if any is found. Returns 0 if the item was
//  found and removed, else -1 if not found.

int
zring_delete (zring_t *self, const void *key)
{
    assert (self);
    assert (key);

    if (zring_lookup (self, key))
        return zring_remove (self, NULL);
    else
        return -1;          //  Not found in dictionary
}


//  --------------------------------------------------------------------------
//  Remove all items from the ring. If the item destructor is set, calls it
//  on every item.

void
zring_purge (zring_t *self)
{
    assert (self);
    while (zring_remove (self, zring_first (self)) == 0) ;
}


//  --------------------------------------------------------------------------
//  Return the number of items in the ring

size_t
zring_size (zring_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Return the item at the head of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the ring is empty.

void *
zring_first (zring_t *self)
{
    assert (self);
    self->cursor = self->head;
    return zring_next (self);
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the ring is empty.

void *
zring_last (zring_t *self)
{
    assert (self);
    self->cursor = self->head;
    return zring_prev (self);
}


//  --------------------------------------------------------------------------
//  Return the next item. At the end of the ring (or in an empty ring),
//  returns NULL. Use repeated zring_next () calls to work through the ring
//  from zring_first ().

void *
zring_next (zring_t *self)
{
    assert (self);
    self->cursor = self->cursor->next;
    return zring_item (self);
}


//  --------------------------------------------------------------------------
//  Return the previous item. At the start of the ring (or in an empty ring),
//  returns NULL. Use repeated zring_prev () calls to work through the ring
//  backwards from zring_last ().

void *
zring_prev (zring_t *self)
{
    assert (self);
    self->cursor = self->cursor->prev;
    return zring_item (self);
}


//  --------------------------------------------------------------------------
//  Return current item in the ring. If the ring is empty, or the cursor
//  passed the end of the ring, returns NULL. Does not change the cursor.

void *
zring_item (zring_t *self)
{
    assert (self);
    if (self->cursor != self->head)
        return self->cursor->item;
    else
        return NULL;            //  Reached head, so finished
}


//  --------------------------------------------------------------------------
//  Sort the ring using the container comparator, which must have been
//  specified. The sort is not stable, so may reorder equal items.

void
zring_sort (zring_t *self)
{
    assert (self);

    //  Uses a comb sort, which is simple and reasonably fast. The
    //  algorithm is based on Wikipedia's C pseudo-code for comb sort.
    size_t gap = self->size;
    bool swapped = false;
    while (gap > 1 || swapped) {
        if (gap > 1)
            gap = (size_t) ((double) gap / 1.247330950103979);

        node_t *base = self->head;
        node_t *test = self->head;
        int jump = gap;
        while (jump--)
            test = test->next;

        swapped = false;
        while (base != self->head && test != self->head) {
            int compare;
            if (self->comparator)
                compare = self->comparator (base->item, test->item);
            else
                compare = (base->item < test->item);

            if (compare) {
                //  We don't actually swap nodes, just the items in the nodes.
                //  This is ridiculously simple and confuses the heck out of
                //  me every time I re-read the code, as I expect to see the
                //  nodes being swapped. Hence this comment. -- PH 2014/09/06
                void *item = base->item;
                base->item = test->item;
                test->item = item;
                swapped = true;
            }
            base = base->next;
            test = test->next;
        }
    }
}


//  --------------------------------------------------------------------------
//  Make a copy of the ring; items are duplicated if you set a duplicator
//  for the ring, otherwise not. Copying a null reference returns a null
//  reference.

zring_t *
zring_dup (zring_t *self)
{
    if (!self)
        return NULL;

    zring_t *copy = zring_new ();
    if (copy) {
        copy->destructor = self->destructor;
        copy->duplicator = self->duplicator;
        copy->comparator = self->comparator;

        node_t *node;
        for (node = self->head->next; node != self->head; node = node->next) {
            void *item = node->item;
            if (self->duplicator)
                item = self->duplicator (item);
            if (!item || zring_append (copy, item)) {
                zring_destroy (&copy);
                break;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Set a user-defined deallocator for ring items; by default items are not
//  freed when the ring is destroyed.

void
zring_set_destructor (zring_t *self, czmq_destructor destructor)
{
    assert (self);
    self->destructor = destructor;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for ring items; by default items are not
//  copied when the ring is duplicated.

void
zring_set_duplicator (zring_t *self, czmq_duplicator duplicator)
{
    assert (self);
    self->duplicator = duplicator;
}


//  --------------------------------------------------------------------------
//  Set a user-defined comparator for zring_find and zring_sort; the method
//  must return -1, 0, or 1 depending on whether item1 is less than, equal to,
//  or greater than, item2.

void
zring_set_comparator (zring_t *self, czmq_comparator comparator)
{
    assert (self);
    self->comparator = comparator;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class

void
zring_test (int verbose)
{
    printf (" * zring: ");

    //  @selftest
    zring_t *ring = zring_new ();
    assert (ring);
    assert (zring_size (ring) == 0);

    //  Three items we'll use as test data
    //  Ring items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";

    int rc = zring_append (ring, cheese);
    assert (!rc);
    assert (zring_size (ring) == 1);
    assert (zring_item (ring) == cheese);
    rc = zring_append (ring, bread);
    assert (!rc);
    assert (zring_size (ring) == 2);
    assert (zring_item (ring) == bread);
    rc = zring_append (ring, wine);
    assert (!rc);
    assert (zring_size (ring) == 3);
    assert (zring_item (ring) == wine);

    assert (zring_first (ring) == cheese);
    assert (zring_next (ring) == bread);
    assert (zring_next (ring) == wine);
    assert (zring_next (ring) == NULL);
    //  After we reach end of ring, next wraps around
    assert (zring_next (ring) == cheese);

    assert (zring_last (ring) == wine);
    assert (zring_prev (ring) == bread);
    assert (zring_prev (ring) == cheese);
    assert (zring_prev (ring) == NULL);
    //  After we reach start of ring, prev wraps around
    assert (zring_prev (ring) == wine);
    zring_purge (ring);

    //  Test some list insertion-deletion combos
    assert (zring_size (ring) == 0);
    zring_prepend (ring, "4");
    zring_append (ring, "3");
    zring_prepend (ring, "5");
    zring_append (ring, "2");
    zring_prepend (ring, "0");
    zring_append (ring, "1");
    assert (zring_size (ring) == 6);

    //  Try the comparator functionality
    zring_set_comparator (ring, (czmq_comparator *) strcmp);
    zring_sort (ring);

    char *item = (char *) zring_first (ring);
    assert (streq (item, "0"));
    item = (char *) zring_find (ring, "5");
    assert (streq (item, "5"));

    //  Try the duplicator and destructor
    zring_set_duplicator (ring, (czmq_duplicator *) strdup);
    zring_t *dup = zring_dup (ring);
    assert (dup);
    zring_set_destructor (dup, (czmq_destructor *) zstr_free);
    assert (zring_size (dup) == 6);
    zring_destroy (&dup);

    //  We're comparing as strings, not item pointers
    rc = zring_remove (ring, "2");
    assert (rc == 0);
    rc = zring_remove (ring, "5");
    assert (rc == 0);
    rc = zring_remove (ring, "3");
    assert (rc == 0);
    item = (char *) zring_detach (ring, NULL);
    zring_purge (ring);

    //  Try the dictionary insert/delete functionality
    rc = zring_insert (ring, "1", "one");
    assert (rc == 0);
    rc = zring_insert (ring, "3", "three");
    assert (rc == 0);
    rc = zring_insert (ring, "2", "two");
    assert (rc == 0);
    rc = zring_insert (ring, "2", "two");
    assert (rc == -1);

    item = (char *) zring_lookup (ring, "2");
    assert (streq (item, "two"));
    item = (char *) zring_lookup (ring, "1");
    assert (streq (item, "one"));
    item = (char *) zring_item (ring);
    assert (streq (item, "one"));

    rc = zring_delete (ring, "3");
    assert (rc == 0);
    rc = zring_delete (ring, "3");
    assert (rc == -1);
    //  Using detach/remove will also remove from dictionary
    rc = zring_remove (ring, "two");
    assert (rc == 0);
    rc = zring_delete (ring, "2");
    assert (rc == -1);
    zring_purge (ring);

    //  Destructor should be safe to call twice
    zring_destroy (&ring);
    assert (ring == NULL);
    zring_destroy (&ring);
    //  @end

    printf ("OK\n");
}
