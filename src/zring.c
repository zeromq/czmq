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
    Provides a generic container implementing a fast doubly-linked list, aka
    a ring. You can use this to construct multi-dimensional rings, and other
    structures together with other generic containers like zhash.
@discuss
    To iterate through a ring, use zring_first to get the first item, then
    loop while not null, and do zring_next at the end of each iteration.
@end
*/

#include "../include/czmq.h"

//  Ring node, used internally only

typedef struct _node_t {
    struct _node_t *next;
    struct _node_t *prev;
    void *item;
} node_t;

//  ---------------------------------------------------------------------
//  Structure of our class

struct _zring_t {
    node_t head;         //  Dummy node acting as head in ring
    node_t *cursor;      //  Current node for iteration
    size_t size;                //  Number of items in ring
    zring_free_fn *free_fn;    //  Callback to destroy an item
    zring_dup_fn *dup_fn;      //  Callback to duplicate an item
};


//  --------------------------------------------------------------------------
//  Initialize a ring node

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
//  Create a new ring container (a ring is a doubly-linked ring)

zring_t *
zring_new (void)
{
    zring_t *self = (zring_t *) zmalloc (sizeof (zring_t));
    if (self) {
        s_node_init (&self->head, &self->head, &self->head, NULL);
        self->cursor = &self->head;
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
//  Return the item at the head of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the head item, or NULL if the ring is empty.

void *
zring_first (zring_t *self)
{
    assert (self);
    self->cursor = &self->head;
    return zring_next (self);
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of ring. If the ring is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the ring is empty.

void *
zring_last (zring_t *self)
{
    assert (self);
    self->cursor = &self->head;
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
    if (self->cursor == &self->head)
        return NULL;        //  Reached head, so finished
    else
        return self->cursor->item;
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
    if (self->cursor == &self->head)
        return NULL;        //  Reached head, so finished
    else
        return self->cursor->item;
}


//  --------------------------------------------------------------------------
//  Append an item to the end of the ring, return 0 if OK, else -1.

int
zring_append (zring_t *self, void *item)
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
//  Push an item to the start of the ring, return 0 if OK, else -1.

int
zring_push (zring_t *self, void *item)
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
//  Pop and destroy item off the start of the ring, if any. Sets cursor to
//  first remaining item in ring.
//  
//  TODO: can't really mix this with returning a value, if we're using
//  free_fn, as that will return deallocated values. Thus I removed the
//  return value. Ditto for _remove. -- PH 2014/08/21

void
zring_pop (zring_t *self)
{
    zring_first (self);
    zring_remove (self);
}


//  --------------------------------------------------------------------------
//  Remove the current item from the ring (as set by first/last/next/prev
//  calls). Sets the cursor to the next node. 

void
zring_remove (zring_t *self)
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
zring_insert (zring_t *self, void *item)
{
    return -1;          //  TODO
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
//  Set a user-defined deallocator for ring items; by default items are not
//  freed when the ring is destroyed.

void
zring_set_free_fn (zring_t *self, zring_free_fn free_fn)
{
    assert (self);
    self->free_fn = free_fn;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for ring items; by default items are not
//  copied when the ring is duplicated.

void
zring_set_dup_fn (zring_t *self, zring_dup_fn dup_fn)
{
    assert (self);
    self->dup_fn = dup_fn;
}


//  --------------------------------------------------------------------------
//  Make a copy of the ring; items are duplicated if you set a duplicator
//  for the ring.

zring_t *
zring_dup (zring_t *self)
{
    //  Duplicating a null reference returns a null reference
    if (!self)
        return NULL;

    zring_t *copy = zring_new ();
    if (copy) {
        copy->free_fn = self->free_fn;
        copy->dup_fn = self->dup_fn;
        node_t *node;
        for (node = self->head.next; node != &self->head; node = node->next) {
            void *item = node->item;
            if (self->dup_fn)
                item = self->dup_fn (item);
            if (zring_append (copy, item)) {
                zring_destroy (&copy);
                break;
            }
        }
    }
    return copy;
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
    rc = zring_append (ring, bread);
    assert (!rc);
    assert (zring_size (ring) == 2);
    rc = zring_append (ring, wine);
    assert (!rc);
    assert (zring_size (ring) == 3);

    assert (zring_next (ring) == cheese);
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

    assert (zring_first (ring) == cheese);
    zring_remove (ring);
    assert (zring_size (ring) == 2);
    assert (zring_first (ring) == bread);
    zring_remove (ring);
    assert (zring_size (ring) == 1);
    assert (zring_first (ring) == wine);
    zring_remove (ring);
    assert (zring_size (ring) == 0);
    assert (zring_first (ring) == NULL);

    rc = zring_push (ring, cheese);
    assert (!rc);
    assert (zring_size (ring) == 1);
    assert (zring_first (ring) == cheese);

    rc = zring_push (ring, bread);
    assert (!rc);
    assert (zring_size (ring) == 2);
    assert (zring_first (ring) == bread);

    rc = zring_append (ring, wine);
    assert (!rc);
    assert (zring_size (ring) == 3);
    assert (zring_first (ring) == bread);

    zring_t *dup_ring = zring_dup (ring);
    assert (dup_ring);
    assert (zring_size (dup_ring) == 3);
    zring_destroy (&dup_ring);

    //  Destructor should be safe to call twice
    zring_destroy (&ring);
    assert (ring == NULL);
    zring_destroy (&ring);
    //  @end

    printf ("OK\n");
}
