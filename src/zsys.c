/*  =========================================================================
    zsys - system wrapper

    -------------------------------------------------------------------------
    Copyright (c) 1991-2012 iMatix Corporation <www.imatix.com>
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
    The zsys class provides a portable wrapper for miscellaneous functions
    that we want to wrap but which don't fit into any of the existing
    classes. Eventually all non-portable functionality might be moved here
    but for now it covers only file systems.
@discuss
@end
*/

#include "../include/czmq.h"

#if defined (__UNIX__)
static bool s_first_time = true;
static struct sigaction sigint_default;
static struct sigaction sigterm_default;
#endif

//  --------------------------------------------------------------------------
//  Set interrupt handler (NULL means external handler)
//  Idempotent; safe to call multiple times

CZMQ_EXPORT void
zsys_handler_set (zsys_handler_fn *handler_fn)
{
#if defined (__UNIX__)
    //  Install signal handler for SIGINT and SIGTERM if not NULL
    //  and if this is the first time we've been called
    if (s_first_time) {
        s_first_time = false;
        if (handler_fn) {
            struct sigaction action;
            action.sa_handler = handler_fn;
            action.sa_flags = 0;
            sigemptyset (&action.sa_mask);
            sigaction (SIGINT, &action, NULL);
            sigaction (SIGTERM, &action, NULL);
        }
        //  Save default handlers if not already done
        sigaction (SIGINT, NULL, &sigint_default);
        sigaction (SIGTERM, NULL, &sigterm_default);
    }
#endif
}


//  --------------------------------------------------------------------------
//  Reset interrupt handler, call this at exit if needed
//  Idempotent; safe to call multiple times

CZMQ_EXPORT void
zsys_handler_reset (void)
{
    //  Restore default handlers if not already done
    if (sigint_default.sa_handler) {
        sigaction (SIGINT, &sigint_default, NULL);
        sigaction (SIGTERM, &sigterm_default, NULL);
        sigint_default.sa_handler = NULL;
        sigterm_default.sa_handler = NULL;
        s_first_time = true;
    }
}


//  --------------------------------------------------------------------------
//  Return file mode

CZMQ_EXPORT mode_t
zsys_mode (const char *filename)
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
//  Create a file path if it doesn't exit

void
zsys_mkdir (const char *pathname)
{
    //  Take copy of string as we're going to mess with it
    char *my_pathname = strdup (pathname);
    
    //  Create parent directory levels if needed
    char *slash = strchr (my_pathname + 1, '/');
    do {
        if (slash)
            *slash = 0;         //  Cut at slash
        mode_t mode = zsys_mode (my_pathname);
        if (mode == (mode_t)-1) {
            //  Does not exist, try to create it
#if (defined (__WINDOWS__))
            if (!CreateDirectory (my_pathname, NULL))
#else
            if (mkdir (my_pathname, 0775))
#endif
                return;         //  Failed
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
}


//  --------------------------------------------------------------------------
//  Remove a file path if empty

void
zsys_rmdir (const char *pathname)
{
#if (defined (__WINDOWS__))
    RemoveDirectory (pathname);
#else
    rmdir (pathname);
#endif
}


//  --------------------------------------------------------------------------
//  Delete a file

CZMQ_EXPORT void
zsys_unlink (const char *filename)
{
#if (defined (__WINDOWS__))
    DeleteFile (filename);
#else
    unlink (filename);
#endif
}


//  --------------------------------------------------------------------------
//  Check if file is 'stable'

CZMQ_EXPORT bool
zsys_stable (const char *filename)
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
//  Selftest

int
zsys_test (bool verbose)
{
    printf (" * zsys: ");
    zsys_handler_reset ();
    zsys_handler_set (NULL);
    zsys_handler_set (NULL);
    zsys_handler_reset ();
    zsys_handler_reset ();
    printf ("OK\n");
    return 0;
}

