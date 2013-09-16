/*  =========================================================================
    zchunk - work with memory chunks

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under the
    terms of the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your option)
    any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
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

//  Set chunk data from user-supplied data; truncate if too large
CZMQ_EXPORT size_t
    zchunk_set (zchunk_t *self, const void *data, size_t size);

//  Fill chunk data from user-supplied octet
CZMQ_EXPORT size_t
    zchunk_fill (zchunk_t *self, byte filler, size_t size);

//  Read chunk from an open file descriptor
CZMQ_EXPORT zchunk_t *
    zchunk_read (FILE *handle, size_t bytes);
    
//  Write chunk to an open file descriptor
CZMQ_EXPORT int
    zchunk_write (zchunk_t *self, FILE *handle);

//  Self test of this class
CZMQ_EXPORT int
    zchunk_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif

