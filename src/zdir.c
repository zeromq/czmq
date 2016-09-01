/*  =========================================================================
    zdir - work with file-system directories

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

#include "czmq_classes.h"

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
    if (streq (entry->d_name, ".")
    ||  streq (entry->d_name, ".."))
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

#ifndef WIN32
static pthread_mutex_t s_readdir_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

//  --------------------------------------------------------------------------
//  Create a new directory item that loads in the full tree of the specified
//  path, optionally located under some parent path. If parent is "-", then
//  loads only the top-level directory, and does not use parent as a path.

zdir_t *
zdir_new (const char *path, const char *parent)
{
    zdir_t *self = (zdir_t *) zmalloc (sizeof (zdir_t));
    assert (self);

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
        // readdir_r is deprecated in glibc 2.24, but readdir is still not
        // guaranteed to be thread safe if the same directory is accessed
        // by different threads at the same time. Unfortunately given it was
        // not a constraint before we cannot change it now as it would be an
        // API breakage. Use a global lock when scanning the directory to
        // work around it.
        pthread_mutex_lock (&s_readdir_mutex);
        struct dirent *entry = readdir (handle);
        pthread_mutex_unlock (&s_readdir_mutex);
        while (entry != NULL) {
            // Beware of recursion. Lock only around readdir calls.
            s_posix_populate_entry (self, entry);
            pthread_mutex_lock (&s_readdir_mutex);
            entry = readdir (handle);
            pthread_mutex_unlock (&s_readdir_mutex);
        }
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

const char *
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
static int s_dir_compare (void *item1, void *item2);
static int s_file_compare (void *item1, void *item2);

zfile_t **
zdir_flatten (zdir_t *self)
{
    size_t flat_size;
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

static int
s_dir_compare (void *item1, void *item2)
{
    assert (item1);
    assert (item2);

    return strcmp (zdir_path ((zdir_t *) item1),
                   zdir_path ((zdir_t *) item2));
}

//  Compare two files, true if they need swapping. We sort by ascending name.

static int
s_file_compare (void *item1, void *item2)
{
    assert (item1);
    assert (item2);

    return strcmp (zfile_filename ((zfile_t *) item1, NULL),
                   zfile_filename ((zfile_t *) item2, NULL));
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
//  Returns a sorted list of zfile objects; Each entry in the list is a pointer
//  to a zfile_t item already allocated in the zdir tree. Do not destroy the
//  original zdir tree until you are done with this list.

zlist_t *
zdir_list (zdir_t *self)
{
    zfile_t **files = zdir_flatten (self);
    zlist_t *list = zlist_new ();
    size_t index;

    if (files)
    {
        for (index = 0 ; files[index] ; index++)
        {
            zlist_append (list, files[index]);
        }
    }

    zdir_flatten_free (&files);
    return list;
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
    if (zlist_size (self->files) == 0
    &&  zlist_size (self->subdirs) == 0)
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
                if (zfile_modified (new_file) != zfile_modified (old_file)
                ||  zfile_cursize (new_file) != zfile_cursize (old_file)) {
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
        if (zlist_append (patches, zdir_patch_new (
            self->path, file, patch_create, alias))) {
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
        const char *filename = zfile_filename (file, self->path);
        if (zhash_lookup (cache, zfile_filename (file, self->path)) == NULL) {
            int rc = zhash_insert (cache, filename, (void *) zfile_digest (file));
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
//  Watch a directory for changes

typedef struct _zdir_watch_t {
    zsock_t *pipe;            // actor command channel
    zloop_t *loop;            // event reactor
    int read_timer_id;        // the zloop timer id to signal directory updating

    bool verbose;             // extra logging to be printed
    zhash_t *subs;            // path -> zdir_watch_sub_t instance hashtable for each active subscription
} zdir_watch_t;

typedef struct _zdir_watch_sub_t {
    zdir_t *dir;
} zdir_watch_sub_t;

static int
s_on_read_timer (zloop_t *loop, int timer_id, void *arg)
{
    zdir_watch_t *watch = (zdir_watch_t *) arg;

    void *data;
    for (data = zhash_first (watch->subs); data != NULL; data = zhash_next (watch->subs))
    {
        zdir_watch_sub_t *sub = (zdir_watch_sub_t *) data;

        zdir_t *new_dir = zdir_new (zdir_path (sub->dir), NULL);
        if (!new_dir) {
            if (watch->verbose)
                zsys_error ("zdir_watch: Unable to create new zdir for path %s", zdir_path (sub->dir));
            continue;
        }

        // Determine if anything has changed.
        zlist_t *diff = zdir_diff (sub->dir, new_dir, "");

        // Do memory management before error handling...
        zdir_destroy (&sub->dir);
        sub->dir = new_dir;

        if (!diff) {
            if (watch->verbose)
                zsys_error ("zdir_watch: Unable to create diff for path %s", zdir_path (sub->dir));
            continue;
        }

        if (zlist_size (diff) > 0) {
            if (watch->verbose) {
                zdir_patch_t *patch = (zdir_patch_t *) zlist_first (diff);

                zsys_info ("zdir_watch: Found %d changes in %s:", zlist_size (diff), zdir_path (sub->dir));
                while (patch)
                {
                    zsys_info ("zdir_watch:   %s %s", zfile_filename (zdir_patch_file (patch), NULL), zdir_patch_op (patch) == ZDIR_PATCH_CREATE? "created": "deleted");
                    patch = (zdir_patch_t *) zlist_next (diff);
                }
            }

            if (zsock_send (watch->pipe, "sp", zdir_path (sub->dir), diff) != 0) {
                if (watch->verbose)
                    zsys_error ("zdir_watch: Unable to send patch list for path %s", zdir_path (sub->dir));
                zlist_destroy (&diff);
            }

            // Successfully sent `diff` list - now owned by receiver
        }
        else {
            zlist_destroy (&diff);
        }
    }

    return 0;
}


static void
s_zdir_watch_destroy (zdir_watch_t **watch_p)
{
    assert (watch_p);
    if (*watch_p) {
        zdir_watch_t *watch = *watch_p;

        zloop_destroy (&watch->loop);
        zhash_destroy (&watch->subs);

        free (watch);
        *watch_p = NULL;
    }
}

static void
s_sub_free (void *data)
{
    zdir_watch_sub_t *sub = (zdir_watch_sub_t *) data;
    zdir_destroy (&sub->dir);

    free (sub);
}

static void
s_zdir_watch_subscribe (zdir_watch_t *watch, const char *path)
{
    if (watch->verbose)
        zsys_info ("zdir_watch: Subscribing to directory path: %s", path);

    zdir_watch_sub_t *sub = (zdir_watch_sub_t *) zmalloc (sizeof (zdir_watch_sub_t));
    sub->dir = zdir_new (path, NULL);
    if (!sub->dir) {
        if (watch->verbose)
            zsys_error ("zdir_watch: Unable to create zdir for path: %s", path);
        zsock_signal (watch->pipe, 1);
        return;
    }

    int rc = zhash_insert (watch->subs, path, sub);
    if (rc) {
        if (watch->verbose)
            zsys_error ("zdir_watch: Unable to insert path '%s' into subscription list", path);
        zsock_signal (watch->pipe, 1);
        return;
    }

    void *item = zhash_freefn (watch->subs, path, s_sub_free);
    if (item != sub) {
        if (watch->verbose)
            zsys_error ("zdir_watch: Unable to set free fn for path %s", path);
        zsock_signal (watch->pipe, 1);
        return;
    }

    if (watch->verbose)
        zsys_info ("zdir_watch: Successfully subscribed to %s", path);
    zsock_signal (watch->pipe, 0);
}

static void
s_zdir_watch_unsubscribe (zdir_watch_t *watch, const char *path)
{
    if (watch->verbose)
        zsys_info ("zdir_watch: Unsubscribing from directory path: %s", path);

    zhash_delete (watch->subs, path);
    if (watch->verbose)
        zsys_info ("zdir_watch: Successfully unsubscribed from %s", path);
    zsock_signal (watch->pipe, 0);
}

static int
s_zdir_watch_timeout (zdir_watch_t *watch, int timeout)
{
    if (watch->verbose)
        zsys_info ("zdir_watch: Setting directory poll timeout to %d", timeout);

    if (watch->read_timer_id != -1) {
        zloop_timer_end (watch->loop, watch->read_timer_id);
        watch->read_timer_id = -1;
    }

    watch->read_timer_id = zloop_timer (watch->loop, timeout, 0, s_on_read_timer, watch);

    if (watch->verbose)
        zsys_info ("zdir_watch: Successfully set directory poll timeout to %d", timeout);
    return 0;
}

static zdir_watch_t *
s_zdir_watch_new (zsock_t *pipe)
{
    zdir_watch_t *watch = (zdir_watch_t *) zmalloc (sizeof (zdir_watch_t));
    if (!watch)
        return NULL;
    watch->pipe = pipe;
    watch->read_timer_id = -1;
    watch->verbose = false;
    return watch;
}

static int
s_on_command (zloop_t *loop, zsock_t *reader, void *arg)
{
    zdir_watch_t *watch = (zdir_watch_t *) arg;

    zmsg_t *msg = zmsg_recv (watch->pipe);
    assert (msg);
    char *command = zmsg_popstr (msg);
    assert (command);

    if (watch->verbose)
        zsys_info ("zdir_watch: Command received: %s", command);

    if (streq (command, "$TERM")) {
        zstr_free (&command);
        zmsg_destroy (&msg);
        return -1;
    }
    else
    if (streq (command, "VERBOSE")) {
        watch->verbose = true;
        zsock_signal (watch->pipe, 0);
    }
    else
    if (streq (command, "SUBSCRIBE")) {
        char *path = zmsg_popstr (msg);
        if (path) {
            s_zdir_watch_subscribe (watch, path);
            free (path);
        }
        else {
            if (watch->verbose)
                zsys_error ("zdir_watch: Unable to extract path from SUBSCRIBE message");
            zsock_signal (watch->pipe, 1);
        }
    }
    else
    if (streq (command, "UNSUBSCRIBE")) {
        char *path = zmsg_popstr (msg);
        if (path) {
            assert (path);
            s_zdir_watch_unsubscribe (watch, path);
            free (path);
        }
        else {
            if (watch->verbose)
                zsys_error ("zdir_watch: Unable to extract path from UNSUBSCRIBE message");
            zsock_signal (watch->pipe, 1);
        }
    }
    else
    if (streq (command, "TIMEOUT")) {
        char *timeout_string = zmsg_popstr (msg);
        if (timeout_string) {
            int timeout = atoi (timeout_string);
            zsock_signal (watch->pipe, s_zdir_watch_timeout (watch, timeout));
            free (timeout_string);
        }
        else {
            if (watch->verbose)
                zsys_error ("zdir_watch: Unable to extract time from TIMEOUT message");
            zsock_signal (watch->pipe, 1);
        }
    }
    else {
        if (watch->verbose)
            zsys_warning ("zdir_watch: Unknown command '%s'", command);
        zsock_signal (watch->pipe, 1);
    }

    free (command);
    zmsg_destroy (&msg);
    return 0;
}

//  --------------------------------------------------------------------------
//  Create a new zdir_watch actor instance

void
zdir_watch (zsock_t *pipe, void *unused)
{
    zdir_watch_t *watch = s_zdir_watch_new (pipe);
    assert (watch);

    watch->loop = zloop_new ();
    assert (watch->loop);

    watch->subs = zhash_new ();
    assert (watch->subs);

    zloop_reader (watch->loop, pipe, s_on_command, watch);
    zloop_reader_set_tolerant (watch->loop, pipe); // command pipe needs to be tolerant, otherwise we'd have a hard time shutting down

    s_zdir_watch_timeout (watch, 250); // default poll time of 250ms

    //  Signal initialization
    zsock_signal (pipe, 0);

    // Dispatch above handlers
    zloop_start (watch->loop);
    if (watch->verbose)
        zsys_info ("zdir_watch: Complete");

    // signal destruction
    zsock_signal (watch->pipe, 0);

    // Done - cleanup and exit
    s_zdir_watch_destroy (&watch);
}


//  --------------------------------------------------------------------------
//  Self test of this class

void
zdir_test (bool verbose)
{
    printf (" * zdir: ");

    //  @selftest
    // need to create a file in the test directory we're watching
    // in order to ensure the directory exists
    zfile_t *initfile = zfile_new ("./zdir-test-dir", "initial_file");
    assert (initfile);
    zfile_output (initfile);
    fprintf (zfile_handle (initfile), "initial file\n");
    zfile_close (initfile);
    zfile_destroy (&initfile);

    zdir_t *older = zdir_new ("zdir-test-dir", NULL);
    assert (older);
    if (verbose) {
        printf ("\n");
        zdir_dump (older, 0);
    }
    zdir_t *newer = zdir_new (".", NULL);
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

    // zdir_watch test:
    zactor_t *watch = zactor_new (zdir_watch, NULL);
    assert (watch);

    if (verbose) {
        zsock_send (watch, "s", "VERBOSE");
        assert (zsock_wait (watch) == 0);
    }

    zclock_sleep (1001); // wait for initial file to become 'stable'

    zsock_send (watch, "si", "TIMEOUT", 100);
    assert (zsock_wait (watch) == 0);

    zsock_send (watch, "ss", "SUBSCRIBE", "zdir-test-dir");
    assert (zsock_wait (watch) == 0);

    zsock_send (watch, "ss", "UNSUBSCRIBE", "zdir-test-dir");
    assert (zsock_wait (watch) == 0);

    zsock_send (watch, "ss", "SUBSCRIBE", "zdir-test-dir");
    assert (zsock_wait (watch) == 0);

    zfile_t *newfile = zfile_new ("zdir-test-dir", "test_abc");
    zfile_output (newfile);
    fprintf (zfile_handle (newfile), "test file\n");
    zfile_close (newfile);

    zpoller_t *watch_poll = zpoller_new (watch, NULL);

    // poll for a certain timeout before giving up and failing the test.
    assert (zpoller_wait (watch_poll, 1001) == watch);

    // wait for notification of the file being added
    char *path;
    int rc = zsock_recv (watch, "sp", &path, &patches);
    assert (rc == 0);

    assert (streq (path, "zdir-test-dir"));
    free (path);

    assert (zlist_size (patches) == 1);

    zdir_patch_t *patch = (zdir_patch_t *) zlist_pop (patches);
    assert (streq (zdir_patch_path (patch), "zdir-test-dir"));

    zfile_t *patch_file = zdir_patch_file (patch);
    assert (streq (zfile_filename (patch_file, ""), "zdir-test-dir/test_abc"));

    zdir_patch_destroy (&patch);
    zlist_destroy (&patches);

    // remove the file
    zfile_remove (newfile);
    zfile_destroy (&newfile);

    // poll for a certain timeout before giving up and failing the test.
    assert (zpoller_wait (watch_poll, 1001) == watch);

    // wait for notification of the file being removed
    rc = zsock_recv (watch, "sp", &path, &patches);
    assert (rc == 0);

    assert (streq (path, "zdir-test-dir"));
    free (path);

    assert (zlist_size (patches) == 1);

    patch = (zdir_patch_t *) zlist_pop (patches);
    assert (streq (zdir_patch_path (patch), "zdir-test-dir"));

    patch_file = zdir_patch_file (patch);
    assert (streq (zfile_filename (patch_file, ""), "zdir-test-dir/test_abc"));

    zdir_patch_destroy (&patch);
    zlist_destroy (&patches);

    zpoller_destroy (&watch_poll);
    zactor_destroy (&watch);

    // clean up by removing the test directory.
    zdir_t *testdir = zdir_new ("zdir-test-dir", NULL);
    zdir_remove (testdir, true);
    zdir_destroy (&testdir);
    //  @end

    printf ("OK\n");
}
