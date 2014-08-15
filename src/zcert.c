/*  =========================================================================
    zcert - work with CURVE security certificates

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

#include "platform.h"
#include "../include/czmq.h"

//  Structure of our class

struct _zcert_t {
    byte public_key [32];       //  Public key in binary
    byte secret_key [32];       //  Secret key in binary
    char public_txt [41];       //  Public key in Z85 text
    char secret_txt [41];       //  Secret key in Z85 text
    zhash_t *metadata;          //  Certificate metadata
    zconfig_t *config;          //  Config tree to save
};

//  This is a null key, when curve is not available
#define FORTY_ZEROES "0000000000000000000000000000000000000000"

//  --------------------------------------------------------------------------
//  Constructor

zcert_t *
zcert_new (void)
{
    zcert_t *self = (zcert_t *) zmalloc (sizeof (zcert_t));
    assert (self);

    //  Initialize metadata, even if keys aren't working
    self->metadata = zhash_new ();
    zhash_autofree (self->metadata);
    
#if (ZMQ_VERSION_MAJOR == 4)
    if (zsys_has_curve ()) {
        int rc = zmq_curve_keypair (self->public_txt, self->secret_txt);
        assert (rc == 0);
        zmq_z85_decode (self->public_key, self->public_txt);
        zmq_z85_decode (self->secret_key, self->secret_txt);
    }
    else {
        strcpy (self->public_txt, FORTY_ZEROES);
        strcpy (self->secret_txt, FORTY_ZEROES);
    }
#else
    strcpy (self->public_txt, FORTY_ZEROES);
    strcpy (self->secret_txt, FORTY_ZEROES);
#endif
    return self;
}


//  --------------------------------------------------------------------------
//  Constructor, accepts public/secret key pair from caller

zcert_t *
zcert_new_from (byte *public_key, byte *secret_key)
{
    zcert_t *self = (zcert_t *) zmalloc (sizeof (zcert_t));
    assert (self);
    assert (public_key);
    assert (secret_key);

    self->metadata = zhash_new ();
    zhash_autofree (self->metadata);
    memcpy (self->public_key, public_key, 32);
    memcpy (self->secret_key, secret_key, 32);
    
#if (ZMQ_VERSION_MAJOR == 4)
    zmq_z85_encode (self->public_txt, self->public_key, 32);
    zmq_z85_encode (self->secret_txt, self->secret_key, 32);
#else
    strcpy (self->public_txt, FORTY_ZEROES);
    strcpy (self->secret_txt, FORTY_ZEROES);
#endif
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
zcert_set_meta (zcert_t *self, const char *name, const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *value = zsys_vprintf (format, argptr);
    va_end (argptr);
    zhash_insert (self->metadata, name, value);
    zstr_free (&value);
}


//  --------------------------------------------------------------------------
//  Get metadata value from certificate; if the metadata value doesn't 
//  exist, returns NULL.

char *
zcert_meta (zcert_t *self, const char *name)
{
    assert (self);
    return (char *) zhash_lookup (self->metadata, name);
}


//  --------------------------------------------------------------------------
//  Get list of metadata fields from certificate. Caller is responsible for
//  destroying list. Caller should not modify the values of list items.

zlist_t *
zcert_meta_keys (zcert_t *self)
{
    assert (self);
    return zhash_keys (self->metadata);
}


//  --------------------------------------------------------------------------
//  Load certificate from file (constructor)

zcert_t *
zcert_load (const char *filename)
{
    assert (filename);

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
        if (public_text && strlen (public_text) == 40) {
            byte public_key [32] = { 0 };
            byte secret_key [32] = { 0 };
#if (ZMQ_VERSION_MAJOR == 4)
            char *secret_text = zconfig_resolve (root, "/curve/secret-key", NULL);
            zmq_z85_decode (public_key, public_text);
            if (secret_text && strlen (secret_text) == 40)
                zmq_z85_decode (secret_key, secret_text);
#endif
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
    return self;
}


//  --------------------------------------------------------------------------
//  Save full certificate (public + secret) to file for persistent storage
//  This creates one public file and one secret file (filename + "_secret").

static void
s_save_metadata_all (zcert_t *self)
{
    zconfig_destroy (&self->config);
    self->config = zconfig_new ("root", NULL);
    zconfig_t *section = zconfig_new ("metadata", self->config);
    
    char *value = (char *) zhash_first (self->metadata);
    while (value) {
        zconfig_t *item = zconfig_new (zhash_cursor (self->metadata), section);
        zconfig_set_value (item, "%s", value);
        value = (char *) zhash_next (self->metadata);
    }
    char *timestr = zclock_timestr ();
    zconfig_set_comment (self->config,
        "   ****  Generated on %s by CZMQ  ****", timestr);
    zstr_free (&timestr);
}


int
zcert_save (zcert_t *self, const char *filename)
{
    assert (self);
    assert (filename);

    //  Save public certificate using specified filename
    zcert_save_public (self, filename);

    //  Now save secret certificate using filename with "_secret" suffix
    char filename_secret [256];
    snprintf (filename_secret, 256, "%s_secret", filename);
    int rc = zcert_save_secret (self, filename_secret);
    return rc;
}

//  --------------------------------------------------------------------------
//  Save public certificate only to file for persistent storage.

int
zcert_save_public (zcert_t *self, const char *filename)
{
    assert (self);
    assert (filename);

    s_save_metadata_all (self);
    zconfig_set_comment (self->config,
        "   ZeroMQ CURVE Public Certificate");
    zconfig_set_comment (self->config,
        "   Exchange securely, or use a secure mechanism to verify the contents");
    zconfig_set_comment (self->config,
        "   of this file after exchange. Store public certificates in your home");
    zconfig_set_comment (self->config,
        "   directory, in the .curve subdirectory.");
    
    zconfig_put (self->config, "/curve/public-key", self->public_txt);
    int rc = zconfig_save (self->config, filename);
    return rc;
}

//  --------------------------------------------------------------------------
//  Save public certificate only to file for persistent storage.

int
zcert_save_secret (zcert_t *self, const char *filename)
{
    assert (self);
    assert (filename);

    s_save_metadata_all (self);
    zconfig_set_comment (self->config,
        "   ZeroMQ CURVE **Secret** Certificate");
    zconfig_set_comment (self->config,
        "   DO NOT PROVIDE THIS FILE TO OTHER USERS nor change its permissions.");
    zconfig_put (self->config, "/curve/public-key", self->public_txt);
    zconfig_put (self->config, "/curve/secret-key", self->secret_txt);
    
    zsys_file_mode_private ();
    int rc = zconfig_save (self->config, filename);
    zsys_file_mode_default ();
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
    void *handle = zsock_resolve (zocket);
#if (ZMQ_VERSION_MAJOR == 4)
    if (zsys_has_curve ()) {
        zsocket_set_curve_secretkey_bin (handle, self->secret_key);
        zsocket_set_curve_publickey_bin (handle, self->public_key);
    }
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
//  Print certificate contents to stdout

void
zcert_print (zcert_t *self)
{
    assert (self);
    zsys_info ("zcert: metadata");

    char *value = (char *) zhash_first (self->metadata);
    while (value) {
        zsys_info ("zcert:     %s = \"%s\"",
                 zhash_cursor (self->metadata), value);
        value = (char *) zhash_next (self->metadata);
    }
    zsys_info ("zcert: curve");
    zsys_info ("zcert:     public-key = \"%s\"", self->public_txt);
    zsys_info ("zcert:     secret-key = \"%s\"", self->secret_txt);
}


//  --------------------------------------------------------------------------
//  DEPRECATED as incompatible with centralized logging
//  Print certificate contents to open stream

void
zcert_fprint (zcert_t *self, FILE *file)
{
    assert (self);
    fprintf (file, "metadata\n");

    char *value = (char *) zhash_first (self->metadata);
    while (value) {
        fprintf (file, "    %s = \"%s\"\n",
                 zhash_cursor (self->metadata), value);
        value = (char *) zhash_next (self->metadata);
    }
    fprintf (file, "curve\n");
    fprintf (file, "    public-key = \"%s\"\n", self->public_txt);
    fprintf (file, "    secret-key = \"%s\"\n", self->secret_txt);
}

//  --------------------------------------------------------------------------
//  Selftest

void
zcert_test (bool verbose)
{
    printf (" * zcert: ");
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
    zlist_t *keys = zcert_meta_keys (cert);
    assert (zlist_size (keys) == 4);
    zlist_destroy (&keys);

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
    int rc = zsys_file_delete (TESTDIR "/mycert.txt_secret");
    assert (rc == 0);
    shadow = zcert_load (TESTDIR "/mycert.txt");
    
    //  32-byte null key encodes as 40 '0' characters
    assert (streq (zcert_secret_txt (shadow),
                   FORTY_ZEROES));
    
    zcert_destroy (&shadow);
    zcert_destroy (&cert);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
    //  @end
    
    printf ("OK\n");
}

