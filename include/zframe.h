/*  =========================================================================
    zframe - working with single message frames

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

//  Create a new frame with optional size, and optional data
CZMQ_EXPORT zframe_t *
    zframe_new (const void *data, size_t size);

//  Create an empty (zero-sized) frame
CZMQ_EXPORT zframe_t *
    zframe_new_empty (void);

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

// Send a frame to a socket, destroy frame after sending.
// Return -1 on error, 0 on success.
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

//  Return frame MORE indicator (1 or 0), set when reading frame from socket
//  or by the zframe_set_more() method
CZMQ_EXPORT int
    zframe_more (zframe_t *self);

//  Set frame MORE indicator (1 or 0). Note this is NOT used when sending 
//  frame to socket, you have to specify flag explicitly.
CZMQ_EXPORT void
    zframe_set_more (zframe_t *self, int more);
    
//  Return TRUE if two frames have identical size and data
//  If either frame is NULL, equality is always false.
CZMQ_EXPORT bool
    zframe_eq (zframe_t *self, zframe_t *other);

//   Print contents of the frame to FILE stream.
CZMQ_EXPORT void
    zframe_fprint (zframe_t *self, const char *prefix, FILE *file);

//  Print contents of frame to stderr
CZMQ_EXPORT void
    zframe_print (zframe_t *self, const char *prefix);

//  Set new contents for frame
CZMQ_EXPORT void
    zframe_reset (zframe_t *self, const void *data, size_t size);

//  Put a block of data to the frame payload.
CZMQ_EXPORT int
    zframe_put_block (zframe_t *self, byte *data, size_t size);

//  Self test of this class
CZMQ_EXPORT int
    zframe_test (bool verbose);
//  @end

//  Deprecated method aliases
#define zframe_print_to_stream(s,p,F) zframe_fprint(s,p,F)

#ifdef __cplusplus
}
#endif

#endif
