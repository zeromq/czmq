/*  =========================================================================
    zdir - work with file-system directories

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

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
    =========================================================================*/

/*
@header
    The zdir class gives access to the file system index. It will load
    a directory tree (a directory plus all child directories) into a
    zdir structure and then let you navigate that structure. It exists
    mainly to wrap non-portable OS functions to do this.
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zdir_t {
    char *path;             //  Directory name + separator
    zlist_t *files;         //  List of files in directory
    zlist_t *subdirs;       //  List of subdirectories
    time_t modified;        //  Most recent file including subdirs
    off_t cursize;          //  Total file size including subdirs
    size_t count;           //  Total file count including subdirs
    bool trimmed;           //  Load only top level directory
};

#if (defined (WIN32))
static void
s_win32_populate_entry (zdir_t *self, WIN32_FIND_DATAA *entry)
{
    if (entry->cFileName [0] == '.')
        ; //  Skip hidden files
    else
    //  If we have a subdirectory, go load that
    if (entry->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if (!self->trimmed) {
            zdir_t *subdir = zdir_new (entry->cFileName, self->path);
            zlist_append (self->subdirs, subdir);
        }
    }
    else {
        //  Add file entry to directory list
        zfile_t *file = zfile_new (self->path, entry->cFileName);
        assert (file);
        zlist_append (self->files, file);
    }
}

#else
static void
s_posix_populate_entry (zdir_t *self, struct dirent *entry)
{
    //  Skip . and ..
    if (  streq (entry->d_name, ".")
       || streq (entry->d_name, ".."))
        return;

    char fullpath [1024 + 1];
    snprintf (fullpath, 1024, "%s/%s", self->path, entry->d_name);
    struct stat stat_buf;
    if (stat (fullpath, &stat_buf))
        return;

    if (entry->d_name [0] == '.')
        ; //  Skip hidden files
    else
    //  If we have a subdirectory, go load that
    if (S_ISDIR (stat_buf.st_mode)) {
        if (!self->trimmed) {
            zdir_t *subdir = zdir_new (entry->d_name, self->path);
            assert (subdir);
            zlist_append (self->subdirs, subdir);
        }
    }
    else {
        //  Add file entry to directory list
        zfile_t *file = zfile_new (self->path, entry->d_name);
        assert (file);
        zlist_append (self->files, file);
    }
}
#endif


//  --------------------------------------------------------------------------
//  Create a new directory item that loads in the full tree of the specified
//  path, optionally located under some parent path. If parent is "-", then
//  loads only the top-level directory (and does not use parent as a path).

zdir_t *
zdir_new (const char *path, const char *parent)
{
    zdir_t *self = (zdir_t *) zmalloc (sizeof (zdir_t));
    if (!self)
        return NULL;

    if (parent) {
        if (streq (parent, "-")) {
            self->trimmed = true;
            self->path = strdup (path);
            if (!self->path) {
                zdir_destroy (&self);
                return NULL;
            }
        }
        else {
            self->path = (char *) zmalloc (strlen (path) + strlen (parent) + 2);
            if (self->path)
                sprintf (self->path, "%s/%s", parent, path);

            else {
                zdir_destroy (&self);
                return NULL;
            }
        }
    }
    else {
        self->path = strdup (path);
        if (!self->path) {
            zdir_destroy (&self);
            return NULL;
        }
    }

    if (self->path)
        self->files = zlist_new ();
    if (self->files)
        self->subdirs = zlist_new ();
    if (!self->subdirs) {
        zdir_destroy (&self);
        return NULL;
    }

#if (defined (WIN32))
    //  On Windows, replace backslashes by normal slashes
    char *path_clean_ptr = self->path;
    while (*path_clean_ptr) {
        if (*path_clean_ptr == '\\')
            *path_clean_ptr = '/';
        path_clean_ptr++;
    }
    //  Remove any trailing slash
    if (self->path [strlen (self->path) - 1] == '/')
        self->path [strlen (self->path) - 1] = 0;

    //  Win32 wants a wildcard at the end of the path
    char *wildcard = (char *) zmalloc (strlen (self->path) + 3);
    if (!wildcard) {
        zdir_destroy (&self);
        return NULL;
    }
    sprintf (wildcard, "%s/*", self->path);
    WIN32_FIND_DATAA entry;
    HANDLE handle = FindFirstFileA (wildcard, &entry);
    free (wildcard);

    if (handle != INVALID_HANDLE_VALUE) {
        //  We have read an entry, so return those values
        s_win32_populate_entry (self, &entry);
        while (FindNextFileA (handle, &entry))
            s_win32_populate_entry (self, &entry);
        FindClose (handle);
    }
#else
    //  Remove any trailing slash
    if (self->path [strlen (self->path) - 1] == '/')
        self->path [strlen (self->path) - 1] = 0;

    DIR *handle = opendir (self->path);
    if (handle) {
        //  Calculate system-specific size of dirent block
        int dirent_size = offsetof (struct dirent, d_name)
                          + pathconf (self->path, _PC_NAME_MAX) + 1;
        struct dirent *entry = (struct dirent *) zmalloc (dirent_size);
        if (!entry) {
            zdir_destroy (&self);
            return NULL;
        }
        struct dirent *result;

        int rc = readdir_r (handle, entry, &result);
        while (rc == 0 && result != NULL) {
            s_posix_populate_entry (self, entry);
            rc = readdir_r (handle, entry, &result);
        }
        free (entry);
        closedir (handle);
    }
#endif
    else {
        zdir_destroy (&self);
        return NULL;
    }
    //  Update directory signatures
    zdir_t *subdir = (zdir_t *) zlist_first (self->subdirs);
    while (subdir) {
        if (self->modified < subdir->modified)
            self->modified = subdir->modified;
        self->cursize += subdir->cursize;
        self->count += subdir->count;
        subdir = (zdir_t *) zlist_next (self->subdirs);
    }
    zfile_t *file = (zfile_t *) zlist_first (self->files);
    while (file) {
        if (self->modified < zfile_modified (file))
            self->modified = zfile_modified (file);
        self->cursize += zfile_cursize (file);
        self->count += 1;
        file = (zfile_t *) zlist_next (self->files);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a directory item

void
zdir_destroy (zdir_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdir_t *self = *self_p;
        if (self->subdirs)
            while (zlist_size (self->subdirs)) {
                zdir_t *subdir = (zdir_t *) zlist_pop (self->subdirs);
                zdir_destroy (&subdir);
            }
        if (self->files)
            while (zlist_size (self->files)) {
                zfile_t *file = (zfile_t *) zlist_pop (self->files);
                zfile_destroy (&file);
            }
        zlist_destroy (&self->subdirs);
        zlist_destroy (&self->files);
        free (self->path);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Return directory path

char *
zdir_path (zdir_t *self)
{
    return self->path;
}


//  --------------------------------------------------------------------------
//  Return last modification time for directory.

time_t
zdir_modified (zdir_t *self)
{
    assert (self);
    return self->modified;
}


//  --------------------------------------------------------------------------
//  Return total hierarchy size, in bytes of data contained in all files
//  in the directory tree.

off_t
zdir_cursize (zdir_t *self)
{
    assert (self);
    return self->cursize;
}


//  --------------------------------------------------------------------------
//  Return directory count

size_t
zdir_count (zdir_t *self)
{
    assert (self);
    return self->count;
}


//  --------------------------------------------------------------------------
//  Returns a sorted array of zfile objects; returns a single block of memory,
//  that you destroy by calling zdir_flatten_free(). Each entry in the array
//  is a pointer to a zfile_t item already allocated in the zdir tree. The
//  array ends with a null pointer. Do not destroy the original zdir tree
//  until you are done with this array.

static int  s_dir_flatten (zdir_t *self, zfile_t **files, int index);
static bool s_dir_compare (void *item1, void *item2);
static bool s_file_compare (void *item1, void *item2);

zfile_t **
zdir_flatten (zdir_t *self)
{
    int flat_size;
    if (self)
        flat_size = self->count + 1;
    else
        flat_size = 1;      //  Just null terminator

    zfile_t **files = (zfile_t **) zmalloc (sizeof (zfile_t *) * flat_size);
    uint index = 0;
    if (self)
        index = s_dir_flatten (self, files, index);
    return files;
}

//  Flatten one directory, calls itself recursively

static int
s_dir_flatten (zdir_t *self, zfile_t **files, int index)
{
    //  First flatten the normal files
    zlist_sort (self->files, s_file_compare);
    zfile_t *file = (zfile_t *) zlist_first (self->files);
    while (file) {
        files [index++] = file;
        file = (zfile_t *) zlist_next (self->files);
    }
    //  Now flatten subdirectories, recursively
    zlist_sort (self->subdirs, s_dir_compare);
    zdir_t *subdir = (zdir_t *) zlist_first (self->subdirs);
    while (subdir) {
        index = s_dir_flatten (subdir, files, index);
        subdir = (zdir_t *) zlist_next (self->subdirs);
    }
    return index;
}

//  Compare two subdirs, true if they need swapping

static bool
s_dir_compare (void *item1, void *item2)
{
    if (strcmp (zdir_path ((zdir_t *) item1),
                zdir_path ((zdir_t *) item2)) > 0)
        return true;
    else
        return false;
}

//  Compare two files, true if they need swapping. We sort by ascending name.

static bool
s_file_compare (void *item1, void *item2)
{
    if (!item2)
        return false;
    if (!item1)
        return true;

    if (strcmp (zfile_filename ((zfile_t *) item1, NULL),
                zfile_filename ((zfile_t *) item2, NULL)) > 0)
        return true;
    else
        return false;
}


//  --------------------------------------------------------------------------
//  Free a provided string, and nullify the parent pointer. Safe to call on
//  a null pointer.

void
zdir_flatten_free (zfile_t ***files_p)
{
    assert (files_p);
    free (*files_p);
    *files_p = NULL;
}


//  --------------------------------------------------------------------------
//  Remove directory, optionally including all files that it contains, at
//  all levels. If force is false, will only remove the directory if empty.
//  If force is true, will remove all files and all subdirectories.

void
zdir_remove (zdir_t *self, bool force)
{
    //  If forced, remove all subdirectories and files
    if (force) {
        zfile_t *file = (zfile_t *) zlist_pop (self->files);
        while (file) {
            zfile_remove (file);
            zfile_destroy (&file);
            file = (zfile_t *) zlist_pop (self->files);
        }
        zdir_t *subdir = (zdir_t *) zlist_pop (self->subdirs);
        while (subdir) {
            zdir_remove (subdir, force);
            zdir_destroy (&subdir);
            subdir = (zdir_t *) zlist_pop (self->subdirs);
        }
        self->cursize = 0;
        self->count = 0;
    }
    //  Remove if empty
    if (  zlist_size (self->files) == 0
       && zlist_size (self->subdirs) == 0)
        zsys_dir_delete (self->path);
}


//  --------------------------------------------------------------------------
//  Calculate differences between two versions of a directory tree.
//  Returns a list of zdir_patch_t patches. Either older or newer may
//  be null, indicating the directory is empty/absent. If alias is set,
//  generates virtual filename (minus path, plus alias).

zlist_t *
zdir_diff (zdir_t *older, zdir_t *newer, const char *alias)
{
    zlist_t *patches = zlist_new ();
    if (!patches)
        return NULL;

    zfile_t **old_files = zdir_flatten (older);
    zfile_t **new_files = zdir_flatten (newer);

    int old_index = 0;
    int new_index = 0;

    //  Note that both lists are sorted, so detecting differences
    //  is rather trivial
    while (old_files [old_index] || new_files [new_index]) {
        zfile_t *old_file = old_files [old_index];
        zfile_t *new_file = new_files [new_index];

        int cmp;
        if (!old_file)
            cmp = 1;        //  Old file was deleted at end of list
        else
        if (!new_file)
            cmp = -1;       //  New file was added at end of list
        else
            cmp = strcmp (zfile_filename (old_file, NULL), zfile_filename (new_file, NULL));

        if (cmp > 0) {
            //  New file was created
            if (zfile_is_stable (new_file)) {
                int rc = zlist_append (patches, zdir_patch_new (newer->path, new_file, patch_create, alias));
                if (rc != 0) {
                    zlist_destroy (&patches);
                    break;
                }
            }
            old_index--;
        }
        else
        if (cmp < 0) {
            //  Old file was deleted
            if (zfile_is_stable (old_file)) {
                int rc = zlist_append (patches, zdir_patch_new (older->path, old_file, patch_delete, alias));
                if (rc != 0) {
                    zlist_destroy (&patches);
                    break;
                }
            }
            new_index--;
        }
        else
        if (cmp == 0 && zfile_is_stable (new_file)) {
            if (zfile_is_stable (old_file)) {
                //  Old file was modified or replaced
                //  Since we don't check file contents, treat as created
                //  Could better do SHA check on file here
                if (  zfile_modified (new_file) != zfile_modified (old_file)
                   || zfile_cursize (new_file) != zfile_cursize (old_file)) {
                    int rc = zlist_append (patches, zdir_patch_new (newer->path, new_file, patch_create, alias));
                    if (rc != 0) {
                        zlist_destroy (&patches);
                        break;
                    }
                }
            }
            else {
                //  File was created over some period of time
                int rc = zlist_append (patches, zdir_patch_new (newer->path, new_file, patch_create, alias));
                if (rc != 0) {
                    zlist_destroy (&patches);
                    break;
                }
            }
        }
        old_index++;
        new_index++;
    }
    free (old_files);
    free (new_files);

    return patches;
}


//  --------------------------------------------------------------------------
//  Return full contents of directory as a patch list. If alias is set,
//  generates virtual filename (minus path, plus alias).

zlist_t *
zdir_resync (zdir_t *self, const char *alias)
{
    zlist_t *patches = zlist_new ();
    if (!patches)
        return NULL;
    zfile_t **files = zdir_flatten (self);
    uint index;
    for (index = 0;; index++) {
        zfile_t *file = files [index];
        if (!file)
            break;
        int rc = zlist_append (patches, zdir_patch_new (self->path, file, patch_create, alias));
        if (rc != 0) {
            zlist_destroy (&patches);
            break;
        }
    }
    free (files);
    return patches;
}


//  --------------------------------------------------------------------------
//  Load directory cache; returns a hash table containing the SHA-1 digests
//  of every file in the tree. The cache is saved between runs in .cache.
//  The caller must destroy the hash table when done with it.

zhash_t *
zdir_cache (zdir_t *self)
{
    assert (self);

    //  Load any previous cache from disk
    zhash_t *cache = zhash_new ();
    if (!cache)
        return NULL;
    zhash_autofree (cache);
    char *cache_file = (char *) zmalloc (strlen (self->path) + strlen ("/.cache") + 1);
    if (!cache_file) {
        zhash_destroy (&cache);
        return NULL;
    }
    sprintf (cache_file, "%s/.cache", self->path);
    zhash_load (cache, cache_file);

    //  Recalculate digest for any new files
    zfile_t **files = zdir_flatten (self);
    uint index;
    for (index = 0;; index++) {
        zfile_t *file = files [index];
        if (!file)
            break;
        char *filename = zfile_filename (file, self->path);
        if (zhash_lookup (cache, zfile_filename (file, self->path)) == NULL) {
            int rc = zhash_insert (cache, filename, zfile_digest (file));
            if (rc != 0) {
                zhash_destroy (&cache);
                break;
            }
        }
    }
    free (files);

    //  Save cache to disk for future reference
    if (cache)
        zhash_save (cache, cache_file);
    free (cache_file);
    return cache;
}


//  --------------------------------------------------------------------------
//  Print contents of directory to open stream

void
zdir_fprint (zdir_t *self, FILE *stream, int indent)
{
    assert (self);

    zfile_t **files = zdir_flatten (self);
    uint index;
    for (index = 0;; index++) {
        zfile_t *file = files [index];
        if (!file)
            break;
        fprintf (stream, "%s\n", zfile_filename (file, NULL));
    }
    zdir_flatten_free (&files);
}


//  --------------------------------------------------------------------------
//  Print contents of directory to stdout

void
zdir_print (zdir_t *self, int indent)
{
    zdir_fprint (self, stdout, indent);
}


//  --------------------------------------------------------------------------
//  Self test of this class

void
zdir_test (bool verbose)
{
    printf (" * zdir: ");

    //  @selftest
    zdir_t *older = zdir_new (".", NULL);
    assert (older);
    if (verbose) {
        printf ("\n");
        zdir_dump (older, 0);
    }
    zdir_t *newer = zdir_new ("..", NULL);
    assert (newer);
    zlist_t *patches = zdir_diff (older, newer, "/");
    assert (patches);
    while (zlist_size (patches)) {
        zdir_patch_t *patch = (zdir_patch_t *) zlist_pop (patches);
        zdir_patch_destroy (&patch);
    }
    zlist_destroy (&patches);
    zdir_destroy (&older);
    zdir_destroy (&newer);

    zdir_t *nosuch = zdir_new ("does-not-exist", NULL);
    assert (nosuch == NULL);
    //  @end

    printf ("OK\n");
}
