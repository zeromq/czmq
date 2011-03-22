/*  =========================================================================
    zmsg - working with multipart messages

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of zapi, the C binding for 0MQ: http://zapi.zeromq.org.

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

#ifndef __ZMSG_H_INCLUDED__
#define __ZMSG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zmsg_t zmsg_t;

zmsg_t *
    zmsg_new (void);
void
    zmsg_destroy (zmsg_t **self_p);
zmsg_t *
    zmsg_recv (void *socket);
void
    zmsg_send (zmsg_t **self_p, void *socket);
size_t
    zmsg_size (zmsg_t *self);
void
    zmsg_push (zmsg_t *self, zframe_t *frame);
void
    zmsg_append (zmsg_t *self, zframe_t *frame);
void
    zmsg_pushmem (zmsg_t *self, const void *src, size_t size);
void
    zmsg_appendmem (zmsg_t *self, const void *src, size_t size);
zframe_t *
    zmsg_pop (zmsg_t *self);
void
    zmsg_remove (zmsg_t *self, zframe_t *frame);
zframe_t *
    zmsg_first (zmsg_t *self);
zframe_t *
    zmsg_next (zmsg_t *self);
zframe_t *
    zmsg_body (zmsg_t *self);
void
    zmsg_save (zmsg_t *self, FILE *file);
zmsg_t *
    zmsg_load (FILE *file);
void
    zmsg_dump (zmsg_t *self);
int
    zmsg_test (Bool verbose);

#ifdef __cplusplus
}
#endif

#endif
