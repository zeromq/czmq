/*  =========================================================================
    zhashring - generic type-free doubly linked ring container

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
    items. The zhashring class is more complex and powerful than zlist, which
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

struct _zhashring_t {
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
//  Initialize a ring node and attach to the prev and next nodes, or itself
//  if these are specified as null. Returns new node, or NULL if process ran
//  out of heap memory.

static node_t *
s_node_new (node_t *prev, node_t *next, void *item)
{
    node_t *self = (node_t *) zmalloc (sizeof (node_t));
    if (self) {
        self->prev = prev? prev: self;
        self->next = next? next: self;
        self->item = item;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Create a new ring container (a ring is a doubly-linked ring)

zhashring_t *
zhashring_new (void)
{
    zhashring_t *self = (zhashring_t *) zmalloc (sizeof (zhashring_t));
    if (self) {
        self->head = s_node_new (NULL, NULL, NULL);
        self->cursor = self->head;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a ring container

void
zhashring_destroy (zhashring_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zhashring_t *self = *self_p;
        zhashring_purge (self);
        assert (!self->hash || zhash_size (self->hash) == 0);
        zhash_destroy (&self->hash);
        free (self->head);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Prepend an item to the start of the ring, return 0 if OK, else -1.
//  Leaves cursor at newly inserted item.

int
zhashring_prepend (zhashring_t *self, void *item)
{
    assert (self);
    node_t *node = s_node_new (self->head, self->head->next, item);
    if (node) {
        self->head->next->prev = node;
        self->head->next = node;
        self->cursor = node;
        self->size++;
        return 0;
    }
    return -1;
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the ring, return 0 if OK, else -1.
//  Leaves cursor at newly inserted item.

int
zhashring_append (zhashring_t *self, void *item)
{
    assert (self);
    node_t *node = s_node_new (self->head->prev, self->head, item);
    if (node) {
        self->head->prev->next = node;
        self->head->prev = node;
        self->cursor = node;
        self->size++;
        return 0;
    }
    return -1;
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the ring, and insert into the ring
//  dictionary, so that you can find the item rapidly using zhashring_lookup.
//  If you do a lot of item searches, this is faster than zhashring_find,
//  which is at worst an O(N) operation. When items leave the ring, they
//  are always removed from the dictionary. Returns 0 on success, -1 if
//  the key already existed in the dictionary, or heap memory ran out.

int
zhashring_insert (zhashring_t *self, const void *key, void *item)
{
    assert (self);
    assert (key);
    assert (item);
    
    if (!self->hash)
        self->hash = zhash_new ();
    if (!self->hash)
        return -1;

    //  If item isn't already in dictionary, append to list and then
    //  store item node (which is in cursor) in dictionary
    if (!zhash_lookup (self->hash, key)
    &&  !zhashring_append (self, item)
    &&  !zhash_insert (self->hash, key, self->cursor)) {
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
zhashring_find (zhashring_t *self, void *item)
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
//  dictionary (via zhashring_insert), then sets the ring cursor to the item,
//  and returns the item value. If not, leaves the cursor unchanged, and
//  returns NULL.

void *
zhashring_lookup (zhashring_t *self, const void *key)
{
    assert (self);
    assert (key);
    
    if (self->hash) {
      node_t *node =
          (node_t *) zhash_lookup (self->hash, key);
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
zhashring_detach (zhashring_t *self, void *item)
{
    assert (self);
    
    node_t *found = NULL;
    if ((item && zhashring_find (self, item))
    || (!item && self->cursor != self->head))
        found = self->cursor;

    //  Now detach node from list, without destroying it
    if (found) {
        item = found->item;
        found->next->prev = found->prev;
        found->prev->next = found->next;
        self->cursor = found->next;
        self->size--;
        if (found->key)
            zhash_delete (self->hash, found->key);
        free (found);
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
zhashring_remove (zhashring_t *self, void *item)
{
    assert (self);
    if ((item = zhashring_detach (self, item))) {
        if (self->destructor)
            self->destructor (&item);
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Search the ring dictionary for an item, by key. If the item is in the
//  dictionary (via zhashring_insert), then removes the item from the ring and
//  calls the item destructor, if any is found. Returns 0 if the item was
//  found and removed, else -1 if not found.

int
zhashring_delete (zhashring_t *self, const void *key)
{
    assert (self);
    assert (key);

    if (zhashring_lookup (self, key))
        return zhashring_remove (self, NULL);
    else
        return -1;          //  Not found in dictionary
}


//  --------------------------------------------------------------------------
//  Remove all items from the ring. If the item destructor is set, calls it
//  on every item.

void
zhashring_purge (zhashring_t *self)
{
    assert (self);
    while (zhashring_remove (self, zhashring_first (self)) == 0);
}


//  --------------------------------------------------------------------------
//  Return the number of items in the ring

size_t
zhashring_size (zhashring_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Return the item at the head of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the ring is empty.

void *
zhashring_first (zhashring_t *self)
{
    assert (self);
    self->cursor = self->head;
    return zhashring_next (self);
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the ring is empty.

void *
zhashring_last (zhashring_t *self)
{
    assert (self);
    self->cursor = self->head;
    return zhashring_prev (self);
}


//  --------------------------------------------------------------------------
//  Return the next item. At the end of the ring (or in an empty ring),
//  returns NULL. Use repeated zhashring_next () calls to work through the
//  ring from zhashring_first ().

void *
zhashring_next (zhashring_t *self)
{
    assert (self);
    self->cursor = self->cursor->next;
    return zhashring_item (self);
}


//  --------------------------------------------------------------------------
//  Return the previous item. At the start of the ring (or in an empty ring),
//  returns NULL. Use repeated zhashring_prev () calls to work through the
//  ring backwards from zhashring_last ().

void *
zhashring_prev (zhashring_t *self)
{
    assert (self);
    self->cursor = self->cursor->prev;
    return zhashring_item (self);
}


//  --------------------------------------------------------------------------
//  Return current item in the ring. If the ring is empty, or the cursor
//  passed the end of the ring, returns NULL. Does not change the cursor.

void *
zhashring_item (zhashring_t *self)
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
zhashring_sort (zhashring_t *self)
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

zhashring_t *
zhashring_dup (zhashring_t *self)
{
    if (!self)
        return NULL;

    zhashring_t *copy = zhashring_new ();
    if (copy) {
        copy->destructor = self->destructor;
        copy->duplicator = self->duplicator;
        copy->comparator = self->comparator;
        
        node_t *node;
        for (node = self->head->next; node != self->head; node = node->next) {
            void *item = node->item;
            if (self->duplicator)
                item = self->duplicator (item);
            if (!item || zhashring_append (copy, item) == -1) {
                zhashring_destroy (&copy);
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
zhashring_set_destructor (zhashring_t *self, czmq_destructor destructor)
{
    assert (self);
    self->destructor = destructor;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for ring items; by default items are not
//  copied when the ring is duplicated.

void
zhashring_set_duplicator (zhashring_t *self, czmq_duplicator duplicator)
{
    assert (self);
    self->duplicator = duplicator;
}


//  --------------------------------------------------------------------------
//  Set a user-defined comparator for zhashring_find and zhashring_sort; the
//  method must return -1, 0, or 1 depending on whether item1 is less than,
//  equal to, or greater than, item2.

void
zhashring_set_comparator (zhashring_t *self, czmq_comparator comparator)
{
    assert (self);
    self->comparator = comparator;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class

void
zhashring_test (int verbose)
{
    printf (" * zhashring: ");

    //  @selftest
    zhashring_t *ring = zhashring_new ();
    assert (ring);
    assert (zhashring_size (ring) == 0);

    //  Three items we'll use as test data
    //  Ring items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";

    int rc = zhashring_append (ring, cheese);
    assert (rc == 0);
    assert (zhashring_size (ring) == 1);
    assert (zhashring_item (ring) == cheese);
    rc = zhashring_append (ring, bread);
    assert (rc == 0);
    assert (zhashring_size (ring) == 2);
    assert (zhashring_item (ring) == bread);
    rc = zhashring_append (ring, wine);
    assert (rc == 0);
    assert (zhashring_size (ring) == 3);
    assert (zhashring_item (ring) == wine);

    assert (zhashring_first (ring) == cheese);
    assert (zhashring_next (ring) == bread);
    assert (zhashring_next (ring) == wine);
    assert (zhashring_next (ring) == NULL);
    //  After we reach end of ring, next wraps around
    assert (zhashring_next (ring) == cheese);
    
    assert (zhashring_last (ring) == wine);
    assert (zhashring_prev (ring) == bread);
    assert (zhashring_prev (ring) == cheese);
    assert (zhashring_prev (ring) == NULL);
    //  After we reach start of ring, prev wraps around
    assert (zhashring_prev (ring) == wine);
    zhashring_purge (ring);

    //  Test some list insertion-deletion combos
    assert (zhashring_size (ring) == 0);
    zhashring_prepend (ring, "4");
    zhashring_append (ring, "3");
    zhashring_prepend (ring, "5");
    zhashring_append (ring, "2");
    zhashring_prepend (ring, "0");
    zhashring_append (ring, "1");
    assert (zhashring_size (ring) == 6);

    //  Try the comparator functionality
    zhashring_set_comparator (ring, (czmq_comparator *) strcmp);
    zhashring_sort (ring);
    
    char *item = (char *) zhashring_first (ring);
    assert (streq (item, "0"));
    item = (char *) zhashring_find (ring, "5");
    assert (streq (item, "5"));

    //  Try the duplicator and destructor
    zhashring_set_duplicator (ring, (czmq_duplicator *) strdup);
    zhashring_t *dup = zhashring_dup (ring);
    assert (dup);
    zhashring_set_destructor (dup, (czmq_destructor *) zstr_free);
    assert (zhashring_size (dup) == 6);
    zhashring_destroy (&dup);

    //  We're comparing as strings, not item pointers
    rc = zhashring_remove (ring, "2");
    assert (rc == 0);
    rc = zhashring_remove (ring, "5");
    assert (rc == 0);
    rc = zhashring_remove (ring, "3");
    assert (rc == 0);
    item = (char *) zhashring_detach (ring, NULL);
    zhashring_purge (ring);

    //  Try the dictionary insert/delete functionality
    rc = zhashring_insert (ring, "1", "one");
    assert (rc == 0);
    rc = zhashring_insert (ring, "3", "three");
    assert (rc == 0);
    rc = zhashring_insert (ring, "2", "two");
    assert (rc == 0);
    rc = zhashring_insert (ring, "2", "two");
    assert (rc == -1);
    
    item = (char *) zhashring_lookup (ring, "2");
    assert (streq (item, "two"));
    item = (char *) zhashring_lookup (ring, "1");
    assert (streq (item, "one"));
    item = (char *) zhashring_item (ring);
    assert (streq (item, "one"));
    
    rc = zhashring_delete (ring, "3");
    assert (rc == 0);
    rc = zhashring_delete (ring, "3");
    assert (rc == -1);
    //  Using detach/remove will also remove from dictionary
    rc = zhashring_remove (ring, "two");
    assert (rc == 0);
    rc = zhashring_delete (ring, "2");
    assert (rc == -1);
    zhashring_purge (ring);
    
    //  Destructor should be safe to call twice
    zhashring_destroy (&ring);
    assert (ring == NULL);
    zhashring_destroy (&ring);
    //  @end

    printf ("OK\n");
}
