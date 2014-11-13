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
    This is a reworking of the old V2 list container. It is faster to insert
    and delete items anywhere in the list, and to keep ordered lists.
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
//  Create a new, empty list.

zlistx_t *
zlistx_new (void)
{
    zlistx_t *self = (zlistx_t *) zmalloc (sizeof (zlistx_t));
    if (self) {
        self->head = s_node_new (NULL, NULL, NULL);
        self->cursor = self->head;
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
//  on the item. Leaves cursor at newly inserted item. Returns 0 on success,
//  -1 if failed due to lack of memory.

int
zlistx_add_start (zlistx_t *self, void *item)
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
//  Add an item to the tail of the list. Calls the item duplicator, if any,
//  on the item. Leaves cursor at newly inserted item. Returns 0 on success,
//  -1 if failed due to lack of memory.

int
zlistx_add_end (zlistx_t *self, void *item)
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
//  Add an item before the current cursor, if any. If no cursor is set, adds
//  to the start of the list. Calls the item duplicator, if any, on the item.
//  Leaves cursor at newly inserted item. Returns 0 on success, -1 if failed
//  due to lack of memory.

int
zlistx_add_before (zlistx_t *self, void *item)
{
    return -1;
}


//  --------------------------------------------------------------------------
//  Add an item after the current cursor, if any. If no cursor is set, adds
//  to the end of the list. Calls the item duplicator, if any, on the item.
//  Leaves cursor at newly inserted item. Returns 0 on success, -1 if failed
//  due to lack of memory.

int
zlistx_add_after (zlistx_t *self, void *item)
{
    return -1;
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
    self->cursor = self->head;
    return zlistx_next (self);
}


//  --------------------------------------------------------------------------
//  Return the item at the tail of list. If the list is empty, returns NULL.
//  Leaves cursor pointing at the tail item, or NULL if the list is empty.

void *
zlistx_last (zlistx_t *self)
{
    assert (self);
    self->cursor = self->head;
    return zlistx_prev (self);
}


//  --------------------------------------------------------------------------
//  Return the next item. At the end of the list (or in an empty list),
//  returns NULL. Use repeated zlistx_next () calls to work through the list
//  from zlistx_first ().

void *
zlistx_next (zlistx_t *self)
{
    assert (self);
    self->cursor = self->cursor->next;
    return zlistx_item (self);
}


//  --------------------------------------------------------------------------
//  Return the previous item. At the start of the list (or in an empty list),
//  returns NULL. Use repeated zlistx_prev () calls to work through the list
//  backwards from zlistx_last ().

void *
zlistx_prev (zlistx_t *self)
{
    assert (self);
    self->cursor = self->cursor->prev;
    return zlistx_item (self);
}


//  --------------------------------------------------------------------------
//  Find an item in the list, looking first at the cursor, and then from 
//  the first to last item. If an item comparator was set, calls this to
//  compare each item in the list with the supplied target item. If none
//  was set, compares the two item pointers for equality. If the item is
//  found, leaves the cursor at the found item. Returns the item if found,
//  else null.

void *
zlistx_find (zlistx_t *self, void *item)
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
    //  Now scan list for item, this is a O(N) operation
    node_t *node = self->head->next;
    while (node != self->head) {
        if (self->comparator) {
            if (self->comparator (node->item, item) == 0)
                return node->item;
        }
        else
        if (node->item == item) {
            self->cursor = node;
            return node->item;
        }
        node = node->next;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Remove an item from the list, and destroy it, if the item destructor is
//  set. Searches the list for the item, always starting with the cursor, if
//  any is set, and then from the start of the list. If item is null, removes
//  the first item, if any. If the item was found and detached, returns the
//  0, else returns -1.

int
zlistx_delete (zlistx_t *self, void *item)
{
    assert (self);
    if ((item = zlistx_detach (self, item))) {
        if (self->destructor)
            self->destructor (&item);
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Detach an item from the list, without destroying the item. Searches the
//  list for the item, always starting with the cursor, if any is set, and
//  then from the start of the list. If item is null, detaches the first item
//  in the list, if any. If the item was found and detached, returns the item
//  and leaves cursor at next item. Else, returns null.

void *
zlistx_detach (zlistx_t *self, void *item)
{
    assert (self);

    node_t *found = NULL;
    if ((item == NULL && zlistx_first (self))
    ||  (item != NULL && zlistx_find (self, item))) {
        found = self->cursor;
        assert (found != self->head);
    }
    //  Now detach node from list, without destroying it
    if (found) {
        item = found->item;
        found->item = NULL;
        found->prev->next = found->next;
        found->next->prev = found->prev;
        self->cursor = found->next;
        self->size--;
        free (found);
        return item;
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Remove all items from the list, and destroy them if the item destructor
//  is set.

void
zlistx_purge (zlistx_t *self)
{
    assert (self);
    while (zlistx_size (self))
        zlistx_delete (self, zlistx_first (self));
}


//  --------------------------------------------------------------------------
//  Return current item in the list. If the list is empty, or the cursor
//  passed the end of the list, returns NULL. Does not change the cursor.

void *
zlistx_item (zlistx_t *self)
{
    assert (self);
    if (self->cursor != self->head)
        return self->cursor->item;
    else
        return NULL;            //  Reached head, so finished
}


//  --------------------------------------------------------------------------
//  Sort the list. If an item comparator was set, calls that to compare
//  items, otherwise compares on item value. The sort is not stable, so may
//  reorder equal items.

void
zlistx_sort (zlistx_t *self)
{
    assert (self);

    //  Uses a comb sort, which is simple and reasonably fast
    //  See http://en.wikipedia.org/wiki/Comb_sort
    int gap = self->size;
    while (gap > 1) {
        gap = (int) ((double) gap / 1.3);

        node_t *base = self->head->next;
        node_t *test = self->head->next;
        int jump = gap;
        while (jump--)
            test = test->next;

        bool swapped = false;
        while (base != self->head && test != self->head) {
            int compare;
            if (self->comparator)
                compare = self->comparator (base->item, test->item);
            else
                compare = (base->item < test->item);

            if (compare > 0) {
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
        if (!swapped)
            break;
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
            if (zlistx_add_end (copy, node->item)) {
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
    assert (zlistx_delete (list, "hello") == -1);
    assert (zlistx_detach (list, "hello") == NULL);
    assert (zlistx_delete (list, NULL) == -1);
    assert (zlistx_detach (list, NULL) == NULL);
    zlistx_purge (list);
    assert (zlistx_item (list) == NULL);
    zlistx_sort (list);

    //  Use item handlers
    zlistx_set_destructor (list, (czmq_destructor *) zstr_free);
    zlistx_set_duplicator (list, (czmq_duplicator *) strdup);
    zlistx_set_comparator (list, (czmq_comparator *) strcmp);

    //  Try some edge conditions with 0/1/2 items
    zlistx_add_end (list, "hello");
    zlistx_add_end (list, "world");
    zlistx_sort (list);
    char *string = (char *) zlistx_detach (list, NULL);
    assert (streq (string, "hello"));
    free (string);
    string = (char *) zlistx_detach (list, NULL);
    assert (streq (string, "world"));
    free (string);
    zlistx_sort (list);

    //  Check simple add/delete
    zlistx_add_end (list, "hello");
    zlistx_delete (list, "hello");

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

    zlistx_t *copy = zlistx_dup (list);
    zlistx_purge (list);
    zlistx_destroy (&list);

    assert (zlistx_size (copy) == 10);
    assert (streq ((char *) zlistx_first (copy), "eight"));
    assert (streq ((char *) zlistx_last (copy), "two"));
    zlistx_destroy (&copy);
    //  @end

    printf ("OK\n");
}
