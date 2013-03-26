/*  =========================================================================
    zfile - helper functions for working with files.

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
@header
    The zfile class provides methods to work with files and directories.
@discuss
@end
*/

#include "../include/czmq.h"


//  --------------------------------------------------------------------------
//  Return true if file exists, else zero

bool
zfile_exists (const char *filename)
{
    assert (filename);
    return zfile_mode (filename) != (mode_t)-1;
}


//  --------------------------------------------------------------------------
//  Return size of file, or -1 if not found

ssize_t
zfile_size (const char *filename)
{
    struct stat
        stat_buf;

    assert (filename);
    if (stat ((char *) filename, &stat_buf) == 0)
        return stat_buf.st_size;
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Return file mode

mode_t
zfile_mode (const char *filename)
{
#if (defined (__WINDOWS__))
    DWORD dwfa = GetFileAttributes (filename);
    if (dwfa == 0xffffffff)
        return -1;

    dbyte mode = 0;
    if (dwfa & FILE_ATTRIBUTE_DIRECTORY)
        mode |= S_IFDIR;
    else
        mode |= S_IFREG;
    if (!(dwfa & FILE_ATTRIBUTE_HIDDEN))
        mode |= S_IREAD;
    if (!(dwfa & FILE_ATTRIBUTE_READONLY))
        mode |= S_IWRITE;

    return mode;
#else
    struct stat stat_buf;
    if (stat ((char *) filename, &stat_buf) == 0)
        return stat_buf.st_mode;
    else
        return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Delete file, return 0 if OK, -1 if not possible.

int
zfile_delete (const char *filename)
{
    assert (filename);
#if (defined (__WINDOWS__))
    return DeleteFile (filename) ? 0: -1;
#else
    return unlink (filename);
#endif
}


//  --------------------------------------------------------------------------
//  Check if file is 'stable'

bool
zfile_stable (const char *filename)
{
    struct stat stat_buf;
    if (stat (filename, &stat_buf) == 0) {
        //  File is 'stable' if more than 1 second old
#if (defined (WIN32))
#   define EPOCH_DIFFERENCE 11644473600LL
        long age = (long) (zclock_time () - EPOCH_DIFFERENCE * 1000 - (stat_buf.st_mtime * 1000));
#else
        long age = (long) (zclock_time () - (stat_buf.st_mtime * 1000));
#endif
        return (age > 1000);
    }
    else
        return false;           //  File doesn't exist, so not stable
}


//  --------------------------------------------------------------------------
//  Create a file path if it doesn't exist

int
zfile_mkdir (const char *pathname)
{
    //  Take copy of string as we're going to mess with it
    char *my_pathname = strdup (pathname);

    //  Create parent directory levels if needed
    char *slash = strchr (my_pathname + 1, '/');
    do {
        if (slash)
            *slash = 0;         //  Cut at slash
        mode_t mode = zfile_mode (my_pathname);
        if (mode == (mode_t)-1) {
            //  Does not exist, try to create it
#if (defined (__WINDOWS__))
            if (!CreateDirectory (my_pathname, NULL))
#else
            if (mkdir (my_pathname, 0775))
#endif
                return -1;      //  Failed
        }
        else
        if ((mode & S_IFDIR) == 0) {
            //  Not a directory, abort
        }
        if (!slash)             //  End if last segment
            break;
       *slash = '/';
        slash = strchr (slash + 1, '/');
    } while (slash);

    free (my_pathname);
    return 0;
}


//  --------------------------------------------------------------------------
//  Remove a file path if empty

int
zfile_rmdir (const char *pathname)
{
#if (defined (__WINDOWS__))
    return RemoveDirectory (pathname)? 0: -1;
#else
    return rmdir (pathname);
#endif
}


//  --------------------------------------------------------------------------
//  Selftest

int
zfile_test (bool verbose)
{
    printf (" * zfile: ");

    //  @selftest
    int rc = zfile_delete ("nosuchfile");
    assert (rc == -1);

    bool rc_bool = zfile_exists ("nosuchfile");
    assert (rc_bool != true);

    rc = (int) zfile_size ("nosuchfile");
    assert (rc == -1);

    //  @end
    printf ("OK\n");
    return 0;
}
