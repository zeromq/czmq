/*  =========================================================================
    zlist.h - ZFL singly-linked list class

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

#ifndef __ZFL_LIST_H_INCLUDED__
#define __ZFL_LIST_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zlist zlist_t;

zlist_t *
    zlist_new (void);
void
    zlist_destroy (zlist_t **self_p);
void *
    zlist_first (zlist_t *self);
void *
    zlist_next (zlist_t *self);
void
    zlist_append (zlist_t *self, void *value);
void
    zlist_push (zlist_t *self, void *value);
void *
    zlist_pop (zlist_t *self);
void
    zlist_remove (zlist_t *self, void *value);
zlist_t *
    zlist_copy (zlist_t *self);
size_t
    zlist_size (zlist_t *self);
void
    zlist_test (int verbose);

#ifdef __cplusplus
}
#endif

#endif
