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

/*
@header
    Provides a generic doubly-linked list container. This container provides
    hooks for duplicator, comparator, and destructor functions. These tie
    into CZMQ and standard C semantics, so e.g. for string items you can
    use strdup, strcmp, and zstr_free. To store custom objects, define your
    own duplicator and comparator, and use the standard object destructor.
@discuss
    This is a reworking of the simpler zlist container. It is faster to
    insert and delete items anywhere in the list, and to keep ordered lists.
@end
*/

#include "../include/czmq.h"

#define NODE_TAG            0x0006cafe

//  List node, used internally only

typedef struct _node_t {
    uint32_t tag;                   //  Object tag for validity checking
    struct _node_t *next;
    struct _node_t *prev;
    void *item;
} node_t;


//  ---------------------------------------------------------------------
//  Structure of our class

struct _zlistx_t {
    node_t *head;                   //  First item in list, if any
    node_t *tail;                   //  Last item in list, if any
    node_t *cursor;                 //  Current cursors for iteration
    size_t size;                    //  Number of items in list
    czmq_duplicator *duplicator;    //  Item duplicator, if any
    czmq_comparator *comparator;    //  Item comparator, if any
    czmq_destructor *destructor;    //  Item destructor, if any
};


//  Initialize a list node and attach to the prev and next nodes, or itself
//  if these are specified as null. Returns new node, or NULL if there was
//  no more heap memory.

static node_t *
s_node_new (void *item)
{
    node_t *self = (node_t *) zmalloc (sizeof (node_t));
    if (self) {
        self->tag = NODE_TAG;
        self->prev = self;
        self->next = self;
        self->item = item;
    }
    return self;
}


//  Removing and inserting a node are actually the same operation:
//      swap (node->next, prev->next)
//      swap (node->prev, next->prev)
//  Which require only that the node be initialized to point to itself.
//  When inserting, node goes in between prev and next.

static void
s_node_relink (node_t *node, node_t *prev, node_t *next)
{
    node_t *temp = node->next;
    node->next = prev->next;
    prev->next = temp;
    temp = node->prev;
    node->prev = next->prev;
    next->prev = temp;
}

//  Default comparator

static int
s_comparator (const void *item1, const void *item2)
{
    if (item1 == item2)
        return 0;
    else
    if (item1 < item2)
        return -1;
    else
        return 1;
}


//  --------------------------------------------------------------------------
//  Create a new, empty list.

zlistx_t *
zlistx_new (void)
{
    zlistx_t *self = (zlistx_t *) zmalloc (sizeof (zlistx_t));
    if (self) {
        self->head = s_node_new (NULL);
        if (self->head) {
            self->cursor = self->head;
            self->comparator = s_comparator;
        }
        else
            zlistx_destroy (&self);

    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a list. If an item destructor was specified, all items in the
//  list are automatically destroyed as well.

void
zlistx_destroy (zlistx_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zlistx_t *self = *self_p;
        zlistx_purge (self);
        free (self->head);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Add an item to the head of the list. Calls the item duplicator, if any,
//  on the item. Resets cursor to list head. Returns an item handle on
//  success, NULL if memory was exhausted.

void *
zlistx_add_start (zlistx_t *self, void *item)
{
    assert (self);
    assert (item);

    if (self->duplicator) {
        item = (self->duplicator)(item);
        if (!item)
            return NULL;        //  Out of memory
    }
    node_t *node = s_node_new (item);
    if (node) {
        //  Insert after head
        s_node_relink (node, self->head, self->head->next);
        self->cursor = self->head;
        self->size++;
        return node;
    }
    else
        return NULL;            //  Out of memory
}


//  --------------------------------------------------------------------------
//  Add an item to the tail of the list. Calls the item duplicator, if any,
//  on the item. Resets cursor to list head. Returns an item handle on
//  success, NULL if memory was exhausted.

void *
zlistx_add_end (zlistx_t *self, void *item)
{
    assert (self);
    assert (item);

    if (self->duplicator) {
        item = (self->duplicator)(item);
        if (!item)
            return NULL;        //  Out of memory
    }
    node_t *node = s_node_new (item);
    if (node) {
        //  Insert before head
        s_node_relink (node, self->head->prev, self->head);
        self->cursor = self->head;
        self->size++;
        return node;
    }
    else
        return NULL;            //  Out of memory
}


//  --------------------------------------------------------------------------
//  Return the number of items in the list

size_t
zlistx_size (zlistx_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.

void *
zlistx_first (zlistx_t *self)
{
    assert (self);
    self->cursor = self->head->next;
    return self->cursor == self->head? NULL: self->cursor->item;
}


//  --------------------------------------------------------------------------
//  Return the next item. At the end of the list (or in an empty list),
//  returns NULL. Use repeated zlistx_next () calls to work through the list
//  from zlistx_first (). First time, acts as zlistx_first().

void *
zlistx_next (zlistx_t *self)
{
    assert (self);
    self->cursor = self->cursor->next;
    return self->cursor == self->head? NULL: self->cursor->item;
}


//  --------------------------------------------------------------------------
//  Return the previous item. At the start of the list (or in an empty list),
//  returns NULL. Use repeated zlistx_prev () calls to work through the list
//  backwards from zlistx_last (). First time, acts as zlistx_last().

void *
zlistx_prev (zlistx_t *self)
{
    assert (self);
    self->cursor = self->cursor->prev;
    return self->cursor == self->head? NULL: self->cursor->item;
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.

void *
zlistx_last (zlistx_t *self)
{
    assert (self);
    self->cursor = self->head->prev;
    return self->cursor == self->head? NULL: self->cursor->item;
}


//  --------------------------------------------------------------------------
//  Returns the value of the item at the cursor, or NULL if the cursor is
//  not pointing to an item.

void *
zlistx_item (zlistx_t *self)
{
    assert (self);
    return self->cursor == self->head? NULL: self->cursor->item;
}


//  --------------------------------------------------------------------------
//  Returns the handle of the item at the cursor, or NULL if the cursor is
//  not pointing to an item.

void *
zlistx_cursor (zlistx_t *self)
{
    assert (self);
    return self->cursor == self->head? NULL: self->cursor;
}

//  --------------------------------------------------------------------------
//  Returns the item associated with the given list handle, or NULL if passed
//  handle is NULL. asserts that the passed in ptr points to a list node.

void *
zlistx_handle_item (void *handle)
{
    if (!handle)
        return NULL;

    node_t *node = (node_t *) handle;
    assert (node->tag == NODE_TAG);
    return node->item;
}


//  --------------------------------------------------------------------------
//  Find an item in the list, searching from the start. Uses the item
//  comparator, if any, else compares item values directly. Returns the
//  item handle found, or NULL. Sets the cursor to the found item, if any.

void *
zlistx_find (zlistx_t *self, void *item)
{
    assert (self);
    assert (item);

    //  Scan list for item, this is a O(N) operation
    node_t *node = self->head->next;
    while (node != self->head) {
        if (self->comparator (node->item, item) == 0) {
            self->cursor = node;
            return node;
        }
        node = node->next;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Detach an item from the list, using its handle. The item is not modified,
//  and the caller is responsible for destroying it if necessary. If handle is
//  null, detaches the first item on the list. Returns item that was detached,
//  or null if none was. If cursor was at item, moves cursor to previous item,
//  so you can detach items while iterating forwards through a list.

void *
zlistx_detach (zlistx_t *self, void *handle)
{
    assert (self);
    node_t *node = (node_t *) handle;
    if (!node)
        node = self->head->next == self->head? NULL: self->head->next;

    //  Now detach node from list, without destroying item
    if (node) {
        //  Reposition cursor so that delete/detach works during iteration
        if (self->cursor == node)
            self->cursor = self->cursor->prev;
        //  Remove node from list
        assert (node->tag == NODE_TAG);
        s_node_relink (node, node->prev, node->next);
        node->tag = 0xDeadBeef;
        void *item = node->item;
        free (node);
        self->size--;
        return item;
    }
    else {
        assert (self->size == 0);
        return NULL;
    }
}


//  --------------------------------------------------------------------------
//  Delete an item, using its handle. Calls the item destructor is any is
//  set. If handle is null, deletes the first item on the list. Returns 0
//  if an item was deleted, -1 if not. If cursor was at item, moves cursor
//  to previous item, so you can delete items while iterating forwards
//  through a list.

int
zlistx_delete (zlistx_t *self, void *handle)
{
    assert (self);
    void *item = zlistx_detach (self, handle);
    if (item) {
        if (self->destructor)
            self->destructor (&item);
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Move an item to the start of the list, via its handle.

void
zlistx_move_start (zlistx_t *self, void *handle)
{
    assert (self);
    assert (handle);
    node_t *node = (node_t *) handle;
    assert (node->tag == NODE_TAG);

    node_t *next = self->head->next;
    if (node != next) {
        //  Remove node from list, insert before next node
        s_node_relink (node, node->prev, node->next);
        s_node_relink (node, next->prev, next);
    }
}


//  --------------------------------------------------------------------------
//  Move an item to the end of the list, via its handle.

void
zlistx_move_end (zlistx_t *self, void *handle)
{
    assert (self);
    assert (handle);
    node_t *node = (node_t *) handle;
    assert (node->tag == NODE_TAG);

    node_t *prev = self->head->prev;
    if (node != prev) {
        //  Remove node from list, insert after prev node
        s_node_relink (node, node->prev, node->next);
        s_node_relink (node, prev, prev->next);
    }
}


//  --------------------------------------------------------------------------
//  Remove all items from the list, and destroy them if the item destructor
//  is set.

void
zlistx_purge (zlistx_t *self)
{
    assert (self);
    while (zlistx_size (self) > 0)
        zlistx_delete (self, NULL);
}


//  --------------------------------------------------------------------------
//  Sort the list. If an item comparator was set, calls that to compare
//  items, otherwise compares on item value. The sort is not stable, so may
//  reorder equal items.

void
zlistx_sort (zlistx_t *self)
{
    //  Uses a comb sort, which is simple and reasonably fast
    //  See http://en.wikipedia.org/wiki/Comb_sort
    assert (self);
    int gap = self->size;
    bool swapped = false;
    while (gap > 1 || swapped) {
        gap = (int) ((double) gap / 1.3);
        node_t *base = self->head->next;
        node_t *test = self->head->next;
        int jump = gap;
        while (jump--)
            test = test->next;

        swapped = false;
        while (base != self->head && test != self->head) {
            if (self->comparator (base->item, test->item) > 0) {
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
//  Create a new node and insert it into a sorted list. Calls the item
//  duplicator, if any, on the item. If low_value is true, starts searching
//  from the start of the list, otherwise searches from the end. Use the item
//  comparator, if any, to find where to place the new node. Returns a handle
//  to the new node, or NULL if memory was exhausted. Resets the cursor to the
//  list head.

void *
zlistx_insert (zlistx_t *self, void *item, bool low_value)
{
    assert (self);
    if (self->duplicator) {
        item = (self->duplicator)(item);
        if (!item)
            return NULL;        //  Out of memory
    }
    node_t *node = s_node_new (item);
    if (node) {
        zlistx_reorder (self, node, low_value);
        self->cursor = self->head;
        self->size++;
    }
    return node;
}


//  --------------------------------------------------------------------------
//  Move an item, specified by handle, into position in a sorted list. Uses
//  the item comparator, if any, to determine the new location. If low_value
//  is true, starts searching from the start of the list, otherwise searches
//  from the end.

void
zlistx_reorder (zlistx_t *self, void *handle, bool low_value)
{
    assert (self);
    assert (handle);
    node_t *node = (node_t *) handle;
    assert (node->tag == NODE_TAG);

    //  Remove node from list, if it's attached
    s_node_relink (node, node->prev, node->next);

    if (low_value) {
        node_t *next = self->head->next;
        while (next != self->head) {
            if (self->comparator (node->item, next->item) <= 0)
                break;
            next = next->next;
        }
        //  Relink node before next node
        s_node_relink (node, next->prev, next);
    }
    else {
        node_t *prev = self->head->prev;
        while (prev != self->head) {
            if (self->comparator (prev->item, node->item) <= 0)
                break;
            prev = prev->prev;
        }
        //  Relink node after prev node
        s_node_relink (node, prev, prev->next);
    }
}


//  --------------------------------------------------------------------------
//  Make a copy of the list; items are duplicated if you set a duplicator
//  for the list, otherwise not. Copying a null reference returns a null
//  reference.

zlistx_t *
zlistx_dup (zlistx_t *self)
{
    if (!self)
        return NULL;

    zlistx_t *copy = zlistx_new ();
    if (copy) {
        //  Copy item handlers
        copy->destructor = self->destructor;
        copy->duplicator = self->duplicator;
        copy->comparator = self->comparator;

        //  Copy nodes
        node_t *node;
        for (node = self->head->next; node != self->head; node = node->next) {
            if (!zlistx_add_end (copy, node->item)) {
                zlistx_destroy (&copy);
                break;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Set a user-defined deallocator for list items; by default items are not
//  freed when the list is destroyed.

void
zlistx_set_destructor (zlistx_t *self, czmq_destructor destructor)
{
    assert (self);
    self->destructor = destructor;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for list items; by default items are not
//  copied when the list is duplicated.

void
zlistx_set_duplicator (zlistx_t *self, czmq_duplicator duplicator)
{
    assert (self);
    self->duplicator = duplicator;
}


//  --------------------------------------------------------------------------
//  Set a user-defined comparator for zlistx_find and zlistx_sort; the method
//  must return -1, 0, or 1 depending on whether item1 is less than, equal to,
//  or greater than, item2.

void
zlistx_set_comparator (zlistx_t *self, czmq_comparator comparator)
{
    assert (self);
    self->comparator = comparator;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class

void
zlistx_test (int verbose)
{
    printf (" * zlistx: ");

    //  @selftest
    zlistx_t *list = zlistx_new ();
    assert (list);
    assert (zlistx_size (list) == 0);

    //  Test operations on an empty list
    assert (zlistx_first (list) == NULL);
    assert (zlistx_last (list) == NULL);
    assert (zlistx_next (list) == NULL);
    assert (zlistx_prev (list) == NULL);
    assert (zlistx_find (list, "hello") == NULL);
    assert (zlistx_delete (list, NULL) == -1);
    assert (zlistx_detach (list, NULL) == NULL);
    assert (zlistx_delete (list, NULL) == -1);
    assert (zlistx_detach (list, NULL) == NULL);
    zlistx_purge (list);
    zlistx_sort (list);

    //  Use item handlers
    zlistx_set_destructor (list, (czmq_destructor *) zstr_free);
    zlistx_set_duplicator (list, (czmq_duplicator *) strdup);
    zlistx_set_comparator (list, (czmq_comparator *) strcmp);

    //  Try simple insert/sort/delete/next
    assert (zlistx_next (list) == NULL);
    zlistx_add_end (list, "world");
    assert (streq ((char *) zlistx_next (list), "world"));
    zlistx_add_end (list, "hello");
    assert (streq ((char *) zlistx_prev (list), "hello"));
    zlistx_sort (list);
    assert (zlistx_size (list) == 2);
    void *handle = zlistx_find (list, "hello");
    char *item1 = (char *) zlistx_item (list);
    char *item2 = (char *) zlistx_handle_item (handle);
    assert (item1 == item2);
    assert (streq (item1, "hello"));
    zlistx_delete (list, handle);
    assert (zlistx_size (list) == 1);
    char *string = (char *) zlistx_detach (list, NULL);
    assert (streq (string, "world"));
    free (string);
    assert (zlistx_size (list) == 0);

    //  Check next/back work
    //  Now populate the list with items
    zlistx_add_start (list, "five");
    zlistx_add_end   (list, "six");
    zlistx_add_start (list, "four");
    zlistx_add_end   (list, "seven");
    zlistx_add_start (list, "three");
    zlistx_add_end   (list, "eight");
    zlistx_add_start (list, "two");
    zlistx_add_end   (list, "nine");
    zlistx_add_start (list, "one");
    zlistx_add_end   (list, "ten");

    //  Test our navigation skills
    assert (zlistx_size (list) == 10);
    assert (streq ((char *) zlistx_last (list), "ten"));
    assert (streq ((char *) zlistx_prev (list), "nine"));
    assert (streq ((char *) zlistx_prev (list), "eight"));
    assert (streq ((char *) zlistx_prev (list), "seven"));
    assert (streq ((char *) zlistx_prev (list), "six"));
    assert (streq ((char *) zlistx_prev (list), "five"));
    assert (streq ((char *) zlistx_first (list), "one"));
    assert (streq ((char *) zlistx_next (list), "two"));
    assert (streq ((char *) zlistx_next (list), "three"));
    assert (streq ((char *) zlistx_next (list), "four"));

    //  Sort by alphabetical order
    zlistx_sort (list);
    assert (streq ((char *) zlistx_first (list), "eight"));
    assert (streq ((char *) zlistx_last (list), "two"));

    //  Moving items around
    handle = zlistx_find (list, "six");
    zlistx_move_start (list, handle);
    assert (streq ((char *) zlistx_first (list), "six"));
    zlistx_move_end (list, handle);
    assert (streq ((char *) zlistx_last (list), "six"));
    zlistx_sort (list);
    assert (streq ((char *) zlistx_last (list), "two"));

    //  Copying a list
    zlistx_t *copy = zlistx_dup (list);
    assert (copy);
    assert (zlistx_size (copy) == 10);
    assert (streq ((char *) zlistx_first (copy), "eight"));
    assert (streq ((char *) zlistx_last (copy), "two"));
    zlistx_destroy (&copy);

    //  Delete items while iterating
    string = (char *) zlistx_first (list);
    assert (streq (string, "eight"));
    string = (char *) zlistx_next (list);
    assert (streq (string, "five"));
    zlistx_delete (list, zlistx_cursor (list));
    string = (char *) zlistx_next (list);
    assert (streq (string, "four"));

    zlistx_purge (list);
    zlistx_destroy (&list);
    //  @end

    printf ("OK\n");
}
