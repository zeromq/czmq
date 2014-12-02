/*  =========================================================================
    zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZCONFIG_H_INCLUDED__
#define __ZCONFIG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Function that executes config
typedef int (zconfig_fct) (zconfig_t *self, void *arg, int level);

//  Create new config item
CZMQ_EXPORT zconfig_t *
    zconfig_new (const char *name, zconfig_t *parent);

//  Destroy a config item and all its children
CZMQ_EXPORT void
    zconfig_destroy (zconfig_t **self_p);

//  Return name of config item
CZMQ_EXPORT char *
    zconfig_name (zconfig_t *self);

//  Return value of config item
CZMQ_EXPORT char *
    zconfig_value (zconfig_t *self);

//  Insert or update configuration key with value
CZMQ_EXPORT void
    zconfig_put (zconfig_t *self, const char *path, const char *value);

//  Set config item name, name may be NULL
CZMQ_EXPORT void
    zconfig_set_name (zconfig_t *self, const char *name);

//  Set new value for config item. The new value may be a string, a printf
//  format, or NULL. Note that if string may possibly contain '%', or if it
//  comes from an insecure source, you must use '%s' as the format, followed
//  by the string.
CZMQ_EXPORT void
    zconfig_set_value (zconfig_t *self, const char *format, ...);
    
//  Find our first child, if any
CZMQ_EXPORT zconfig_t *
    zconfig_child (zconfig_t *self);

//  Find our first sibling, if any
CZMQ_EXPORT zconfig_t *
    zconfig_next (zconfig_t *self);

//  Find a config item along a path; leading slash is optional and ignored.
CZMQ_EXPORT zconfig_t *
    zconfig_locate (zconfig_t *self, const char *path);

//  Resolve a config path into a string value; leading slash is optional
//  and ignored.
CZMQ_EXPORT char *
    zconfig_resolve (zconfig_t *self, const char *path, const char *default_value);

//  Set config item name, name may be NULL
CZMQ_EXPORT void
    zconfig_set_path (zconfig_t *self, const char *path, const char *value);

//  Locate the last config item at a specified depth
CZMQ_EXPORT zconfig_t *
    zconfig_at_depth (zconfig_t *self, int level);

//  Execute a callback for each config item in the tree; returns zero if
//  successful, else -1.
CZMQ_EXPORT int
    zconfig_execute (zconfig_t *self, zconfig_fct handler, void *arg);

//  Add comment to config item before saving to disk. You can add as many
//  comment lines as you like. If you use a null format, all comments are
//  deleted.
CZMQ_EXPORT void
    zconfig_set_comment (zconfig_t *self, const char *format, ...);

//  Return comments of config item, as zlist.
CZMQ_EXPORT zlist_t *
    zconfig_comments (zconfig_t *self);

//  Load a config tree from a specified ZPL text file
CZMQ_EXPORT zconfig_t *
    zconfig_load (const char *filename);

//  Save a config tree to a specified ZPL text file, where a filename
//  "-" means dump to standard output.
CZMQ_EXPORT int
    zconfig_save (zconfig_t *self, const char *filename);

//  Report filename used during zconfig_load, or NULL if none
CZMQ_EXPORT char *
    zconfig_filename (zconfig_t *self);

//  Reload config tree from same file that it was previously loaded from.
//  Returns 0 if OK, -1 if there was an error (and then does not change
//  existing data).
CZMQ_EXPORT int
    zconfig_reload (zconfig_t **self_p);

//  Load a config tree from a memory chunk
CZMQ_EXPORT zconfig_t *
    zconfig_chunk_load (zchunk_t *chunk);

//  Save a config tree to a new memory chunk
CZMQ_EXPORT zchunk_t *
    zconfig_chunk_save (zconfig_t *self);

//  Return true if a configuration tree was loaded from a file and that 
//  file has changed in since the tree was loaded.
CZMQ_EXPORT bool
    zconfig_has_changed (zconfig_t *self);

//  Print the config file to open stream
CZMQ_EXPORT void
    zconfig_fprint (zconfig_t *self, FILE *file);

//  Print the config file to stdout
CZMQ_EXPORT void
    zconfig_print (zconfig_t *self);

//  @end
//  Self test of this class
CZMQ_EXPORT void
    zconfig_test (bool verbose);

//  Compiler hints
CZMQ_EXPORT void zconfig_set_value (zconfig_t *self, const char *format, ...) CHECK_PRINTF (2);

#ifdef __cplusplus
}
#endif

//  Deprecated method aliases
#define zconfig_dump(s) zconfig_print(s)

#endif
