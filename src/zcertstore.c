/*  =========================================================================
    zcertstore - work with CURVE security certificate stores

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
    To authenticate new clients using the ZeroMQ CURVE security mechanism,
    we have to check that the client's public key matches a key we know and
    accept. There are numerous ways to store accepted client public keys.
    The mechanism CZMQ implements is "certificates" (plain text files) held
    in a "certificate store" (a disk directory). This class works with such
    certificate stores, and lets you easily load them from disk, and check
    if a given client public key is known or not. The zcert class does the
    work of managing a single certificate.
@discuss
    The certificate store can be memory-only, in which case you can load it
    yourself by inserting certificate objects one by one, or it can be loaded
    from disk, in which case you can add, modify, or remove certificates on
    disk at any time, and the store will detect such changes and refresh
    itself automatically. In most applications you won't use this class
    directly but through the zauth class, which provides a high-level API for
    authentication (and manages certificate stores for you). To actually
    create certificates on disk, use the zcert class in code, or the
    tools/makecert.c command line tool, or any text editor. The format of a
    certificate file is defined in the zcert man page.
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zcertstore_t {
    char *location;             //  Directory location
    //  This isn't sufficient; we should check the hash of all files
    //  or else use a trigger like inotify on Linux.
    time_t modified;            //  Modified time of directory
    size_t count;               //  Number of certificates
    size_t cursize;             //  Total size of certificates
    zhashx_t *certs;            //  Loaded certificates
};


//  --------------------------------------------------------------------------
//  Constructor
//
//  Create a new certificate store from a disk directory, loading and
//  indexing all certificates in that location. The directory itself may be
//  absent, and created later, or modified at any time. The certificate store
//  is automatically refreshed on any zcertstore_lookup() call. If the
//  location is specified as NULL, creates a pure-memory store, which you
//  can work with by inserting certificates at runtime.

static void s_load_certs_from_disk (zcertstore_t *self);

zcertstore_t *
zcertstore_new (const char *location)
{
    zcertstore_t *self = (zcertstore_t *) zmalloc (sizeof (zcertstore_t));
    if (!self)
        return NULL;

    self->certs = zhashx_new ();
    if (self->certs) {
        zhashx_set_destructor (self->certs, (czmq_destructor *) zcert_destroy);
        if (location) {
            self->location = strdup (location);
            if (!self->location) {
                zcertstore_destroy (&self);
                return NULL;
            }
            s_load_certs_from_disk (self);
        }
    }
    else
        zcertstore_destroy (&self);
    return self;
}


//  Load certificates from directory location, if it exists

static void
s_load_certs_from_disk (zcertstore_t *self)
{
    zhashx_purge (self->certs);
    zdir_t *dir = zdir_new (self->location, NULL);
    if (dir) {
        //  Load all certificates including those in subdirectories
        zfile_t **filelist = zdir_flatten (dir);
        assert (filelist);
        zrex_t *rex = zrex_new ("_secret$");
        assert (rex);

        uint index;
        for (index = 0;; index++) {
            zfile_t *file = filelist [index];
            if (!file)
                break;      //  End of list
            if (  zfile_is_regular (file)
               && !zrex_matches (rex, zfile_filename (file, NULL))) {
                zcert_t *cert = zcert_load (zfile_filename (file, NULL));
                if (cert)
                    zcertstore_insert (self, &cert);
            }
        }
        zdir_flatten_free (&filelist);
        self->modified = zdir_modified (dir);
        self->count = zdir_count (dir);
        self->cursize = zdir_cursize (dir);

        zrex_destroy (&rex);
        zdir_destroy (&dir);
    }
}


//  --------------------------------------------------------------------------
//  Destructor
//
//  Destroy a certificate store object in memory. Does not affect anything
//  stored on disk.

void
zcertstore_destroy (zcertstore_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zcertstore_t *self = *self_p;
        zhashx_destroy (&self->certs);
        free (self->location);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Look up certificate by public key, returns zcert_t object if found,
//  else returns NULL. The public key is provided in Z85 text format.

zcert_t *
zcertstore_lookup (zcertstore_t *self, const char *public_key)
{
    //  If directory has changed, reload all certificates
    if (self->location) {
        zdir_t *dir = zdir_new (self->location, NULL);
        if (  dir
           && (  self->modified != zdir_modified (dir)
              || self->count != zdir_count (dir)
              || self->cursize != zdir_cursize (dir))) {
            s_load_certs_from_disk (self);
        }
        zdir_destroy (&dir);
    }
    return (zcert_t *) zhashx_lookup (self->certs, public_key);
}


//  --------------------------------------------------------------------------
//  Insert certificate into certificate store in memory. Note that this
//  does not save the certificate to disk. To do that, use zcert_save()
//  directly on the certificate.

void
zcertstore_insert (zcertstore_t *self, zcert_t **cert_p)
{
    int rc = zhashx_insert (self->certs, zcert_public_txt (*cert_p), *cert_p);
    assert (rc == 0);
    *cert_p = NULL;             //  We own this now
}


//  --------------------------------------------------------------------------
//  Print list of certificates in store to stdout

void
zcertstore_print (zcertstore_t *self)
{
    if (self->location)
        zsys_info ("zcertstore: certificates at location=%s:", self->location);
    else
        zsys_info ("zcertstore: certificates in memory");

    zcert_t *cert = (zcert_t *) zhashx_first (self->certs);
    while (cert) {
        zcert_print (cert);
        cert = (zcert_t *) zhashx_next (self->certs);
    }
}


//  --------------------------------------------------------------------------
//  DEPRECATED as incompatible with centralized logging
//  Print list of certificates in store to open stream

void
zcertstore_fprint (zcertstore_t *self, FILE *file)
{
    if (self->location)
        fprintf (file, "Certificate store at %s:\n", self->location);
    else
        fprintf (file, "Certificate store\n");

    zcert_t *cert = (zcert_t *) zhashx_first (self->certs);
    while (cert) {
        zcert_fprint (cert, file);
        cert = (zcert_t *) zhashx_next (self->certs);
    }
}


//  --------------------------------------------------------------------------
//  Selftest

void
zcertstore_test (bool verbose)
{
    printf (" * zcertstore: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    //  Create temporary directory for test files
#   define TESTDIR ".test_zcertstore"
    zsys_dir_create (TESTDIR);

    //  Load certificate store from disk; it will be empty
    zcertstore_t *certstore = zcertstore_new (TESTDIR);
    assert (certstore);

    //  Create a single new certificate and save to disk
    zcert_t *cert = zcert_new ();
    assert (cert);
    char *client_key = strdup (zcert_public_txt (cert));
    assert (client_key);
    zcert_set_meta (cert, "name", "John Doe");
    zcert_save (cert, TESTDIR "/mycert.txt");
    zcert_destroy (&cert);

    //  Check that certificate store refreshes as expected
    cert = zcertstore_lookup (certstore, client_key);
    assert (cert);
    assert (streq (zcert_meta (cert, "name"), "John Doe"));
    free (client_key);

    if (verbose)
        zcertstore_print (certstore);
    zcertstore_destroy (&certstore);

    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
    //  @end
    printf ("OK\n");
}
