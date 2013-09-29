/*  =========================================================================
    zchunk - work with memory chunks

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTA-
    BILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see http://www.gnu.org/licenses/.
    =========================================================================*/

/*
@header
    The zchunk class works with variable sized blobs. Not as efficient as
    ØMQ's messages but they do less weirdness and so are easier to understand.
    The chunk class has methods to read and write chunks from disk.
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zchunk_t {
    size_t size;                //  Current size of data part
    size_t max_size;            //  Maximum allocated size
    byte  *data;                //  Data part follows here
};


//  --------------------------------------------------------------------------
//  Constructor

zchunk_t *
zchunk_new (const void *data, size_t size)
{
    zchunk_t *self = (zchunk_t *) zmalloc (sizeof (zchunk_t) + size);
    if (self) {
        self->max_size = size;
        self->data = (byte *) self + sizeof (zchunk_t);
        if (data) {
            self->size = size;
            memcpy (self->data, data, size);
        }
        else
            self->size = 0;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a chunk

void
zchunk_destroy (zchunk_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zchunk_t *self = *self_p;
        //  If data was reallocated independently, free it independently
        if (self->data != (byte *) self + sizeof (zchunk_t))
            free (self->data);

        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Resizes chunk max_size as requested; chunk_cur size is set to zero

void
zchunk_resize (zchunk_t *self, size_t size)
{
    //  If data was reallocated independently, free it independently
    if (self->data != (byte *) self + sizeof (zchunk_t))
        free (self->data);

    self->data = (byte *) zmalloc (size);
    self->max_size = size;
    self->size = 0;
}


//  --------------------------------------------------------------------------
//  Set chunk data from user-supplied data; truncate if too large
//  Returns actual size of chunk

size_t
zchunk_set (zchunk_t *self, const void *data, size_t size)
{
    assert (self);
    if (size > self->max_size)
        size = self->max_size;
    memcpy (self->data, data, size);
    self->size = size;
    return size;
}


//  --------------------------------------------------------------------------
//  Fill chunk data from user-supplied octet
//  Returns actual size of chunk

size_t
zchunk_fill (zchunk_t *self, byte filler, size_t size)
{
    assert (self);
    if (size > self->max_size)
        size = self->max_size;
    memset (self->data, filler, size);
    self->size = size;
    return size;
}


//  --------------------------------------------------------------------------
//  Return chunk cur size

size_t
zchunk_size (zchunk_t *self)
{
    assert (self);
    return self->size;
}


//  --------------------------------------------------------------------------
//  Return chunk max size

size_t
zchunk_max_size (zchunk_t *self)
{
    assert (self);
    return self->max_size;
}


//  --------------------------------------------------------------------------
//  Return chunk data

byte *
zchunk_data (zchunk_t *self)
{
    assert (self);
    return self->data;
}


//  --------------------------------------------------------------------------
//  Read chunk from an open file descriptor

zchunk_t *
zchunk_read (FILE *handle, size_t bytes)
{
    zchunk_t *self = zchunk_new (NULL, bytes);
    self->size = fread (self->data, 1, bytes, handle);
    return self;
}


//  --------------------------------------------------------------------------
//  Write chunk to an open file descriptor

int
zchunk_write (zchunk_t *self, FILE *handle)
{
    size_t items = fwrite (self->data, 1, self->size, handle);
    int rc = (items < self->size)? -1: 0;
    return rc;
}


//  --------------------------------------------------------------------------
//  Self test of this class
int
zchunk_test (bool verbose)
{
    printf (" * zchunk: ");

    //  @selftest
    zchunk_t *chunk = zchunk_new ("1234567890", 10);
    assert (chunk);
    assert (zchunk_size (chunk) == 10);
    assert (memcmp (zchunk_data (chunk), "1234567890", 10) == 0);
    zchunk_destroy (&chunk);
    //  @end

    printf ("OK\n");
    return 0;
}
