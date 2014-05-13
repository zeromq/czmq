/*  =========================================================================
    zactor - simple actor framework

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
    The zactor class provides a simple actor framework.
@discuss
@end
*/

#include "../include/czmq.h"

//  zactor_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZACTOR_TAG          0x0005cafe

//  Structure of our class

struct _zactor_t {
    uint32_t tag;               //  Object tag for runtime detection
    void *handle;               //  The libzmq actor handle
};


//  --------------------------------------------------------------------------
//  Create a new actor.

zactor_t *
zactor_new (zactor_fn *task, void *args)
{
    zactor_t *self = (zactor_t *) zmalloc (sizeof (zactor_t));
    if (!self)
        return NULL;

    self->tag = ZACTOR_TAG;
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the actor.

void
zactor_destroy (zactor_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zactor_t *self = *self_p;
        assert (zactor_is (self));
        self->tag = 0xDeadBeef;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zactor_t.

bool
zactor_is (void *self)
{
    assert (self);
    return ((zactor_t *) self)->tag == ZACTOR_TAG;
}


//  --------------------------------------------------------------------------
//  Probe the supplied reference. If it looks like a zactor_t instance,
//  return the underlying libzmq actor handle; else if it looks like
//  a libzmq actor handle, return the supplied value.

void *
zactor_resolve (void *self)
{
    assert (self);
    if (zactor_is (self))
//         return ((zactor_t *) self)->handle;
        return NULL;
    else
        return self;
}


//  --------------------------------------------------------------------------
//  Selftest

void
zactor_test (bool verbose)
{
    printf (" * zactor: ");

    //  @selftest
    zactor_t *actor = zactor_new (NULL, NULL);
    assert (actor);
    zactor_destroy (&actor);
    //  @end

    printf ("OK\n");
}
