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

#include "czmq_classes.h"

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
    char *curline;          //  Last read line, if any
    size_t linemax;         //  Size of allocated buffer
    bool remove_on_destroy; //  Whenever delete file on destroy
                            //  Typically for tempfiles
    int fd;                 //  File descriptor - set up by zfile_tmp
    bool close_fd;          //  XXX: for some reason self->fd == 0 in
                            //  zdir and zdir_patch tests, this is a
                            //  workaround for the problem

    //  Properties from files that exist on file system
    time_t modified;        //  Modification time
    off_t cursize;          //  Size of the file
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
    assert (self);

    //  Format full path to file
    if (path) {
        self->fullname = (char *) zmalloc (strlen (path) + strlen (name) + 2);
        assert (self->fullname);
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
                if (self->link)
                    self->fullname [strlen (self->fullname) - 3] = 0;
                else {
                    fclose (handle);
                    zfile_destroy (&self);
                    return NULL;
                }
            }
            fclose (handle);
        }
    }
    self->handle = 0;
    zfile_restat (self);
    self->fd = -1;
    self->close_fd = false;
    return self;
}

//  --------------------------------------------------------------------------
//  Constructor
//  Create new temporary file for writing via tmpfile. File is automatically
//  deleted on destroy

zfile_t *
zfile_tmp (void)
{
    zfile_t *self = NULL;

#if defined (__WINDOWS__)
    DWORD result;
    UINT value;
    char name [MAX_PATH];
    char path [MAX_PATH];

    result = GetTempPathA (MAX_PATH, path);
    if (result > MAX_PATH || (result == 0))
        return NULL;

    value = GetTempFileNameA (path, "CZMQ_ZFILE", 0, name);
    if (value == 0)
        return NULL;

    self = (zfile_t *) zmalloc (sizeof (zfile_t));
    assert (self);
    self->fullname = strdup (name);
    self->handle = fopen (self->fullname, "w");
#else

# if (defined (PATH_MAX))
    char buffer [PATH_MAX];
# else
   # if (defined (_MAX_PATH))
    char buffer [_MAX_PATH];
   # else
    char buffer [1024];
   # endif
# endif
    memset (buffer, 0, sizeof (buffer));
    strncpy (buffer, "/tmp/czmq_zfile.XXXXXX", sizeof(buffer)-1);
    int fd = mkstemp (buffer);
    if (fd == -1)
        return NULL;

    self = (zfile_t *) zmalloc (sizeof (zfile_t));
    assert (self);
    self->fd = fd;
    self->close_fd = true;
    self->fullname = strdup (buffer);
    self->handle = fdopen (self->fd, "w");
#endif

    if (!self->handle) {
        if (self->close_fd)
            close (self->fd);
        zstr_free (&self->fullname);
        free (self);
        return NULL;
    }
    self->remove_on_destroy = true;
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
        if (self->remove_on_destroy)
            zfile_remove (self);
        zfile_close (self);
        freen (self->fullname);
        freen (self->curline);
        freen (self->link);
        freen (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Duplicate a file item, returns a newly constructed item. If the file
//  is null, or memory was exhausted, returns null.

zfile_t *
zfile_dup (zfile_t *self)
{
    if (self) {
        zfile_t *copy = (zfile_t *) zmalloc (sizeof (zfile_t));
        assert (copy);
        copy->fullname = strdup (self->fullname);
        assert (copy->fullname);
        copy->modified = self->modified;
        copy->cursize = self->cursize;
        copy->link = self->link? strdup (self->link): NULL;
        copy->mode = self->mode;
        return copy;
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return file name, remove path if provided

const char *
zfile_filename (zfile_t *self, const char *path)
{
    assert (self);
    char *name = self->fullname;
    if (path
    &&  strlen (self->fullname) >= strlen (path)
    &&  memcmp (self->fullname, path, strlen (path)) == 0) {
        name += strlen (path);
        while (*name == '/')
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
        if(self->handle) {
            long cpos = ftell(self->handle);
            fseek(self->handle, 0L, SEEK_END);
            self->cursize = ftell(self->handle);
            fseek(self->handle, cpos, SEEK_SET);
        } else {
            self->cursize = 0;
        }
        self->modified = 0;
        self->mode = 0;
        self->stable = false;
    }
    zdigest_destroy (&self->digest);
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
    return S_ISDIR (self->mode);
}


//  --------------------------------------------------------------------------
//  Return true if the file is a regular file. If you want this to reflect
//  any external changes, call zfile_restat before checking this property.

bool
zfile_is_regular (zfile_t *self)
{
    assert (self);
    return S_ISREG (self->mode);
}


//  --------------------------------------------------------------------------
//  Return true if the file is readable by this process. If you want this to
//  reflect any external changes, call zfile_restat before checking this
//  property.

bool
zfile_is_readable (zfile_t *self)
{
    assert (self);
    return (self->mode & S_IRUSR) != 0;
}


//  --------------------------------------------------------------------------
//  Return true if the file is writeable by this process. If you want this
//  to reflect any external changes, call zfile_restat before checking this
//  property.

bool
zfile_is_writeable (zfile_t *self)
{
    assert (self);
    return (self->mode & S_IWUSR) != 0;
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
//  Return true if the file was changed on disk since the zfile_t object
//  was created, or the last zfile_restat() call made on it. Note that the
//  file will not necessarily yet be stable.

bool
zfile_has_changed (zfile_t *self)
{
    assert (self);
    struct stat stat_buf;
    char *real_name = self->link? self->link: self->fullname;
    if (stat (real_name, &stat_buf) == 0) {
        //  It's not a foolproof heuristic but catches most cases
        if (stat_buf.st_mtime != self->modified
        ||  stat_buf.st_size != self->cursize)
            return true;
    }
    return false;
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
        if (stat (real_name, &stat_buf) == 0) {
            self->cursize = stat_buf.st_size;
        } else {
            long cpos = ftell(self->handle);
            fseek(self->handle, 0L, SEEK_END);
            self->cursize = ftell(self->handle);
            fseek(self->handle, cpos, SEEK_SET);
        }
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
    int rc = 0;
    assert (self);

    //  Create file path if it doesn't exist
    char *file_path = strdup (self->fullname);
    if (!file_path)
        return -1;
    char *last_slash = strrchr (file_path, '/');
    if (last_slash)
    {
        *last_slash = 0;
        rc = zsys_dir_create (file_path);
    }

    //  Wipe symbolic link if that's what the file was
    if (self->link) {
        freen (self->link);
        self->link = NULL;
    }
    freen (file_path);
    if (rc != 0)
        return -1;

    //  Create file if it doesn't exist
    if (self->handle)
        zfile_close (self);

    self->handle = fopen (self->fullname, "r+b");
    if (!self->handle)
        self->handle = fopen (self->fullname, "w+b");

    return self->handle? 0: -1;
}


//  --------------------------------------------------------------------------
//  Read chunk from file at specified position. If this was the last chunk,
//  sets the eof property. Returns a null chunk in case of error.

zchunk_t *
zfile_read (zfile_t *self, size_t bytes, off_t offset)
{
    assert (self);
    assert (self->handle);

    self->eof = false;
    //  Calculate real number of bytes to read
    if (offset > self->cursize) {
        // if we tried to read 'after' the cursise, then we are at the end
        bytes = 0;
        self->eof = true;
    }
    else
    if (bytes > (size_t) (self->cursize - offset)) {
        // if we are trying to read more than there is, we are at the end
        self->eof = true;
        bytes = (size_t) (self->cursize - offset);
    }

    if (fseek (self->handle, (long) offset, SEEK_SET) == -1) {
        return NULL;
    }

    return zchunk_read (self->handle, bytes);
}


//  --------------------------------------------------------------------------
//  Returns the eof property.

bool
zfile_eof (zfile_t *self)
{
    assert (self);
    return self->eof;
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
//  Read next line of text from file. Returns a pointer to the text line,
//  or NULL if there was nothing more to read from the file.

const char *
zfile_readln (zfile_t *self)
{
    assert (self);
    assert (self->handle);

    //  Opportunistically allocate line buffer if needed; we'll grow this
    //  if needed, so the initial linemax is not a big deal
    if (!self->curline) {
        self->linemax = 512; 
        self->curline = (char *) malloc (self->linemax);
        assert (self->curline);
    }
    uint char_nbr = 0;
    while (true) {
        int cur_char = fgetc (self->handle);
        if (cur_char == '\r')
            continue;               //  Skip CR in MS-DOS format files
        else
        if (cur_char == EOF && char_nbr == 0)
            return NULL;            //  Signal end of file
        else
        if (cur_char == '\n' || cur_char == EOF)
            cur_char = 0;
        if (char_nbr == self->linemax - 1) {
            self->linemax *= 2;
            self->curline = (char *) realloc (self->curline, self->linemax);
        }
        self->curline [char_nbr++] = cur_char;
        if (cur_char == 0)
            break;
    }
    return self->curline;
}


//  --------------------------------------------------------------------------
//  Close file, if open

void
zfile_close (zfile_t *self)
{
    assert (self);
    if (self->handle) {
        fclose (self->handle);
        self->handle = 0;
        zfile_restat (self);
        self->eof = false;
    }
    if (self->close_fd)
        close (self->fd);
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
//  Calculate SHA1 digest for file, using zdigest class.

const char *
zfile_digest (zfile_t *self)
{
    assert (self);
    if (!self->digest) {
        if (zfile_input (self) == -1)
            return NULL;            //  Problem reading file

        //  Now calculate hash for file data, chunk by chunk
        long blocksz = 65535;
        off_t offset = 0;

        self->digest = zdigest_new ();
        if (!self->digest)
            return NULL;
        zchunk_t *chunk = zfile_read (self, blocksz, offset);
        while (zchunk_size (chunk)) {
            zdigest_update (self->digest,
                            zchunk_data (chunk), zchunk_size (chunk));
            zchunk_destroy (&chunk);

            //  off_t is defined as long (32 bit on Windows, 64 bit otherwise)
            //  This guards against overflow in both contexts.
            if (blocksz > LONG_MAX - offset)
                return NULL;

            offset += blocksz;
            chunk = zfile_read (self, (size_t) blocksz, offset);
        }
        zchunk_destroy (&chunk);
        fclose (self->handle);
        self->handle = 0;
    }
    return zdigest_string (self->digest);
}


//  Deprecated API, moved to zsys class. The zfile class works with
//  an object instance, which is more consistent with the CLASS style
//  and lets us do more interesting things. These functions were
//  essentially about portability, so now sit in zsys.

bool
zfile_exists (const char *filename)
{
    return zsys_file_exists (filename);
}
ssize_t
zfile_size (const char *filename)
{
    return zsys_file_size (filename);
}
mode_t
zfile_mode (const char *filename)
{
    return zsys_file_mode (filename);
}
int
zfile_delete (const char *filename)
{
    return zsys_file_delete (filename);
}
bool
zfile_stable (const char *filename)
{
    return zsys_file_stable (filename);
}
int
zfile_mkdir (const char *pathname)
{
    return zsys_dir_create (pathname);
}
int
zfile_rmdir (const char *pathname)
{
    return zsys_dir_delete (pathname);
}
void
zfile_mode_private (void)
{
    zsys_file_mode_private ();
}
void
zfile_mode_default (void)
{
    zsys_file_mode_default ();
}


//  --------------------------------------------------------------------------
//  Self test of this class

void
zfile_test (bool verbose)
{
    printf (" * zfile: ");

    //  @selftest

    const char *SELFTEST_DIR_RW = "src/selftest-rw";

    const char *testbasedir  = "this";
    const char *testsubdir  = "is/a/test";
    const char *testfile = "bilbo";
    const char *testlink = "bilbo.ln";
    char *basedirpath = NULL;   // subdir in a test, under SELFTEST_DIR_RW
    char *dirpath = NULL;       // subdir in a test, under basedirpath
    char *filepath = NULL;      // pathname to testfile in a test, in dirpath
    char *linkpath = NULL;      // pathname to testlink in a test, in dirpath

    basedirpath = zsys_sprintf ("%s/%s", SELFTEST_DIR_RW, testbasedir);
    assert (basedirpath);
    dirpath = zsys_sprintf ("%s/%s", basedirpath, testsubdir);
    assert (dirpath);
    filepath = zsys_sprintf ("%s/%s", dirpath, testfile);
    assert (filepath);
    linkpath = zsys_sprintf ("%s/%s", dirpath, testlink);
    assert (linkpath);

    // This subtest is specifically for NULL as current directory, so
    // no SELFTEST_DIR_RW here; testfile should have no slashes inside.
    // Normally tests clean up in zfile_destroy(), but if a selftest run
    // dies e.g. on assert(), workspace remains dirty. Better clean it up.
    if (zfile_exists (testfile) ) {
        if (verbose)
            zsys_debug ("zfile_test() has to remove ./%s that should not have been here", testfile);
        zfile_delete (testfile);
    }
    zfile_t *file = zfile_new (NULL, testfile);
    assert (file);
    assert (streq (zfile_filename (file, "."), testfile));
    assert (zfile_is_readable (file) == false);
    zfile_destroy (&file);

    //  Create a test file in some random subdirectory
    if (verbose)
        zsys_debug ("zfile_test() at timestamp %" PRIi64 ": "
            "Creating new zfile %s",
            zclock_time(), filepath );

    if (zfile_exists (filepath) ) {
        if (verbose)
            zsys_debug ("zfile_test() has to remove %s that should not have been here", filepath);
        zfile_delete (filepath);
    }

    file = zfile_new (dirpath, testfile);
    assert (file);
    int rc = zfile_output (file);
    assert (rc == 0);
    zchunk_t *chunk = zchunk_new (NULL, 100);
    assert (chunk);
    zchunk_fill (chunk, 0, 100);

    //  Write 100 bytes at position 1,000,000 in the file
    if (verbose)
        zsys_debug ("zfile_test() at timestamp %" PRIi64 ": "
            "Writing 100 bytes at position 1,000,000 in the file",
            zclock_time() );
    rc = zfile_write (file, chunk, 1000000);
    if (verbose)
        zsys_debug ("zfile_test() at timestamp %" PRIi64 ": "
            "Wrote 100 bytes at position 1,000,000 in the file, result code %d",
            zclock_time(), rc );
    assert (rc == 0);
    zchunk_destroy (&chunk);
    zfile_close (file);
    assert (zfile_is_readable (file));
    assert (zfile_cursize (file) == 1000100);
    if (verbose)
        zsys_debug ("zfile_test() at timestamp %" PRIi64 ": "
            "Testing if file is NOT stable (is younger than 1 sec)",
            zclock_time() );
    assert (!zfile_is_stable (file));
    if (verbose)
        zsys_debug ("zfile_test() at timestamp %" PRIi64 ": "
            "Passed the lag-dependent tests",
            zclock_time() );
    assert (zfile_digest (file));

    //  Now truncate file from outside
    int handle = open (filepath, O_WRONLY | O_TRUNC | O_BINARY, 0);
    assert (handle >= 0);
    rc = write (handle, "Hello, World\n", 13);
    assert (rc == 13);
    close (handle);
    assert (zfile_has_changed (file));
#ifdef CZMQ_BUILD_DRAFT_API
    zclock_sleep ((int)zsys_file_stable_age_msec() + 50);
#else
    zclock_sleep (5050);
#endif
    assert (zfile_has_changed (file));

    assert (!zfile_is_stable (file));
    zfile_restat (file);
    assert (zfile_is_stable (file));
    assert (streq (zfile_digest (file), "4AB299C8AD6ED14F31923DD94F8B5F5CB89DFB54"));

    //  Check we can read from file
    rc = zfile_input (file);
    assert (rc == 0);
    chunk = zfile_read (file, 1000100, 0);
    assert (chunk);
    assert (zchunk_size (chunk) == 13);
    zchunk_destroy (&chunk);
    zfile_close (file);

    //  Check we can read lines from file
    rc = zfile_input (file);
    assert (rc == 0);
    const char *line = zfile_readln (file);
    assert (streq (line, "Hello, World"));
    line = zfile_readln (file);
    assert (line == NULL);
    zfile_close (file);

    //  Try some fun with symbolic links
    zfile_t *link = zfile_new (dirpath, testlink);
    assert (link);
    rc = zfile_output (link);
    assert (rc == 0);
    fprintf (zfile_handle (link), "%s\n", filepath);
    zfile_destroy (&link);

    link = zfile_new (dirpath, testlink);
    assert (link);
    rc = zfile_input (link);
    assert (rc == 0);
    chunk = zfile_read (link, 1000100, 0);
    assert (chunk);
    assert (zchunk_size (chunk) == 13);
    zchunk_destroy (&chunk);
    zfile_destroy (&link);

    //  Remove file and directory
    zdir_t *dir = zdir_new (basedirpath, NULL);
    assert (dir);
    assert (zdir_cursize (dir) == 26);
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

    // This set of tests is done, free the strings for reuse
    zstr_free (&basedirpath);
    zstr_free (&dirpath);
    zstr_free (&filepath);
    zstr_free (&linkpath);

    const char *eof_checkfile = "eof_checkfile";
    filepath = zsys_sprintf ("%s/%s", SELFTEST_DIR_RW, eof_checkfile);
    assert (filepath);

    if (zfile_exists (filepath) ) {
        if (verbose)
            zsys_debug ("zfile_test() has to remove %s that should not have been here", filepath);
        zfile_delete (filepath);
    }
    zstr_free (&filepath);

    file = zfile_new (SELFTEST_DIR_RW, eof_checkfile);
    assert (file);

    //  1. Write something first
    rc = zfile_output (file);
    assert (rc == 0);
    chunk = zchunk_new ("123456789", 9);
    assert (chunk);

    rc = zfile_write (file, chunk, 0);
    assert (rc == 0);
    zchunk_destroy (&chunk);
    zfile_close (file);
    assert (zfile_cursize (file) == 9);

    // 2. Read the written something
    rc = zfile_input (file);
    assert (rc != -1);
    // try to read more bytes than there is in the file
    chunk = zfile_read (file, 1000, 0);
    assert (zfile_eof(file));
    assert (zchunk_streq (chunk, "123456789"));
    zchunk_destroy (&chunk);

    // reading is ok
    chunk = zfile_read (file, 5, 0);
    assert (!zfile_eof(file));
    assert (zchunk_streq (chunk, "12345"));
    zchunk_destroy (&chunk);

    // read from non zero offset until the end
    chunk = zfile_read (file, 5, 5);
    assert (zfile_eof(file));
    assert (zchunk_streq (chunk, "6789"));
    zchunk_destroy (&chunk);
    zfile_remove (file);
    zfile_close (file);
    zfile_destroy (&file);

#ifdef CZMQ_BUILD_DRAFT_API
    zfile_t *tempfile = zfile_tmp ();
    assert (tempfile);
    assert (zfile_filename (tempfile, NULL));
    assert (zsys_file_exists (zfile_filename (tempfile, NULL)));
    zchunk_t *tchunk = zchunk_new ("HELLO", 6);
    assert (zfile_write (tempfile, tchunk, 0) == 0);
    zchunk_destroy (&tchunk);

    char *filename = strdup (zfile_filename (tempfile, NULL));
    zfile_destroy (&tempfile);
    assert (!zsys_file_exists (filename));
    zstr_free (&filename);
#endif // CZMQ_BUILD_DRAFT_API

    // create a file without a path, test for issue #2208
    zfile_t *relfile = zfile_new(NULL, "relfile.tst");
    assert(relfile);
    int rco = zfile_output(relfile);
    assert(rco == 0);
    zfile_delete("relfile.tst");
    zfile_close (relfile);
    zfile_destroy (&relfile);

#if defined (__WINDOWS__)
    zsys_shutdown();
#endif

    //  @end

    printf ("OK\n");
}
