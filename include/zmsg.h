/*  =========================================================================
    zmsg - working with multipart messages

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

#ifndef __ZMSG_H_INCLUDED__
#define __ZMSG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zmsg_t zmsg_t;

//  @interface
//  Create a new empty message object
CZMQ_EXPORT zmsg_t *
    zmsg_new (void);

//  Destroy a message object and all frames it contains
CZMQ_EXPORT void
    zmsg_destroy (zmsg_t **self_p);

//  Receive message from socket, returns zmsg_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  the zloop class or zmq_poll to check for socket input before receiving.
CZMQ_EXPORT zmsg_t *
    zmsg_recv (void *socket);

//  Send message to socket, destroy after sending. If the message has no
//  frames, sends nothing but destroys the message anyhow. Safe to call
//  if zmsg is null.
CZMQ_EXPORT int
    zmsg_send (zmsg_t **self_p, void *socket);

//  Return size of message, i.e. number of frames (0 or more).
CZMQ_EXPORT size_t
    zmsg_size (zmsg_t *self);

//  Return total size of all frames in message.
CZMQ_EXPORT size_t
    zmsg_content_size (zmsg_t *self);

//  Push frame to the front of the message, i.e. before all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_push (zmsg_t *self, zframe_t *frame);

//  Remove first frame from message, if any. Returns frame, or NULL. Caller
//  now owns frame and must destroy it when finished with it.
CZMQ_EXPORT zframe_t *
    zmsg_pop (zmsg_t *self);

//  Add frame to the end of the message, i.e. after all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success. Deprecates zmsg_add, which did not nullify the
//  caller's frame reference.
CZMQ_EXPORT int
    zmsg_append (zmsg_t *self, zframe_t **frame_p);

//  Add frame to the end of the message, i.e. after all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success.
//  DEPRECATED - will be removed for next stable release
CZMQ_EXPORT int
    zmsg_add (zmsg_t *self, zframe_t *frame);

//  Push block of memory to front of message, as a new frame.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_pushmem (zmsg_t *self, const void *src, size_t size);

//  Add block of memory to the end of the message, as a new frame.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_addmem (zmsg_t *self, const void *src, size_t size);
    
//  Push string as new frame to front of message.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_pushstr (zmsg_t *self, const char *format, ...);

//  Push string as new frame to end of message.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_addstr (zmsg_t *self, const char *format, ...);

//  Pop frame off front of message, return as fresh string. If there were
//  no more frames in the message, returns NULL.
CZMQ_EXPORT char *
    zmsg_popstr (zmsg_t *self);

//  Push frame plus empty frame to front of message, before first frame.
//  Message takes ownership of frame, will destroy it when message is sent.
CZMQ_EXPORT void
    zmsg_wrap (zmsg_t *self, zframe_t *frame);

//  Pop frame off front of message, caller now owns frame
//  If next frame is empty, pops and destroys that empty frame.
CZMQ_EXPORT zframe_t *
    zmsg_unwrap (zmsg_t *self);

//  Remove specified frame from list, if present. Does not destroy frame.
CZMQ_EXPORT void
    zmsg_remove (zmsg_t *self, zframe_t *frame);

//  Set cursor to first frame in message. Returns frame, or NULL, if the 
//  message is empty. Use this to navigate the frames as a list.
CZMQ_EXPORT zframe_t *
    zmsg_first (zmsg_t *self);

//  Return the next frame. If there are no more frames, returns NULL. To move
//  to the first frame call zmsg_first(). Advances the cursor.
CZMQ_EXPORT zframe_t *
    zmsg_next (zmsg_t *self);

//  Return the last frame. If there are no frames, returns NULL.
CZMQ_EXPORT zframe_t *
    zmsg_last (zmsg_t *self);

//  Save message to an open file, return 0 if OK, else -1. The message is 
//  saved as a series of frames, each with length and data. Note that the
//  file is NOT guaranteed to be portable between operating systems, not
//  versions of CZMQ. The file format is at present undocumented and liable
//  to arbitrary change.
CZMQ_EXPORT int
    zmsg_save (zmsg_t *self, FILE *file);

//  Load/append an open file into message, create new message if
//  null message provided. Returns NULL if the message could not 
//  be loaded.
CZMQ_EXPORT zmsg_t *
    zmsg_load (zmsg_t *self, FILE *file);

//  Serialize multipart message to a single buffer. Use this method to send
//  structured messages across transports that do not support multipart data.
//  Allocates and returns a new buffer containing the serialized message.
//  To decode a serialized message buffer, use zmsg_decode ().
CZMQ_EXPORT size_t
    zmsg_encode (zmsg_t *self, byte **buffer);

//  Decodes a serialized message buffer created by zmsg_encode () and returns
//  a new zmsg_t object. Returns NULL if the buffer was badly formatted or 
//  there was insufficient memory to work.
CZMQ_EXPORT zmsg_t *
    zmsg_decode (byte *buffer, size_t buffer_size);

//  Create copy of message, as new message object. Returns a fresh zmsg_t
//  object, or NULL if there was not enough heap memory.
CZMQ_EXPORT zmsg_t *
    zmsg_dup (zmsg_t *self);

//  Dump message to stderr, for debugging and tracing.
//  See zmsg_dump_to_stream() for details
CZMQ_EXPORT void
    zmsg_dump (zmsg_t *self);

//  Dump message to FILE stream, for debugging and tracing. 
//  Truncates to first 10 frames, for readability; this may be unfortunate
//  when debugging larger and more complex messages.
CZMQ_EXPORT void
    zmsg_dump_to_stream (zmsg_t *self, FILE *file);

//  Self test of this class
CZMQ_EXPORT int
    zmsg_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
