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
    To exchange certificates, send the public file via some secure route. 
    Certificates are not signed but are text files that can be verified by 
    eye.
@discuss
    Certificates are stored in the ZPL (ZMQ RFC 4) format. They have two
    sections, "metadata" and "curve". The first contains a list of 'name =
    value' pairs, one per line. Values may be enclosed in quotes. The curve
    section has a 'public-key = keyvalue' and, for secret certificates, a
    'secret-key = keyvalue' line. The keyvalue is a Z85-encoded CURVE key.
@end
*/

#include "../include/czmq.h"
#include "platform.h"

#if defined (HAVE_LIBSODIUM)
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
    zhash_t *metadata;          //  Certificate metadata
    zconfig_t *config;          //  Config tree to save
};


//  --------------------------------------------------------------------------
//  Constructor

zcert_t *
zcert_new (void)
{
    byte public_key [32] = { 0 };
    byte secret_key [32] = { 0 };
#if defined (HAVE_LIBSODIUM)
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
#if (ZMQ_VERSION_MAJOR == 4)
    zcert_t *self =
        (zcert_t *) zmalloc (sizeof (zcert_t));
    assert (self);

    assert (public_key);
    memcpy (self->public_key, public_key, 32);
    zmq_z85_encode (self->public_txt, self->public_key, 32);

    assert (secret_key);
    memcpy (self->secret_key, secret_key, 32);
    zmq_z85_encode (self->secret_txt, self->secret_key, 32);

    self->metadata = zhash_new ();
    zhash_autofree (self->metadata);

    return self;
#else
    return NULL;
#endif
}


//  --------------------------------------------------------------------------
//  Destructor

void
zcert_destroy (zcert_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zcert_t *self = *self_p;
        zhash_destroy (&self->metadata);
        zconfig_destroy (&self->config);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Return public part of key pair as 32-byte binary string

byte *
zcert_public_key (zcert_t *self)
{
    assert (self);
    return self->public_key;
}


//  --------------------------------------------------------------------------
//  Return secret part of key pair as 32-byte binary string

byte *
zcert_secret_key (zcert_t *self)
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
//  Set certificate metadata from formatted string.

void
zcert_set_meta (zcert_t *self, char *name, char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *value = zsys_vprintf (format, argptr);
    va_end (argptr);
    zhash_insert (self->metadata, name, value);
    free (value);
}


//  --------------------------------------------------------------------------
//  Get metadata value from certificate; if the metadata value doesn't 
//  exist, returns NULL.

char *
zcert_meta (zcert_t *self, char *name)
{
    assert (self);
    return (char *) zhash_lookup (self->metadata, name);
}


//  --------------------------------------------------------------------------
//  Load certificate from file (constructor)
//  The filename is treated as a printf format specifier.

zcert_t *
zcert_load (char *format, ...)
{
#if (ZMQ_VERSION_MAJOR == 4)
    assert (format);
    va_list argptr;
    va_start (argptr, format);
    char *filename = zsys_vprintf (format, argptr);
    va_end (argptr);

    //  Try first to load secret certificate, which has both keys
    //  Then fallback to loading public certificate
    char filename_secret [256];
    snprintf (filename_secret, 256, "%s_secret", filename);
    zconfig_t *root = zconfig_load (filename_secret);
    if (!root)
        root = zconfig_load (filename);
        
    zcert_t *self = NULL;
    if (root) {
        char *public_text = zconfig_resolve (root, "/curve/public-key", NULL);
        char *secret_text = zconfig_resolve (root, "/curve/secret-key", NULL);
        if (public_text && strlen (public_text) == 40) {
            byte public_key [32] = { 0 };
            byte secret_key [32] = { 0 };
            zmq_z85_decode (public_key, public_text);
            if (secret_text && strlen (secret_text) == 40)
                zmq_z85_decode (secret_key, secret_text);

            //  Load metadata into certificate
            self = zcert_new_from (public_key, secret_key);
            zconfig_t *metadata = zconfig_locate (root, "/metadata");
            zconfig_t *item = metadata? zconfig_child (metadata): NULL;
            while (item) {
                zcert_set_meta (self, zconfig_name (item), zconfig_value (item));
                item = zconfig_next (item);
            }
        }
    }
    zconfig_destroy (&root);
    free (filename);
    return self;
#else   
    return NULL;
#endif
}


//  --------------------------------------------------------------------------
//  Save full certificate (public + secret) to file for persistent storage
//  This creates one public file and one secret file (filename + "_secret").
//  The filename is treated as a printf format specifier.

static int
s_save_metadata (const char *name, void *value, void *args)
{
    zconfig_t *item = zconfig_new ((char *) name, (zconfig_t *) args);
    zconfig_set_value (item, "%s", (char *) value);
    return 0;
}

static void
s_save_metadata_all (zcert_t *self)
{
    zconfig_destroy (&self->config);
    self->config = zconfig_new ("root", NULL);
    zconfig_t *section = zconfig_new ("metadata", self->config);
    zhash_foreach (self->metadata, s_save_metadata, section);
    
    char *timestr = zclock_timestr ();
    zconfig_comment (self->config, "   ****  Generated on %s by CZMQ  ****", timestr);
    free (timestr);
}


int
zcert_save (zcert_t *self, char *format, ...)
{
    assert (self);
    assert (format);
    va_list argptr;
    va_start (argptr, format);
    char *filename = zsys_vprintf (format, argptr);
    va_end (argptr);

    //  Save public certificate using specified filename
    zcert_save_public (self, filename);

    //  Now save secret certificate using filename with "_secret" suffix
    s_save_metadata_all (self);
    zconfig_comment (self->config, "   ZeroMQ CURVE **Secret** Certificate");
    zconfig_comment (self->config, "   DO NOT PROVIDE THIS FILE TO OTHER USERS nor change its permissions.");
    zconfig_put (self->config, "/curve/public-key", self->public_txt);
    zconfig_put (self->config, "/curve/secret-key", self->secret_txt);
    
    char filename_secret [256];
    snprintf (filename_secret, 256, "%s_secret", filename);
    zsys_file_mode_private ();
    int rc = zconfig_save (self->config, filename_secret);
    zsys_file_mode_default ();
    
    free (filename);
    return rc;
}


//  --------------------------------------------------------------------------
//  Save public certificate only to file for persistent storage.
//  The filename is treated as a printf format specifier.

int
zcert_save_public (zcert_t *self, char *format, ...)
{
    assert (self);
    assert (format);
    va_list argptr;
    va_start (argptr, format);
    char *filename = zsys_vprintf (format, argptr);
    va_end (argptr);

    s_save_metadata_all (self);
    zconfig_comment (self->config, "   ZeroMQ CURVE Public Certificate");
    zconfig_comment (self->config, "   Exchange securely, or use a secure mechanism to verify the contents");
    zconfig_comment (self->config, "   of this file after exchange. Store public certificates in your home");
    zconfig_comment (self->config, "   directory, in the .curve subdirectory.");
    
    zconfig_put (self->config, "/curve/public-key", self->public_txt);
    int rc = zconfig_save (self->config, filename);
    free (filename);
    return rc;
}


//  --------------------------------------------------------------------------
//  Apply certificate to socket, i.e. use for CURVE security on socket.
//  If certificate was loaded from public file, the secret key will be
//  undefined, and this certificate will not work successfully.

void
zcert_apply (zcert_t *self, void *zocket)
{
    assert (self);
#if (ZMQ_VERSION_MAJOR == 4)
    zsocket_set_curve_secretkey_bin (zocket, self->secret_key);
    zsocket_set_curve_publickey_bin (zocket, self->public_key);
#endif
}


//  --------------------------------------------------------------------------
//  Return copy of certificate

zcert_t *
zcert_dup (zcert_t *source)
{
    zcert_t *self = zcert_new_from (source->public_key, source->secret_key);
    zhash_destroy (&self->metadata);
    self->metadata = zhash_dup (source->metadata);
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
s_dump_metadata (const char *name, void *value, void *args)
{
    fprintf (stderr, "    %s = \"%s\"\n", name, (char *) value);
    return 0;
}

void
zcert_dump (zcert_t *self)
{
    assert (self);
    fprintf (stderr, "metadata\n");
    zhash_foreach (self->metadata, s_dump_metadata, NULL);
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
#if (ZMQ_VERSION_MAJOR == 4)
    //  @selftest
    //  Create temporary directory for test files
#   define TESTDIR ".test_zcert"
    zsys_dir_create (TESTDIR);
    
    //  Create a simple certificate with metadata
    zcert_t *cert = zcert_new ();
    zcert_set_meta (cert, "email", "ph@imatix.com");
    zcert_set_meta (cert, "name", "Pieter Hintjens");
    zcert_set_meta (cert, "organization", "iMatix Corporation");
    zcert_set_meta (cert, "version", "%d", 1);
    assert (streq (zcert_meta (cert, "email"), "ph@imatix.com"));

    //  Check the dup and eq methods
    zcert_t *shadow = zcert_dup (cert);
    assert (zcert_eq (cert, shadow));
    zcert_destroy (&shadow);

    //  Check we can save and load certificate
    zcert_save (cert, TESTDIR "/mycert.txt");
    assert (zsys_file_exists (TESTDIR "/mycert.txt"));
    assert (zsys_file_exists (TESTDIR "/mycert.txt_secret"));

    //  Load certificate, will in fact load secret one
    shadow = zcert_load (TESTDIR "/mycert.txt");
    assert (shadow);
    assert (zcert_eq (cert, shadow));
    zcert_destroy (&shadow);

    //  Delete secret certificate, load public one
    //int rc = zsys_file_delete (TESTDIR "/mycert.txt_secret");
    //assert (rc == 0);
    //shadow = zcert_load (TESTDIR "/mycert.txt");
    //  32-byte null key encodes as 40 '0' characters
    //assert (streq (zcert_secret_txt (shadow),
        //"0000000000000000000000000000000000000000"));
    //zcert_destroy (&shadow);
    zcert_destroy (&cert);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
    //  @end
#endif
    printf ("OK\n");
    return 0;
}

