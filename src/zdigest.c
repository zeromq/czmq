/*  =========================================================================
    zdigest - provides hashing functions (SHA-1 at present)

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
    The zdigest class generates a hash from zchunks of data. The current
    algorithm is SHA-1, chosen for speed. We are aiming to generate a
    unique digest for a file, and there are no security issues in this
    use case.
@discuss
    The current code depends on OpenSSL, which might be replaced by hard
    coded SHA-1 implementation to reduce build dependencies.
@end
*/

#include "../include/czmq.h"
#include "../foreign/sha1/sha1.c"


//  Structure of our class

struct _zdigest_t {
    SHA_CTX context;            //  Digest context
    //  Binary hash
    byte hash [SHA_DIGEST_LENGTH];
    //  ASCII representation (hex)
    char string [SHA_DIGEST_LENGTH * 2 + 1];
    bool final;                 //  Finished calculating
};


//  --------------------------------------------------------------------------
//  Constructor - creates new digest object, which you use to build up a
//  digest by repeatedly calling zdigest_update() on chunks of data.

zdigest_t *
zdigest_new (void)
{
    zdigest_t *self = (zdigest_t *) zmalloc (sizeof (zdigest_t));
    SHA1_Init (&self->context);
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a digest object

void
zdigest_destroy (zdigest_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdigest_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Add buffer into digest calculation

void
zdigest_update (zdigest_t *self, byte *buffer, size_t length)
{
    //  Calling this after zdigest_data() is illegal use of the API
    assert (self);
    assert (!self->final);
    SHA1_Update (&self->context, buffer, length);
}


//  --------------------------------------------------------------------------
//  Return final digest hash data. If built without crypto support, returns
//  NULL.

byte *
zdigest_data (zdigest_t *self)
{
    assert (self);
    if (!self->final) {
        SHA1_Final (self->hash, &self->context);
        self->final = true;
    }
    return self->hash;
}


//  --------------------------------------------------------------------------
//  Return final digest hash size

size_t
zdigest_size (zdigest_t *self)
{
    assert (self);
    return SHA_DIGEST_LENGTH;
}


//  --------------------------------------------------------------------------
//  Return digest as printable hex string; caller should not modify nor
//  free this string. After calling this, you may not use zdigest_update()
//  on the same digest. If built without crypto support, returns NULL.

char *
zdigest_string (zdigest_t *self)
{
    assert (self);
    byte *data = zdigest_data (self);
    char hex_char [] = "0123456789ABCDEF";
    int byte_nbr;
    for (byte_nbr = 0; byte_nbr < SHA_DIGEST_LENGTH; byte_nbr++) {
        self->string [byte_nbr * 2 + 0] = hex_char [data [byte_nbr] >> 4];
        self->string [byte_nbr * 2 + 1] = hex_char [data [byte_nbr] & 15];
    }
    self->string [SHA_DIGEST_LENGTH * 2] = 0;
    return self->string;
}


//  --------------------------------------------------------------------------
//  Self test of this class

int
zdigest_test (bool verbose)
{
    printf (" * zdigest: ");

    //  @selftest
    byte *buffer = (byte *) zmalloc (1024);
    memset (buffer, 0xAA, 1024);
    
    zdigest_t *digest = zdigest_new ();
    zdigest_update (digest, buffer, 1024);
    byte *data = zdigest_data (digest);
    assert (data [0] == 0xDE);
    assert (data [1] == 0xB2);
    assert (data [2] == 0x38);
    assert (data [3] == 0x07);
    assert (streq (zdigest_string (digest),
        "DEB23807D4FE025E900FE9A9C7D8410C3DDE9671"));
    zdigest_destroy (&digest);
    free (buffer);
    //  @end

    printf ("OK\n");
    return 0;
}
