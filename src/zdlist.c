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

//  ---------------------------------------------------------------------
//  List node, used internally and as abstract handle

struct _zdlist_node_t {
    struct _zdlist_node_t *next;
    struct _zdlist_node_t *prev;
    void *item;
};

//  ---------------------------------------------------------------------
//  Structure of our class

struct _zdlist_t {
    zdlist_node_t head;		//  Dummy node acting as head in list
    zdlist_node_t *cursor;	//  Current node for iteration
    size_t size;                //  Number of items in list
    zdlist_free_fn *free_fn;	//  Callback to destroy an item
};


//  --------------------------------------------------------------------------
//  Simple free_fn function that calls free
void
zdlist_item_destroy(void **item)
{
    free(*item);
    *item = NULL;
}

//  --------------------------------------------------------------------------
//  Node initializer

static void
s_zdlist_node_init(zdlist_node_t *node, zdlist_node_t *next,
		   zdlist_node_t *prev, void *item)
{
    node->next = next;
    node->prev = prev;
    node->item = item;
}


//  --------------------------------------------------------------------------
//  List constructor

zdlist_t *
zdlist_new (zdlist_free_fn free_fn)
{
    zdlist_t *self = (zdlist_t *) zmalloc (sizeof (zdlist_t));
    if (self) {
	s_zdlist_node_init(&self->head, &self->head, &self->head, NULL);
	self->cursor = &self->head;
	self->free_fn = free_fn;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  List destructor

void
zdlist_destroy (zdlist_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdlist_t *self = *self_p;
        zdlist_node_t *node = self->head.next;
        while (node != &self->head) {
	    zdlist_node_t *next = node->next;
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
    self->cursor = self->head.next;
    if (self->cursor != &self->head)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the next item. If the list is empty, returns NULL. To move to
//  the start of the list call zdlist_first (). Advances the cursor.

void *
zdlist_next (zdlist_t *self)
{
    assert (self);
    self->cursor = self->cursor->next;
    if (self->cursor != &self->head)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.

void *
zdlist_last (zdlist_t *self)
{
    assert (self);
    self->cursor = self->head.prev;
    if (self->cursor != &self->head)
        return self->cursor->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the previous item. If the list is empty, returns NULL. To move to
//  the end of the list call zdlist_last (). Advances the cursor backwards.

void *
zdlist_prev (zdlist_t *self)
{
    assert (self);
    zdlist_node_t *node = self->cursor;
    self->cursor = self->cursor->prev;
    if (node != &self->head)
        return node->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the item at the head of list. If the list is empty, returns NULL.
//  Leaves cursor as-is.

void *
zdlist_head (zdlist_t *self)
{
    assert (self);
    if (self->head.next != &self->head)
	return self->head.next->item;
    else
	return NULL;
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor as-is.

void *
zdlist_tail (zdlist_t *self)
{
    assert (self);
    if (self->head.prev != &self->head)
	return self->head.prev->item;
    else
	return NULL;
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the list, return item handle if OK
//  or NULL if this failed for some reason (out of memory).

zdlist_node_t *
zdlist_append (zdlist_t *self, void *item)
{
    zdlist_node_t *node;
    node = (zdlist_node_t *) zmalloc (sizeof (zdlist_node_t));
    if (!node)
        return NULL;

    s_zdlist_node_init(node, &self->head, self->head.prev, item);
    self->head.prev->next = node;
    self->head.prev = node;
    
    self->size++;
    return node;
}


//  --------------------------------------------------------------------------
//  Push an item to the start of the list, return 0 if OK
//  or -1 if this failed for some reason (out of memory).

zdlist_node_t *
zdlist_push (zdlist_t *self, void *item)
{
    zdlist_node_t *node;
    node = (zdlist_node_t *) zmalloc (sizeof (zdlist_node_t));
    if (!node)
        return NULL;

    s_zdlist_node_init(node, self->head.next, &self->head, item);
    self->head.next->prev = node;
    self->head.next = node;
    
    self->size++;
    return node;
}


//  --------------------------------------------------------------------------
//  Pop the item off the start of the list, if any. Resets the cursor if if
//  points to the first node.

void *
zdlist_pop (zdlist_t *self)
{
    zdlist_node_t *node = self->head.next;
    void *item = NULL;
    if (node != &self->head) {
        item = node->item;
	node->next->prev = node->prev;
	node->prev->next = node->next;
	if (self->cursor == node)
	    self->cursor = node->prev;

        free (node);
        self->size--;
    }
    return item;
}


//  --------------------------------------------------------------------------
//  Remove the specified item handle from the list. Sets the cursor to the
//  previous node if it points to this node.

void
zdlist_remove (zdlist_t *self, zdlist_node_t **node_p)
{
    assert (node_p);
    zdlist_node_t *node = *node_p;
    assert(node);
    
    node->next->prev = node->prev;
    node->prev->next = node->next;

    if (self->cursor == node)
	self->cursor = node->prev;
    if (self->free_fn)
	(self->free_fn) (&node->item);
    free (node);
    self->size--;
}

//  Make a copy of list using the dup callback if any to duplicate each item.
//  The new list will use the new free_fn function to destroy items in the
//  future.
zdlist_t *
zdlist_dup (zdlist_t *self, zdlist_dup_fn dup, zdlist_free_fn free_fn)
{
    if (!self)
        return NULL;

    zdlist_t *copy = zdlist_new (free_fn);

    if (copy) {
        zdlist_node_t *node;
        for (node = self->head.next; node != &self->head; node = node->next) {
	    void *item = node->item;
	    if (dup)
		item = dup(item);
            if (zdlist_append (copy, item) == NULL) {
                zdlist_destroy (&copy);
                break;
            }
        }
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Return the number of items in the list

size_t
zdlist_size (zdlist_t *self)
{
    return self->size;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class

void
zdlist_test (int verbose)
{
    printf (" * zdlist: ");

    //  @selftest
    zdlist_t *list = zdlist_new (NULL);
    assert (list);
    assert (zdlist_size (list) == 0);

    //  Three items we'll use as test data
    //  List items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";

    zdlist_node_t *node_cheese = zdlist_append (list, cheese);
    assert (zdlist_size (list) == 1);
    zdlist_node_t *node_bread = zdlist_append (list, bread);
    assert (zdlist_size (list) == 2);
    zdlist_node_t *node_wine = zdlist_append (list, wine);
    assert (zdlist_size (list) == 3);

    assert (zdlist_head (list) == cheese);
    assert (zdlist_next (list) == cheese);

    assert (zdlist_first (list) == cheese);
    assert (zdlist_tail (list) == wine);
    assert (zdlist_next (list) == bread);

    assert (zdlist_first (list) == cheese);
    assert (zdlist_next (list) == bread);
    assert (zdlist_next (list) == wine);
    assert (zdlist_next (list) == NULL);
    //  After we reach end of list, next wraps around
    assert (zdlist_next (list) == cheese);
    assert (zdlist_size (list) == 3);

    zdlist_remove (list, &node_wine);
    assert (zdlist_size (list) == 2);

    assert (zdlist_first (list) == cheese);
    zdlist_remove (list, &node_cheese);
    assert (zdlist_size (list) == 1);
    assert (zdlist_first (list) == bread);

    zdlist_remove (list, &node_bread);
    assert (zdlist_size (list) == 0);

    node_cheese = zdlist_append (list, cheese);
    node_bread = zdlist_append (list, bread);
    assert (zdlist_last (list) == bread);
    zdlist_remove (list, &node_bread);
    assert (zdlist_last (list) == cheese);
    zdlist_remove (list, &node_cheese);
    assert (zdlist_last (list) == NULL);

    node_cheese = zdlist_push (list, cheese);
    assert (zdlist_size (list) == 1);
    assert (zdlist_first (list) == cheese);

    node_bread = zdlist_push (list, bread);
    assert (zdlist_size (list) == 2);
    assert (zdlist_first (list) == bread);

    node_wine = zdlist_append (list, wine);
    assert (zdlist_size (list) == 3);
    assert (zdlist_first (list) == bread);

    zdlist_t *dup_list = zdlist_dup (list, NULL, NULL);
    assert (dup_list);
    assert (zdlist_size (dup_list) == 3);

    zdlist_t *list_list = zdlist_new ((zdlist_free_fn*)zdlist_destroy);
    zdlist_push (list_list, list);
    zdlist_push (list_list, dup_list);
    assert (zdlist_size (list_list) == 2);

    //  Destructor should be safe to call twice
    zdlist_destroy (&list_list);
    zdlist_destroy (&list_list);
    assert (list_list == NULL);
    //  @end

    printf ("OK\n");
}
