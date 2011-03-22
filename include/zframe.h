/*  =========================================================================
    zframe - working with 0MQ contexts

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

#ifndef __ZFRAME_H_INCLUDED__
#define __ZFRAME_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zframe_t zframe_t;

#define ZFRAME_MORE     1
#define ZFRAME_REUSE    2
#define ZFRAME_NOWAIT   4

zframe_t *
    zframe_new (const void *data, size_t size);
void
    zframe_destroy (zframe_t **self_p);
zframe_t *
    zframe_recv (void *socket, int flags);
void
    zframe_send (zframe_t **self_p, void *socket, int flags);
size_t
    zframe_size (zframe_t *self);
void *
    zframe_data (zframe_t *self);
int
    zframe_more (zframe_t *self);
int
    zframe_test (int verbose);

#ifdef __cplusplus
}
#endif

#endif
