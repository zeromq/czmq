/*  =========================================================================
    zblob - work with binary long objects (blobs)

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
@header
    Binary long objects, specified by size and data.
@discuss
    This class overlaps with zframe partly to explore a new, simpler API
    for long binary strings. The semantics of zframe are closely tied to
    the libzmq message API while blobs are more generic. We could replace
    zframe with zblob over time.
@end
*/

#include "../include/czmq.h"

//  Structure of our class
struct _zblob_t {
    byte *data;
    size_t size;
};


//  --------------------------------------------------------------------------
//  Constructor. If data is not null, must be at least size bytes, and is
//  taken as value for blob.

zblob_t *
zblob_new (byte *data, size_t size)
{
    zblob_t *self = (zblob_t *) zmalloc (sizeof (zblob_t) + size);
    assert (self);
    self->data = (byte *) self + sizeof (zblob_t);
    self->size = size;
    if (data)
        memcpy (self->data, data, size);
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zblob_destroy (zblob_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zblob_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Return blob size

size_t
zblob_size (zblob_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Return blob data; you can write to this memory block.

byte *
zblob_data (zblob_t *self)
{
    assert (self);
    return self->data;
}


//  --------------------------------------------------------------------------
//  Self test

void
zblob_test (bool verbose)
{
    printf (" * zblob: ");

    //  @selftest
    zblob_t *blob = zblob_new ((byte *) "Hello", 5);
    assert (zblob_size (blob) == 5);
    assert (memcmp (zblob_data (blob), (byte *) "Hello", 5) == 0);
    zblob_destroy (&blob);
    //  @end
    
    printf ("OK\n");
}
