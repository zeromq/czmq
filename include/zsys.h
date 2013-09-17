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

#ifndef __ZSYS_H_INCLUDED__
#define __ZSYS_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Callback for interrupt signal handler
typedef void (zsys_handler_fn) (int signal_value);

//  Set interrupt handler (NULL means external handler)
CZMQ_EXPORT void
    zsys_handler_set (zsys_handler_fn *handler_fn);

//  Reset interrupt handler, call this at exit if needed
CZMQ_EXPORT void
    zsys_handler_reset (void);

//  Set network interface name to use for broadcasts
//  Use this to force the interface for beacons
//  This is experimental; may be merged into zbeacon class.
CZMQ_EXPORT void
    zsys_set_interface (char *iinterface);

//  Return network interface name to use for broadcasts.
//  Returns "" if no interface was set.
//  This is experimental; may be merged into zbeacon class.
CZMQ_EXPORT char *
    zsys_interface (void);

//  Return 1 if file exists, else zero
CZMQ_EXPORT bool
    zsys_file_exists (const char *filename);

//  Return size of file, or -1 if not found
CZMQ_EXPORT ssize_t
    zsys_file_size (const char *filename);

//  Return file modification time. Returns 0 if the file does not exist.
CZMQ_EXPORT time_t
    zsys_file_modified (const char *filename);

//  Return file mode
CZMQ_EXPORT mode_t
    zsys_file_mode (const char *filename);

//  Delete file. Does not complain if the file is absent
CZMQ_EXPORT int
    zsys_file_delete (const char *filename);

//  Check if file is 'stable'
CZMQ_EXPORT bool
    zsys_file_stable (const char *filename);

//  Create a file path if it doesn't exit
CZMQ_EXPORT int
    zsys_dir_create (const char *pathname);

//  Remove a file path if empty
CZMQ_EXPORT int
    zsys_dir_delete (const char *pathname);

//  Set private file creation mode; all files created from here will be
//  readable/writable by the owner only.
CZMQ_EXPORT void
    zsys_file_mode_private (void);

//  Reset default file creation mode; all files created from here will use
//  process file mode defaults.
CZMQ_EXPORT void
    zsys_file_mode_default (void);

//  Format a string with variable arguments, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL.
CZMQ_EXPORT char *
    zsys_vprintf (const char *format, va_list argptr);

//  Self test of this class
CZMQ_EXPORT int
    zsys_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
