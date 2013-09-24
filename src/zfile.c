/*  =========================================================================
    zfile - provides methods to work with files in a portable fashion.

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
    The zfile class provides methods to work with files. This class is a new
    API, deprecating the old zfile class (which still exists but is implemented
    in zsys now).
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zfile_t {
    //  Properties for all file objects
    char *fullname;         //  File name with path
    bool exists;            //  true if file exists
    bool stable;            //  true if file is stable
    bool eof;               //  true if at end of file
    FILE *handle;           //  Read/write handle
    //  Properties from files that exist on file system
    time_t modified;        //  Modification time
    off_t  cursize;         //  Size of the file
    mode_t mode;            //  POSIX permission bits
};


//  Prototypes of local functions
static void s_restat (zfile_t *self);

//  --------------------------------------------------------------------------
//  Constructor
//  If file exists, populates properties

zfile_t *
zfile_new (const char *path, const char *name)
{
    zfile_t *self = (zfile_t *) zmalloc (sizeof (zfile_t));

    //  Format full path to file
    self->fullname = (char*)malloc (strlen (path) + strlen (name) + 2);
    sprintf (self->fullname, "%s/%s", path, name);
    s_restat (self);
    return self;
}

//  Refreshes file properties from file system

static void
s_restat (zfile_t *self)
{
    assert (self);
    struct stat stat_buf;
    if (stat (self->fullname, &stat_buf) == 0) {
        self->cursize = stat_buf.st_size;
        self->modified = stat_buf.st_mtime;
        self->mode = zsys_file_mode (self->fullname);
        self->stable = zsys_file_stable (self->fullname);
    }
    else {
        self->cursize = 0;
        self->modified = 0;
        self->mode = 0;
        self->stable = false;
    }
}


//  --------------------------------------------------------------------------
//  Destroy a file item

void
zfile_destroy (zfile_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zfile_t *self = *self_p;
        if (self->handle)
            fclose (self->handle);
        free (self->fullname);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Duplicate a file item

zfile_t *
zfile_dup (zfile_t *self)
{
    zfile_t
        *copy;

    copy = (zfile_t *) zmalloc (sizeof (zfile_t));
    copy->fullname = strdup (self->fullname);
    copy->modified = self->modified;
    copy->cursize = self->cursize;
    copy->mode = self->mode;
    return copy;
}


//  --------------------------------------------------------------------------
//  Return file name, remove path if provided

char *
zfile_filename (zfile_t *self, char *path)
{
    assert (self);
    char *name = self->fullname;
    if (path
    &&  strlen (self->fullname) >= strlen (path)
    &&  memcmp (self->fullname, path, strlen (path)) == 0) {
        name += strlen (path);
        if (*name == '/')
            name++;
    }
    return name;
}


//  --------------------------------------------------------------------------
//  Return when the file was last modified.
//  Updates the file statistics from disk at every call.

time_t
zfile_modified (zfile_t *self)
{
    assert (self);
    s_restat (self);
    return self->modified;
}


//  --------------------------------------------------------------------------
//  Return the last-known size of the file.
//  Updates the file statistics from disk at every call.

off_t
zfile_cursize (zfile_t *self)
{
    assert (self);
    s_restat (self);
    return self->cursize;
}


//  --------------------------------------------------------------------------
//  Return true if the file is a directory.
//  Updates the file statistics from disk at every call.

bool
zfile_is_directory (zfile_t *self)
{
    assert (self);
    s_restat (self);
    return (self->mode & S_IFDIR) != 0;
}


//  --------------------------------------------------------------------------
//  Return true if the file is a regular file.
//  Updates the file statistics from disk at every call.

bool
zfile_is_regular (zfile_t *self)
{
    assert (self);
    s_restat (self);
    return (self->mode & S_IFREG) != 0;
}


//  --------------------------------------------------------------------------
//  Return true if the file is readable by this process
//  Updates the file statistics from disk at every call.

bool
zfile_is_readable (zfile_t *self)
{
    assert (self);
    s_restat (self);
    return (self->mode & S_IRUSR) != 0;
}


//  --------------------------------------------------------------------------
//  Return true if the file is writeable by this process
//  Updates the file statistics from disk at every call.

bool
zfile_is_writeable (zfile_t *self)
{
    assert (self);
    s_restat (self);
    return (self->mode & S_IWUSR) != 0;
}


//  --------------------------------------------------------------------------
//  Check if file has stopped changing and can be safely processed.
//  Updates the file statistics from disk at every call.

bool
zfile_is_stable (zfile_t *self)
{
    assert (self);
    s_restat (self);
    return self->stable;
}


//  --------------------------------------------------------------------------
//  Remove the file

void
zfile_remove (zfile_t *self)
{
    assert (self);
    zfile_close (self);
    zsys_file_delete (self->fullname);
}


//  --------------------------------------------------------------------------
//  Open file for reading
//  Returns 0 if OK, -1 if not found or not accessible

int
zfile_input (zfile_t *self)
{
    assert (self);
    if (self->handle)
        zfile_close (self);

    self->handle = fopen (self->fullname, "rb");
    if (self->handle) {
        struct stat stat_buf;
        if (stat (self->fullname, &stat_buf) == 0)
            self->cursize = stat_buf.st_size;
        else
            self->cursize = 0;
    }
    return self->handle? 0: -1;
}


//  --------------------------------------------------------------------------
//  Open file for writing, creating directory if needed
//  File is created if necessary; chunks can be written to file at any
//  location. Returns 0 if OK, -1 if error.

int
zfile_output (zfile_t *self)
{
    assert (self);
    
    //  Create file path if it doesn't exist
    char *file_path = strdup (self->fullname);
    char *last_slash = strrchr (file_path, '/');
    if (last_slash)
        *last_slash = 0;
    if (zsys_dir_create (file_path))
        return -1;
    free (file_path);

    //  Create file if it doesn't exist
    if (self->handle)
        zfile_close (self);
    self->handle = fopen (self->fullname, "r+b"); 
    if (!self->handle) {
        self->handle = fopen (self->fullname, "w+b");
        if (!self->handle)
            self->handle = fopen (self->fullname, "w+b");
    }
    return self->handle? 0: -1;
}


//  --------------------------------------------------------------------------
//  Read chunk from file at specified position
//  If this was the last chunk, sets self->eof
//  Null chunk means there was another error

zchunk_t *
zfile_read (zfile_t *self, size_t bytes, off_t offset)
{
    assert (self);
    assert (self->handle);
    //  Calculate real number of bytes to read
    if (offset > self->cursize)
        bytes = 0;
    else
    if (bytes > self->cursize - offset)
        bytes = self->cursize - offset;

    int rc = fseek (self->handle, (long) offset, SEEK_SET);
    if (rc == -1)
        return NULL;

    self->eof = false;
    zchunk_t *chunk = zchunk_read (self->handle, bytes);
    if (chunk)
        self->eof = zchunk_size (chunk) < bytes;
    return chunk;
}


//  --------------------------------------------------------------------------
//  Write chunk to file at specified position
//  Return 0 if OK, else -1

int
zfile_write (zfile_t *self, zchunk_t *chunk, off_t offset)
{
    assert (self);
    assert (self->handle);
    int rc = fseek (self->handle, (long) offset, SEEK_SET);
    if (rc >= 0)
        rc = zchunk_write (chunk, self->handle);
    return rc;
}


//  --------------------------------------------------------------------------
//  Close file, if open

void
zfile_close (zfile_t *self)
{
    assert (self);
    if (self->handle) {
        fclose (self->handle);
        s_restat (self);
    }
    self->handle = 0;
}


//  --------------------------------------------------------------------------
//  Return file handle, if opened

FILE *
zfile_handle (zfile_t *self)
{
    assert (self);
    return self->handle;
}

//  Deprecated API, moved to zsys class. The zfile class works with
//  an object instance, which is more consistent with the CLASS style
//  and lets us do more interesting things. These functions were
//  essentially about portability, so now sit in zsys.

bool zfile_exists (const char *filename) {
 return zsys_file_exists (filename);
}
ssize_t zfile_size (const char *filename) {
    return zsys_file_size (filename);
}
mode_t zfile_mode (const char *filename) {
    return zsys_file_mode (filename);
}
int zfile_delete (const char *filename) {
    return zsys_file_delete (filename);
}
bool zfile_stable (const char *filename) {
    return zsys_file_stable (filename);
}
int zfile_mkdir (const char *pathname) {
    return zsys_dir_create (pathname);
}
int zfile_rmdir (const char *pathname) {
    return zsys_dir_delete (pathname);
}
void zfile_mode_private (void) {
    zsys_file_mode_private ();
}
void zfile_mode_default (void) {
    zsys_file_mode_default ();
}


//  --------------------------------------------------------------------------
//  Self test of this class

int
zfile_test (bool verbose)
{
    printf (" * zfile: ");

    //  @selftest
    zfile_t *file = zfile_new (".", "bilbo");
    assert (streq (zfile_filename (file, "."), "bilbo"));
    assert (zfile_is_readable (file) == false);
    zfile_destroy (&file);

    //  Create a test file in some random subdirectory
    file = zfile_new ("./this/is/a/test", "bilbo");
    int rc = zfile_output (file);
    assert (rc == 0);
    zchunk_t *chunk = zchunk_new (NULL, 100);
    zchunk_fill (chunk, 0, 100);
    //  Write 100 bytes at position 1,000,000 in the file
    rc = zfile_write (file, chunk, 1000000);
    assert (rc == 0);
    zfile_close (file);
    assert (zfile_is_readable (file));
    assert (zfile_cursize (file) == 1000100);
    assert (!zfile_is_stable (file));
    zchunk_destroy (&chunk);
    zclock_sleep (1001);
    assert (zfile_is_stable (file));

    //  Check we can read from file
    rc = zfile_input (file);
    assert (rc == 0);
    chunk = zfile_read (file, 1000100, 0);
    assert (chunk);
    assert (zchunk_size (chunk) == 1000100);
    zchunk_destroy (&chunk);

    //  Remove file and directory
    zdir_t *dir = zdir_new ("./this", NULL);
    assert (zdir_cursize (dir) == 1000100);
    zdir_remove (dir, true);
    assert (zdir_cursize (dir) == 0);
    zdir_destroy (&dir);

    //  Check we can no longer read from file
    assert (!zfile_is_readable (file));
    rc = zfile_input (file);
    assert (rc == -1);
    zfile_destroy (&file);
    //  @end

    printf ("OK\n");
    return 0;
}

