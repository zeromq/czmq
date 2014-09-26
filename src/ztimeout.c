/*  =========================================================================
    ztimeout - spezialized priority queue for timeout handling

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
    Provides a specialized priority queue for handling timeouts. The
    design is optimized for the case that most timeouts will be
    canceled.
@discuss
    Adding and canceling a timeout takes constant time at the cost of
    making finding the lowest timeout more costly. ztimeout_first()
    can take O(n) time worst case but has constant average time,
    excluding the time spend on the timeout callbacks.
@end
*/

#include "../include/czmq.h"

//  ---------------------------------------------------------------------
//  Performance parameters

#define NUM_BUCKETS        32           //  Number of buckets used in queue

//  ---------------------------------------------------------------------
//  Queue node, used internally and as abstract handle

struct _ztimeout_node_t {
    zring_node_t *node;                 // handle from zring
    int64_t time;                       // time of expirey
    uint current_slot;                  // current bucket slot in the queue
    void *item;                         // user supplied item
    czmq_destructor *destructor;        // callback to destroy item
};


//  ---------------------------------------------------------------------
//  Structure of our class

struct _ztimeout_t {
    zring_t *bucket [NUM_BUCKETS + 1];   //  Buckets holding timeouts
    int64_t next_time [NUM_BUCKETS + 1]; //  Next time buckets are processed
    size_t size;                         //  Number of items in list
    ztimeout_trigger_fn *trigger_fn;     //  Callback to trigger a timeout
    //  Function callbacks for duplicating and destroying items, if any
    czmq_duplicator *duplicator;
    czmq_destructor *destructor;
};


//  --------------------------------------------------------------------------
//  Helper to destroy a node

static void
s_ztimeout_node_destroy (ztimeout_node_t **node_p)
{
    ztimeout_node_t *node = *node_p;
    if (node->destructor)
        (node->destructor) (&node->item);
    free (node);
    *node_p = NULL;
}


//  --------------------------------------------------------------------------
//  compute time for next process run for a bucket
//  interleave buckets so that buckets have a unique time
//  bucket 0 is always processed
//  bucket 1 is processed at time % 2 == 1
//  bucket 2 is processed at time % 4 == 2
//  bucket 3 is processed at time % 8 == 4
// ...

static void
s_set_bucket_time (ztimeout_t *self, uint slot, int64_t now) {
    int64_t mask = (~0LL) << slot;
    int64_t step = (1LL << slot) / 2;
    self->next_time [slot] = ((now + step) & mask) + step;
}


//  --------------------------------------------------------------------------
//  Queue constructor

ztimeout_t *
ztimeout_new (ztimeout_trigger_fn trigger_fn)
{
    ztimeout_t *self = (ztimeout_t *) zmalloc (sizeof (ztimeout_t));
    if (self) {
        int64_t now = zclock_mono ();
        self->trigger_fn = trigger_fn;
        for (int i = 0; i < NUM_BUCKETS + 1; ++i) {
            self->bucket [i] = zring_new ();
            if (self->bucket [i] == NULL) {
                ztimeout_destroy (&self);
                break;
            }
            zring_set_destructor (self->bucket [i],
                                  (czmq_destructor *) s_ztimeout_node_destroy);
            s_set_bucket_time (self, i, now - (1LL << i));
        }
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Queue destructor

void
ztimeout_destroy (ztimeout_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        ztimeout_t *self = *self_p;
        for (int i = 0; i < NUM_BUCKETS + 1; ++i)
            zring_destroy (&self->bucket [i]);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  which bucket does a timeout belong to (basically log base 2)

static uint
s_calc_bucket (int64_t timeout)
{
    if (timeout <= 0) return -1;
    uint bucket_num = 0;
    while (timeout > 1) {
        timeout >>= 1;
        ++bucket_num;
    }
    assert (bucket_num < NUM_BUCKETS);
    return bucket_num;
}


//  --------------------------------------------------------------------------
//  Return remaining time till first timeout in ms. Processes timeout
//  callbacks as a side effect.

int64_t
ztimeout_first (ztimeout_t *self)
{
    assert (self);
    int64_t now = zclock_mono ();
    int64_t next = INT64_MAX;
    for (int i = 0; i < NUM_BUCKETS; ++i) {
        // go through all buckets and check if they need processing
        if (self->next_time [i] <= now) {
            // process bucket

            // swap bucket with the spare list so we never insert into
            // the list we pop from
            zring_t *list = self->bucket [i];
            self->bucket [i] = self->bucket [NUM_BUCKETS];
            self->bucket [NUM_BUCKETS] = list;

            // process timeouts in the bucket
            zring_first (list);
            while (zring_size (list) > 0) {
                ztimeout_node_t *node =
                    (ztimeout_node_t *)zring_detach (list, NULL);
                int slot = s_calc_bucket (node->time - now);
                if (slot < 0) {
                    // timeout expired, trigger it
                    (self->trigger_fn) (&node->item);
                    if (self->destructor) {
                        (self->destructor) (&node->item);
                    }
                    free (node);
                    self->size--;
                } else {
                    // timeout is still pending, reshelf it
                    node->current_slot = slot;
                    node->node = zring_append (self->bucket [slot], node);
                    if (node->time < next) {
                        next = node->time;
                    }
                }
            }
            s_set_bucket_time (self, i, now);
        }
        // next contains the time the next timeout expires or
        // processing must be done. Does it need updating?
        if (self->next_time [i] < next) {
            // this bucket has an event earlier than next, update next
            switch (zring_size (self->bucket [i])) {
            case 0: // empty bucket, ignore
                break;
            case 1: { // only one timeout in the bucket, use its exact time
                ztimeout_node_t *node =
                    (ztimeout_node_t*)zring_first (self->bucket [i]);
                if (node->time < next) {
                    next = node->time;
                }
                break;
            }
            default: // multiple timeouts, use the buckets next processing
                next = self->next_time [i];
            }
        }
    }
    // return time interval till the next timeout or processing is required
    return next - now;
}


//  --------------------------------------------------------------------------
//  Add a timeout at time <time> to the queue, return timeout handle if
//  OK or NULL if this failed for some reason (out of memory).

ztimeout_node_t *
ztimeout_add (ztimeout_t *self, void *item, int64_t time) {
    assert (self);
    assert (time > 0);
    ztimeout_node_t *node;
    node = (ztimeout_node_t *) zmalloc (sizeof (ztimeout_node_t));
    if (!node)
        return NULL;

    // store item
    if (self->duplicator)
        node->item = (self->duplicator) (item);
    else
        node->item = item;

    if (!node->item) {
        s_ztimeout_node_destroy (&node);
        return NULL;
    }
    
    uint slot = s_calc_bucket (time);
    node->node = zring_append (self->bucket [slot], node);
    node->time = zclock_mono () + time;
    node->current_slot = slot;
    node->destructor = self->destructor;
    self->size++;
    return node;
}


//  --------------------------------------------------------------------------
//  Cancel a timeout by handle.

void
ztimeout_cancel (ztimeout_t *self, ztimeout_node_t *node)
{
    assert (self);
    assert (node);
    zring_goto (self->bucket [node->current_slot], node->node);
    zring_remove (self->bucket [node->current_slot], NULL);
    self->size--;
}


//  --------------------------------------------------------------------------
//  Restart a timeout by handle.

void
ztimeout_restart (ztimeout_t *self, ztimeout_node_t *node, int64_t time)
{
    assert (self);
    assert (node);
    zring_goto (self->bucket [node->current_slot], node->node);
    zring_detach (self->bucket [node->current_slot], NULL);
    uint slot = s_calc_bucket (time);
    node->node = zring_append (self->bucket [slot], node);
    node->time = zclock_mono () + time;
    node->current_slot = slot;
}


//  --------------------------------------------------------------------------
//  Return the number of items in the list

size_t
ztimeout_size (ztimeout_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Set a user-defined destructor for items; by default items are not
//  freed when the timeout is destroyed.
void
ztimeout_set_destructor (ztimeout_t *self, czmq_destructor destructor)
{
    assert (self);
    self->destructor = destructor;
}


//  --------------------------------------------------------------------------
//  Set a user-defined duplicator for items; by default items are not
//  copied when the timeout is duplicated.
void
ztimeout_set_duplicator (ztimeout_t *self, czmq_duplicator duplicator)
{
    assert (self);
    self->duplicator = duplicator;
}


//  --------------------------------------------------------------------------
//  Dump state of timeout queue

static void
s_dump (ztimeout_t *self)
{
    assert (self);
    int64_t now = zclock_mono();
    printf ("now = %lx\n", now);
    for (int i = 0; i < NUM_BUCKETS; ++i) {
        printf ("Bucket [%d]: next = %ld\n", i, self->next_time [i] - now);
        ztimeout_node_t *node = (ztimeout_node_t*)zring_first (self->bucket [i]);
        while (node) {
            printf (" %ld,", node->time - now);
            node = zring_next (self->bucket [i]);
        }
        printf ("\n");
    }
}


//  --------------------------------------------------------------------------
//  Runs selftest of class

#define NUM_NODES 1000
#define DELAY 10
#define MAX_ERROR 10

struct _test_item {
    int timeout;
    int64_t triggered;
};

static void
s_test_trigger (void **item_p)
{
    struct _test_item *item = *(struct _test_item **)item_p;
    item->triggered = zclock_mono ();
}

void
ztimeout_test (int verbose)
{
    printf (" * ztimeout: ");
    //  @selftest
    int64_t start = zclock_mono ();
    struct _test_item item [NUM_NODES];
    ztimeout_t *queue = ztimeout_new (s_test_trigger);
    assert (queue);
    assert (ztimeout_size (queue) == 0);

    ztimeout_node_t *node [NUM_NODES];
    for (int i = 0; i < NUM_NODES; ++i) {
        item [i].timeout = (i + 1) * DELAY;
        item [i].triggered = 0;
        node [i] = ztimeout_add (queue, (void*)&item [i], item [i].timeout);
        assert (node [i]);
    }
    assert (ztimeout_size (queue) == NUM_NODES);

    for (int i = 0; i < NUM_NODES; i += 2) {
        ztimeout_cancel (queue, node [i]);
    }
    assert (ztimeout_size (queue) == NUM_NODES / 2);

    while (true) {
        int64_t next = ztimeout_first (queue);
        if (ztimeout_size (queue) == 0)
            break;
        // timeouts are 2 * DELAY apart except when the clock ticked
        // during creation
        if (next > 2 * DELAY + 1)
            s_dump (queue);
        assert (next <= 2 * DELAY + 1);
        zclock_sleep (next);
    }

    for (int i = 0; i < NUM_NODES; ++i) {
        if (item [i].triggered == 0) {
            assert (i % 2 == 0); // even timeouts don't get triggered
        }
        else {
            int64_t triggered = item [i].triggered - start;
            int64_t error = triggered - item [i].timeout;
            assert (i % 2 == 1); // odd timeouts get triggered
            assert (error >= 0); // not too early
            assert (error <= MAX_ERROR); // and not too much too late
        }
    }
    //  @end
    printf ("OK\n");
}
