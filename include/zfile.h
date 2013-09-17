/*  =========================================================================
    zfile - helper functions for working with files.

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

#ifndef __ZFILE_H_INCLUDED__
#define __ZFILE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zfile_t zfile_t;

//  @interface
//  Create new file item
CZMQ_EXPORT zfile_t *
    zfile_new (const char *path, const char *name);

//  Destroy a file item
CZMQ_EXPORT void
    zfile_destroy (zfile_t **self_p);

//  Duplicate a file item, returns a newly constructed item
CZMQ_EXPORT zfile_t *
    zfile_dup (zfile_t *self);

//  Return file name, remove path if provided
CZMQ_EXPORT char *
    zfile_filename (zfile_t *self, char *path);

//  Return when the file was last modified.
//  Updates the file statistics from disk at every call.
CZMQ_EXPORT time_t
    zfile_modified (zfile_t *self);

//  Return the last-known size of the file.
//  Updates the file statistics from disk at every call.
CZMQ_EXPORT off_t
    zfile_cursize (zfile_t *self);

//  Return true if the file is a directory.
//  Updates the file statistics from disk at every call.
CZMQ_EXPORT bool
    zfile_is_directory (zfile_t *self);

//  Return true if the file is a regular file.
//  Updates the file statistics from disk at every call.
CZMQ_EXPORT bool
    zfile_is_regular (zfile_t *self);

//  Return true if the file is readable by this process
//  Updates the file statistics from disk at every call.
CZMQ_EXPORT bool
    zfile_is_readable (zfile_t *self);

//  Return true if the file is writeable by this process
//  Updates the file statistics from disk at every call.
CZMQ_EXPORT bool
    zfile_is_writeable (zfile_t *self);

//  Check if file has stopped changing and can be safely processed.
//  Updates the file statistics from disk at every call.
CZMQ_EXPORT bool
    zfile_is_stable (zfile_t *self);

//  Remove the file from disk
CZMQ_EXPORT void
    zfile_remove (zfile_t *self);

//  Open file for reading, returns 0 if OK, else -1.
CZMQ_EXPORT int
    zfile_input (zfile_t *self);

//  Open file for writing, creating full directory path if needed
CZMQ_EXPORT int
    zfile_output (zfile_t *self);

//  Read chunk from file at specified position
CZMQ_EXPORT zchunk_t *
    zfile_read (zfile_t *self, size_t bytes, off_t offset);

//  Write chunk to file at specified position
CZMQ_EXPORT int
    zfile_write (zfile_t *self, zchunk_t *chunk, off_t offset);

//  Close file, if open
CZMQ_EXPORT void
    zfile_close (zfile_t *self);

//  Return file handle, if opened
CZMQ_EXPORT FILE *
    zfile_handle (zfile_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zfile_test (bool verbose);
//  @end

//  @interface
//  These methods are deprecated, and now moved to zsys class.
CZMQ_EXPORT bool
    zfile_exists (const char *filename);
CZMQ_EXPORT ssize_t
    zfile_size   (const char *filename);
CZMQ_EXPORT mode_t
    zfile_mode   (const char *filename);
CZMQ_EXPORT int
    zfile_delete (const char *filename);
CZMQ_EXPORT bool
    zfile_stable (const char *filename);
CZMQ_EXPORT int
    zfile_mkdir  (const char *pathname);
CZMQ_EXPORT int
    zfile_rmdir  (const char *pathname);
CZMQ_EXPORT void
    zfile_mode_private (void);
CZMQ_EXPORT void
    zfile_mode_default (void);
//  @end

#ifdef __cplusplus
}
#endif


#endif  //  __ZFILE_H_INCLUDED__
