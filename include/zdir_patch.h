/*  =========================================================================
    zdir_patch - work with directory patches

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

//  @interface
//  Create new patch
CZMQ_EXPORT zdir_patch_t *
    zdir_patch_new (const char *path, zfile_t *file,
                    zdir_patch_op_t op, const char *alias);

//  Destroy a patch
CZMQ_EXPORT void
    zdir_patch_destroy (zdir_patch_t **self_p);

//  Create copy of a patch. If the patch is null, or memory was exhausted,
//  returns null.
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
CZMQ_EXPORT void
    zdir_patch_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
