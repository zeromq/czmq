/*  =========================================================================
    zlist - generic type-free list container

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
    To iterate through a list, use zlist_first to get the first item, then
    loop while not null, and do zlist_next at the end of each iteration.
@end
*/

#include "../include/czmq.h"

//  List node, used internally only

typedef struct _node_t {
    struct _node_t *next;
    void *item;
    //  Supporting deprecated v2 functionality
    zlist_free_fn *free_fn;
} node_t;


//  ---------------------------------------------------------------------
//  Structure of our class

struct _zlist_t {
    node_t *head;               //  First item in list, if any
    node_t *tail;               //  Last item in list, if any
    node_t *cursor;             //  Current cursor for iteration
    size_t size;                //  Number of items in list
    //  Function callbacks for duplicating and destroying items, if any
    czmq_duplicator *duplicator;
    czmq_destructor *destructor;
    //  Supporting deprecated v2 functionality
    bool autofree;              //  If true, free items in destructor
};


//  --------------------------------------------------------------------------
//  List constructor

zlist_t *
zlist_new (void)
{
    zlist_t *self = (zlist_t *) zmalloc (sizeof (zlist_t));
    return self;
}


//  --------------------------------------------------------------------------
//  List destructor

void
zlist_destroy (zlist_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zlist_t *self = *self_p;
        node_t *node = (*self_p)->head;
        while (node) {
            node_t *next = node->next;
            if (self->destructor)
                (self->destructor) (&node->item);
            else
            if (node->free_fn)
                (node->free_fn) (node->item);
            else
            if (self->autofree)
                free (node->item);
            free (node);
            node = next;
        }
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the list is empty.

void *
zlist_first (zlist_t *self)
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
//  the start of the list call zlist_first (). Advances the cursor.

void *
zlist_next (zlist_t *self)
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
zlist_last (zlist_t *self)
{
    assert (self);
    self->cursor = self->tail;
    if (self->cursor)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return current item in the list. If the list is empty, or the cursor
//  passed the end of the list, returns NULL. Does not change the cursor.

void *
zlist_item (zlist_t *self)
{
    assert (self);
    if (self->cursor)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Prepend an item to the start of the list, return 0 if OK, else -1.
//  Leaves cursor at newly inserted item.

int
zlist_prepend (zlist_t *self, void *item)
{
    node_t *node = (node_t *) zmalloc (sizeof (node_t));
    if (!node)
        return -1;

    //  If necessary, take duplicate of (string) item
    if (self->autofree)
        item = strdup ((char *) item);
    node->item = item;

    //  Link node into list at start
    node->next = self->head;
    self->head = node;
    if (self->tail == NULL)
        self->tail = node;

    self->size++;
    self->cursor = node;
    return 0;
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the list, return 0 if OK, else -1.
//  Leaves cursor at newly inserted item.

int
zlist_append (zlist_t *self, void *item)
{
    if (!item)
        return -1;

    node_t *node = (node_t *) zmalloc (sizeof (node_t));
    if (!node)
        return -1;

    //  If necessary, take duplicate of (string) item
    if (self->autofree)
        item = strdup ((char *) item);
    node->item = item;
    
    //  Link node into list at end
    if (self->tail)
        self->tail->next = node;
    else
        self->head = node;
    self->tail = node;
    node->next = NULL;

    self->size++;
    self->cursor = node;
    return 0;
}


//  --------------------------------------------------------------------------
//  Detach specified item from the list. If the item is not present in the
//  list, returns null. Caller must destroy item when finished with it.
//  Returns 0 if an item was detached, else -1.

int
zlist_detach (zlist_t *self, void *item)
{
    assert (self);
    //  Find item in ring, and detach if present
    node_t *node, *prev = NULL;
    for (node = self->head; node != NULL; node = node->next) {
        if (node->item == item) {
            if (prev)
                prev->next = node->next;
            else
                self->head = node->next;
            if (node->next == NULL)
                self->tail = prev;
            if (self->cursor == node)
                self->cursor = prev;
            free (node);
            self->cursor = self->head;
            self->size--;
            return 0;
        }
        prev = node;
    }
    return -1;
}


//  --------------------------------------------------------------------------
//  Delete specified item from the list. If the item is not present in the
//  list, returns null. Calls the list item destructor if one was set.
//  Returns 0 if an item was deleted, else -1.

int
zlist_delete (zlist_t *self, void *item)
{
    assert (self);
    //  Find item in ring, and detach if present
    node_t *node, *prev = NULL;
    for (node = self->head; node != NULL; node = node->next) {
        if (node->item == item) {
            if (prev)
                prev->next = node->next;
            else
                self->head = node->next;
            if (node->next == NULL)
                self->tail = prev;
            if (self->cursor == node)
                self->cursor = prev;
            if (self->destructor)
                (self->destructor) (&node->item);
            free (node);
            self->cursor = self->head;
            self->size--;
            return 0;
        }
        prev = node;
    }
    return -1;
}


//  --------------------------------------------------------------------------
//  Delete all items from the list. If the item destructor is set, calls it
//  on every item.

void
zlist_purge (zlist_t *self)
{
    assert (self);
    node_t *node = self->head;
    while (node) {
        node_t *next = node->next;
        if (self->destructor)
            (self->destructor) (&node->item);
        free (node);
        node = next;
    }
    self->head = self->tail = self->cursor = NULL;
    self->size = 0;
}


//  --------------------------------------------------------------------------
//  Set a user-defined deallocator for list items; by default items are not
//  freed when the list is destroyed.

void
zlist_set_destructor (zlist_t *self, czmq_destructor destructor)
{
    assert (self);
    self->destructor = destructor;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for list items; by default items are not
//  copied when the list is duplicated.

void
zlist_set_duplicator (zlist_t *self, czmq_duplicator duplicator)
{
    assert (self);
    self->duplicator = duplicator;
}


//  --------------------------------------------------------------------------
//  Make a copy of the list; items are duplicated if you set a duplicator
//  for the list, otherwise not. Copying a null reference returns a null
//  reference. Note that this method's behavior changed slightly for CZMQ
//  v3.x. The old behavior is in zhash_dup_v2.

zlist_t *
zlist_dup (zlist_t *self)
{
    if (!self)
        return NULL;

    zlist_t *copy = zlist_new ();
    if (copy) {
        copy->destructor = self->destructor;
        copy->duplicator = self->duplicator;
        node_t *node;
        for (node = self->head; node; node = node->next) {
            void *item = node->item;
            if (self->duplicator)
                item = self->duplicator (item);
            if (zlist_append (copy, item)) {
                zlist_destroy (&copy);
                break;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Return the number of items in the list

size_t
zlist_size (zlist_t *self)
{
    return self->size;
}


//  --------------------------------------------------------------------------
//  Sort the list by ascending key value using a straight ASCII comparison.
//  The sort is not stable, so may reorder items with the same keys.

void
zlist_sort (zlist_t *self, czmq_comparator *compare)
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
            if ((*compare) (base->item, test->item)) {
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
//  DEPRECATED by zlist_prepend
//  Push an item to the start of the list, return 0 if OK
//  or -1 if this failed for some reason (out of memory).

int
zlist_push (zlist_t *self, void *item)
{
    return zlist_prepend (self, item);
}


//  --------------------------------------------------------------------------
//  DEPRECATED as bad style (use delete or detach for clarity)
//  Remove item from the beginning of the list, returns NULL if none

void *
zlist_pop (zlist_t *self)
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
//  DEPRECATED by zlist_detach / zlist_delete
//  Remove the item from the list, if present. Safe to call on items that
//  are not in the list.

void
zlist_remove (zlist_t *self, void *item)
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
        if (node->free_fn)
            (node->free_fn) (node->item);
        else
        if (self->autofree)
            free (node->item);
        free (node);
        self->size--;
    }
}


//  --------------------------------------------------------------------------
//  DEPRECATED by zlist_copy
//  Make a copy of list. If the list has autofree set, the copied list will
//  duplicate all items, which must be strings. Otherwise, the list will hold
//  pointers back to the items in the original list.

zlist_t *
zlist_dup_v2 (zlist_t *self)
{
    if (!self)
        return NULL;

    zlist_t *copy = zlist_new ();
    if (copy) {
        copy->autofree = self->autofree;
        node_t *node;
        for (node = self->head; node; node = node->next) {
            if (zlist_append (copy, node->item)) {
                zlist_destroy (&copy);
                break;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as clumsy -- use set_destructor instead
//  Set list for automatic item destruction; item values MUST be strings.
//  By default a list item refers to a value held elsewhere. When you set
//  this, each time you append or push a list item, zlist will take a copy
//  of the string value. Then, when you destroy the list, it will free all
//  item values automatically. If you use any other technique to allocate
//  list values, you must free them explicitly before destroying the list.
//  The usual technique is to pop list items and destroy them, until the
//  list is empty.

void
zlist_autofree (zlist_t *self)
{
    assert (self);
    self->autofree = true;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as clumsy -- callers should set destructor at list level
//  which can be mapped directly to an object destructor.
//  
//  Set a free function for the specified list item. When the item is
//  destroyed, the free function, if any, is called on that item.
//  Use this when list items are dynamically allocated, to ensure that
//  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
//  Returns the item, or NULL if there is no such item.

void *
zlist_freefn (zlist_t *self, void *item, zlist_free_fn *fn, bool at_tail)
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
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor as-is.
//  DEPRECATED as over-designed and not useful

void *
zlist_head (zlist_t *self)
{
    assert (self);
    return self->head? self->head->item: NULL;
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor as-is.
//  DEPRECATED as over-designed and not useful

void *
zlist_tail (zlist_t *self)
{
    assert (self);
    return self->tail? self->tail->item: NULL;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class

static bool
s_compare (void *item1, void *item2)
{
    if (strcmp ((char *) item1, (char *) item2) > 0)
        return true;
    else
        return false;
}

static void
s_zlist_free (void *data)
{
    zlist_t *self = (zlist_t *) data;
    zlist_destroy (&self);
}


void
zlist_test (int verbose)
{
    printf (" * zlist: ");

    //  @selftest
    zlist_t *list = zlist_new ();
    assert (list);
    assert (zlist_size (list) == 0);

    //  Three items we'll use as test data
    //  List items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";

    zlist_append (list, cheese);
    assert (zlist_size (list) == 1);
    assert (zlist_item (list) == cheese);
    zlist_append (list, bread);
    assert (zlist_size (list) == 2);
    assert (zlist_item (list) == bread);
    zlist_append (list, wine);
    assert (zlist_size (list) == 3);
    assert (zlist_item (list) == wine);
    
    assert (zlist_first (list) == cheese);
    assert (zlist_next (list) == bread);
    assert (zlist_first (list) == cheese);
    assert (zlist_next (list) == bread);
    assert (zlist_next (list) == wine);
    assert (zlist_next (list) == NULL);
    //  After we reach end of list, next wraps around
    assert (zlist_next (list) == cheese);
    assert (zlist_size (list) == 3);

    char *item;
    //  Test delete and detach
//     char *item = (void *) zlist_detach (list, NULL);
//     assert (item == cheese);
//     item = (void *) zlist_detach (list, NULL);
//     assert (item == bread);
//     item = (void *) zlist_detach (list, NULL);
//     assert (item == wine);
    
//     void zlist_delete (list, wine);
//     assert (zlist_size (list) == 2);
//     assert (zlist_first (list) == cheese);
//     zlist_delete (list, cheese);
//     assert (zlist_size (list) == 1);
//     assert (zlist_first (list) == bread);
//     zlist_delete (list, NULL);
//     assert (zlist_size (list) == 0);

    
    zlist_sort (list, s_compare);

    zlist_t *sub_list = zlist_dup (list);
    assert (sub_list);
    assert (zlist_size (sub_list) == 3);

    zlist_purge (list);
    zlist_prepend (list, sub_list);
    zlist_t *sub_list_2 = zlist_dup (sub_list);
    zlist_append (list, sub_list_2);
    zlist_set_destructor (list, (czmq_destructor *) zlist_destroy);

    //  Destructor should be safe to call twice
    zlist_destroy (&list);
    zlist_destroy (&list);
    assert (list == NULL);

    //  -------------------------------------------------------------------
    //  Deprecated v2 API for zlist
    list = zlist_new ();
    zlist_append (list, cheese);
    zlist_append (list, bread);
    zlist_append (list, wine);

    assert (zlist_head (list) == cheese);
    assert (zlist_first (list) == cheese);
    assert (zlist_tail (list) == wine);
    assert (zlist_next (list) == bread);

    assert (zlist_first (list) == cheese);
    assert (zlist_next (list) == bread);
    assert (zlist_next (list) == wine);
    assert (zlist_next (list) == NULL);
    //  After we reach end of list, next wraps around
    assert (zlist_next (list) == cheese);
    assert (zlist_size (list) == 3);

    zlist_remove (list, wine);
    assert (zlist_size (list) == 2);

    assert (zlist_first (list) == cheese);
    zlist_remove (list, cheese);
    assert (zlist_size (list) == 1);
    assert (zlist_first (list) == bread);

    zlist_remove (list, bread);
    assert (zlist_size (list) == 0);

    zlist_append (list, cheese);
    zlist_append (list, bread);
    assert (zlist_last (list) == bread);
    zlist_remove (list, bread);
    assert (zlist_last (list) == cheese);
    zlist_remove (list, cheese);
    assert (zlist_last (list) == NULL);

    zlist_push (list, cheese);
    assert (zlist_size (list) == 1);
    assert (zlist_first (list) == cheese);

    zlist_push (list, bread);
    assert (zlist_size (list) == 2);
    assert (zlist_first (list) == bread);
    assert (zlist_item (list) == bread);

    zlist_append (list, wine);
    assert (zlist_size (list) == 3);
    assert (zlist_first (list) == bread);

    sub_list = zlist_dup_v2 (list);
    assert (sub_list);
    assert (zlist_size (sub_list) == 3);

    zlist_sort (list, s_compare);
    item = (char *) zlist_pop (list);
    assert (item == bread);
    item = (char *) zlist_pop (list);
    assert (item == wine);
    item = (char *) zlist_pop (list);
    assert (item == cheese);
    assert (zlist_size (list) == 0);

    assert (zlist_size (sub_list) == 3);
    zlist_push (list, sub_list);
    sub_list_2 = zlist_dup_v2 (sub_list);
    zlist_append (list, sub_list_2);
    assert (zlist_freefn (list, sub_list, &s_zlist_free, false) == sub_list);
    assert (zlist_freefn (list, sub_list_2, &s_zlist_free, true) == sub_list_2);

    zlist_destroy (&list);
    //  @end

    printf ("OK\n");
}
