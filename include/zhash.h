/*  =========================================================================
    zhash.h - ZFL singly-linked hash class

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of the ZeroMQ Function Library: http://zfl.zeromq.org

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

#ifndef __ZFL_HASH_H_INCLUDED__
#define __ZFL_HASH_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Callback function for zhash_foreach method
typedef int (zhash_foreach_fn) (char *key, void *value, void *argument);
//  Callback function for zhash_freefn method
typedef void (zhash_free_fn) (void *data);

//  Opaque class structure
typedef struct _zhash zhash_t;

zhash_t *
    zhash_new (void);
void
    zhash_destroy (zhash_t **self_p);
int
    zhash_insert (zhash_t *self, char *key, void *value);
void
    zhash_update (zhash_t *self, char *key, void *value);
void
    zhash_delete (zhash_t *self, char *key);
void *
    zhash_lookup (zhash_t *self, char *key);
void *
    zhash_freefn (zhash_t *self, char *key, zhash_free_fn *free_fn);
size_t
    zhash_size (zhash_t *self);
int
    zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument);
void
    zhash_test (int verbose);

#ifdef __cplusplus
}
#endif

#endif
