/*  =========================================================================
    zfile - provides methods to work with files in a portable fashion.

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
    The zfile class provides methods to work with disk files. A file object
    provides the modified date, current size, and type of the file. You can
    create a file object for a filename that does not yet exist. To read or
    write data from the file, use the input and output methods, and then
    read and write chunks. The output method lets you both read and write
    chunks, at any offset. Finally, this class provides portable symbolic
    links. If a filename ends in ".ln", the first line of text in the file
    is read, and used as the underlying file for read/write operations.
    This lets you manipulate (e.g.) copy symbolic links without copying
    the perhaps very large files they point to.
@discuss
    This class is a new API, deprecating the old zfile class (which still
    exists but is implemented in zsys now).
@end
*/

#include "../include/czmq.h"
#include "platform.h"

//  Structure of our class

struct _zfile_t {
    //  Properties for all file objects
    char *fullname;         //  File name with path
    char *link;             //  Optional linked file
    bool exists;            //  true if file exists
    bool stable;            //  true if file is stable
    bool eof;               //  true if at end of file
    FILE *handle;           //  Read/write handle
    zdigest_t *digest;      //  File digest, if known

    //  Properties from files that exist on file system
    time_t modified;        //  Modification time
    off_t  cursize;         //  Size of the file
    mode_t mode;            //  POSIX permission bits
};


//  --------------------------------------------------------------------------
//  Constructor
//  If file exists, populates properties. CZMQ supports portable symbolic
//  links, which are files with the extension ".ln". A symbolic link is a
//  text file containing one line, the filename of a target file. Reading
//  data from the symbolic link actually reads from the target file. Path
//  may be NULL, in which case it is not used.

zfile_t *
zfile_new (const char *path, const char *name)
{
    zfile_t *self = (zfile_t *) zmalloc (sizeof (zfile_t));

    //  Format full path to file
    if (path) {
        self->fullname = (char *) zmalloc (strlen (path) + strlen (name) + 2);
        sprintf (self->fullname, "%s/%s", path, name);
    }
    else
        self->fullname = strdup (name);

    //  Resolve symbolic link if possible
    if (strlen (self->fullname) > 3
    &&  streq (self->fullname + strlen (self->fullname) - 3, ".ln")) {
        FILE *handle = fopen (self->fullname, "r");
        if (handle) {
            char buffer [256];
            if (fgets (buffer, 256, handle)) {
                //  We have the contents of the symbolic link
                if (buffer [strlen (buffer) - 1] == '\n')
                    buffer [strlen (buffer) - 1] = 0;
                self->link = strdup (buffer);
                //  Chop ".ln" off name for external use
                self->fullname [strlen (self->fullname) - 3] = 0;
            }
            fclose (handle);
        }
    }
    zfile_restat (self);
    return self;
}

//  --------------------------------------------------------------------------
//  Destroy a file item

void
zfile_destroy (zfile_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zfile_t *self = *self_p;
        zdigest_destroy (&self->digest);
        if (self->handle)
            fclose (self->handle);
        free (self->fullname);
        free (self->link);
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
    copy->link = self->link? strdup (self->link): NULL;
    copy->mode = self->mode;
    return copy;
}


//  --------------------------------------------------------------------------
//  Return file name, remove path if provided

char *
zfile_filename (zfile_t *self, const char *path)
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
//  Refresh file properties from disk; this is not done automatically
//  on access methods, otherwise it is not possible to compare directory
//  snapshots.

void
zfile_restat (zfile_t *self)
{
    assert (self);
    struct stat stat_buf;
    char *real_name = self->link? self->link: self->fullname;
    if (stat (real_name, &stat_buf) == 0) {
        self->cursize = stat_buf.st_size;
        self->modified = stat_buf.st_mtime;
        self->mode = zsys_file_mode (real_name);
        self->stable = zsys_file_stable (real_name);
    }
    else {
        self->cursize = 0;
        self->modified = 0;
        self->mode = 0;
        self->stable = false;
    }
}


//  --------------------------------------------------------------------------
//  Return when the file was last modified. If you want this to reflect the
//  current situation, call zfile_restat before checking this property.

time_t
zfile_modified (zfile_t *self)
{
    assert (self);
    return self->modified;
}


//  --------------------------------------------------------------------------
//  Return the last-known size of the file. If you want this to reflect the
//  current situation, call zfile_restat before checking this property.

off_t
zfile_cursize (zfile_t *self)
{
    assert (self);
    return self->cursize;
}


//  --------------------------------------------------------------------------
//  Return true if the file is a directory. If you want this to reflect
//  any external changes, call zfile_restat before checking this property.

bool
zfile_is_directory (zfile_t *self)
{
    assert (self);
    return (self->mode & S_IFDIR) != 0;
}


//  --------------------------------------------------------------------------
//  Return true if the file is a regular file. If you want this to reflect
//  any external changes, call zfile_restat before checking this property.

bool
zfile_is_regular (zfile_t *self)
{
    assert (self);
    return (self->mode & S_IFREG) != 0;
}


//  --------------------------------------------------------------------------
//  Return true if the file is readable by this process. If you want this to
//  reflect any external changes, call zfile_restat before checking this
//  property.

bool
zfile_is_readable (zfile_t *self)
{
    assert (self);
    return (self->mode & S_IREAD) != 0;
}


//  --------------------------------------------------------------------------
//  Return true if the file is writeable by this process. If you want this
//  to reflect any external changes, call zfile_restat before checking this
//  property.

bool
zfile_is_writeable (zfile_t *self)
{
    assert (self);
    return (self->mode & S_IWRITE) != 0;
}


//  --------------------------------------------------------------------------
//  Check if file has stopped changing and can be safely processed.
//  If you want this to reflect the current situation, call zfile_restat
//  before checking this property.

bool
zfile_is_stable (zfile_t *self)
{
    assert (self);
    return self->stable;
}


//  --------------------------------------------------------------------------
//  Remove the file

void
zfile_remove (zfile_t *self)
{
    assert (self);
    //  Restore ".ln" in file name if this was a symbolic link
    if (self->link)
        self->fullname [strlen (self->fullname)] = '.';
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

    char *real_name = self->link? self->link: self->fullname;
    self->handle = fopen (real_name, "rb");
    if (self->handle) {
        struct stat stat_buf;
        if (stat (real_name, &stat_buf) == 0)
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
    
    //  Wipe symbolic link if that's what the file was
    if (self->link) {
        free (self->link);
        self->link = NULL;
    }
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
//  Read chunk from file at specified position. If this was the last chunk,
//  sets self->eof. Returns a null chunk in case of error.

zchunk_t *
zfile_read (zfile_t *self, size_t bytes, off_t offset)
{
    assert (self);
    assert (self->handle);
    //  Calculate real number of bytes to read
    if (offset > self->cursize)
        bytes = 0;
    else
    if (bytes > (size_t) (self->cursize - offset))
        bytes = (size_t) (self->cursize - offset);

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
        zfile_restat (self);
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


//  --------------------------------------------------------------------------
//  Calculate SHA1 digest for file, using zdigest class. Caller should not
//  modify digest.

char *
zfile_digest (zfile_t *self)
{
    if (!self->digest) {
        if (zfile_input (self) == -1)
            return NULL;            //  Problem reading file

        //  Now calculate hash for file data, chunk by chunk
        size_t blocksz = 65535;
        off_t offset = 0;

        self->digest = zdigest_new ();
        zchunk_t *chunk = zfile_read (self, blocksz, offset);
        while (zchunk_size (chunk)) {
            zdigest_update (self->digest,
                zchunk_data (chunk), zchunk_size (chunk));
            zchunk_destroy (&chunk);
            offset += blocksz;
            chunk = zfile_read (self, blocksz, offset);
        }
        zchunk_destroy (&chunk);
        zfile_close (self);
    }
    return zdigest_string (self->digest);
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
    zfile_t *file = zfile_new (NULL, "bilbo");
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
    zchunk_destroy (&chunk);
    zfile_close (file);
    assert (zfile_is_readable (file));
    assert (zfile_cursize (file) == 1000100);
    zfile_restat (file);
    assert (!zfile_is_stable (file));
    zclock_sleep (1001);
    assert (!zfile_is_stable (file));
    zfile_restat (file);
    assert (zfile_is_stable (file));
    assert (streq (zfile_digest (file), "F6CA2B0E6609C2B556F651F46A5A14C86153D0BF"));
    
    //  Check we can read from file
    rc = zfile_input (file);
    assert (rc == 0);
    chunk = zfile_read (file, 1000100, 0);
    assert (chunk);
    assert (zchunk_size (chunk) == 1000100);
    zchunk_destroy (&chunk);
    zfile_close (file);

    //  Try some fun with symbolic links
    zfile_t *link = zfile_new ("./this/is/a/test", "bilbo.ln");
    rc = zfile_output (link);
    assert (rc == 0);
    fprintf (zfile_handle (link), "./this/is/a/test/bilbo\n");
    zfile_destroy (&link);

    link = zfile_new ("./this/is/a/test", "bilbo.ln");
    rc = zfile_input (link);
    assert (rc == 0);
    chunk = zfile_read (link, 1000100, 0);
    assert (chunk);
    assert (zchunk_size (chunk) == 1000100);
    zchunk_destroy (&chunk);
    zfile_destroy (&link);

    //  Remove file and directory
    zdir_t *dir = zdir_new ("./this", NULL);
    assert (zdir_cursize (dir) == 2000200);
    zdir_remove (dir, true);
    assert (zdir_cursize (dir) == 0);
    zdir_destroy (&dir);

    //  Check we can no longer read from file
    assert (zfile_is_readable (file));
    zfile_restat (file);
    assert (!zfile_is_readable (file));
    rc = zfile_input (file);
    assert (rc == -1);
    zfile_destroy (&file);
    //  @end

    printf ("OK\n");
    return 0;
}
