/*  =========================================================================
    zdir_patch - work with directory patches

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
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

#ifndef __ZDIR_PATCH_H_INCLUDED__
#define __ZDIR_PATCH_H_INCLUDED__

typedef enum {
    patch_create = 1,
    patch_delete = 2
} zdir_patch_op_t;

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zdir_patch_t zdir_patch_t;

//  @interface
//  Create new patch
CZMQ_EXPORT zdir_patch_t *
    zdir_patch_new (const char *path, zfile_t *file,
                    zdir_patch_op_t op, const char *alias);

//  Destroy a patch
CZMQ_EXPORT void
    zdir_patch_destroy (zdir_patch_t **self_p);

//  Create copy of a patch
CZMQ_EXPORT zdir_patch_t *
    zdir_patch_dup (zdir_patch_t *self);

//  Return patch file directory path
CZMQ_EXPORT char *
    zdir_patch_path (zdir_patch_t *self);

//  Return patch file item
CZMQ_EXPORT zfile_t *
    zdir_patch_file (zdir_patch_t *self);

//  Return operation
CZMQ_EXPORT zdir_patch_op_t
    zdir_patch_op (zdir_patch_t *self);
    
//  Return patch virtual file path
CZMQ_EXPORT char *
    zdir_patch_vpath (zdir_patch_t *self);

//  Calculate hash digest for file (create only)
CZMQ_EXPORT void
    zdir_patch_digest_set (zdir_patch_t *self);

//  Return hash digest for patch file 
CZMQ_EXPORT char *
    zdir_patch_digest (zdir_patch_t *self);
    
//  Self test of this class
CZMQ_EXPORT int
    zdir_patch_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
