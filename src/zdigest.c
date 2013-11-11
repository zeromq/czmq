/*  =========================================================================
    fmq_hash - provides hashing functions (SHA-1 at present)

    -------------------------------------------------------------------------
    Copyright (c) 1991-2012 iMatix Corporation -- http://www.imatix.com
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of FILEMQ, see http://filemq.org.

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
    =========================================================================
*/

/*
@header
    The fmq_hash class allows to hash blobs.
@discuss
@end
*/

#include <czmq.h>
#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
#  include <openssl/sha.h>
#endif
#include "../include/fmq.h"


//  Structure of our class

struct _fmq_hash_t {
    SHA_CTX context;
    byte hash [SHA_DIGEST_LENGTH];
    bool final;
};


//  --------------------------------------------------------------------------
//  Constructor
//  Create new SHA object

fmq_hash_t *
fmq_hash_new (void)
{
    fmq_hash_t *self = (fmq_hash_t *) zmalloc (sizeof (fmq_hash_t));
    SHA1_Init (&self->context);
    return self;
}

//  --------------------------------------------------------------------------
//  Destroy a SHA object

void
fmq_hash_destroy (fmq_hash_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        fmq_hash_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Add buffer into SHA calculation

void
fmq_hash_update (fmq_hash_t *self, byte *buffer, size_t length)
{
    //  Buffer has to be on 64-bit boundary
    assert (((long) buffer & 7L) == 0);
    SHA1_Update (&self->context, buffer, length);
}


//  --------------------------------------------------------------------------
//  Return final SHA hash data

byte *
fmq_hash_data (fmq_hash_t *self)
{
    if (!self->final) {
        SHA1_Final (self->hash, &self->context);
        self->final = true;
    }
    return self->hash;
}


//  --------------------------------------------------------------------------
//  Return final SHA hash size

size_t
fmq_hash_size (fmq_hash_t *self)
{
    return SHA_DIGEST_LENGTH;
}


//  --------------------------------------------------------------------------
//  Self test of this class

int
fmq_hash_test (bool verbose)
{
    printf (" * fmq_hash: ");

    //  @selftest
    byte *buffer = (byte *) zmalloc (1024);
    memset (buffer, 0xAA, 1024);
    
    fmq_hash_t *hash = fmq_hash_new ();
    fmq_hash_update (hash, buffer, 1024);
    byte *data = fmq_hash_data (hash);
    size_t size = fmq_hash_size (hash);
    assert (data [0] == 0xDE);
    assert (data [1] == 0xB2);
    assert (data [2] == 0x38);
    assert (data [3] == 0x07);
    fmq_hash_destroy (&hash);
    free (buffer);
    //  @end

    printf ("OK\n");
    return 0;
}
