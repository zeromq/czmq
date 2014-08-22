/*  =========================================================================
    zdlist - generic type-free doubly linked list container

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
    Provides a generic container implementing a fast doubly-linked list. You
    can use this to construct multi-dimensional lists, and other structures
    together with other generic containers like zhash.
@discuss
    To iterate through a list, use zdlist_first to get the first item, then
    loop while not null, and do zdlist_next at the end of each iteration.
@end
*/

#include "../include/czmq.h"

//  List node, used internally only

typedef struct _node_t {
    struct _node_t *next;
    struct _node_t *prev;
    void *item;
} node_t;

//  ---------------------------------------------------------------------
//  Structure of our class

struct _zdlist_t {
    node_t head;         //  Dummy node acting as head in list
    node_t *cursor;      //  Current node for iteration
    size_t size;                //  Number of items in list
    zdlist_free_fn *free_fn;    //  Callback to destroy an item
    zdlist_dup_fn *dup_fn;      //  Callback to duplicate an item
};


//  --------------------------------------------------------------------------
//  Initialize a list node

static void
s_node_init (
    node_t *node,
    node_t *next,
    node_t *prev,
    void *item)
{
    node->next = next;
    node->prev = prev;
    node->item = item;
}


//  --------------------------------------------------------------------------
//  Create a new list container

zdlist_t *
zdlist_new (void)
{
    zdlist_t *self = (zdlist_t *) zmalloc (sizeof (zdlist_t));
    if (self) {
        s_node_init (&self->head, &self->head, &self->head, NULL);
        self->cursor = &self->head;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a list container

void
zdlist_destroy (zdlist_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdlist_t *self = *self_p;
        node_t *node = self->head.next;
        while (node != &self->head) {
            node_t *next = node->next;
            if (self->free_fn)
                (self->free_fn) (&node->item);
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
zdlist_first (zdlist_t *self)
{
    assert (self);
    self->cursor = &self->head;
    return zdlist_next (self);
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.

void *
zdlist_last (zdlist_t *self)
{
    assert (self);
    self->cursor = &self->head;
    return zdlist_prev (self);
}


//  --------------------------------------------------------------------------
//  Return the next item. At the end of the list (or in an empty list),
//  returns NULL. Use repeated zdlist_next () calls to work through the list
//  from zdlist_first ().

void *
zdlist_next (zdlist_t *self)
{
    assert (self);
    self->cursor = self->cursor->next;
    if (self->cursor == &self->head)
        return NULL;        //  Reached head, so finished
    else
        return self->cursor->item;
}


//  --------------------------------------------------------------------------
//  Return the previous item. At the start of the list (or in an empty list),
//  returns NULL. Use repeated zdlist_prev () calls to work through the list
//  backwards from zdlist_last ().

void *
zdlist_prev (zdlist_t *self)
{
    assert (self);
    self->cursor = self->cursor->prev;
    if (self->cursor == &self->head)
        return NULL;        //  Reached head, so finished
    else
        return self->cursor->item;
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the list, return 0 if OK, else -1.

int
zdlist_append (zdlist_t *self, void *item)
{
    assert (self);
    node_t *node = (node_t *) zmalloc (sizeof (node_t));
    if (!node)
        return -1;
    
    s_node_init (node, &self->head, self->head.prev, item);
    self->head.prev->next = node;
    self->head.prev = node;
    self->size++;
    return 0;
}


//  --------------------------------------------------------------------------
//  Push an item to the start of the list, return 0 if OK, else -1.

int
zdlist_push (zdlist_t *self, void *item)
{
    assert (self);
    node_t *node = (node_t *) zmalloc (sizeof (node_t));
    if (!node)
        return -1;

    s_node_init (node, self->head.next, &self->head, item);
    self->head.next->prev = node;
    self->head.next = node;
    self->size++;
    return 0;
}


//  --------------------------------------------------------------------------
//  Pop and destroy item off the start of the list, if any. Sets cursor to
//  first remaining item in list.
//  
//  TODO: can't really mix this with returning a value, if we're using
//  free_fn, as that will return deallocated values. Thus I removed the
//  return value. Ditto for _remove. -- PH 2014/08/21

void
zdlist_pop (zdlist_t *self)
{
    zdlist_first (self);
    zdlist_remove (self);
}


//  --------------------------------------------------------------------------
//  Remove the current item from the list (as set by first/last/next/prev
//  calls). Sets the cursor to the next node. 

void
zdlist_remove (zdlist_t *self)
{
    assert (self);
    if (self->cursor != &self->head) {
        node_t *node = self->cursor;
        node->next->prev = self->cursor = node->prev;
        node->prev->next = node->next;

        if (self->free_fn)
            (self->free_fn) (&node->item);
        free (node);
        self->size--;
    }
}


//  --------------------------------------------------------------------------
//  Insert an item after cursor, return 0 if OK, else -1.

int
zdlist_insert (zdlist_t *self, void *item)
{
    return -1;          //  TODO
}


//  --------------------------------------------------------------------------
//  Return the number of items in the list

size_t
zdlist_size (zdlist_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Set a user-defined deallocator for list items; by default items are not
//  freed when the list is destroyed.

void
zdlist_set_free_fn (zdlist_t *self, zdlist_free_fn free_fn)
{
    assert (self);
    self->free_fn = free_fn;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for list items; by default items are not
//  copied when the list is duplicated.

void
zdlist_set_dup_fn (zdlist_t *self, zdlist_dup_fn dup_fn)
{
    assert (self);
    self->dup_fn = dup_fn;
}


//  --------------------------------------------------------------------------
//  Make a copy of the list; items are duplicated if you set a duplicator
//  for the list.

zdlist_t *
zdlist_dup (zdlist_t *self)
{
    //  Duplicating a null reference returns a null reference
    if (!self)
        return NULL;

    zdlist_t *copy = zdlist_new ();
    if (copy) {
        copy->free_fn = self->free_fn;
        copy->dup_fn = self->dup_fn;
        node_t *node;
        for (node = self->head.next; node != &self->head; node = node->next) {
            void *item = node->item;
            if (self->dup_fn)
                item = self->dup_fn (item);
            if (zdlist_append (copy, item)) {
                zdlist_destroy (&copy);
                break;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class

void
zdlist_test (int verbose)
{
    printf (" * zdlist: ");

    //  @selftest
    zdlist_t *list = zdlist_new ();
    assert (list);
    assert (zdlist_size (list) == 0);

    //  Three items we'll use as test data
    //  List items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";

    int rc = zdlist_append (list, cheese);
    assert (!rc);
    assert (zdlist_size (list) == 1);
    rc = zdlist_append (list, bread);
    assert (!rc);
    assert (zdlist_size (list) == 2);
    rc = zdlist_append (list, wine);
    assert (!rc);
    assert (zdlist_size (list) == 3);

    assert (zdlist_next (list) == cheese);
    assert (zdlist_first (list) == cheese);
    assert (zdlist_next (list) == bread);
    assert (zdlist_next (list) == wine);
    assert (zdlist_next (list) == NULL);
    //  After we reach end of list, next wraps around
    assert (zdlist_next (list) == cheese);
    
    assert (zdlist_last (list) == wine);
    assert (zdlist_prev (list) == bread);
    assert (zdlist_prev (list) == cheese);
    assert (zdlist_prev (list) == NULL);
    //  After we reach start of list, prev wraps around
    assert (zdlist_prev (list) == wine);

    assert (zdlist_first (list) == cheese);
    zdlist_remove (list);
    assert (zdlist_size (list) == 2);
    assert (zdlist_first (list) == bread);
    zdlist_remove (list);
    assert (zdlist_size (list) == 1);
    assert (zdlist_first (list) == wine);
    zdlist_remove (list);
    assert (zdlist_size (list) == 0);
    assert (zdlist_first (list) == NULL);

    rc = zdlist_push (list, cheese);
    assert (!rc);
    assert (zdlist_size (list) == 1);
    assert (zdlist_first (list) == cheese);

    rc = zdlist_push (list, bread);
    assert (!rc);
    assert (zdlist_size (list) == 2);
    assert (zdlist_first (list) == bread);

    rc = zdlist_append (list, wine);
    assert (!rc);
    assert (zdlist_size (list) == 3);
    assert (zdlist_first (list) == bread);

    zdlist_t *dup_list = zdlist_dup (list);
    assert (dup_list);
    assert (zdlist_size (dup_list) == 3);

    //  Destructor should be safe to call twice
    zdlist_destroy (&list);
    assert (list == NULL);
    zdlist_destroy (&list);
    //  @end

    printf ("OK\n");
}
