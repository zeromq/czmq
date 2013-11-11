/*  =========================================================================
    fmq_hash - provides hashing functions (SHA-1 at present)

    -------------------------------------------------------------------------
    Copyright (c) 1991-2012 iMatix Corporation -- http://www.imatix.com
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of FILEMQ, see http://filemq.org.

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

#ifndef __FMQ_HASH_H_INCLUDED__
#define __FMQ_HASH_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _fmq_hash_t fmq_hash_t;

//  @interface
//  Create new HASH object
fmq_hash_t *
    fmq_hash_new (void);

//  Destroy a HASH object
void
    fmq_hash_destroy (fmq_hash_t **self_p);

//  Add buffer into HASH calculation
void
    fmq_hash_update (fmq_hash_t *self, byte *buffer, size_t length);
    
//  Return final HASH hash data
byte *
    fmq_hash_data (fmq_hash_t *self);

//  Return final HASH hash size
size_t
    fmq_hash_size (fmq_hash_t *self);

//  Self test of this class
int
    fmq_hash_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
