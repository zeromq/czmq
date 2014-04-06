/*  =========================================================================
    zchunk - work with memory chunks

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
    size_t consumed;            //  Amount already consumed
    byte *data;                 //  Data part follows here
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
//  Resizes chunk max_size as requested; chunk size is set to zero

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
//  Return chunk current size

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
//  Set chunk data from user-supplied data; truncate if too large. Data may
//  be null. Returns actual size of chunk

size_t
zchunk_set (zchunk_t *self, const void *data, size_t size)
{
    assert (self);
    if (size > self->max_size)
        size = self->max_size;
    if (data)
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
//  Append user-supplied data to chunk, return resulting chunk size

size_t
zchunk_append (zchunk_t *self, const void *data, size_t size)
{
    assert (self);
    if (self->size + size > self->max_size)
        size = self->max_size - self->size;
    memcpy (self->data + self->size, data, size);
    self->size += size;
    return self->size;
}


//  --------------------------------------------------------------------------
//  Copy as much data from 'source' into the chunk as possible; returns the
//  new size of chunk. If all data from 'source' is used, returns exhausted
//  on the source chunk. Source can be consumed as many times as needed until
//  it is exhausted. If source was already exhausted, does not change chunk.

size_t
zchunk_consume (zchunk_t *self, zchunk_t *source)
{
    assert (self);
    assert (source);

    //  We can take at most this many bytes from source
    size_t size = source->size - source->consumed;

    //  And we can store at most this many bytes in chunk
    if (self->size + size > self->max_size)
        size = self->max_size - self->size;

    memcpy (self->data + self->size, source->data + source->consumed, size);
    source->consumed += size;
    self->size += size;
    return self->size;
}


//  --------------------------------------------------------------------------
//  Returns true if the chunk was exhausted by consume methods, or if the
//  chunk has a size of zero.

bool
zchunk_exhausted (zchunk_t *self)
{
    assert (self);
    assert (self->consumed <= self->size);
    return self->consumed == self->size;
}


//  --------------------------------------------------------------------------
//  Read chunk from an open file descriptor

zchunk_t *
zchunk_read (FILE *handle, size_t bytes)
{
    assert (handle);
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
//  Create copy of chunk, as new chunk object. Returns a fresh zchunk_t
//  object, or NULL if there was not enough heap memory.

zchunk_t *
zchunk_dup (zchunk_t *self)
{
    assert (self);
    return zchunk_new (self->data, self->max_size);
}


//  --------------------------------------------------------------------------
//  Dump chunk to FILE stream, for debugging and tracing.

void
zchunk_fprint (zchunk_t *self, FILE *file)
{
    fprintf (file, "--------------------------------------\n");
    if (!self) {
        fprintf (file, "NULL");
        return;
    }
    assert (self);
    int is_bin = 0;
    uint char_nbr;
    for (char_nbr = 0; char_nbr < self->size; char_nbr++)
        if (self->data [char_nbr] < 9 || self->data [char_nbr] > 127)
            is_bin = 1;

    fprintf (file, "[%03d] ", (int) self->size);
    for (char_nbr = 0; char_nbr < self->size; char_nbr++) {
        if (is_bin) {
            fprintf (file, "%02X", (unsigned char) self->data [char_nbr]);
            if (char_nbr > 35) {
                fprintf (file, "...");
                break;
            }
        }
        else {
            fprintf (file, "%c", self->data [char_nbr]);
            if (char_nbr > 70) {
                fprintf (file, "...");
                break;
            }
        }
    }
    fprintf (file, "\n");
}



//  --------------------------------------------------------------------------
//  Dump message to stderr, for debugging and tracing.
//  See zchunk_fprint for details

void
zchunk_print (zchunk_t *self)
{
    zchunk_fprint (self, stderr);
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

    chunk = zchunk_new (NULL, 10);
    zchunk_append (chunk, "12345678", 8);
    zchunk_append (chunk, "90ABCDEF", 8);
    zchunk_append (chunk, "GHIJKLMN", 8);
    assert (memcmp (zchunk_data (chunk), "1234567890", 10) == 0);
    assert (zchunk_size (chunk) == 10);
    
    zchunk_t *copy = zchunk_dup (chunk);
    assert (memcmp (zchunk_data (copy), "1234567890", 10) == 0);
    assert (zchunk_size (copy) == 10);
    zchunk_destroy (&copy);
    zchunk_destroy (&chunk);

    copy = zchunk_new ("1234567890abcdefghij", 20);
    chunk = zchunk_new (NULL, 8);
    zchunk_consume (chunk, copy);
    assert (!zchunk_exhausted (copy));
    assert (memcmp (zchunk_data (chunk), "12345678", 8) == 0);
    zchunk_set (chunk, NULL, 0);
    zchunk_consume (chunk, copy);
    assert (!zchunk_exhausted (copy));
    assert (memcmp (zchunk_data (chunk), "90abcdef", 8) == 0);
    zchunk_set (chunk, NULL, 0);
    zchunk_consume (chunk, copy);
    assert (zchunk_exhausted (copy));
    assert (zchunk_size (chunk) == 4);
    assert (memcmp (zchunk_data (chunk), "ghij", 4) == 0);
    zchunk_destroy (&copy);
    zchunk_destroy (&chunk);
    //  @end

    printf ("OK\n");
    return 0;
}
