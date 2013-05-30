/*  =========================================================================
    zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.

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

#ifndef __ZCONFIG_H_INCLUDED__
#define __ZCONFIG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zconfig_t zconfig_t;

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
    zconfig_set_name (zconfig_t *self, const char *format, ...);

//  Set new value for config item. The new value may be a string, a printf
//  format, or NULL.
CZMQ_EXPORT void
    zconfig_set_value (zconfig_t *self, const char *format, ...);
    
//  Find our first child, if any
CZMQ_EXPORT zconfig_t *
    zconfig_child (zconfig_t *self);

//  Find our first sibling, if any
CZMQ_EXPORT zconfig_t *
    zconfig_next (zconfig_t *self);

//  Find a config item along a path
CZMQ_EXPORT zconfig_t *
    zconfig_locate (zconfig_t *self, const char *path);

//  Resolve a config path into a string value
CZMQ_EXPORT char *
    zconfig_resolve (zconfig_t *self, const char *path, const char *default_value);

//  Set config item name, name may be NULL
CZMQ_EXPORT void
    zconfig_set_path (zconfig_t *self, const char *path, const char *value);

//  Locate the last config item at a specified depth
CZMQ_EXPORT zconfig_t *
    zconfig_at_depth (zconfig_t *self, int level);

//  Execute a callback for each config item in the tree
CZMQ_EXPORT int
    zconfig_execute (zconfig_t *self, zconfig_fct handler, void *arg);

//  Load a config item tree from a specified ZPL text file
CZMQ_EXPORT zconfig_t *
    zconfig_load (const char *filename);

//  Save a config item tree to a specified ZPL text file, where a filename
//  "-" means dump to standard output.
CZMQ_EXPORT int
    zconfig_save (zconfig_t *self, char *filename);

//  Dump the config file to stdout for tracing
CZMQ_EXPORT void
    zconfig_dump (zconfig_t *self);

//  Self test of this class
void
    zconfig_test (bool verbose);

#ifdef __cplusplus
}
#endif

#endif
