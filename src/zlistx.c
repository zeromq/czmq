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
    Provides a generic container implementing a fast singly-linked list. You
    can use this to construct multi-dimensional lists, and other structures
    together with other generic containers like zhash.
@discuss
    To iterate through a list, use zlistx_first to get the first item, then
    loop while not null, and do zlistx_next at the end of each iteration.
@end
*/

#include "../include/czmq.h"

//  List node, used internally only

typedef struct _node_t {
    struct _node_t *next;
    void *item;
    zlistx_free_fn *free_fn;
} node_t;


//  ---------------------------------------------------------------------
//  Structure of our class

struct _zlistx_t {
    node_t *head;               //  First item in list, if any
    node_t *tail;               //  Last item in list, if any
    node_t *cursor;             //  Current cursors for iteration
    size_t size;                //  Number of items in list
    //  Function callbacks for duplicating and destroying items, if any
    czmq_duplicator *duplicator;
    czmq_destructor *destructor;
};


//  --------------------------------------------------------------------------
//  List constructor

zlistx_t *
zlistx_new (void)
{
    zlistx_t *self = (zlistx_t *) zmalloc (sizeof (zlistx_t));
    return self;
}


//  --------------------------------------------------------------------------
//  List destructor

void
zlistx_destroy (zlistx_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zlistx_t *self = *self_p;
        zlistx_purge (self);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.

void *
zlistx_first (zlistx_t *self)
{
    assert (self);
    self->cursor = self->head;
    if (self->cursor)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the next item. If the list is empty, returns NULL. To move to
//  the start of the list call zlistx_first (). Advances the cursor.

void *
zlistx_next (zlistx_t *self)
{
    assert (self);
    if (self->cursor)
        self->cursor = self->cursor->next;
    else
        self->cursor = self->head;
    if (self->cursor)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.

void *
zlistx_last (zlistx_t *self)
{
    assert (self);
    self->cursor = self->tail;
    if (self->cursor)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor as-is.

void *
zlistx_head (zlistx_t *self)
{
    assert (self);
    return self->head ? self->head->item : NULL;
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor as-is.

void *
zlistx_tail (zlistx_t *self)
{
    assert (self);
    return self->tail ? self->tail->item : NULL;
}


//  --------------------------------------------------------------------------
//  Return current item in the list. If the list is empty, or the cursor
//  passed the end of the list, returns NULL. Does not change the cursor.

void *
zlistx_item (zlistx_t *self)
{
    assert (self);
    if (self->cursor)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the list, return 0 if OK or -1 if this
//  failed for some reason (out of memory). Note that if a duplicator has
//  been set, this method will also duplicate the item.

int
zlistx_append (zlistx_t *self, void *item)
{
    if (!item)
        return -1;

    node_t *node;
    node = (node_t *) zmalloc (sizeof (node_t));
    if (!node)
        return -1;

    //  If necessary, take duplicate of (string) item
    if (self->duplicator)
        item = (self->duplicator)(item);

    node->item = item;
    if (self->tail)
        self->tail->next = node;
    else
        self->head = node;

    self->tail = node;
    node->next = NULL;

    self->size++;
    self->cursor = NULL;
    return 0;
}


//  --------------------------------------------------------------------------
//  Push an item to the start of the list, return 0 if OK or -1 if this
//  failed for some reason (out of memory). Note that if a duplicator has
//  been set, this method will also duplicate the item.

int
zlistx_push (zlistx_t *self, void *item)
{
    node_t *node;
    node = (node_t *) zmalloc (sizeof (node_t));
    if (!node)
        return -1;

    //  If necessary, take duplicate of (string) item
    if (self->duplicator)
        item = (self->duplicator)(item);

    node->item = item;
    node->next = self->head;
    self->head = node;
    if (self->tail == NULL)
        self->tail = node;

    self->size++;
    self->cursor = NULL;
    return 0;
}


//  --------------------------------------------------------------------------
//  Remove item from the beginning of the list, returns NULL if none

void *
zlistx_pop (zlistx_t *self)
{
    node_t *node = self->head;
    void *item = NULL;
    if (node) {
        item = node->item;
        self->head = node->next;
        if (self->tail == node)
            self->tail = NULL;
        free (node);
        self->size--;
    }
    self->cursor = NULL;
    return item;
}


//  --------------------------------------------------------------------------
//  Remove the item from the list, if present. Safe to call on items that
//  are not in the list.

void
zlistx_remove (zlistx_t *self, void *item)
{
    node_t *node, *prev = NULL;

    //  First off, we need to find the list node
    for (node = self->head; node != NULL; node = node->next) {
        if (node->item == item)
            break;
        prev = node;
    }
    if (node) {
        if (prev)
            prev->next = node->next;
        else
            self->head = node->next;

        if (node->next == NULL)
            self->tail = prev;
        if (self->cursor == node)
            self->cursor = prev;

        if (self->destructor)
            (self->destructor)(&node->item);
        else
        if (node->free_fn)
            (node->free_fn)(node->item);

        free (node);
        self->size--;
    }
}


//  --------------------------------------------------------------------------
//  Make a copy of list. If the list has a duplicator set, the copied list will
//  duplicate all items. Otherwise, the list will hold pointers back
//  to the items in the original list.

zlistx_t *
zlistx_dup (zlistx_t *self)
{
    if (!self)
        return NULL;

    zlistx_t *copy = zlistx_new ();

    if (copy) {
        copy->destructor = self->destructor;
        copy->duplicator = self->duplicator;
        node_t *node;
        for (node = self->head; node; node = node->next) {
            if (zlistx_append (copy, node->item) == -1) {
                zlistx_destroy (&copy);
                break;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Purge all items from list

void
zlistx_purge (zlistx_t *self)
{
    assert (self);
    node_t *node = self->head;
    while (node) {
        node_t *next = node->next;
        if (self->destructor)
            (self->destructor)(&node->item);
        else
        if (node->free_fn)
            (node->free_fn)(node->item);

        free (node);
        node = next;
    }
    self->head = NULL;
    self->tail = NULL;
    self->cursor = NULL;
    self->size = 0;
}


//  --------------------------------------------------------------------------
//  Return the number of items in the list

size_t
zlistx_size (zlistx_t *self)
{
    return self->size;
}


//  --------------------------------------------------------------------------
//  Sort the list by ascending key value using a straight ASCII comparison.
//  The sort is not stable, so may reorder items with the same keys.

void
zlistx_sort (zlistx_t *self, zlistx_compare_fn *compare)
{
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
        while (base && test) {
            if ((*compare)(base->item, test->item)) {
                //  It's trivial to swap items in a generic container
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
//  Set a user-defined deallocator for items; by default items are not
//  freed when the list is destroyed.

void
zlistx_set_destructor (zlistx_t *self, czmq_destructor destructor)
{
    assert (self);
    self->destructor = destructor;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for items; by default items are not
//  copied when the list is duplicated.

void
zlistx_set_duplicator (zlistx_t *self, czmq_duplicator duplicator)
{
    assert (self);
    self->duplicator = duplicator;
}


//  --------------------------------------------------------------------------
//  DEPRECATED by zlist_set_duplicator/zlist_set_destructor
//  Set a free function for the specified list item. When the item is
//  destroyed, the free function, if any, is called on that item.
//  Use this when list items are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.

void *
zlistx_freefn (zlistx_t *self, void *item, zlistx_free_fn *fn, bool at_tail)
{
    node_t *node = self->head;
    if (at_tail)
        node = self->tail;

    while (node) {
        if (node->item == item) {
            node->free_fn = fn;
            return item;
        }
        node = node->next;
    }
    return NULL;
}

//  --------------------------------------------------------------------------
//  DEPRECATED by zlist_set_duplicator/zlist_set_destructor
//  Set list for automatic item destruction; item values MUST be strings.
//  By default a list item refers to a value held elsewhere. When you set
//  this, each time you append or push a list item, zlistx will take a copy
//  of the string value. Then, when you destroy the list, it will free all
//  item values automatically. If you use any other technique to allocate
//  list values, you must free them explicitly before destroying the list.
//  The usual technique is to pop list items and destroy them, until the
//  list is empty.

void
zlistx_autofree (zlistx_t *self)
{
    assert (self);
    zlistx_set_destructor (self, (czmq_destructor *) zstr_free);
    zlistx_set_duplicator (self, (czmq_duplicator *) strdup);
}


static void
s_zlistx_free (void *data)
{
    zlistx_t *self = (zlistx_t *) data;
    zlistx_destroy (&self);
}

static bool
s_compare (void *item1, void *item2)
{
    if (strcmp ((char *) item1, (char *) item2) > 0)
        return true;
    else
        return false;
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

    //  Three items we'll use as test data
    //  List items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";

    zlistx_append (list, cheese);
    assert (zlistx_size (list) == 1);
    zlistx_append (list, bread);
    assert (zlistx_size (list) == 2);
    zlistx_append (list, wine);
    assert (zlistx_size (list) == 3);

    assert (zlistx_head (list) == cheese);
    assert (zlistx_next (list) == cheese);

    assert (zlistx_first (list) == cheese);
    assert (zlistx_tail (list) == wine);
    assert (zlistx_next (list) == bread);

    assert (zlistx_first (list) == cheese);
    assert (zlistx_next (list) == bread);
    assert (zlistx_next (list) == wine);
    assert (zlistx_next (list) == NULL);
    //  After we reach end of list, next wraps around
    assert (zlistx_next (list) == cheese);
    assert (zlistx_size (list) == 3);

    zlistx_remove (list, wine);
    assert (zlistx_size (list) == 2);

    assert (zlistx_first (list) == cheese);
    zlistx_remove (list, cheese);
    assert (zlistx_size (list) == 1);
    assert (zlistx_first (list) == bread);

    zlistx_remove (list, bread);
    assert (zlistx_size (list) == 0);

    zlistx_append (list, cheese);
    zlistx_append (list, bread);
    assert (zlistx_last (list) == bread);
    zlistx_remove (list, bread);
    assert (zlistx_last (list) == cheese);
    zlistx_remove (list, cheese);
    assert (zlistx_last (list) == NULL);

    zlistx_push (list, cheese);
    assert (zlistx_size (list) == 1);
    assert (zlistx_first (list) == cheese);

    zlistx_push (list, bread);
    assert (zlistx_size (list) == 2);
    assert (zlistx_first (list) == bread);
    assert (zlistx_item (list) == bread);

    zlistx_append (list, wine);
    assert (zlistx_size (list) == 3);
    assert (zlistx_first (list) == bread);

    zlistx_t *sub_list = zlistx_dup (list);
    assert (sub_list);
    assert (zlistx_size (sub_list) == 3);

    zlistx_sort (list, s_compare);
    char *item;
    item = (char *) zlistx_pop (list);
    assert (item == bread);
    item = (char *) zlistx_pop (list);
    assert (item == wine);
    item = (char *) zlistx_pop (list);
    assert (item == cheese);
    assert (zlistx_size (list) == 0);

    assert (zlistx_size (sub_list) == 3);
    zlistx_push (list, sub_list);
    zlistx_t *sub_list_2 = zlistx_dup (sub_list);
    zlistx_append (list, sub_list_2);
    assert (zlistx_freefn (list, sub_list, &s_zlistx_free, false) == sub_list);
    assert (zlistx_freefn (list, sub_list_2, &s_zlistx_free, true) == sub_list_2);

    //  Destructor should be safe to call twice
    zlistx_destroy (&list);
    zlistx_destroy (&list);
    assert (list == NULL);
    //  @end

    printf ("OK\n");
}
