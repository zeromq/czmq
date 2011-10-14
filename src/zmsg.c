/*  =========================================================================
    zmsg - working with multipart messages

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
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

/*
@header
    The zmsg class provides methods to send and receive multipart messages
    across 0MQ sockets. This class provides a list-like container interface,
    with methods to work with the overall container. zmsg_t messages are
    composed of zero or more zframe_t frames.
@discuss
@end
*/

#include "../include/czmq_prelude.h"
#include "../include/zctx.h"
#include "../include/zframe.h"
#include "../include/zlist.h"
#include "../include/zsocket.h"
#include "../include/zmsg.h"

//  Structure of our class

struct _zmsg_t {
    zlist_t *frames;            //  List of frames
    size_t content_size;        //  Total content size
};


//  --------------------------------------------------------------------------
//  Constructor

zmsg_t *
zmsg_new (void)
{
    zmsg_t
        *self;

    self = (zmsg_t *) zmalloc (sizeof (zmsg_t));
    if (!self)
        goto end;

    self->frames = zlist_new ();
    if (!(self->frames)) {
        free (self);
        self = NULL;
        goto end;
    }
end:
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
        while (zlist_size (self->frames) > 0) {
            zframe_t *frame = (zframe_t *) zlist_pop (self->frames);
            zframe_destroy (&frame);
        }
        zlist_destroy (&self->frames);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive message from socket, returns zmsg_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  the zloop class or zmq_poll to check for socket input before receiving.

zmsg_t *
zmsg_recv (void *socket)
{
    assert (socket);
    zmsg_t *self = zmsg_new ();
    if (!self)
        goto end;
    while (1) {
        zframe_t *frame = zframe_recv (socket);
        if (!frame) {
            zmsg_destroy (&self);
            break;              //  Interrupted or terminated
        }
        if (zmsg_add (self, frame)) {
            zmsg_destroy (&self);
            break;
        }
        if (!zframe_more (frame))
            break;              //  Last message frame
    }
end:
    return self;
}


//  --------------------------------------------------------------------------
//  Send message to socket, destroy after sending. If the message has no
//  frames, sends nothing but destroys the message anyhow. Safe to call
//  if zmsg is null.

void
zmsg_send (zmsg_t **self_p, void *socket)
{
    assert (self_p);
    assert (socket);
    zmsg_t *self = *self_p;

    if (self) {
        zframe_t *frame = (zframe_t *) zlist_pop (self->frames);
        while (frame) {
            int rc;
            rc = zframe_send (&frame, socket,
                              zlist_size (self->frames)? ZFRAME_MORE: 0);
            assert(rc == 0);
            frame = (zframe_t *) zlist_pop (self->frames);
        }
        zmsg_destroy (self_p);
    }
}


//  --------------------------------------------------------------------------
//  Return size of message, i.e. number of frames (0 or more).

size_t
zmsg_size (zmsg_t *self)
{
    assert (self);
    return zlist_size (self->frames);
}


//  --------------------------------------------------------------------------
//  Return size of message, i.e. number of frames (0 or more).

size_t
zmsg_content_size (zmsg_t *self)
{
    assert (self);
    return self->content_size;
}


//  --------------------------------------------------------------------------
//  Push frame to the front of the message, i.e. before all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success;

int
zmsg_push (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    int error = 0;
    self->content_size += zframe_size (frame);
    error = zlist_push (self->frames, (void *) frame);
    return error;
}


//  --------------------------------------------------------------------------
//  Remove first frame from message, if any. Returns frame, or NULL. Caller
//  now owns frame and must destroy it when finished with it.

zframe_t *
zmsg_pop (zmsg_t *self)
{
    assert (self);
    zframe_t *frame = zlist_pop (self->frames);
    if (frame)
        self->content_size -= zframe_size (frame);
    return frame;
}


//  --------------------------------------------------------------------------
//  Add frame to the end of the message, i.e. after all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.
//  Returns 0 on success

int
zmsg_add (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    int error = 0;
    self->content_size += zframe_size (frame);
    error = zlist_append (self->frames, frame);
    return error;
}


//  --------------------------------------------------------------------------
//  Push block of memory to front of message, as a new frame.

int
zmsg_pushmem (zmsg_t *self, const void *src, size_t size)
{
    assert (self);
    int error = 0;
    zframe_t *frame = zframe_new (src, size);
    if (frame) {
        self->content_size += size;
        error = zlist_push (self->frames, frame);
    }
    else
        error = ENOMEM;

    return error;
}


//  --------------------------------------------------------------------------
//  Add block of memory to the end of the message, as a new frame.

int
zmsg_addmem (zmsg_t *self, const void *src, size_t size)
{
    assert (self);
    int error = 0;
    zframe_t *frame = zframe_new (src, size);
    if (frame) {
        self->content_size += size;
        error = zlist_append (self->frames, frame);
    }
    else
        error = ENOMEM;

    return error;
}


// --------------------------------------------------------------------------
//  Handle the mechanics of pushing the string frame into the list - the list
//  function passed in determines how the frame containing the string
//  enters the list

static int zmsg_add_str_to_framelist (
    int (*list_fxn) (zlist_t *, void *),
    zmsg_t *self,
    const char *format,
    va_list argptr)
{
    assert (list_fxn);
    assert (self);
    assert (format);
    int error = 0;
    int size = 255 + 1;
    char *string = (char *) malloc (size);
    if (!string) {
        error = ENOMEM;
        goto end;
    }
    int required = vsnprintf (string, size, format, argptr);
    if (required >= size) {
        char *tmpstring;
        size = required + 1;
        tmpstring = (char *) realloc (string, size);
        if (!tmpstring) {
            error = ENOMEM;
            goto end;
        }
        string = tmpstring;
        vsnprintf (string, size, format, argptr);
    }
    self->content_size += strlen (string);
    zframe_t *frame = zframe_new (string, strlen (string));
    if (!frame) {
        error = ENOMEM;
        goto end;
    }
    error = list_fxn (self->frames, frame);

end:
    free (string);
    return error;
}



//  --------------------------------------------------------------------------
//  Push string as new frame to front of message

int zmsg_pushstr (zmsg_t *self, const char *format, ...)
{
    assert (format);
    va_list argptr;
    va_start (argptr, format);

    int error = zmsg_add_str_to_framelist (&zlist_push, self, format, argptr);
    va_end (argptr);

    return error;
}


//  --------------------------------------------------------------------------
//  Push string as new frame to end of message

int
zmsg_addstr (zmsg_t *self, const char *format, ...)
{
    assert (format);
    va_list argptr;
    va_start (argptr, format);

    int error = zmsg_add_str_to_framelist (&zlist_append, self, format, argptr);
    va_end (argptr);

    return error;
}


//  --------------------------------------------------------------------------
//  Pop frame off front of message, return as fresh string

char *
zmsg_popstr (zmsg_t *self)
{
    assert (self);
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
//  Push frame plus empty frame to front of message, before first frame.
//  Message takes ownership of frame, will destroy it when message is sent.

int
zmsg_wrap (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    int error = 0;
    error = zmsg_pushmem (self, "", 0);
    if (error)
        goto end;

    error = zmsg_push (self, frame);
    if (error) {
        // Rewind the empty frame
        zmsg_pop (self);
        goto end;
    }
end:
    return error;
}


//  --------------------------------------------------------------------------
//  Pop frame off front of message, caller now owns frame
//  If next frame is empty, pops and destroys that empty frame.

zframe_t *
zmsg_unwrap (zmsg_t *self)
{
    assert (self);
    zframe_t *frame = zmsg_pop (self);
    zframe_t *empty = zmsg_first (self);
    if (zframe_size (empty) == 0) {
        empty = zmsg_pop (self);
        zframe_destroy (&empty);
    }
    return frame;
}


//  --------------------------------------------------------------------------
//  Remove specified frame from list, if present. Does not destroy frame.

void
zmsg_remove (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    self->content_size -= zframe_size (frame);
    zlist_remove (self->frames, frame);
}


//  --------------------------------------------------------------------------
//  Set cursor to first frame in message. Returns frame, or NULL.

zframe_t *
zmsg_first (zmsg_t *self)
{
    assert (self);
    return (zframe_t *) zlist_first (self->frames);
}


//  --------------------------------------------------------------------------
//  Return the next frame. If there are no more frames, returns NULL. To move
//  to the first frame call zmsg_first(). Advances the cursor.

zframe_t *
zmsg_next (zmsg_t *self)
{
    assert (self);
    return (zframe_t *) zlist_next (self->frames);
}


//  --------------------------------------------------------------------------
//  Return the last frame. If there are no frames, returns NULL.

zframe_t *
zmsg_last (zmsg_t *self)
{
    assert (self);
    zframe_t *frame = (zframe_t *) zlist_first (self->frames);
    while (frame) {
        zframe_t *next = (zframe_t *) zlist_next (self->frames);
        if (!next)
            break;
        frame = next;
    }
    return frame;
}


//  --------------------------------------------------------------------------
//  Save message to an open file, return 0 if OK, else -1.
int
zmsg_save (zmsg_t *self, FILE *file)
{
    assert (self);
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
//  Load/append an open file into message, create new message if
//  null message provided.  Return 0 if OK, non-zero otherwise.  The
//  returned message should be identical to the passed in message on
//  error.
//

int
zmsg_load (zmsg_t *self, FILE *file, zmsg_t **return_self)
{
    assert (file);
    int error = 0;
    int frame_count = 0;
    int created_msg = 0;
    zmsg_t *tmp_msg = NULL;

    // NULL out return_self so the caller knows not to use it on failure
    *return_self = NULL;

    if (self)
        tmp_msg = self;
    else {
        tmp_msg = zmsg_new ();
        if (!tmp_msg) {
            error = ENOMEM;
            goto end;
        }
        created_msg = 1;
    }

    while (TRUE) {
        size_t frame_size;
        clearerr (file);
        size_t rc = fread (&frame_size, sizeof (frame_size), 1, file);
        if (rc == 1) {
            zframe_t *frame = zframe_new (NULL, frame_size);
            if (frame) {
                if (frame_size) {
                    clearerr (file);
                    rc = fread (zframe_data (frame), frame_size, 1, file);
                    if (rc != 1) {
                        error = ferror (file);
                        if (!error)
                            error = 1;
                        goto end;
                    }
                }
                error = zmsg_add (tmp_msg, frame);
                if (error)
                    goto end;
                frame_count++;
            }
            else {
                error = ENOMEM;
                goto end;          // Unable to allocate frame, quit
            }
        }
        else {
            if (feof (file))
                goto end;
            error = ferror (file);
            if (!error)
                error = 1;
            goto end;              //  Unable to read properly, quit
        }
    }

end:
    if (error) {
        if (tmp_msg) {
            // need to rewind the frames we added to the message
            // to return the message to its original state
            int i = 0;
            zframe_t *lastframe;
            for(i = 0; i < frame_count; i++) {
                lastframe = zmsg_last (tmp_msg);
                zmsg_remove (tmp_msg, lastframe);
                zframe_destroy (&lastframe);
            }
            // Only destroy the message if we created it in this function
            if (created_msg)
                zmsg_destroy (&tmp_msg);
        }
    }
    *return_self = tmp_msg;
    return error;
}


//  --------------------------------------------------------------------------
//  Encode message to a new buffer, return buffer size

//  Frame lengths are encoded as 1, 1+2, or 1+4 bytes
//  0..253 bytes        octet + data
//  254..64k-1 bytes    0xFE + 2octet + data
//  64k..4Gb-1 bytes    0xFF + 4octet + data

#define ZMSG_SHORT_LEN      0xFE
#define ZMSG_LONG_LEN       0xFF

size_t
zmsg_encode (zmsg_t *self, byte **buffer)
{
    assert (self);

    //  Calculate real size of buffer
    size_t buffer_size = 0;
    zframe_t *frame = zmsg_first (self);
    while (frame) {
        size_t frame_size = zframe_size (frame);
        if (frame_size < ZMSG_SHORT_LEN)
            buffer_size += frame_size + 1;
        else
        if (frame_size < 0x10000)
            buffer_size += frame_size + 3;
        else
            buffer_size += frame_size + 5;
        frame = zmsg_next (self);
    }
    *buffer = malloc (buffer_size);

    //  Encode message now
    byte *dest = *buffer;
    frame = zmsg_first (self);
    while (frame) {
        size_t frame_size = zframe_size (frame);
        if (frame_size < ZMSG_SHORT_LEN) {
            *dest++ = (byte) frame_size;
            memcpy (dest, zframe_data (frame), frame_size);
            dest += frame_size;
        }
        else
        if (frame_size < 0x10000) {
            *dest++ = ZMSG_SHORT_LEN;
            *dest++ = (frame_size >> 8) & 255;
            *dest++ =  frame_size       & 255;
            memcpy (dest, zframe_data (frame), frame_size);
            dest += frame_size;
        }
        else {
            *dest++ = ZMSG_LONG_LEN;
            *dest++ = (frame_size >> 24) & 255;
            *dest++ = (frame_size >> 16) & 255;
            *dest++ = (frame_size >>  8) & 255;
            *dest++ =  frame_size        & 255;
            memcpy (dest, zframe_data (frame), frame_size);
            dest += frame_size;
        }
        frame = zmsg_next (self);
    }
    assert ((dest - *buffer) == buffer_size);
    return buffer_size;
}


//  --------------------------------------------------------------------------
//  Decode a buffer into a new message, returns NULL if buffer is not
//  properly formatted.

zmsg_t *
zmsg_decode (byte *buffer, size_t buffer_size)
{
    zmsg_t *self = zmsg_new ();
    if (!self)
        goto end;

    byte *source = buffer;
    byte *limit = buffer + buffer_size;
    while (source < limit) {
        size_t frame_size = *source++;
        if (frame_size == ZMSG_SHORT_LEN) {
            if (source > limit - 2) {
                zmsg_destroy (&self);
                break;
            }
            frame_size = (source [0] << 8) + source [1];
            source += 2;
        }
        else
        if (frame_size == ZMSG_LONG_LEN) {
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
        zframe_t *frame = zframe_new (source, frame_size);
        if (frame) {
            if (zmsg_add (self, frame)) {
                zmsg_destroy (&self);
                break;
            }
            source += frame_size;
        }
        else {
            zmsg_destroy (&self);
            break;
        }
    }

end:
    return self;
}


//  --------------------------------------------------------------------------
//  Create copy of message, as new message object
//  FIXME: It would be nice if this function returned the actual error
//  up the stack rather than just passing back NULL on error.

zmsg_t *
zmsg_dup (zmsg_t *self)
{
    assert (self);
    int rc = 0;
    zmsg_t *copy = NULL;
    zframe_t *frame = zmsg_first (self);
    if (!frame) {
        goto end;
    }
    copy = zmsg_new ();
    if (!copy)
        goto end;

    while (frame) {
        rc = zmsg_addmem (copy, zframe_data (frame), zframe_size (frame));
        if (rc) {
            zmsg_destroy (&copy);
            goto end;
        }
        frame = zmsg_next (self);
    }

end:
    return copy;
}


//  --------------------------------------------------------------------------
//  Dump message to stderr, for debugging and tracing
//  Prints first 10 frames, for larger messages

void
zmsg_dump (zmsg_t *self)
{
    fprintf (stderr, "--------------------------------------\n");
    if (!self) {
        fprintf (stderr, "NULL");
        return;
    }
    zframe_t *frame = zmsg_first (self);
    int frame_nbr = 0;
    while (frame && frame_nbr++ < 10) {
        zframe_print (frame, "");
        frame = zmsg_next (self);
    }
}


//  --------------------------------------------------------------------------
//  Selftest

int
zmsg_test (Bool verbose)
{
    printf (" * zmsg: ");

    int rc = 0;
    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);

    void *output = zsocket_new (ctx, ZMQ_PAIR);
    assert (output);
    zsocket_bind (output, "inproc://zmsg.test");
    void *input = zsocket_new (ctx, ZMQ_PAIR);
    assert (input);
    zsocket_connect (input, "inproc://zmsg.test");

    //  Test send and receive of single-frame message
    zmsg_t *msg = zmsg_new ();
    assert (msg);
    zframe_t *frame = zframe_new ("Hello", 5);
    assert (frame);
    zmsg_push (msg, frame);
    assert (zmsg_size (msg) == 1);
    assert (zmsg_content_size (msg) == 5);
    zmsg_send (&msg, output);
    assert (msg == NULL);

    msg = zmsg_recv (input);
    assert (msg);
    assert (zmsg_size (msg) == 1);
    assert (zmsg_content_size (msg) == 5);
    zmsg_destroy (&msg);

    //  Test send and receive of multi-frame message
    msg = zmsg_new ();
    assert (msg);
    rc = zmsg_addmem (msg, "Frame0", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame1", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame2", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame3", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame4", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame5", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame6", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame7", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame8", 6);
    assert(rc == 0);
    rc = zmsg_addmem (msg, "Frame9", 6);
    assert(rc == 0);
    zmsg_t *copy = zmsg_dup (msg);
    assert (copy);
    zmsg_send (&copy, output);
    zmsg_send (&msg, output);

    copy = zmsg_recv (input);
    assert (copy);
    assert (zmsg_size (copy) == 10);
    assert (zmsg_content_size (copy) == 60);
    zmsg_destroy (&copy);

    msg = zmsg_recv (input);
    assert (msg);
    assert (zmsg_size (msg) == 10);
    assert (zmsg_content_size (msg) == 60);
    if (verbose)
        zmsg_dump (msg);

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
    rc = zmsg_load (NULL, file, &msg);
    assert (rc == 0);
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
    assert (zmsg_content_size (msg) == 12);
    frame = zframe_new ("Address", 7);
    assert (frame);
    rc = zmsg_wrap (msg, frame);
    assert (rc == 0);
    assert (zmsg_size (msg) == 4);
    rc = zmsg_addstr (msg, "Body");
    assert (rc == 0);
    assert (zmsg_size (msg) == 5);
    frame = zmsg_unwrap (msg);
    zframe_destroy (&frame);
    assert (zmsg_size (msg) == 3);
    char *body = zmsg_popstr (msg);
    assert (streq (body, "Frame0"));
    free (body);
    zmsg_destroy (&msg);

    //  Test encoding/decoding
    msg = zmsg_new ();
    assert (msg);
    byte *blank = zmalloc (100000);
    assert (blank);
    rc = zmsg_addmem (msg, blank, 0);
    assert(rc == 0);
    rc = zmsg_addmem (msg, blank, 1);
    assert(rc == 0);
    rc = zmsg_addmem (msg, blank, 253);
    assert(rc == 0);
    rc = zmsg_addmem (msg, blank, 254);
    assert(rc == 0);
    rc = zmsg_addmem (msg, blank, 255);
    assert(rc == 0);
    rc = zmsg_addmem (msg, blank, 256);
    assert(rc == 0);
    rc = zmsg_addmem (msg, blank, 65535);
    assert(rc == 0);
    rc = zmsg_addmem (msg, blank, 65536);
    assert(rc == 0);
    rc = zmsg_addmem (msg, blank, 65537);
    assert(rc == 0);
    free (blank);
    assert (zmsg_size (msg) == 9);
    byte *buffer;
    size_t buffer_size = zmsg_encode (msg, &buffer);
    zmsg_destroy (&msg);
    msg = zmsg_decode (buffer, buffer_size);
    assert (msg);
    free (buffer);
    zmsg_destroy (&msg);

    //  Now try methods on an empty message
    msg = zmsg_new ();
    assert (msg);
    assert (zmsg_size (msg) == 0);
    assert (zmsg_first (msg) == NULL);
    assert (zmsg_next (msg) == NULL);
    assert (zmsg_pop (msg) == NULL);
    zmsg_destroy (&msg);

    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
    return 0;
}
