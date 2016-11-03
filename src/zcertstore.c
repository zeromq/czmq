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
    tools/zmakecert.c command line tool, or any text editor. The format of a
    certificate file is defined in the zcert man page.
@end
*/

#include "czmq_classes.h"


//  --------------------------------------------------------------------------
//  Disk loader

typedef struct _disk_loader_state {
    char *location;  //  Directory location
    //  This isn't sufficient; we should check the hash of all files
    //  or else use a trigger like inotify on Linux.
    time_t modified; //  Modified time of directory
    size_t count;    //  Number of certificates
    size_t cursize;  //  Total size of certificates
} disk_loader_state;


//  --------------------------------------------------------------------------
//  Zcertstore

struct _zcertstore_t {
    zcertstore_loader *loader;         //  Certificate loader fn
    zcertstore_destructor *destructor; //  Certificate loader state destructor
    void *state;                       //  Certificate loader state data
    zhashx_t *certs;                   //  Loaded certificates
};


//  --------------------------------------------------------------------------
//  Disk loader

static void s_disk_loader (zcertstore_t *self);

static void
s_disk_loader (zcertstore_t *certstore)
{
    disk_loader_state *state = (disk_loader_state *)certstore->state;
    zdir_t *dir = zdir_new (state->location, NULL);
    if (dir
    && (state->modified != zdir_modified (dir)
    ||  state->count != zdir_count (dir)
    ||  state->cursize != (size_t) zdir_cursize (dir)))
    {
        zhashx_purge (certstore->certs);

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
            if (zfile_is_regular (file)
            && !zrex_matches (rex, zfile_filename (file, NULL))) {
                zcert_t *cert = zcert_load (zfile_filename (file, NULL));
                if (cert)
                    zcertstore_insert (certstore, &cert);
            }
        }
        zdir_flatten_free (&filelist);
        state->modified = zdir_modified (dir);
        state->count = zdir_count (dir);
        state->cursize = zdir_cursize (dir);

        zrex_destroy (&rex);
    }
    zdir_destroy (&dir);
}


//  --------------------------------------------------------------------------
//  Disk loader destructor

static void
s_disk_loader_state_destroy (void **self_p)
{
    assert (self_p);
    if (*self_p) {
        disk_loader_state *self = (disk_loader_state *)*self_p;
        free (self->location);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Constructor
//
//  Create a new certificate store, loading and indexing all certificates from
//  the loader callback. The default loader is the directory loader, which
//  gathers certificates from a local system folder. Specifying the location
//  argument will setup the directory loader for this zcertstore instance. The
//  directory itself may be absent, and created later, or modified at any time.
//  The certificate store is automatically refreshed on any zcertstore_lookup()
//  call. If the location is specified as NULL, creates a pure-memory store,
//  which you can work with by inserting certificates at runtime.

zcertstore_t *
zcertstore_new (const char *location)
{
    zcertstore_t *self = (zcertstore_t *) zmalloc (sizeof (zcertstore_t));
    assert (self);

    self->certs = zhashx_new ();
    assert (self->certs);
    zhashx_set_destructor (self->certs, (czmq_destructor *) zcert_destroy);

    if (location) {
        disk_loader_state *state = (disk_loader_state *) zmalloc (sizeof (disk_loader_state));
        state->location = strdup (location);
        assert (state->location);
        state->modified = 0;
        state->count = 0;
        state->cursize = 0;
        zcertstore_set_loader (self, s_disk_loader, s_disk_loader_state_destroy, (void *)state);
    }
    return self;
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

        if (self->destructor)
            self->destructor (&self->state);

        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Override the default disk loader with a custom loader fn.

void
zcertstore_set_loader (zcertstore_t *self, zcertstore_loader loader, zcertstore_destructor destructor, void *state)
{
    if (self->destructor && self->state)
        self->destructor (&self->state);
    self->loader = loader;
    self->destructor = destructor;
    self->state = state;
    self->loader (self);
}


//  --------------------------------------------------------------------------
//  Look up certificate by public key, returns zcert_t object if found,
//  else returns NULL. The public key is provided in Z85 text format.

zcert_t *
zcertstore_lookup (zcertstore_t *self, const char *public_key)
{
    if (self->loader)
        self->loader (self);

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
//  Empty certificate hashtable
//  This wrapper exists to be friendly to bindings, which don't usually have
//  access to struct internals (e.g. self->certs).

void
zcertstore_empty (zcertstore_t *self)
{
    zhashx_purge (self->certs);
}


//  --------------------------------------------------------------------------
//  Print list of certificates in store to stdout

void
zcertstore_print (zcertstore_t *self)
{
    if (self->loader)
        zsys_info ("zcertstore: certificates in store");
    else
        zsys_info ("zcertstore: certificates in memory");

    zcert_t *cert = (zcert_t *) zhashx_first (self->certs);
    while (cert) {
        zcert_print (cert);
        cert = (zcert_t *) zhashx_next (self->certs);
    }
}


//  --------------------------------------------------------------------------
//  Selftest

// Trivial example state for testing
typedef struct _test_loader_state {
    int index;
} test_loader_state;

static void
s_test_loader (zcertstore_t *certstore)
{
    zcertstore_empty (certstore);

    byte public_key [32] = { 31, 133, 154, 36, 47, 67, 155, 5, 63, 1,
                             155, 230, 78, 191, 156, 199, 94, 125, 157, 168,
                             109, 69, 19, 241, 44, 29, 154, 216, 59, 219,
                             155, 185 };
    byte secret_key [32] = { 31, 133, 154, 36, 47, 67, 155, 5, 63, 1,
                             155, 230, 78, 191, 156, 199, 94, 125, 157, 168,
                             109, 69, 19, 241, 44, 29, 154, 216, 59, 219,
                             155, 185 };

    zcert_t *cert = zcert_new_from (public_key, secret_key);
    zcertstore_insert (certstore, &cert);

    test_loader_state *state = (test_loader_state *)certstore->state;
    state->index++;
}

static void
s_test_destructor (void **self_p)
{
    assert (self_p);
    if (*self_p) {
        test_loader_state *self = (test_loader_state *)*self_p;
        free (self);
        *self_p = NULL;
    }
}

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

    //  Test custom loader
    test_loader_state *state = (test_loader_state *) zmalloc (sizeof (test_loader_state));
    state->index = 0;
    zcertstore_set_loader (certstore, s_test_loader, s_test_destructor, (void *)state);
#if (ZMQ_VERSION_MAJOR >= 4)
    cert = zcertstore_lookup (certstore, client_key);
    assert (cert == NULL);
    cert = zcertstore_lookup (certstore, "abcdefghijklmnopqrstuvwxyzabcdefghijklmn");
    assert (cert);
#endif

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
