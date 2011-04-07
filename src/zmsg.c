/*  =========================================================================
    zmsg - working with multipart messages

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is frame of zapi, the C binding for 0MQ: http://zapi.zeromq.org.

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

/*
@header
    The zmsg class provides methods to send and receive multipart messages
    across 0MQ sockets. This class provides a list-like container interface,
    with methods to work with the overall container. zmsg_t messages are
    composed of zero or more zframe_t frames.
@discuss
@end
*/

#include "../include/zapi_prelude.h"
#include "../include/zctx.h"
#include "../include/zframe.h"
#include "../include/zlist.h"
#include "../include/zsocket.h"
#include "../include/zmsg.h"

//  Structure of our class

struct _zmsg_t {
    zlist_t *frames;             //  List of frames
};


//  --------------------------------------------------------------------------
//  Constructor

zmsg_t *
zmsg_new (void)
{
    zmsg_t
        *self;

    self = (zmsg_t *) zmalloc (sizeof (zmsg_t));
    self->frames = zlist_new ();
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
    while (1) {
        zframe_t *frame = zframe_recv (socket);
        if (!frame) {
            zmsg_destroy (&self);
            break;              //  Interrupted or terminated
        }
        zmsg_add (self, frame);
        if (!zframe_more (frame))
            break;              //  Last message frame
    }
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
            zframe_send (&frame, socket,
                zlist_size (self->frames)? ZFRAME_MORE: 0);
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
//  Push frame to the front of the message, i.e. before all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.

void
zmsg_push (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    zlist_push (self->frames, (void *) frame);
}


//  --------------------------------------------------------------------------
//  Remove first frame from message, if any. Returns frame, or NULL. Caller
//  now owns frame and must destroy it when finished with it.

zframe_t *
zmsg_pop (zmsg_t *self)
{
    assert (self);
    return (zframe_t *) zlist_pop (self->frames);
}


//  --------------------------------------------------------------------------
//  Add frame to the end of the message, i.e. after all other frames.
//  Message takes ownership of frame, will destroy it when message is sent.

void
zmsg_add (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    zlist_append (self->frames, frame);
}


//  --------------------------------------------------------------------------
//  Push block of memory to front of message, as a new frame.

void
zmsg_pushmem (zmsg_t *self, const void *src, size_t size)
{
    assert (self);
    assert (src);
    zframe_t *frame = zframe_new (src, size);
    zlist_push (self->frames, frame);
}


//  --------------------------------------------------------------------------
//  Add block of memory to the end of the message, as a new frame.

void
zmsg_addmem (zmsg_t *self, const void *src, size_t size)
{
    assert (self);
    assert (src);
    zframe_t *frame = zframe_new (src, size);
    zlist_append (self->frames, frame);
}


//  --------------------------------------------------------------------------
//  Push string as new frame to front of message

void
zmsg_pushstr (zmsg_t *self, const char *string)
{
    assert (self);
    assert (string);
    zlist_push (self->frames, zframe_new (string, strlen (string)));
}


//  --------------------------------------------------------------------------
//  Push string as new frame to end of message

void
zmsg_addstr (zmsg_t *self, const char *string)
{
    assert (self);
    assert (string);
    zlist_append (self->frames, zframe_new (string, strlen (string)));
}


//  --------------------------------------------------------------------------
//  Pop frame off front of message, return as fresh string

char *
zmsg_popstr (zmsg_t *self)
{
    assert (self);
    zframe_t *frame = (zframe_t *) zlist_pop (self->frames);
    char *string = zframe_strdup (frame);
    zframe_destroy (&frame);
    return string;
}


//  --------------------------------------------------------------------------
//  Push frame plus empty frame to front of message, before first frame.
//  Message takes ownership of frame, will destroy it when message is sent.

void
zmsg_wrap (zmsg_t *self, zframe_t *frame)
{
    assert (self);
    assert (frame);
    zmsg_pushmem (self, "", 0);
    zmsg_push (self, frame);
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
//  Save message to file

void
zmsg_save (zmsg_t *self, FILE *file)
{
    assert (self);
    assert (file);

    zframe_t *frame = zmsg_first (self);
    while (frame) {
        size_t frame_size = zframe_size (frame);
        fwrite (&frame_size, sizeof (frame_size), 1, file);
        fwrite (zframe_data (frame), frame_size, 1, file);
        frame = zmsg_next (self);
    }
}


//  --------------------------------------------------------------------------
//  Load message from file
//  Creates a new message and returns as many frames as can be read.

zmsg_t *
zmsg_load (FILE *file)
{
    assert (file);
    zmsg_t *self = zmsg_new ();

    while (TRUE) {
        size_t frame_size;
        size_t rc = fread (&frame_size, sizeof (frame_size), 1, file);
        if (rc == 1) {
            zframe_t *frame = zframe_new (NULL, frame_size);
            rc = fread (zframe_data (frame), frame_size, 1, file);
            if (frame_size > 0 && rc != 1)
                break;          //  Unable to read properly, quit
            zmsg_add (self, frame);
        }
        else
            break;              //  Unable to read properly, quit
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Create copy of message, as new message object
zmsg_t *
zmsg_dup (zmsg_t *self)
{
    assert (self);
    zmsg_t *copy = zmsg_new ();
    zframe_t *frame = zmsg_first (self);
    while (frame) {
        zmsg_addmem (copy, zframe_data (frame), zframe_size (frame));
        frame = zmsg_next (self);
    }
    return copy;
}


//  --------------------------------------------------------------------------
//  Dump message to stderr, for debugging and tracing

void
zmsg_dump (zmsg_t *self)
{
    fprintf (stderr, "--------------------------------------\n");
    if (!self) {
        fprintf (stderr, "NULL");
        return;
    }
    zframe_t *frame = zmsg_first (self);
    while (frame) {
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

    //  @selftest
    zctx_t *ctx = zctx_new ();

    void *output = zsocket_new (ctx, ZMQ_PAIR);
    zsocket_bind (output, "inproc://zmsg.test");
    void *input = zsocket_new (ctx, ZMQ_PAIR);
    zsocket_connect (input, "inproc://zmsg.test");

    //  Test send and receive of single-frame message
    zmsg_t *msg = zmsg_new ();
    zframe_t *frame = zframe_new ("Hello", 5);
    zmsg_push (msg, frame);
    assert (zmsg_size (msg) == 1);
    zmsg_send (&msg, output);
    assert (msg == NULL);

    msg = zmsg_recv (input);
    assert (msg);
    assert (zmsg_size (msg) == 1);
    zmsg_destroy (&msg);

    //  Test send and receive of multi-frame message
    msg = zmsg_new ();
    zmsg_addmem (msg, "Frame0", 6);
    zmsg_addmem (msg, "Frame1", 6);
    zmsg_addmem (msg, "Frame2", 6);
    zmsg_addmem (msg, "Frame3", 6);
    zmsg_addmem (msg, "Frame4", 6);
    zmsg_addmem (msg, "Frame5", 6);
    zmsg_addmem (msg, "Frame6", 6);
    zmsg_addmem (msg, "Frame7", 6);
    zmsg_addmem (msg, "Frame8", 6);
    zmsg_addmem (msg, "Frame9", 6);
    zmsg_t *copy = zmsg_dup (msg);
    zmsg_send (&copy, output);
    zmsg_send (&msg, output);

    copy = zmsg_recv (input);
    assert (copy);
    assert (zmsg_size (copy) == 10);
    zmsg_destroy (&copy);

    msg = zmsg_recv (input);
    assert (msg);
    assert (zmsg_size (msg) == 10);
    if (verbose)
        zmsg_dump (msg);

    //  Save to a file, read back
    FILE *file = fopen ("zmsg.test", "w");
    assert (file);
    zmsg_save (msg, file);
    zmsg_destroy (&msg);
    fclose (file);

    file = fopen ("zmsg.test", "r");
    msg = zmsg_load (file);
    fclose (file);
    remove ("zmsg.test");
    assert (zmsg_size (msg) == 10);

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
    frame = zframe_new ("Address", 7);
    zmsg_wrap (msg, frame);
    assert (zmsg_size (msg) == 4);
    zmsg_addstr (msg, "Body");
    assert (zmsg_size (msg) == 5);
    frame = zmsg_unwrap (msg);
    zframe_destroy (&frame);
    assert (zmsg_size (msg) == 3);
    char *body = zmsg_popstr (msg);
    assert (streq (body, "Frame0"));
    free (body);
    zmsg_destroy (&msg);

    //  Now try methods on an empty message
    msg = zmsg_new ();
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
