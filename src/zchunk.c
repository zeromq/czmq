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
    ZeroMQ's messages but they do less weirdness and so are easier to understand.
    The chunk class has methods to read and write chunks from disk.
@discuss
@end
*/

#include "czmq_classes.h"

//  zchunk_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZCHUNK_TAG              0x0001cafe

//  Structure of our class

struct _zchunk_t {
    uint32_t tag;               //  Object tag for runtime detection
    size_t size;                //  Current size of data part
    size_t max_size;            //  Maximum allocated size
    size_t consumed;            //  Amount already consumed
    zdigest_t *digest;          //  Chunk digest, if known
    byte *data;                 //  Data part follows here
};


//  --------------------------------------------------------------------------
//  Create a new chunk of the specified size. If you specify the data, it
//  is copied into the chunk. If you do not specify the data, the chunk is
//  allocated and left empty, and you can then add data using zchunk_append.

zchunk_t *
zchunk_new (const void *data, size_t size)
{
    //  Use malloc, not zmalloc, to avoid nullification costs
    zchunk_t *self = (zchunk_t *) malloc (sizeof (zchunk_t) + size);
    //  Catch memory exhaustion in this specific class
    if (self) {
        self->tag = ZCHUNK_TAG;
        self->size = 0;
        self->max_size = size;
        self->consumed = 0;
        self->data = (byte *) self + sizeof (zchunk_t);
        self->digest = NULL;
        if (data) {
            self->size = size;
            memcpy (self->data, data, self->size);
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
        assert (zchunk_is (self));
        //  If data was reallocated independently, free it independently
        if (self->data != (byte *) self + sizeof (zchunk_t))
            free (self->data);
        self->tag = 0xDeadBeef;
        zdigest_destroy (&self->digest);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Resizes chunk max_size as requested; chunk size is set to zero

void
zchunk_resize (zchunk_t *self, size_t size)
{
    assert (self);
    assert (zchunk_is (self));
    zdigest_destroy (&self->digest);

    //  Set new sizes
    self->max_size = size;
    self->size = 0;             //  TODO: this is a bit annoying, is it needed?

    //  We can't realloc the chunk itself, as the caller's reference
    //  won't change. So we modify self->data only, depending on whether
    //  it was already reallocated, or not.
    if (self->data == (byte *) self + sizeof (zchunk_t))
        self->data = (byte *) malloc (self->max_size);
    else
        self->data = (byte *) realloc (self->data, self->max_size);
}


//  --------------------------------------------------------------------------
//  Return chunk current size

size_t
zchunk_size (zchunk_t *self)
{
    assert (self);
    assert (zchunk_is (self));
    return self->size;
}


//  --------------------------------------------------------------------------
//  Return chunk max size

size_t
zchunk_max_size (zchunk_t *self)
{
    assert (self);
    assert (zchunk_is (self));
    return self->max_size;
}


//  --------------------------------------------------------------------------
//  Return chunk data

byte *
zchunk_data (zchunk_t *self)
{
    assert (self);
    assert (zchunk_is (self));
    return self->data;
}


//  --------------------------------------------------------------------------
//  Set chunk data from user-supplied data; truncate if too large. Data may
//  be null. Returns actual size of chunk

size_t
zchunk_set (zchunk_t *self, const void *data, size_t size)
{
    assert (self);
    assert (zchunk_is (self));
    zdigest_destroy (&self->digest);

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
    assert (zchunk_is (self));
    zdigest_destroy (&self->digest);

    if (size > self->max_size)
        size = self->max_size;

    memset (self->data, filler, size);
    self->size = size;
    return size;
}


//  --------------------------------------------------------------------------
//  Append user-supplied data to chunk, return resulting chunk size. If the
//  data would exceeded the available space, it is truncated. If you want to
//  grow the chunk to accommodate new data, use the zchunk_extend method.

size_t
zchunk_append (zchunk_t *self, const void *data, size_t size)
{
    assert (self);
    assert (zchunk_is (self));
    zdigest_destroy (&self->digest);

    if (self->size + size > self->max_size)
        size = self->max_size - self->size;

    memcpy (self->data + self->size, data, size);
    self->size += size;
    return self->size;
}


//  --------------------------------------------------------------------------
//  Append user-supplied data to chunk, return resulting chunk size. If the
//  data would exceeded the available space, the chunk grows in size.

size_t
zchunk_extend (zchunk_t *self, const void *data, size_t size)
{
    assert (self);
    if (self->size + size > self->max_size) {
        self->max_size = (self->size + size) * 2;

        //  We can't realloc the chunk itself, as the caller's reference
        //  won't change. So we modify self->data only, depending on whether
        //  it was already reallocated, or not.
        if (self->data == (byte *) self + sizeof (zchunk_t)) {
            byte *old_data = self->data;
            self->data = (byte *) malloc (self->max_size);
            memcpy (self->data, old_data, self->size);
        }
        else
            self->data = (byte *) realloc (self->data, self->max_size);
    }
    assert (self->size + size <= self->max_size);
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
    assert (zchunk_is (self));
    assert (source);
    assert (zchunk_is (source));

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
    assert (zchunk_is (self));

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
    if (self)
        self->size = fread (self->data, 1, bytes, handle);
    return self;
}


//  --------------------------------------------------------------------------
//  Write chunk to an open file descriptor

int
zchunk_write (zchunk_t *self, FILE *handle)
{
    assert (self);
    assert (zchunk_is (self));

    size_t items = fwrite (self->data, 1, self->size, handle);
    int rc = (items < self->size)? -1: 0;
    return rc;
}


//  --------------------------------------------------------------------------
//  Try to slurp an entire file into a chunk. Will read up to maxsize of
//  the file. If maxsize is 0, will attempt to read the entire file and
//  fail with an assertion if that cannot fit into memory. Returns a new
//  chunk containing the file data, or NULL if the file could not be read.

zchunk_t *
zchunk_slurp (const char *filename, size_t maxsize)
{
    size_t size = zsys_file_size (filename);
    if ((ssize_t) size == -1)
        return NULL;

    if (size > maxsize && maxsize != 0)
        size = maxsize;

    FILE *handle = fopen (filename, "r");
    zchunk_t *chunk = zchunk_read (handle, size);
    assert (chunk);
    fclose (handle);
    return chunk;
}


//  --------------------------------------------------------------------------
//  Create copy of chunk, as new chunk object. Returns a fresh zchunk_t
//  object, or null if there was not enough heap memory. If chunk is null,
//  or memory was exhausted, returns null.

zchunk_t *
zchunk_dup (zchunk_t *self)
{
    if (self) {
        assert (zchunk_is (self));
        return zchunk_new (self->data, self->max_size);
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return chunk data encoded as printable hex string. Caller must free
//  string when finished with it.

char *
zchunk_strhex (zchunk_t *self)
{
    assert (self);
    assert (zchunk_is (self));

    static const char
        hex_char [] = "0123456789ABCDEF";

    size_t size = zchunk_size (self);
    byte *data = zchunk_data (self);
    char *hex_str = (char *) zmalloc (size * 2 + 1);
    if (!hex_str)
        return NULL;

    uint byte_nbr;
    for (byte_nbr = 0; byte_nbr < size; byte_nbr++) {
        hex_str [byte_nbr * 2 + 0] = hex_char [data [byte_nbr] >> 4];
        hex_str [byte_nbr * 2 + 1] = hex_char [data [byte_nbr] & 15];
    }
    hex_str [size * 2] = 0;
    return hex_str;
}


//  --------------------------------------------------------------------------
//  Return chunk data copied into freshly allocated string
//  Caller must free string when finished with it.

char *
zchunk_strdup (zchunk_t *self)
{
    assert (self);
    assert (zchunk_is (self));

    size_t size = zchunk_size (self);
    char *string = (char *) malloc (size + 1);
    if (string) {
        memcpy (string, zchunk_data (self), size);
        string [size] = 0;
    }
    return string;
}


//  --------------------------------------------------------------------------
//  Return true if chunk body is equal to string, excluding terminator

bool
zchunk_streq (zchunk_t *self, const char *string)
{
    assert (self);
    assert (zchunk_is (self));

    if (zchunk_size (self) == strlen (string)
    &&  memcmp (zchunk_data (self), string, strlen (string)) == 0)
        return true;
    else
        return false;
}


//  --------------------------------------------------------------------------
//  Create a zframe from a zchunk.  The zframe can be sent in a message.

zframe_t *
zchunk_pack (zchunk_t *self)
{
    assert (self);
    assert (zchunk_is (self));
    return zframe_new (self->data, self->max_size);
}


//  --------------------------------------------------------------------------
//  Create a zchunk from a zframe.

zchunk_t *
zchunk_unpack (zframe_t *frame)
{
    assert (frame);
    assert (zframe_is (frame));
    return zchunk_new (zframe_data (frame), zframe_size (frame));
}


//  --------------------------------------------------------------------------
//  Calculate SHA1 digest for chunk, using zdigest class. Caller should not
//  modify digest.

const char *
zchunk_digest (zchunk_t *self)
{
    assert (self);
    if (!self->digest)
        self->digest = zdigest_new ();
    if (self->digest) {
        zdigest_update (self->digest, self->data, self->size);
        return zdigest_string (self->digest);
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Dump chunk to FILE stream, for debugging and tracing.

void
zchunk_fprint (zchunk_t *self, FILE *file)
{
    assert (self);
    assert (zchunk_is (self));

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
    assert (self);
    assert (zchunk_is (self));

    zchunk_fprint (self, stderr);
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zchunk_t.

bool
zchunk_is (void *self)
{
    assert (self);
    return ((zchunk_t *) self)->tag == ZCHUNK_TAG;
}


//  --------------------------------------------------------------------------
//  Self test of this class

void
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
    assert (chunk);
    zchunk_append (chunk, "12345678", 8);
    zchunk_append (chunk, "90ABCDEF", 8);
    zchunk_append (chunk, "GHIJKLMN", 8);
    assert (memcmp (zchunk_data (chunk), "1234567890", 10) == 0);
    assert (zchunk_size (chunk) == 10);
    assert (zchunk_streq (chunk, "1234567890"));
    assert (streq (zchunk_digest (chunk), "01B307ACBA4F54F55AAFC33BB06BBBF6CA803E9A"));
    char *string = zchunk_strdup (chunk);
    assert (streq (string, "1234567890"));
    free (string);
    string = zchunk_strhex (chunk);
    assert (streq (string, "31323334353637383930"));
    free (string);

    zframe_t *frame = zchunk_pack (chunk);
    assert (frame);

    zchunk_t *chunk2 = zchunk_unpack (frame);
    assert (chunk2);
    assert (memcmp (zchunk_data (chunk2), "1234567890", 10) == 0);
    zframe_destroy (&frame);
    zchunk_destroy (&chunk2);

    zchunk_t *copy = zchunk_dup (chunk);
    assert (copy);
    assert (memcmp (zchunk_data (copy), "1234567890", 10) == 0);
    assert (zchunk_size (copy) == 10);
    zchunk_destroy (&copy);
    zchunk_destroy (&chunk);

    chunk = zchunk_new (NULL, 0);
    zchunk_extend (chunk, "12345678", 8);
    zchunk_extend (chunk, "90ABCDEF", 8);
    zchunk_extend (chunk, "GHIJKLMN", 8);
    assert (zchunk_size (chunk) == 24);
    assert (zchunk_streq (chunk, "1234567890ABCDEFGHIJKLMN"));
    zchunk_destroy (&chunk);

    copy = zchunk_new ("1234567890abcdefghij", 20);
    assert (copy);
    chunk = zchunk_new (NULL, 8);
    assert (chunk);
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
}
