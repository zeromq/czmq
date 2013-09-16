/*  =========================================================================
    zsys - system wrapper

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

void
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
            sigaction (SIGINT, &action, &sigint_default);
            sigaction (SIGTERM, &action, &sigterm_default);
        }
        else {
            //  Save default handlers if not already done
            sigaction (SIGINT, NULL, &sigint_default);
            sigaction (SIGTERM, NULL, &sigterm_default);
        }
    }
#endif
}


//  --------------------------------------------------------------------------
//  Reset interrupt handler, call this at exit if needed
//  Idempotent; safe to call multiple times

void
zsys_handler_reset (void)
{
#if defined (__UNIX__)
    //  Restore default handlers if not already done
    if (sigint_default.sa_handler) {
        sigaction (SIGINT, &sigint_default, NULL);
        sigaction (SIGTERM, &sigterm_default, NULL);
        sigint_default.sa_handler = NULL;
        sigterm_default.sa_handler = NULL;
        s_first_time = true;
    }
#endif
}


//  --------------------------------------------------------------------------
//  Set network interface name to use for broadcasts
//  Use this to force the interface for beacons
//  This is experimental; may be merged into zbeacon class.

//  NOT thread safe, not a good design; this is to test the feasibility
//  of forcing a network interface name instead of writing code to find it.
static char *s_interface = NULL;

void
zsys_set_interface (char *interface_name)
{
    free (s_interface);
    s_interface = strdup (interface_name);
}


//  Return network interface name to use for broadcasts.
//  Returns "" if no interface was set.
//  This is experimental; may be merged into zbeacon class.

char *
zsys_interface (void)
{
    return (s_interface? s_interface: "");
}


//  --------------------------------------------------------------------------
//  Return true if file exists, else zero

bool
zsys_file_exists (const char *filename)
{
    assert (filename);
    return zsys_file_mode (filename) != (mode_t) -1;
}


//  --------------------------------------------------------------------------
//  Return size of file, or -1 if not found

ssize_t
zsys_file_size (const char *filename)
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
//  Return file modification time. Returns 0 if the file does not exist.

time_t
zsys_file_modified (const char *filename)
{
    struct stat stat_buf;
    if (stat (filename, &stat_buf) == 0)
        return stat_buf.st_mtime;
    else
        return 0;
}


//  --------------------------------------------------------------------------
//  Return file mode

mode_t
zsys_file_mode (const char *filename)
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
zsys_file_delete (const char *filename)
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
zsys_file_stable (const char *filename)
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
zsys_dir_create (const char *pathname)
{
    //  Take copy of string as we're going to mess with it
    char *my_pathname = strdup (pathname);

    //  Create parent directory levels if needed
    char *slash = strchr (my_pathname + 1, '/');
    do {
        if (slash)
            *slash = 0;         //  Cut at slash
        mode_t mode = zsys_file_mode (my_pathname);
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
zsys_dir_delete (const char *pathname)
{
#if (defined (__WINDOWS__))
    return RemoveDirectory (pathname)? 0: -1;
#else
    return rmdir (pathname);
#endif
}


//  --------------------------------------------------------------------------
//  Set private file creation mode; all files created from here will be
//  readable/writable by the owner only.

static mode_t s_old_mask = 0;

void
zsys_file_mode_private (void)
{
#   if !defined(__WINDOWS__)
    s_old_mask = umask (S_IWGRP | S_IWOTH | S_IRGRP | S_IROTH);
#   endif
}


//  --------------------------------------------------------------------------
//  Reset default file creation mode; all files created from here will use
//  process file mode defaults.

void
zsys_file_mode_default (void)
{
    //  Reset process file create mask
#   if !defined(__WINDOWS__)
    if (s_old_mask)
        umask (s_old_mask);
#   endif
}


//  --------------------------------------------------------------------------
//  Format a string with variable arguments, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL.
 
char *
zsys_vprintf (const char *format, va_list argptr)
{
    int size = 256;
    char *string = (char *) malloc (size);
    int required = vsnprintf (string, size, format, argptr);
#if defined (__WINDOWS__)
    if (required < 0 || required >= size)
        required = _vscprintf (format, argptr);
#endif
    //  If formatted string cannot fit into small string, reallocate a
    //  larger buffer for it.
    if (required >= size) {
        size = required + 1;
        string = (char *) realloc (string, size);
        if (!string)
            return NULL;
        vsnprintf (string, size, format, argptr);
    }
    return string;
}


//  --------------------------------------------------------------------------
//  Selftest

static char *
s_vprintf (const char *format, ...) 
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    return (string);
}
    
int
zsys_test (bool verbose)
{
    printf (" * zsys: ");

    zsys_handler_reset ();
    zsys_handler_set (NULL);
    zsys_handler_set (NULL);
    zsys_handler_reset ();
    zsys_handler_reset ();

    int rc = zsys_file_delete ("nosuchfile");
    assert (rc == -1);

    bool rc_bool = zsys_file_exists ("nosuchfile");
    assert (rc_bool != true);

    rc = (int) zsys_file_size ("nosuchfile");
    assert (rc == -1);

    time_t when = zsys_file_modified (".");
    assert (when > 0);

    char *string = s_vprintf ("%s %02x", "Hello", 16);
    assert (streq (string, "Hello 10"));
    free (string);
    
    printf ("OK\n");
    return 0;
}
