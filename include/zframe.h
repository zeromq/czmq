/*  =========================================================================
    zframe - working with single message frames

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZFRAME_H_INCLUDED__
#define __ZFRAME_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zframe_t zframe_t;

//  @interface
#define ZFRAME_MORE     1
#define ZFRAME_REUSE    2
#define ZFRAME_DONTWAIT 4

//  Callback function for zframe_free_fn method
typedef void (zframe_free_fn) (void *data, void *arg);

//  Create a new frame with optional size, and optional data
CZMQ_EXPORT zframe_t *
    zframe_new (const void *data, size_t size);

//  Create a zero-copy frame
CZMQ_EXPORT zframe_t *
    zframe_new_zero_copy (void *data, size_t size,
                          zframe_free_fn *free_fn, void *arg);

//  Destroy a frame
CZMQ_EXPORT void
    zframe_destroy (zframe_t **self_p);

//  Receive frame from socket, returns zframe_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  zframe_recv_nowait().
CZMQ_EXPORT zframe_t *
    zframe_recv (void *socket);

//  Receive a new frame off the socket. Returns newly allocated frame, or
//  NULL if there was no input waiting, or if the read was interrupted.
CZMQ_EXPORT zframe_t *
    zframe_recv_nowait (void *socket);

// Send a frame to a socket, destroy frame after sending.  Returns
// non-zero error code on failure.
CZMQ_EXPORT int
    zframe_send (zframe_t **self_p, void *socket, int flags);

//  Return number of bytes in frame data
CZMQ_EXPORT size_t
    zframe_size (zframe_t *self);

//  Return address of frame data
CZMQ_EXPORT byte *
    zframe_data (zframe_t *self);

//  Create a new frame that duplicates an existing frame
CZMQ_EXPORT zframe_t *
    zframe_dup (zframe_t *self);

//  Return frame data encoded as printable hex string
CZMQ_EXPORT char *
    zframe_strhex (zframe_t *self);

//  Return frame data copied into freshly allocated string
CZMQ_EXPORT char *
    zframe_strdup (zframe_t *self);

//  Return TRUE if frame body is equal to string, excluding terminator
CZMQ_EXPORT bool
    zframe_streq (zframe_t *self, const char *string);

// Return frame zero copy indicator (1 or 0)
CZMQ_EXPORT int
    zframe_zero_copy (zframe_t *self);

//  Return frame 'more' property
CZMQ_EXPORT int
    zframe_more (const zframe_t *self);

//  Return TRUE if two frames have identical size and data
//  If either frame is NULL, equality is always false.
CZMQ_EXPORT bool
    zframe_eq (zframe_t *self, zframe_t *other);

//  Print contents of frame to stderr
CZMQ_EXPORT void
    zframe_print (zframe_t *self, const char *prefix);

//  Set new contents for frame
CZMQ_EXPORT void
    zframe_reset (zframe_t *self, const void *data, size_t size);

//  Set the free callback for frame
CZMQ_EXPORT void
    zframe_freefn(zframe_t *self, zframe_free_fn *free_fn, void *arg);

//  Self test of this class
CZMQ_EXPORT int
    zframe_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
