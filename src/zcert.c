/*  =========================================================================
    zcert - work with CURVE security certificates

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
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
@header
    The zcert class provides a way to create and work with security
    certificates for the ZMQ CURVE mechanism. A certificate contains a
    public + secret key pair, plus metadata. It can be used as a
    temporary object in memory, or persisted to disk. On disk, a
    certificate is stored as two files. One is public and contains only
    the public key. The second is secret and contains both keys. The
    two have the same filename, with the secret file adding "_secret".
    To exchange certificates, send the public file via some secure
    route. Certificates are not signed but are text files that can
    be verified by eye.
@discuss
    Certificates are stored in the ZPL (ZMQ RFC 4) format.
@end
*/

#include "../include/czmq.h"
#include "platform.h"

#ifdef HAVE_LIBSODIUM
#   include <sodium.h>
#   if crypto_box_PUBLICKEYBYTES != 32 \
    || crypto_box_SECRETKEYBYTES != 32 \
    || crypto_sign_PUBLICKEYBYTES != 32 \
    || crypto_sign_PUBLICKEYBYTES != 32
#       error "libsodium not built correctly"
#   endif
#endif

//  Structure of our class

struct _zcert_t {
    byte public_key [32];       //  Public key in binary
    byte secret_key [32];       //  Secret key in binary
    char public_txt [41];       //  Public key in Z85 text
    char secret_txt [41];       //  Secret key in Z85 text
    zhash_t *headers;           //  Certificate contents
};


//  --------------------------------------------------------------------------
//  Constructor

zcert_t *
zcert_new (void)
{
    byte public_key [32] = { 0 };
    byte secret_key [32] = { 0 };
#ifdef HAVE_LIBSODIUM
    int rc = crypto_box_keypair (public_key, secret_key);
    assert (rc == 0);
#endif
    return zcert_new_from (public_key, secret_key);
}


//  --------------------------------------------------------------------------
//  Constructor, accepts public/secret key pair from caller

zcert_t *
zcert_new_from (byte *public_key, byte *secret_key)
{
    zcert_t *self =
        (zcert_t *) zmalloc (sizeof (zcert_t));
    assert (self);

    assert (public_key);
    memcpy (self->public_key, public_key, 32);
    zmq_z85_encode (self->public_txt, self->public_key, 32);

    assert (secret_key);
    memcpy (self->secret_key, secret_key, 32);
    zmq_z85_encode (self->secret_txt, self->secret_key, 32);

    self->headers = zhash_new ();
    zhash_autofree (self->headers);

    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zcert_destroy (zcert_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zcert_t *self = *self_p;
        zhash_destroy (&self->headers);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Return public part of key pair as 32-byte binary string

byte *
zcert_public (zcert_t *self)
{
    assert (self);
    return self->public_key;
}


//  --------------------------------------------------------------------------
//  Return secret part of key pair as 32-byte binary string

byte *
zcert_secret (zcert_t *self)
{
    assert (self);
    return self->secret_key;
}


//  --------------------------------------------------------------------------
//  Return public part of key pair as Z85 armored string

char *
zcert_public_txt (zcert_t *self)
{
    assert (self);
    return self->public_txt;
}


//  --------------------------------------------------------------------------
//  Return secret part of key pair as Z85 armored string

char *
zcert_secret_txt (zcert_t *self)
{
    assert (self);
    return self->secret_txt;
}


//  --------------------------------------------------------------------------
//  Set certificate header (metadata) from formatted string.

void
zcert_set_header (zcert_t *self, const char *name, const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *value = zsys_vprintf (format, argptr);
    va_end (argptr);
    zhash_insert (self->headers, name, value);
    free (value);
}


//  --------------------------------------------------------------------------
//  Get header value from certificate; if the header doesn't exist, returns
//  NULL.

char *
zcert_header (zcert_t *self, const char *name)
{
    assert (self);
    return zhash_lookup (self->headers, name);
}


//  --------------------------------------------------------------------------
//  Save certificate to the file system persistent storage. This creates
//  one public file and one secret file (filename + "_secret").

static int
s_save_metadata (const char *name, void *value, void *args)
{
    zconfig_t *item = zconfig_new (name, (zconfig_t *) args);
    zconfig_set_value (item, "%s", (char *) value);
    return 0;
}

int
zcert_save (zcert_t *self, char *filename)
{
    assert (self);
    assert (filename);

    zconfig_t *root = zconfig_new ("root", NULL);
    zconfig_t *section = zconfig_new ("headers", root);
    zhash_foreach (self->headers, s_save_metadata, section);

    char *timestr = zclock_timestr ();
    section = zconfig_new ("curve", root);
    
    //  Store public key into file and save
    zconfig_t *item = zconfig_new ("public-key", section);
    zconfig_set_value (item, "%s", self->public_txt);
    zconfig_comment (root, "   ZeroMQ CURVE Public Certificate");
    zconfig_comment (root, "   Generated on %s by CZMQ", timestr);
    zconfig_comment (root, "   Exchange securely, or use a secure mechanism to verify the contents");
    zconfig_comment (root, "   of this file after exchange. Store public certificates in your home");
    zconfig_comment (root, "   directory, in the .curve subdirectory.");
    zconfig_save (root, filename);

    //  Add secret key into file and save secretly
    item = zconfig_new ("secret-key", section);
    zconfig_set_value (item, "%s", self->secret_txt);
    zconfig_comment (root, NULL);
    zconfig_comment (root, "   ZeroMQ CURVE **Secret** Certificate");
    zconfig_comment (root, "   Generated on %s by CZMQ", timestr);
    zconfig_comment (root, "   DO NOT PROVIDE THIS FILE TO OTHER USERS nor change its permissions.");
    char filename_secret [256];
    snprintf (filename_secret, 256, "%s_secret", filename);
    zsys_file_mode_private ();
    int rc = zconfig_save (root, filename_secret);
    zsys_file_mode_default ();
    zconfig_destroy (&root);

    free (timestr);
    return rc;
}


//  --------------------------------------------------------------------------
//  Load certificate from file (constructor)

zcert_t *
zcert_load (char *filename)
{
    assert (filename);

    //  Try first to load secret certificate, which has both keys
    //  Then fallback to loading public certificate
    char filename_secret [256];
    snprintf (filename_secret, 256, "%s_secret", filename);
    zconfig_t *root = zconfig_load (filename_secret);
    if (!root)
        root = zconfig_load (filename);
    if (!root)
        return NULL;
        
    char *public_text = zconfig_resolve (root, "/curve/public-key", NULL);
    char *secret_text = zconfig_resolve (root, "/curve/secret-key", NULL);

    zcert_t *self = NULL;
    if (public_text && strlen (public_text) == 40) {
        byte public_key [32] = { 0 };
        byte secret_key [32] = { 0 };
        zmq_z85_decode (public_key, public_text);
        if (secret_text && strlen (secret_text) == 40)
            zmq_z85_decode (secret_key, secret_text);

        //  Load headers into certificate
        self = zcert_new_from (public_key, secret_key);
        zconfig_t *headers = zconfig_locate (root, "/headers");
        zconfig_t *item = headers? zconfig_child (headers): NULL;
        while (item) {
            zcert_set_header (self, zconfig_name (item), zconfig_value (item));
            item = zconfig_next (item);
        }
    }
    zconfig_destroy (&root);
    return self;
}


//  --------------------------------------------------------------------------
//  Return copy of certificate

zcert_t *
zcert_dup (zcert_t *source)
{
    zcert_t *self = zcert_new_from (source->public_key, source->secret_key);
    zhash_destroy (&self->headers);
    self->headers = zhash_dup (source->headers);
    return self;
}


//  --------------------------------------------------------------------------
//  Return true if two certificates have the same keys

bool
zcert_eq (zcert_t *self, zcert_t *compare)
{
    assert (self);
    assert (compare);

    return (streq (self->public_txt, compare->public_txt)
        &&  streq (self->secret_txt, compare->secret_txt));
}


//  --------------------------------------------------------------------------
//  Dump certificate contents to stderr for debugging

static int
s_dump_header (const char *name, void *value, void *args)
{
    fprintf (stderr, "    %s = \"%s\"\n", name, (char *) value);
    return 0;
}

void
zcert_dump (zcert_t *self)
{
    assert (self);
    fprintf (stderr, "headers\n");
    zhash_foreach (self->headers, s_dump_header, NULL);
    fprintf (stderr, "curve\n");
    fprintf (stderr, "    public-key = \"%s\"\n", self->public_txt);
    fprintf (stderr, "    secret-key = \"%s\"\n", self->secret_txt);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zcert_test (bool verbose)
{
    printf (" * zcert: ");
    //  @selftest
    //  Create a simple certificate with metadata
    zcert_t *cert = zcert_new ();
    zcert_set_header (cert, "email", "ph@imatix.com");
    zcert_set_header (cert, "name", "Pieter Hintjens");
    zcert_set_header (cert, "organization", "iMatix Corporation");
    zcert_set_header (cert, "version", "%d", 1);
    assert (streq (zcert_header (cert, "email"), "ph@imatix.com"));
    if (verbose)
        zcert_dump (cert);

    //  Check the dup and eq methods
    zcert_t *shadow = zcert_dup (cert);
    assert (zcert_eq (cert, shadow));
    zcert_destroy (&shadow);

    //  Check we can save and load certificate
    zcert_save (cert, "test_certificate.txt");
    assert (zsys_file_exists ("test_certificate.txt"));
    assert (zsys_file_exists ("test_certificate.txt_secret"));

    //  Load certificate, will in fact load secret one
    shadow = zcert_load ("test_certificate.txt");
    assert (shadow);
    assert (zcert_eq (cert, shadow));
    zcert_destroy (&shadow);

    //  Delete secret certificate, load public one
    int rc = zsys_file_delete ("test_certificate.txt_secret");
    assert (rc == 0);
    shadow = zcert_load ("test_certificate.txt");
    //  32-byte null key encodes as 40 '0' characters
    assert (streq (zcert_secret_txt (shadow),
        "0000000000000000000000000000000000000000"));
    zcert_destroy (&shadow);
    rc = zsys_file_delete ("test_certificate.txt");
    assert (rc == 0);

    zcert_destroy (&cert);
    //  @end
    printf ("OK\n");
    return 0;
}
