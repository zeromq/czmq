/*  =========================================================================
    zchunk - work with memory chunks

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZCHUNK_H_INCLUDED__
#define __ZCHUNK_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zchunk_t zchunk_t;

//  @interface
//  Create new chunk
CZMQ_EXPORT zchunk_t *
    zchunk_new (const void *data, size_t size);

//  Destroy a chunk
CZMQ_EXPORT void
    zchunk_destroy (zchunk_t **self_p);

//  Resizes chunk max_size as requested; chunk_cur size is set to zero
CZMQ_EXPORT void
    zchunk_resize (zchunk_t *self, size_t size);

//  Return chunk cur size
CZMQ_EXPORT size_t
    zchunk_size (zchunk_t *self);

//  Return chunk max size
CZMQ_EXPORT size_t
    zchunk_max_size (zchunk_t *self);

//  Return chunk data
CZMQ_EXPORT byte *
    zchunk_data (zchunk_t *self);

//  Set chunk data from user-supplied data; truncate if too large. Data may
//  be null. Returns actual size of chunk
CZMQ_EXPORT size_t
    zchunk_set (zchunk_t *self, const void *data, size_t size);

//  Fill chunk data from user-supplied octet
CZMQ_EXPORT size_t
    zchunk_fill (zchunk_t *self, byte filler, size_t size);

//  Append user-supplied data to chunk, return resulting chunk size
CZMQ_EXPORT size_t
    zchunk_append (zchunk_t *self, const void *data, size_t size);

//  Copy as much data from 'source' into the chunk as possible; returns the
//  new size of chunk. If all data from 'source' is used, returns exhausted
//  on the source chunk. Source can be consumed as many times as needed until
//  it is exhausted. If source was already exhausted, does not change chunk.
CZMQ_EXPORT size_t
    zchunk_consume (zchunk_t *self, zchunk_t *source);

//  Returns true if the chunk was exhausted by consume methods, or if the
//  chunk has a size of zero.
CZMQ_EXPORT bool
    zchunk_exhausted (zchunk_t *self);

//  Read chunk from an open file descriptor
CZMQ_EXPORT zchunk_t *
    zchunk_read (FILE *handle, size_t bytes);
    
//  Write chunk to an open file descriptor
CZMQ_EXPORT int
    zchunk_write (zchunk_t *self, FILE *handle);

//  Create copy of chunk, as new chunk object. Returns a fresh zchunk_t
//  object, or NULL if there was not enough heap memory.
CZMQ_EXPORT zchunk_t *
    zchunk_dup (zchunk_t *self);

//  Dump chunk to FILE stream, for debugging and tracing.
CZMQ_EXPORT void
    zchunk_fprint (zchunk_t *self, FILE *file);

//  Dump message to stderr, for debugging and tracing.
//  See zchunk_fprint for details
CZMQ_EXPORT void
    zchunk_print (zchunk_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zchunk_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif

