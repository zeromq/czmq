/*  =========================================================================
    zdir_patch - work with directory patches
    A patch is a change to the directory (create/delete).

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
    The zdir_patch class works with one patch, which says "create this
    file" or "delete this file" (referring to a zfile item each time).
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class
//  If you modify this beware to also change _dup

struct _zdir_patch_t {
    char *path;                 //  Directory path
    char *vpath;                //  Virtual file path
    zfile_t *file;              //  File we refer to
    zdir_patch_op_t op;         //  Operation
    char *digest;               //  File SHA-1 digest
};


//  --------------------------------------------------------------------------
//  Constructor
//  Create new patch, create virtual path from alias

zdir_patch_t *
zdir_patch_new (const char *path, zfile_t *file,
                zdir_patch_op_t op, const char *alias)
{
    zdir_patch_t *self = (zdir_patch_t *) zmalloc (sizeof (zdir_patch_t));
    if (!self)
        return NULL;
    self->path = strdup (path);
    if (self->path)
        self->file = zfile_dup (file);
    if (!self->file) {
        zdir_patch_destroy (&self);
        return NULL;
    }

    self->op = op;

    //  Calculate virtual path for patch (remove path, prefix alias)
    char *filename = zfile_filename (file, path);
    if (!filename) {
        zdir_patch_destroy (&self);
        return NULL;
    }
    assert (*filename != '/');
    self->vpath = (char *) zmalloc (strlen (alias) + strlen (filename) + 2);
    if (!self->vpath) {
        zdir_patch_destroy (&self);
        return NULL;
    }
    if (alias [strlen (alias) - 1] == '/')
        sprintf (self->vpath, "%s%s", alias, filename);
    else
        sprintf (self->vpath, "%s/%s", alias, filename);
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a patch

void
zdir_patch_destroy (zdir_patch_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdir_patch_t *self = *self_p;
        free (self->path);
        free (self->vpath);
        free (self->digest);
        zfile_destroy (&self->file);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Create copy of a patch. If the patch is null, or memory was exhausted,
//  returns null.

zdir_patch_t *
zdir_patch_dup (zdir_patch_t *self)
{
    if (self) {
        zdir_patch_t *copy = (zdir_patch_t *) zmalloc (sizeof (zdir_patch_t));
        if (copy) {
            copy->op = self->op;
            copy->path = strdup (self->path);
            if (copy->path)
                copy->file = zfile_dup (self->file);
            if (copy->file)
                copy->vpath = strdup (self->vpath);
            if (copy->vpath)
                //  Don't recalculate hash when we duplicate patch
                copy->digest = self->digest ? strdup (self->digest) : NULL;

            if (copy->digest == NULL)
                zdir_patch_destroy (&copy);
        }
        return copy;
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return patch file directory path

char *
zdir_patch_path (zdir_patch_t *self)
{
    assert (self);
    return self->path;
}


//  --------------------------------------------------------------------------
//  Return patch file item

zfile_t *
zdir_patch_file (zdir_patch_t *self)
{
    assert (self);
    return self->file;
}


//  --------------------------------------------------------------------------
//  Return operation

zdir_patch_op_t
zdir_patch_op (zdir_patch_t *self)
{
    assert (self);
    return self->op;
}


//  --------------------------------------------------------------------------
//  Return patch virtual file path

char *
zdir_patch_vpath (zdir_patch_t *self)
{
    assert (self);
    return self->vpath;
}


//  --------------------------------------------------------------------------
//  Calculate hash digest for file (create only)

void
zdir_patch_digest_set (zdir_patch_t *self)
{
    if (  self->op == patch_create
       && self->digest == NULL) {
        self->digest = strdup (zfile_digest (self->file));
        assert (self->digest);
    }

}


//  --------------------------------------------------------------------------
//  Return hash digest for patch file (create only)

char *
zdir_patch_digest (zdir_patch_t *self)
{
    assert (self);
    return self->digest;
}


//  --------------------------------------------------------------------------
//  Self test of this class

void
zdir_patch_test (bool verbose)
{
    printf (" * zdir_patch: ");

    //  @selftest
    zfile_t *file = zfile_new (".", "bilbo");
    assert (file);
    zdir_patch_t *patch = zdir_patch_new (".", file, patch_create, "/");
    assert (patch);
    zfile_destroy (&file);

    file = zdir_patch_file (patch);
    assert (file);
    assert (streq (zfile_filename (file, "."), "bilbo"));
    assert (streq (zdir_patch_vpath (patch), "/bilbo"));
    zdir_patch_destroy (&patch);
    //  @end

    printf ("OK\n");
}
