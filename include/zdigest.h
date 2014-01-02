/*  =========================================================================
    zdigest - provides hashing functions (SHA-1 at present)

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZDIGEST_H_INCLUDED__
#define __ZDIGEST_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zdigest_t zdigest_t;

//  @interface
//  Constructor - creates new digest object, which you use to build up a
//  digest by repeatedly calling zdigest_update() on chunks of data.
CZMQ_EXPORT zdigest_t *
    zdigest_new (void);
    
//  Destroy a digest object
CZMQ_EXPORT void
    zdigest_destroy (zdigest_t **self_p);
    
//  Add buffer into digest calculation
CZMQ_EXPORT void
    zdigest_update (zdigest_t *self, byte *buffer, size_t length);
    
//  Return final digest hash data. If built without crypto support, returns
//  NULL.
CZMQ_EXPORT byte *
    zdigest_data (zdigest_t *self);
    
//  Return final digest hash size
CZMQ_EXPORT size_t
    zdigest_size (zdigest_t *self);
    
//  Return digest as printable hex string; caller should not modify nor
//  free this string. After calling this, you may not use zdigest_update()
//  on the same digest. If built without crypto support, returns NULL.
CZMQ_EXPORT char *
    zdigest_string (zdigest_t *self);
    
//  Self test of this class
CZMQ_EXPORT int
    zdigest_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
