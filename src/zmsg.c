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

/*
@header
    The zmsg class provides methods to send and receive multipart messages
    across 0MQ sockets. This class provides a list-like container interface,
    with methods to work with the overall container. zmsg_t messages are
    composed of zero or more zframe_t frames.
@discuss
@end
*/

#include "czmq_classes.h"

//  zmsg_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZMSG_TAG            0x0003cafe

//  Structure of our class

struct _zmsg_t {
    uint32_t tag;               //  Object tag for runtime detection
    zlist_t *frames;            //  List of frames
    size_t content_size;        //  Total content size
    uint32_t routing_id;        //  Routing ID back to sender, if any
};


//  --------------------------------------------------------------------------
//  Constructor

zmsg_t *
zmsg_new (void)
{
    zmsg_t *self = (zmsg_t *) zmalloc (sizeof (zmsg_t));
    assert (self);
    self->tag = ZMSG_TAG;
    self->frames = zlist_new ();
    assert (self->frames);
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zmsg_destroy (zmsg_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmsg_t *self = *self_p;
        assert (zmsg_is (self));
        zframe_t *frame;
        while ((frame = (zframe_t *) zlist_pop (self->frames)))
            zframe_destroy (&frame);
        zlist_destroy (&self->frames);
        self->tag = 0xDeadBeef;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive a message from a socket. The socket may be a libzmq socket, or a
//  zsock_t object. Returns a new zmsg_t object if successful, or NULL if the
//  receive was interrupted. The recv call blocks: if you want a non-blocking
//  call, use the zloop or zpoller class to receive only from ready sockets.
//  Keeps retrying if interrupted in a middle of a multipart sequence.

zmsg_t *
zmsg_recv (void *source)
{
    assert (source);
    zmsg_t *self = zmsg_new ();
    if (!self)
        return NULL;

    while (true) {
        zframe_t *frame = zframe_recv (source);
        if (!frame) {
            if (errno == EINTR && zlist_head (self->frames))
                continue;
            else {
                zmsg_destroy (&self);
                break;              //  Interrupted or terminated
            }
        }
#if defined (ZMQ_SERVER)
        //  Grab routing ID if we're reading from a SERVER socket (ZMQ 4.2 and later)
        if (zsock_type (source) == ZMQ_SERVER)
            self->routing_id = zframe_routing_id (frame);
#endif
        if (zmsg_append (self, &frame)) {
            zmsg_destroy (&self);
            break;
        }
        if (!zsock_rcvmore (source))
            break;              //  Last message frame
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Send message to destination socket, and destroy the message after sending
//  it successfully. If the message has no frames, sends nothing but destroys
//  the message anyhow. Nullifies the caller's reference to the message (as
//  it is a destructor).
//  Keeps retrying if interrupted in a middle of a multipart sequence.

int
zmsg_send (zmsg_t **self_p, void *dest)
{
    assert (self_p);
    assert (dest);
    zmsg_t *self = *self_p;

    int rc = 0;
    if (self) {
        assert (zmsg_is (self));
        bool sent_some = false;
        zframe_t *frame;
        while ((frame = (zframe_t *) zlist_head (self->frames))) {
            zframe_set_routing_id (frame, self->routing_id);
            rc = zframe_send (&frame, dest,
                              zlist_size (self->frames) > 1? ZFRAME_MORE: 0);
            if (rc != 0) {
                if (errno == EINTR && sent_some)
                    continue;
                else
                    break;
            }
            sent_some = true;
            (void) zlist_pop (self->frames);
        }
        if (rc == 0)
            zmsg_destroy (self_p);
    }
    return rc;
}

//  --------------------------------------------------------------------------
//  Send message to destination socket as part of a multipart sequence, and
//  destroy the message after sending it successfully. Note that after a
//  zmsg_sendm, you must call zmsg_send or another method that sends a final
//  message part. If the message has no frames, sends nothing but destroys
//  the message anyhow. Nullifies the caller's reference to the message (as
//  it is a destructor).
//  Keeps retrying if interrupted in a middle of a multipart sequence.

int
zmsg_sendm (zmsg_t **self_p, void *dest)
{
    assert (self_p);
    assert (dest);
    zmsg_t *self = *self_p;

    int rc = 0;
    if (self) {
        assert (zmsg_is (self));
        bool sent_some = false;
        zframe_t *frame;
        while ((frame = (zframe_t *) zlist_head (self->frames))) {
            zframe_set_routing_id (frame, self->routing_id);
            rc = zframe_send (&frame, dest, ZFRAME_MORE);
            if (rc != 0) {
                if (errno == EINTR && sent_some)
                    continue;
                else
                    break;
            }
            sent_some = true;
            (void) zlist_pop (self->frames);
        }
        if (rc == 0)
            zmsg_destroy (self_p);
    }
    return rc;
}

//  --------------------------------------------------------------------------
//  Return size of message, i.e. number of frames (0 or more).

size_t
zmsg_size (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));

    return zlist_size (self->frames);
}


//  --------------------------------------------------------------------------
//  Return total size of all frames in message.

size_t
zmsg_content_size (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));
    return self->content_size;
}


//  --------------------------------------------------------------------------
//  Return message routing ID, if the message came from a ZMQ_SERVER socket.
//  Else returns zero.

uint32_t
zmsg_routing_id (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));
    return self->routing_id;
}


//  --------------------------------------------------------------------------
//  Set routing ID on message. This is used if/when the message is sent to a
//  ZMQ_SERVER socket.

void
zmsg_set_routing_id (zmsg_t *self, uint32_t routing_id)
{
    assert (self);
    assert (zmsg_is (self));
    self->routing_id = routing_id;
}


//  --------------------------------------------------------------------------
//  Push frame to the front of the message, i.e. before all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success, -1 on error. Deprecates zmsg_push, which did not
//  nullify the caller's frame reference.

int
zmsg_prepend (zmsg_t *self, zframe_t **frame_p)
{
    assert (self);
    assert (zmsg_is (self));
    assert (frame_p);

    zframe_t *frame = *frame_p;
    *frame_p = NULL;            //  We now own frame
    self->content_size += zframe_size (frame);
    zlist_push (self->frames, frame);
    return 0;
}


//  --------------------------------------------------------------------------
//  Add frame to the end of the message, i.e. after all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success. Nullifies caller's reference to frame.

int
zmsg_append (zmsg_t *self, zframe_t **frame_p)
{
    assert (self);
    assert (zmsg_is (self));
    assert (frame_p);

    zframe_t *frame = *frame_p;
    *frame_p = NULL;            //  We now own frame
    self->content_size += zframe_size (frame);
    zlist_append (self->frames, frame);
    return 0;
}


//  --------------------------------------------------------------------------
//  Remove first frame from message, if any. Returns frame, or NULL. Caller
//  now owns frame and must destroy it when finished with it.

zframe_t *
zmsg_pop (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));

    zframe_t *frame = (zframe_t *) zlist_pop (self->frames);
    if (frame)
        self->content_size -= zframe_size (frame);

    return frame;
}


//  --------------------------------------------------------------------------
//  Push block of memory to front of message, as a new frame.
//  Returns 0 on success, -1 on error.

int
zmsg_pushmem (zmsg_t *self, const void *data, size_t size)
{
    assert (self);
    assert (zmsg_is (self));

    zframe_t *frame = zframe_new (data, size);
    assert (frame);
    self->content_size += size;
    zlist_push (self->frames, frame);
    return 0;
}

//  --------------------------------------------------------------------------
//  Add block of memory to the end of the message, as a new frame.
//  Returns 0 on success, -1 on error.

int
zmsg_addmem (zmsg_t *self, const void *data, size_t size)
{
    assert (self);
    assert (zmsg_is (self));

    zframe_t *frame = zframe_new (data, size);
    assert (frame);
    self->content_size += size;
    zlist_append (self->frames, frame);
    return 0;
}


//  --------------------------------------------------------------------------
//  Push string as new frame to front of message.
//  Returns 0 on success, -1 on error.

int
zmsg_pushstr (zmsg_t *self, const char *string)
{
    assert (self);
    assert (zmsg_is (self));
    assert (string);

    size_t len = strlen (string);
    zframe_t *frame = zframe_new (string, len);
    assert (frame);
    self->content_size += len;
    zlist_push (self->frames, frame);
    return 0;
}


//  --------------------------------------------------------------------------
//  Push string as new frame to end of message.
//  Returns 0 on success, -1 on error.

int
zmsg_addstr (zmsg_t *self, const char *string)
{
    assert (self);
    assert (zmsg_is (self));
    assert (string);

    size_t len = strlen (string);
    zframe_t *frame = zframe_new (string, len);
    assert (frame);
    self->content_size += len;
    zlist_append (self->frames, frame);
    return 0;
}


//  --------------------------------------------------------------------------
//  Push formatted string as new frame to front of message.
//  Returns 0 on success, -1 on error.

int
zmsg_pushstrf (zmsg_t *self, const char *format, ...)
{
    assert (self);
    assert (zmsg_is (self));
    assert (format);

    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (!string)
        return -1;

    size_t len = strlen (string);
    zframe_t *frame = zframe_new (string, len);
    assert (frame);
    self->content_size += len;
    zlist_push (self->frames, frame);
    zstr_free (&string);
    return 0;
}


//  --------------------------------------------------------------------------
//  Push formatted string as new frame to end of message.
//  Returns 0 on success, -1 on error.

int
zmsg_addstrf (zmsg_t *self, const char *format, ...)
{
    assert (self);
    assert (zmsg_is (self));
    assert (format);

    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    if (!string)
        return -1;

    size_t len = strlen (string);
    zframe_t *frame = zframe_new (string, len);
    assert (frame);
    self->content_size += len;
    zlist_append (self->frames, frame);
    zstr_free (&string);
    return 0;
}


//  --------------------------------------------------------------------------
//  Pop frame off front of message, return as fresh string. If there were
//  no more frames in the message, returns NULL.

char *
zmsg_popstr (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));

    zframe_t *frame = (zframe_t *) zlist_pop (self->frames);
    char *string = NULL;
    if (frame) {
        self->content_size -= zframe_size (frame);
        string = zframe_strdup (frame);
        zframe_destroy (&frame);
    }
    return string;
}


//  --------------------------------------------------------------------------
//  Push encoded message as a new frame. Message takes ownership of
//  submessage, so the original is destroyed in this call. Returns 0 on
//  success, -1 on error.

int
zmsg_addmsg (zmsg_t *self, zmsg_t **msg_p)
{
    assert (self);
    assert (zmsg_is (self));
    assert (msg_p);

    zframe_t *frame = zmsg_encode (*msg_p);
    zmsg_append (self, &frame);
    zmsg_destroy (msg_p);
    return 0;
}


//  --------------------------------------------------------------------------
//  Remove first submessage from message, if any. Returns zmsg_t, or NULL if
//  decoding was not successful. Caller now owns message and must destroy it
//  when finished with it.

zmsg_t *
zmsg_popmsg (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));

    zframe_t *frame = zmsg_pop (self);
    if (!frame)
        return NULL;
    zmsg_t *msg = zmsg_decode (frame);
    zframe_destroy (&frame);
    return msg;
}


//  --------------------------------------------------------------------------
//  Remove specified frame from list, if present. Does not destroy frame.

void
zmsg_remove (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (zmsg_is (self));

    self->content_size -= zframe_size (frame);
    zlist_remove (self->frames, frame);
}


//  --------------------------------------------------------------------------
//  Set cursor to first frame in message. Returns frame, or NULL, if the
//  message is empty. Use this to navigate the frames as a list.

zframe_t *
zmsg_first (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));
    return (zframe_t *) zlist_first (self->frames);
}


//  --------------------------------------------------------------------------
//  Return the next frame. If there are no more frames, returns NULL. To move
//  to the first frame call zmsg_first(). Advances the cursor.

zframe_t *
zmsg_next (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));
    return (zframe_t *) zlist_next (self->frames);
}


//  --------------------------------------------------------------------------
//  Return the last frame. If there are no frames, returns NULL.

zframe_t *
zmsg_last (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));
    return (zframe_t *) zlist_last (self->frames);
}


//  --------------------------------------------------------------------------
//  Save message to an open file, return 0 if OK, else -1. The message is
//  saved as a series of frames, each with length and data. Note that the
//  file is NOT guaranteed to be portable between operating systems, not
//  versions of CZMQ. The file format is at present undocumented and liable
//  to arbitrary change.

int
zmsg_save (zmsg_t *self, FILE *file)
{
    assert (self);
    assert (zmsg_is (self));
    assert (file);

    zframe_t *frame = zmsg_first (self);
    while (frame) {
        size_t frame_size = zframe_size (frame);
        if (fwrite (&frame_size, sizeof (frame_size), 1, file) != 1)
            return -1;
        if (fwrite (zframe_data (frame), frame_size, 1, file) != 1)
            return -1;
        frame = zmsg_next (self);
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Load/append an open file into new message, return the message.
//  Returns NULL if the message could not be loaded.


zmsg_t *
zmsg_load (FILE *file)
{
    zmsg_t *self = zmsg_new ();
    assert (self);
    assert (file);

    while (true) {
        size_t frame_size;
        if (fread (&frame_size, sizeof (frame_size), 1, file) == 1) {
            zframe_t *frame = zframe_new (NULL, frame_size);
            if (fread (zframe_data (frame), frame_size, 1, file) == 1
            &&  frame_size > 0)
                zmsg_append (self, &frame);
            else {
                zframe_destroy (&frame);
                zmsg_destroy (&self);
                break;
            }
        }
        else
            break;              //  Unable to read properly, quit
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Serialize multipart message to a single message frame. Use this method
//  to send structured messages across transports that do not support
//  multipart data. Allocates and returns a new frame containing the
//  serialized message. To decode a serialized message frame, use
//  zmsg_decode ().
//
//  Frame lengths are encoded as 1 or 1+4 bytes
//  0..254 bytes        octet + data
//  255..4Gb-1 bytes    0xFF + 4octet + data

zframe_t *
zmsg_encode (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));

    //  Calculate real size of frame
    size_t total_size = 0;
    zframe_t *frame = zmsg_first (self);
    while (frame) {
        size_t frame_size = zframe_size (frame);
        if (frame_size < 255)
            total_size += frame_size + 1;
        else
            total_size += frame_size + 1 + 4;
        frame = zmsg_next (self);
    }
    zframe_t *encoded = zframe_new (NULL, total_size);
    assert (encoded);

    //  Encode message into frame
    byte *dest = zframe_data (encoded);
    frame = zmsg_first (self);
    while (frame) {
        size_t frame_size = zframe_size (frame);
        if (frame_size < 255) {
            *dest++ = (byte) frame_size;
            memcpy (dest, zframe_data (frame), frame_size);
            dest += frame_size;
        }
        else {
            *dest++ = 0xFF;
            *dest++ = (frame_size >> 24) & 255;
            *dest++ = (frame_size >> 16) & 255;
            *dest++ = (frame_size >>  8) & 255;
            *dest++ =  frame_size        & 255;
            memcpy (dest, zframe_data (frame), frame_size);
            dest += frame_size;
        }
        frame = zmsg_next (self);
    }
    assert ((size_t) (dest - zframe_data (encoded)) == total_size);
    return encoded;
}


//  --------------------------------------------------------------------------
//  Decodes a serialized message frame created by zmsg_encode () and returns
//  a new zmsg_t object. Returns NULL if the frame was badly formatted or
//  there was insufficient memory to work.

zmsg_t *
zmsg_decode (zframe_t *frame)
{
    assert (frame);
    zmsg_t *self = zmsg_new ();
    assert (self);

    const byte *source = zframe_data (frame);
    const byte *limit = zframe_data (frame) + zframe_size (frame);
    while (source < limit) {
        size_t frame_size = *source++;
        if (frame_size == 255) {
            if (source > limit - 4) {
                zmsg_destroy (&self);
                break;
            }
            frame_size = (source [0] << 24)
                       + (source [1] << 16)
                       + (source [2] << 8)
                       +  source [3];
            source += 4;
        }
        if (source > limit - frame_size) {
            zmsg_destroy (&self);
            break;
        }
        zframe_t *decoded = zframe_new (source, frame_size);
        assert (decoded);
        zmsg_append (self, &decoded);
        source += frame_size;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Create copy of message, as new message object. Returns a fresh zmsg_t
//  object. If message is null, or memory was exhausted, returns null.

zmsg_t *
zmsg_dup (zmsg_t *self)
{
    if (self) {
        assert (zmsg_is (self));
        zmsg_t *copy = zmsg_new ();
        assert (copy);
        zframe_t *frame = zmsg_first (self);
        while (frame) {
            zmsg_addmem (copy, zframe_data (frame), zframe_size (frame));
            frame = zmsg_next (self);
        }
        return copy;
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Send message to zsys log sink (may be stdout, or system facility as
//  configured by zsys_set_logstream).

void
zmsg_print (zmsg_t *self)
{
    assert (self);
    assert (zmsg_is (self));

    if (!self) {
        zsys_debug ("(NULL)");
        return;
    }
    zframe_t *frame = zmsg_first (self);
    while (frame) {
        zframe_print (frame, NULL);
        frame = zmsg_next (self);
    }
}


//  --------------------------------------------------------------------------
//  Return true if the two messages have the same number of frames and each
//  frame in the first message is identical to the corresponding frame in the
//  other message. As with zframe_eq, return false if either message is NULL.

bool
zmsg_eq (zmsg_t *self, zmsg_t *other)
{
    if (!self || !other)
        return false;

    if (zlist_size (self->frames) != zlist_size (other->frames))
        return false;

    zframe_t *self_frame = (zframe_t *) zlist_first (self->frames);
    zframe_t *other_frame = (zframe_t *) zlist_first (other->frames);
    while (self_frame && other_frame) {
        if (!zframe_eq (self_frame, other_frame))
            return false;
        self_frame = (zframe_t *) zlist_next (self->frames);
        other_frame = (zframe_t *) zlist_next (other->frames);
    }
    return true;
}


//  --------------------------------------------------------------------------
//  Generate a signal message encoding the given status. A signal is a short
//  message carrying a 1-byte success/failure code (by convention, 0 means
//  OK). Signals are encoded to be distinguishable from "normal" messages.

zmsg_t *
zmsg_new_signal (byte status)
{
    zmsg_t *self = zmsg_new ();
    int64_t signal_value = 0x7766554433221100L + status;
    zmsg_addmem (self, &signal_value, 8);
    return self;
}


//  --------------------------------------------------------------------------
//  Return signal value, 0 or greater, if message is a signal, -1 if not.

int
zmsg_signal (zmsg_t *self)
{
    if (zmsg_size (self) == 1
    &&  zmsg_content_size (self) == 8) {
        zframe_t *frame = zmsg_first (self);
        int64_t signal_value = *((int64_t *) zframe_data (frame));
        if ((signal_value & 0xFFFFFFFFFFFFFF00L) == 0x7766554433221100L)
            return signal_value & 255;
    }
    return -1;
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zmsg_t.

bool
zmsg_is (void *self)
{
    assert (self);
    return ((zmsg_t *) self)->tag == ZMSG_TAG;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as over-engineered, poor style
//  Pop frame off front of message, caller now owns frame
//  If next frame is empty, pops and destroys that empty frame.

zframe_t *
zmsg_unwrap (zmsg_t *self)
{
    assert (self);
    zframe_t *frame = zmsg_pop (self);
    zframe_t *empty = zmsg_first (self);
    if (empty && zframe_size (empty) == 0) {
        empty = zmsg_pop (self);
        zframe_destroy (&empty);
    }
    return frame;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as poor style -- callers should use zloop or zpoller
//  Receive message from socket, returns zmsg_t object, or NULL either if
//  there was no input waiting, or the recv was interrupted.

zmsg_t *
zmsg_recv_nowait (void *source)
{
    assert (source);
    zmsg_t *self = zmsg_new ();
    if (!self)
        return NULL;

    while (true) {
        zframe_t *frame = zframe_recv_nowait (source);
        if (!frame) {
            zmsg_destroy (&self);
            break;              //  Interrupted or terminated
        }
        if (zmsg_append (self, &frame)) {
            zmsg_destroy (&self);
            break;
        }
        if (!zsock_rcvmore (source))
            break;              //  Last message frame
    }
    return self;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as unsafe -- does not nullify frame reference.
//  Push frame plus empty frame to front of message, before first frame.
//  Message takes ownership of frame, will destroy it when message is sent.

void
zmsg_wrap (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    if (zmsg_pushmem (self, "", 0) == 0)
        zmsg_push (self, frame);
}


//  --------------------------------------------------------------------------
//  DEPRECATED by zmsg_prepend ().
//  Push frame to the front of the message, i.e. before all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success, -1 on error.

int
zmsg_push (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    self->content_size += zframe_size (frame);
    zlist_push (self->frames, frame);
    return 0;
}


//  --------------------------------------------------------------------------
//  DEPRECATED by zmsg_append ().
//  Add frame to the end of the message, i.e. after all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success.

int
zmsg_add (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    self->content_size += zframe_size (frame);
    zlist_append (self->frames, frame);
    return 0;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as inconsistent; breaks principle that logging should all go
//  to a single destination.
//  Dump message to FILE stream, for debugging and tracing.
//  Truncates to first 10 frames, for readability.

void
zmsg_fprint (zmsg_t *self, FILE *file)
{
    assert (self);
    assert (zmsg_is (self));

    fprintf (file, "--------------------------------------\n");
    if (!self) {
        fprintf (file, "NULL");
        return;
    }
    zframe_t *frame = zmsg_first (self);
    int frame_nbr = 0;
    while (frame && frame_nbr++ < 10) {
        zframe_fprint (frame, NULL, file);
        frame = zmsg_next (self);
    }
}


//  --------------------------------------------------------------------------
//  Selftest

void
zmsg_test (bool verbose)
{
    printf (" * zmsg: ");

    int rc = 0;
    //  @selftest
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new_pair ("@inproc://zmsg.test");
    assert (output);
    zsock_t *input = zsock_new_pair (">inproc://zmsg.test");
    assert (input);

    //  Test send and receive of single-frame message
    zmsg_t *msg = zmsg_new ();
    assert (msg);
    zframe_t *frame = zframe_new ("Hello", 5);
    assert (frame);
    zmsg_prepend (msg, &frame);
    assert (zmsg_size (msg) == 1);
    assert (zmsg_content_size (msg) == 5);
    rc = zmsg_send (&msg, output);
    assert (msg == NULL);
    assert (rc == 0);

    msg = zmsg_recv (input);
    assert (msg);
    assert (zmsg_size (msg) == 1);
    assert (zmsg_content_size (msg) == 5);
    zmsg_destroy (&msg);

    //  Test send and receive of multi-frame message
    msg = zmsg_new ();
    assert (msg);
    rc = zmsg_addmem (msg, "Frame0", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame1", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame2", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame3", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame4", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame5", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame6", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame7", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame8", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame9", 6);
    assert (rc == 0);
    zmsg_t *copy = zmsg_dup (msg);
    assert (copy);
    rc = zmsg_send (&copy, output);
    assert (rc == 0);
    rc = zmsg_send (&msg, output);
    assert (rc == 0);

    copy = zmsg_recv (input);
    assert (copy);
    assert (zmsg_size (copy) == 10);
    assert (zmsg_content_size (copy) == 60);
    zmsg_destroy (&copy);

    msg = zmsg_recv (input);
    assert (msg);
    assert (zmsg_size (msg) == 10);
    assert (zmsg_content_size (msg) == 60);

    //  Save to a file, read back
    FILE *file = fopen ("zmsg.test", "w");
    assert (file);
    rc = zmsg_save (msg, file);
    assert (rc == 0);
    fclose (file);

    file = fopen ("zmsg.test", "r");
    rc = zmsg_save (msg, file);
    assert (rc == -1);
    fclose (file);
    zmsg_destroy (&msg);

    file = fopen ("zmsg.test", "r");
    msg = zmsg_load (file);
    assert (msg);
    fclose (file);
    remove ("zmsg.test");
    assert (zmsg_size (msg) == 10);
    assert (zmsg_content_size (msg) == 60);

    //  Remove all frames except first and last
    int frame_nbr;
    for (frame_nbr = 0; frame_nbr < 8; frame_nbr++) {
        zmsg_first (msg);
        frame = zmsg_next (msg);
        zmsg_remove (msg, frame);
        zframe_destroy (&frame);
    }
    //  Test message frame manipulation
    assert (zmsg_size (msg) == 2);
    frame = zmsg_last (msg);
    assert (zframe_streq (frame, "Frame9"));
    assert (zmsg_content_size (msg) == 12);
    frame = zframe_new ("Address", 7);
    assert (frame);
    zmsg_prepend (msg, &frame);
    assert (zmsg_size (msg) == 3);
    rc = zmsg_addstr (msg, "Body");
    assert (rc == 0);
    assert (zmsg_size (msg) == 4);
    frame = zmsg_pop (msg);
    zframe_destroy (&frame);
    assert (zmsg_size (msg) == 3);
    char *body = zmsg_popstr (msg);
    assert (streq (body, "Frame0"));
    free (body);
    zmsg_destroy (&msg);

    //  Test encoding/decoding
    msg = zmsg_new ();
    assert (msg);
    byte *blank = (byte *) zmalloc (100000);
    assert (blank);
    rc = zmsg_addmem (msg, blank, 0);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 1);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 253);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 254);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 255);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 256);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 65535);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 65536);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 65537);
    assert (rc == 0);
    free (blank);
    assert (zmsg_size (msg) == 9);
    frame = zmsg_encode (msg);
    zmsg_destroy (&msg);
    msg = zmsg_decode (frame);
    assert (msg);
    zmsg_destroy (&msg);
    zframe_destroy (&frame);

    //  Test submessages
    msg = zmsg_new ();
    assert (msg);
    zmsg_t *submsg = zmsg_new ();
    zmsg_pushstr (msg, "matr");
    zmsg_pushstr (submsg, "joska");
    rc = zmsg_addmsg (msg, &submsg);
    assert (rc == 0);
    assert (submsg == NULL);
    submsg = zmsg_popmsg (msg);
    assert (submsg == NULL);   // string "matr" is not encoded zmsg_t, so was discarded
    submsg = zmsg_popmsg (msg);
    assert (submsg);
    body = zmsg_popstr (submsg);
    assert (streq (body, "joska"));
    free (body);
    zmsg_destroy (&submsg);
    frame = zmsg_pop (msg);
    assert (frame == NULL);
    zmsg_destroy (&msg);

    //  Test comparison of two messages
    msg = zmsg_new ();
    zmsg_addstr (msg, "One");
    zmsg_addstr (msg, "Two");
    zmsg_addstr (msg, "Three");
    zmsg_t *msg_other = zmsg_new ();
    zmsg_addstr (msg_other, "One");
    zmsg_addstr (msg_other, "Two");
    zmsg_addstr (msg_other, "One-Hundred");
    zmsg_t *msg_dup = zmsg_dup (msg);
    zmsg_t *empty_msg = zmsg_new ();
    zmsg_t *empty_msg_2 = zmsg_new ();
    assert (zmsg_eq (msg, msg_dup));
    assert (!zmsg_eq (msg, msg_other));
    assert (zmsg_eq (empty_msg, empty_msg_2));
    assert (!zmsg_eq (msg, NULL));
    assert (!zmsg_eq (NULL, empty_msg));
    assert (!zmsg_eq (NULL, NULL));
    zmsg_destroy (&msg);
    zmsg_destroy (&msg_other);
    zmsg_destroy (&msg_dup);
    zmsg_destroy (&empty_msg);
    zmsg_destroy (&empty_msg_2);

    //  Test signal messages
    msg = zmsg_new_signal (0);
    assert (zmsg_signal (msg) == 0);
    zmsg_destroy (&msg);
    msg = zmsg_new_signal (-1);
    assert (zmsg_signal (msg) == 255);
    zmsg_destroy (&msg);

    //  Now try methods on an empty message
    msg = zmsg_new ();
    assert (msg);
    assert (zmsg_size (msg) == 0);
    assert (zmsg_unwrap (msg) == NULL);
    assert (zmsg_first (msg) == NULL);
    assert (zmsg_last (msg) == NULL);
    assert (zmsg_next (msg) == NULL);
    assert (zmsg_pop (msg) == NULL);
    //  Sending an empty message is valid and destroys the message
    assert (zmsg_send (&msg, output) == 0);
    assert (!msg);

    zsock_destroy (&input);
    zsock_destroy (&output);

#if defined (ZMQ_SERVER)
    //  Create server and client sockets and connect over inproc
    zsock_t *server = zsock_new_server ("inproc://zmsg-test-routing");
    assert (server);
    zsock_t *client = zsock_new_client ("inproc://zmsg-test-routing");
    assert (client);

    //  Send request from client to server
    zmsg_t *request = zmsg_new ();
    assert (request);
    zmsg_addstr (request, "Hello");
    rc = zmsg_send (&request, client);
    assert (rc == 0);
    assert (!request);

    //  Read request and send reply
    request = zmsg_recv (server);
    assert (request);
    char *string = zmsg_popstr (request);
    assert (streq (string, "Hello"));
    assert (zmsg_routing_id (request));
    zstr_free (&string);

    zmsg_t *reply = zmsg_new ();
    assert (reply);
    zmsg_addstr (reply, "World");
    zmsg_set_routing_id (reply, zmsg_routing_id (request));
    rc = zmsg_send (&reply, server);
    assert (rc == 0);
    zmsg_destroy (&request);

    //  Read reply
    reply = zmsg_recv (client);
    string = zmsg_popstr (reply);
    assert (streq (string, "World"));
    assert (zmsg_routing_id (reply) == 0);
    zmsg_destroy (&reply);
    zstr_free (&string);

    //  Client and server disallow multipart
    msg = zmsg_new ();
    zmsg_addstr (msg, "One");
    zmsg_addstr (msg, "Two");
    rc = zmsg_send (&msg, client);
    assert (rc == -1);
    assert (zmsg_size (msg) == 2);
    rc = zmsg_send (&msg, server);
    assert (rc == -1);
    assert (zmsg_size (msg) == 2);
    zmsg_destroy (&msg);

    zsock_destroy (&client);
    zsock_destroy (&server);
#endif
    //  @end
    printf ("OK\n");
}
