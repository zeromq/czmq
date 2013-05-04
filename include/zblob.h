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

#ifndef __ZBLOB_H_INCLUDED__
#define __ZBLOB_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zblob_t zblob_t;

//  @interface
//  Constructor. If data is not null, must be at least size bytes, and is
//  taken as value for blob.
CZMQ_EXPORT zblob_t *
    zblob_new (byte *data, size_t size);
    
//  Destructor
CZMQ_EXPORT void
    zblob_destroy (zblob_t **self_p);
    
//  Return blob size
CZMQ_EXPORT size_t
    zblob_size (zblob_t *self);
    
//  Return blob data; you can write to this memory block.
CZMQ_EXPORT byte *
    zblob_data (zblob_t *self);
    
//  Self test of this class
void
    zblob_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
