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
    Provides a generic "ring" container, in which all elements of the
    ring are equipotent. This container provides hooks for duplicator
    and destructor functions. These tie into CZMQ and standard C
    semantics, so e.g. for string items you can use strdup and
    zstr_free. To store custom objects, define your own duplicator and
    use the standard object destructor.
@discuss
    A ring consists of elements doubly linked into a cyclic list. Each
    elements contains a pointer to an item or NULL if it is a guard
    element.
    
    A ring can have 0, 1 or multiple guard elements depending on its
    intended use. A guard element can act as head of the ring,
    simulating an empty ring but able to have a destructor and
    duplicator.

    A ring can be iterated either until the next guard element, or
    until a specific element is reached (allowing for a full round
    trip) while guard elements are skipped.
@end
*/

#include "../include/czmq.h"

//  ---------------------------------------------------------------------
//  Structure of our class

struct _zring_t {
    struct _zring_t *next;
    struct _zring_t *prev;
    void *item;
    czmq_destructor *destructor;
    czmq_duplicator *duplicator;
};


//  --------------------------------------------------------------------------
//  Create a new ring container (a ring is a doubly-linked cyclic list)

zring_t *
zring_new (void *item)
{
    zring_t *self = (zring_t *) zmalloc (sizeof (zring_t));
    if (self) {
        self->prev = self;
        self->next = self;
        self->item = item;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a ring container

void
zring_destroy (zring_t **self_p)
{
    assert (self_p);
    while (*self_p) {
        zring_t *node = zring_detach (self_p);
        if (node->destructor)
            (node->destructor) ((void **) &node);
        free (node);
    }
}


//  --------------------------------------------------------------------------
//  Insert an item after an element, return new element if OK, else
//  NULL. Creates a new ring if self is NULL.
zring_t *
zring_insert_after (zring_t *self, void *item)
{
    // Duplicate item if duplicator is set
    if (self && self->duplicator && item) {
        item = (self->duplicator) (item);
        if (!item)
            return NULL; // duplication failure, abort
    }

    // Create new ring element
    zring_t *node = zring_new (item);
    if (!node) {
        // allocation failure, check if we need to destroy a duplicated item
        if (self->duplicator && self->destructor)
            (self->destructor) (&item);
        return NULL;
    }

    // copy destructor and duplicator and move node into ring
    if (self) {
        node->destructor = self->destructor;
        node->duplicator = self->duplicator;
        zring_move_after (node, self);
    }

    return node;
}


//  --------------------------------------------------------------------------
//  Insert an item before an element, return new element if OK, else
//  NULL. Creates a new ring if self is NULL.
zring_t *
zring_insert_before (zring_t *self, void *item)
{
    if (self)
        self = self->prev;
    return zring_insert_after (self, item);
}


//  --------------------------------------------------------------------------
//  Detach first element from the ring and return it. Advances self_p to
//  the next element in the ring or NULL if none left.
zring_t *
zring_detach (zring_t **self_p)
{
    assert (self_p);
    zring_t *node = *self_p;
    
    if (node) {
        // advance self_p
        if (node->next == node)
            *self_p = NULL;
        else
            *self_p = node->next;

        // split linkage of ring
        node->next->prev = node->prev;
        node->prev->next = node->next;
        node->next = node;
        node->prev = node;
    }

    return node;
}


//  --------------------------------------------------------------------------
//  Remove first element from the ring and return its item. Advances
//  self_p to the next element in the ring or NULL if none left.
void *
zring_remove (zring_t **self_p)
{
    assert (self_p);
    zring_t *node = zring_detach (self_p);
    void *item = NULL;

    if (node) {
        item = node->item;
        free (node);
    }

    return item;
}


//  --------------------------------------------------------------------------
//  Remove first element from the ring and destroy it. Advances
//  self_p to the next element in the ring or NULL if none left.
void
zring_destroy_one (zring_t **self_p)
{
    zring_t *self = zring_detach (self_p);
    zring_destroy (&self);
}


//  --------------------------------------------------------------------------
//  Move first element from the ring to after the destination. Destination may
//  be another ring.
void
zring_move_after (zring_t *self, zring_t *destination)
{
    assert (self);
    assert (destination);
    // remove self from old ring
    self->prev->next = self->next;
    self->next->prev = self->prev;
    // insert into new ring
    self->prev = destination;
    self->next = destination->next;
    self->prev->next = self;
    self->next->prev = self;
}


//  --------------------------------------------------------------------------
//  Move first element from the ring to before the destination. Destination
//  may be another ring.
void
zring_move_before (zring_t *self, zring_t *destination)
{
    assert (destination);
    zring_move_after (self, destination->prev);
}


//  --------------------------------------------------------------------------
//  Return first element with item from the ring. If self is NULL
//  return NULL. If terminator is NULL return NULL when there is no
//  element before the next guard. Otherwise return NULL if there is
//  no element that is not a guard before the terminator.
zring_t *
zring_first (zring_t *self, zring_t *terminator)
{
    if (!self)
        return NULL;
    if (self->item)
        return self;
    return zring_next (self, terminator);
}


//  --------------------------------------------------------------------------
//  Return next element with item from the ring. If terminator is NULL
//  return NULL when the next element is a guard. Otherwise return
//  NULL if the next element that is no a guard is the terminator.
zring_t *
zring_next (zring_t *last, zring_t *terminator)
{
    assert (last);
    zring_t *node = last->next;
    if (node == terminator)
        return NULL;
    if (node->item)
        return node;
    if (terminator)
        return zring_next (node, terminator);
    return NULL;
}


//  --------------------------------------------------------------------------
//  Return the item of an element of the ring. If self is NULL return
//  NULL.
void *
zring_item (zring_t *self)
{
    if (self)
        return self->item;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Set a user-defined deallocator for ring items; by default items are not
//  freed when the ring is destroyed. This is an O(n) operation.

czmq_destructor *
zring_set_destructor (zring_t *self, czmq_destructor destructor)
{
    assert (self);
    czmq_destructor *old = self->destructor;
    self->destructor = destructor;
    zring_t *node = self->next;
    while (node != self) {
        node->destructor = destructor;
        node = node->next;
    }
    return old;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for ring items; by default items are not
//  copied when the ring is duplicated. This is an O(n) operation.

czmq_duplicator *
zring_set_duplicator (zring_t *self, czmq_duplicator duplicator)
{
    assert (self);
    czmq_duplicator *old = self->duplicator;
    self->duplicator = duplicator;
    zring_t *node = self->next;
    while (node != self) {
        node->duplicator = duplicator;
        node = node->next;
    }
    return old;
}


//  --------------------------------------------------------------------------
//  Runs selftest of class

void
zring_test (int verbose)
{
    printf (" * zring: ");

    //  @selftest
    //  Three items we'll use as test data
    //  Ring items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";

    // test insertion
    zring_t *ring = zring_insert_after (NULL, NULL);
    assert (ring);

    zring_t *node = zring_insert_after (ring, cheese);
    assert (zring_item (node) == cheese);

    node = zring_insert_after (ring, bread);
    assert (node);
    assert (zring_item (node) == bread);

    node = zring_insert_before (ring, NULL);
    assert (node);

    node = zring_insert_before (ring, wine);
    assert (node);
    assert (zring_item (node) == wine);

    // test iteration till next guard
    node = zring_first (ring, NULL);
    assert (node);
    assert (zring_item (node) == bread);

    node = zring_next (node, NULL);
    assert (node);
    assert (zring_item (node) == cheese);
    
    node = zring_next (node, NULL);
    assert (!node);

    // test iteration all the way around
    node = zring_first (ring, ring);
    assert (node);
    assert (zring_item (node) == bread);

    node = zring_next (node, ring);
    assert (node);
    assert (zring_item (node) == cheese);
    
    node = zring_next (node, ring);
    assert (node);
    assert (zring_item (node) == wine);
    
    node = zring_next (node, NULL);
    assert (!node);

    // test detach
    node = zring_detach (&ring);
    assert (zring_item (node) == NULL);
    assert (zring_item (ring) == bread);
    zring_destroy (&node);
    assert (!node);

    node = zring_detach (&ring);
    assert (zring_item (node) == bread);
    assert (zring_item (ring) == cheese);
    zring_destroy (&node);
    assert (!node);

    node = zring_detach (&ring);
    assert (zring_item (node) == cheese);
    assert (zring_item (ring) == NULL);
    zring_destroy (&node);
    assert (!node);

    // destroy leftovers
    zring_destroy (&ring);
    assert (!ring);
    //  @end

    printf ("OK\n");
}
