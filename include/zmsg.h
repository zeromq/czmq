/*  =========================================================================
    zmsg - working with multipart messages

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMSG_H_INCLUDED__
#define __ZMSG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new empty message object
CZMQ_EXPORT zmsg_t *
    zmsg_new (void);

//  Destroy a message object and all frames it contains
CZMQ_EXPORT void
    zmsg_destroy (zmsg_t **self_p);

//  Receive message from socket, returns zmsg_t object or NULL if the recv
//  was interrupted. Does a blocking recv. If you want to not block then use
//  the zloop class or zmsg_recv_nowait or zmq_poll to check for socket input
//  before receiving.
CZMQ_EXPORT zmsg_t *
    zmsg_recv (void *source);

//  Send message to destination socket, and destroy the message after sending
//  it successfully. If the message has no frames, sends nothing but destroys
//  the message anyhow. Nullifies the caller's reference to the message (as
//  it is a destructor).
CZMQ_EXPORT int
    zmsg_send (zmsg_t **self_p, void *dest);

//  Return size of message, i.e. number of frames (0 or more).
CZMQ_EXPORT size_t
    zmsg_size (zmsg_t *self);

//  Return total size of all frames in message.
CZMQ_EXPORT size_t
    zmsg_content_size (zmsg_t *self);

//  Push frame to the front of the message, i.e. before all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success, -1 on error. Deprecates zmsg_push, which did not
//  nullify the caller's frame reference.
CZMQ_EXPORT int
    zmsg_prepend (zmsg_t *self, zframe_t **frame_p);

//  Add frame to the end of the message, i.e. after all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success. Deprecates zmsg_add, which did not nullify the
//  caller's frame reference.
CZMQ_EXPORT int
    zmsg_append (zmsg_t *self, zframe_t **frame_p);

//  Remove first frame from message, if any. Returns frame, or NULL. Caller
//  now owns frame and must destroy it when finished with it.
CZMQ_EXPORT zframe_t *
    zmsg_pop (zmsg_t *self);

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
    zmsg_pushstr (zmsg_t *self, const char *string);

//  Push string as new frame to end of message.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_addstr (zmsg_t *self, const char *string);

//  Push formatted string as new frame to front of message.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_pushstrf (zmsg_t *self, const char *format, ...);

//  Push formatted string as new frame to end of message.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_addstrf (zmsg_t *self, const char *format, ...);

//  Pop frame off front of message, return as fresh string. If there were
//  no more frames in the message, returns NULL.
CZMQ_EXPORT char *
    zmsg_popstr (zmsg_t *self);

//  Push encoded message as a new frame. Message takes ownership of
//  submessage, so the original is destroyed in this call. Returns 0 on
//  success, -1 on error.
CZMQ_EXPORT int
    zmsg_addmsg (zmsg_t *self, zmsg_t **msg_p);

//  Remove first submessage from message, if any. Returns zmsg_t, or NULL if
//  decoding was not succesfull. Caller now owns message and must destroy it
//  when finished with it.
CZMQ_EXPORT zmsg_t *
    zmsg_popmsg (zmsg_t *self);

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
//  object. If message is null, or memory was exhausted, returns null.
CZMQ_EXPORT zmsg_t *
    zmsg_dup (zmsg_t *self);

//  Send message to zsys log sink (may be stdout, or system facility as
//  configured by zsys_set_logstream).
CZMQ_EXPORT void
    zmsg_print (zmsg_t *self);

//  Probe the supplied object, and report if it looks like a zmsg_t.
CZMQ_EXPORT bool
    zmsg_is (void *self);

//  Self test of this class
CZMQ_EXPORT void
    zmsg_test (bool verbose);
//  @end

//  DEPRECATED as over-engineered, poor style
//  Pop frame off front of message, caller now owns frame
//  If next frame is empty, pops and destroys that empty frame.
CZMQ_EXPORT zframe_t *
    zmsg_unwrap (zmsg_t *self);

//  DEPRECATED as poor style -- callers should use zloop or zpoller
//  Receive message from socket, returns zmsg_t object, or NULL either if
//  there was no input waiting, or the recv was interrupted.
CZMQ_EXPORT zmsg_t *
    zmsg_recv_nowait (void *source);

//  DEPRECATED as unsafe -- does not nullify frame reference.
//  Push frame plus empty frame to front of message, before first frame.
//  Message takes ownership of frame, will destroy it when message is sent.
CZMQ_EXPORT void
    zmsg_wrap (zmsg_t *self, zframe_t *frame);

//  DEPRECATED - will be removed for next + 1 stable release
//  Add frame to the front of the message, i.e. before all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success, -1 on error.
CZMQ_EXPORT int
    zmsg_push (zmsg_t *self, zframe_t *frame);

//  DEPRECATED - will be removed for next stable release
CZMQ_EXPORT int
    zmsg_add (zmsg_t *self, zframe_t *frame);

//  DEPRECATED as inconsistent; breaks principle that logging should all go
//  to a single destination.
//  Print message to open stream
//  Truncates to first 10 frames, for readability.
CZMQ_EXPORT void
    zmsg_fprint (zmsg_t *self, FILE *file);

//  Compiler hints
CZMQ_EXPORT int zmsg_addstrf (zmsg_t *self, const char *format, ...) CHECK_PRINTF (2);
CZMQ_EXPORT int zmsg_pushstrf (zmsg_t *self, const char *format, ...) CHECK_PRINTF (2);

#ifdef __cplusplus
}
#endif

//  Deprecated method aliases
#define zmsg_dump(s) zmsg_print(s)
#define zmsg_dump_to_stream(s,F) zmsg_fprint(s,F)

#endif
