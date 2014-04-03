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
    zlist_t *cert_list;         //  List of loaded certificates
    zhash_t *cert_hash;         //  Hash of loaded certificates
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
    assert (self);
    
    self->cert_list = zlist_new ();
    self->cert_hash = zhash_new ();
    if (location) {
        self->location = strdup (location);
        s_load_certs_from_disk (self);
    }
    return self;
}


//  Get rid of any existing certificates in the store, but leave the
//  store list and hash in place.

static void 
s_empty_store (zcertstore_t *self) 
{
    zcert_t *cert = (zcert_t *) zlist_pop (self->cert_list);
    while (cert) {
        zhash_delete (self->cert_hash, zcert_public_txt (cert));
        zcert_destroy (&cert);
        cert = (zcert_t *) zlist_pop (self->cert_list);
    }
    assert (zlist_size (self->cert_list) == 0);
    assert (zhash_size (self->cert_hash) == 0);
}


//  Load certificates from directory location, if it exists

static void 
s_load_certs_from_disk (zcertstore_t *self)
{
    s_empty_store (self);
    zdir_t *dir = zdir_new (self->location, NULL);
    if (dir) {
        //  Load all certificates including those in subdirectories
        zfile_t **filelist = zdir_flatten (dir);
        uint index;
        for (index = 0;; index++) {
            zfile_t *file = filelist [index];
            if (!file)
                break;      //  End of list
            if (zfile_is_regular (file)) {
                zcert_t *cert = zcert_load (zfile_filename (file, NULL));
                if (cert)
                    zcertstore_insert (self, &cert);
            }
        }
        zdir_flatten_free (&filelist);
        self->modified = zdir_modified (dir);
        self->count = zdir_count (dir);
        self->cursize = zdir_cursize (dir);

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
        s_empty_store (self);
        zlist_destroy (&self->cert_list);
        zhash_destroy (&self->cert_hash);
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
        if (dir
        && (self->modified != zdir_modified (dir)
        ||  self->count    != zdir_count (dir)
        ||  self->cursize  != zdir_cursize (dir))) {
            s_load_certs_from_disk (self);
        }
        zdir_destroy (&dir);
    }
    return (zcert_t *) zhash_lookup (self->cert_hash, public_key);
}


//  --------------------------------------------------------------------------
//  Insert certificate into certificate store in memory. Note that this 
//  does not save the certificate to disk. To do that, use zcert_save()
//  directly on the certificate.

void
zcertstore_insert (zcertstore_t *self, zcert_t **cert_p)
{
    zlist_append (self->cert_list, *cert_p);
    zhash_insert (self->cert_hash, zcert_public_txt (*cert_p), *cert_p);
    *cert_p = NULL;             //  We own this now
}


//  --------------------------------------------------------------------------
//  Print list of certificates in store to open stream

void
zcertstore_fprint (zcertstore_t *self, FILE *file)
{
    if (self->location)
        fprintf (file, "Certificate store at %s:\n", self->location);
    else
        fprintf (file, "Certificate store\n");

    zcert_t *cert = (zcert_t *) zlist_first (self->cert_list);
    while (cert) {
        zcert_fprint (cert, file);
        cert = (zcert_t *) (zcert_t *) zlist_next (self->cert_list);
    }
}


//  --------------------------------------------------------------------------
//  Print list of certificates in store to stdout

void
zcertstore_print (zcertstore_t *self)
{
    zcertstore_fprint (self, stdout);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zcertstore_test (bool verbose)
{
    printf (" * zcertstore: ");
#if (ZMQ_VERSION_MAJOR == 4)
    //  @selftest
    //  Create temporary directory for test files
#   define TESTDIR ".test_zcertstore"
    zsys_dir_create (TESTDIR);
    
    //  Load certificate store from disk; it will be empty
    zcertstore_t *certstore = zcertstore_new (TESTDIR);
    
#   if defined (HAVE_LIBSODIUM)
    //  Create a single new certificate and save to disk
    zcert_t *cert = zcert_new ();
    char *client_key = strdup (zcert_public_txt (cert));
    zcert_set_meta (cert, "name", "John Doe");
    zcert_save (cert, TESTDIR "/mycert.txt");
    zcert_destroy (&cert);
    
    //  Check that certificate store refreshes as expected
    cert = zcertstore_lookup (certstore, client_key);
    assert (cert);
    assert (streq (zcert_meta (cert, "name"), "John Doe"));
    free (client_key);
#   endif
    if (verbose)
        zcertstore_print (certstore);
    zcertstore_destroy (&certstore);

    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
    //  @end
#endif    
    printf ("OK\n");
    return 0;
}
