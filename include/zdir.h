/*  =========================================================================
    zdir - work with file-system directories

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZDIR_H_INCLUDED__
#define __ZDIR_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new directory item that loads in the full tree of the specified
//  path, optionally located under some parent path. If parent is "-", then
//  loads only the top-level directory (and does not use parent as a path).
CZMQ_EXPORT zdir_t *
    zdir_new (const char *path, const char *parent);

//  Destroy a directory tree and all children it contains.
CZMQ_EXPORT void
    zdir_destroy (zdir_t **self_p);

//  Return directory path
CZMQ_EXPORT char *
    zdir_path (zdir_t *self);

//  Return last modification time for directory.
CZMQ_EXPORT time_t
    zdir_modified (zdir_t *self);

//  Return total hierarchy size, in bytes of data contained in all files
//  in the directory tree.
CZMQ_EXPORT off_t
    zdir_cursize (zdir_t *self);

//  Return directory count
CZMQ_EXPORT size_t
    zdir_count (zdir_t *self);
    
//  Returns a sorted array of zfile objects; returns a single block of memory,
//  that you destroy by calling zstr_free(). Each entry in the array is a pointer
//  to a zfile_t item already allocated in the zdir tree. The array ends with
//  a null pointer. Do not destroy the original zdir tree until you are done
//  with this array.
CZMQ_EXPORT zfile_t **
    zdir_flatten (zdir_t *self);

//  Free a provided string, and nullify the parent pointer. Safe to call on
//  a null pointer.
CZMQ_EXPORT void
    zdir_flatten_free (zfile_t ***files_p);

//  Remove directory, optionally including all files that it contains, at
//  all levels. If force is false, will only remove the directory if empty.
//  If force is true, will remove all files and all subdirectories.
CZMQ_EXPORT void
    zdir_remove (zdir_t *self, bool force);

//  Calculate differences between two versions of a directory tree.
//  Returns a list of zdir_patch_t patches. Either older or newer may
//  be null, indicating the directory is empty/absent. If alias is set,
//  generates virtual filename (minus path, plus alias).
CZMQ_EXPORT zlist_t *
    zdir_diff (zdir_t *older, zdir_t *newer, const char *alias);

//  Return full contents of directory as a zdir_patch list.
CZMQ_EXPORT zlist_t *
    zdir_resync (zdir_t *self, const char *alias);

//  Load directory cache; returns a hash table containing the SHA-1 digests
//  of every file in the tree. The cache is saved between runs in .cache.
//  The caller must destroy the hash table when done with it.
CZMQ_EXPORT zhash_t *
    zdir_cache (zdir_t *self);

//  Print contents of directory to open stream
CZMQ_EXPORT void
    zdir_fprint (zdir_t *self, FILE *file, int indent);

//  Print contents of directory to stdout
CZMQ_EXPORT void
    zdir_print (zdir_t *self, int indent);

//  Self test of this class
CZMQ_EXPORT void
    zdir_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

//  Deprecated method aliases
#define zdir_dump(s,i) zdir_print(s,i)

#endif
