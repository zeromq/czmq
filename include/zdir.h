/*  =========================================================================
    zdir - work with file-system directories

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under the
    terms of the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your option)
    any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZDIR_H_INCLUDED__
#define __ZDIR_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zdir_t zdir_t;

//  @interface
//  Create a new directory item that loads in the full tree of the
//  specified path, optionally located under some parent path.
CZMQ_EXPORT zdir_t *
    zdir_new (const char *path, const char *parent);

//  Destroy a directory tree and all children it contains.
CZMQ_EXPORT void
    zdir_destroy (zdir_t **self_p);

//  Return directory path
CZMQ_EXPORT char *
    zdir_path (zdir_t *self);

//  Return last modification time for directory.
CZMQ_EXPORT time_t
    zdir_modified (zdir_t *self);

//  Return total hierarchy size, in bytes of data contained in all files
//  in the directory tree.
CZMQ_EXPORT off_t
    zdir_cursize (zdir_t *self);

//  Returns a sorted array of zfile objects; returns a single block of memory,
//  that you destroy by calling free(). Each entry in the array is a pointer
//  to a zfile_t item already allocated in the zdir tree. The array ends with
//  a null pointer. Do not destroy the original zdir tree until you are done
//  with this array.
CZMQ_EXPORT zfile_t **
    zdir_flatten (zdir_t *self);

//  Print contents of directory to stderr
CZMQ_EXPORT void
    zdir_dump (zdir_t *self, int indent);

//  Remove directory, optionally including all files that it contains, at
//  all levels. If force is false, will only remove the directory if empty.
//  If force is true, will remove all files and all subdirectories.
CZMQ_EXPORT void
    zdir_remove (zdir_t *self, bool force);

//  Self test of this class
CZMQ_EXPORT int
    zdir_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
