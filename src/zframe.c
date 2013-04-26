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

/*
@header
    The zframe class provides methods to send and receive single message
    frames across 0MQ sockets. A 'frame' corresponds to one zmq_msg_t. When
    you read a frame from a socket, the zframe_more() method indicates if the
    frame is part of an unfinished multipart message. The zframe_send method
    normally destroys the frame, but with the ZFRAME_REUSE flag, you can send
    the same frame many times. Frames are binary, and this class has no
    special support for text data.
@discuss
@end
*/

#include "../include/czmq_prelude.h"
#include "../include/zctx.h"
#include "../include/zsocket.h"
#include "../include/zsockopt.h"
#include "../include/zframe.h"

//  Structure of our class

struct _zframe_t {
    zmq_msg_t zmsg;             //  zmq_msg_t blob for frame
    int more;                   //  More flag, from last read
    int zero_copy;              //  zero-copy flag
    zframe_free_fn *free_fn;    //  destructor callback
    void *free_arg;             //  destructor callback arg
};


//  --------------------------------------------------------------------------
//  Constructor; if size is >0, allocates frame with that size, and if data
//  is not null, copies data into frame.

zframe_t *
zframe_new (const void *data, size_t size)
{
    zframe_t
        *self;

    self = (zframe_t *) zmalloc (sizeof (zframe_t));
    if (!self)
        return NULL;

    if (size) {
        zmq_msg_init_size (&self->zmsg, size);
        if (data)
            memcpy (zmq_msg_data (&self->zmsg), data, size);
    }
    else
        zmq_msg_init (&self->zmsg);

    return self;
}

//  --------------------------------------------------------------------------
//  Constructor; Allows zero-copy semantics.
//  Zero-copy frame is initialised if data != NULL, size > 0, free_fn != 0
//  'arg' is a void pointer that is passed to free_fn as second argument

zframe_t *
zframe_new_zero_copy (void *data, size_t size, zframe_free_fn *free_fn, void *arg)
{
    zframe_t
        *self;

    self = (zframe_t *) zmalloc (sizeof (zframe_t));
    if (!self)
        return NULL;

    if (size) {
        if (data && free_fn) {
            zmq_msg_init_data (&self->zmsg, data, size, free_fn, arg);
            self->zero_copy = 1;
        }
        else
            zmq_msg_init_size (&self->zmsg, size);
    }
    else
        zmq_msg_init (&self->zmsg);

    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zframe_destroy (zframe_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zframe_t *self = *self_p;
        if (self->free_fn)
          (self->free_fn) (self, self->free_arg);
        zmq_msg_close (&self->zmsg);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive frame from socket, returns zframe_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  zframe_recv_nowait().

zframe_t *
zframe_recv (void *zocket)
{
    assert (zocket);
    zframe_t *self = zframe_new (NULL, 0);
    if (self) {
        if (zmq_recvmsg (zocket, &self->zmsg, 0) < 0) {
            zframe_destroy (&self);
            return NULL;            //  Interrupted or terminated
        }
        self->more = zsocket_rcvmore (zocket);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Receive a new frame off the socket. Returns newly allocated frame, or
//  NULL if there was no input waiting, or if the read was interrupted.

zframe_t *
zframe_recv_nowait (void *zocket)
{
    assert (zocket);
    zframe_t *self = zframe_new (NULL, 0);
    if (self) {
        if (zmq_recvmsg (zocket, &self->zmsg, ZMQ_DONTWAIT) < 0) {
            zframe_destroy (&self);
            return NULL;            //  Interrupted or terminated
        }
        self->more = zsocket_rcvmore (zocket);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Send frame to socket, destroy after sending unless ZFRAME_REUSE is
//  set or the attempt to send the message errors out.

int
zframe_send (zframe_t **self_p, void *zocket, int flags)
{
    assert (zocket);
    assert (self_p);

    if (*self_p) {
        zframe_t *self = *self_p;
        int snd_flags = (flags & ZFRAME_MORE)? ZMQ_SNDMORE: 0;
        snd_flags |= (flags & ZFRAME_DONTWAIT)? ZMQ_DONTWAIT: 0;
        if (flags & ZFRAME_REUSE) {
            zmq_msg_t copy;
            zmq_msg_init (&copy);
            if (zmq_msg_copy (&copy, &self->zmsg))
                return -1;
            if (zmq_sendmsg (zocket, &copy, snd_flags) == -1)
                return -1;
        }
        else {
            if (zmq_sendmsg (zocket, &self->zmsg, snd_flags) == -1)
                return -1;
            zframe_destroy (self_p);
        }
    }
    return 0;
}


//  --------------------------------------------------------------------------
//  Return size of frame.

size_t
zframe_size (zframe_t *self)
{
    assert (self);
    return zmq_msg_size (&self->zmsg);
}


//  --------------------------------------------------------------------------
//  Return pointer to frame data.

byte *
zframe_data (zframe_t *self)
{
    assert (self);
    return (byte *) zmq_msg_data (&self->zmsg);
}


//  --------------------------------------------------------------------------
//  Create a new frame that duplicates an existing frame

zframe_t *
zframe_dup (zframe_t *self)
{
    if (self)
        return zframe_new (zframe_data (self), zframe_size (self));
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return frame data encoded as printable hex string, useful for 0MQ UUIDs.
//  Caller must free string when finished with it.

char *
zframe_strhex (zframe_t *self)
{
    static const char
        hex_char [] = "0123456789ABCDEF";

    size_t size = zframe_size (self);
    byte *data = zframe_data (self);
    char *hex_str = (char *) malloc (size * 2 + 1);

    uint byte_nbr;
    for (byte_nbr = 0; byte_nbr < size; byte_nbr++) {
        hex_str [byte_nbr * 2 + 0] = hex_char [data [byte_nbr] >> 4];
        hex_str [byte_nbr * 2 + 1] = hex_char [data [byte_nbr] & 15];
    }
    hex_str [size * 2] = 0;
    return hex_str;
}


//  --------------------------------------------------------------------------
//  Return frame data copied into freshly allocated string
//  Caller must free string when finished with it.

char *
zframe_strdup (zframe_t *self)
{
    size_t size = zframe_size (self);
    char *string = (char *) malloc (size + 1);
    memcpy (string, zframe_data (self), size);
    string [size] = 0;
    return string;
}


//  --------------------------------------------------------------------------
//  Return true if frame body is equal to string, excluding terminator

bool
zframe_streq (zframe_t *self, const char *string)
{
    assert (self);
    if (zframe_size (self) == strlen (string)
    &&  memcmp (zframe_data (self), string, strlen (string)) == 0)
        return true;
    else
        return false;
}


//  --------------------------------------------------------------------------
//  Return frame MORE indicator (1 or 0), set when reading frame from socket

int
zframe_more (const zframe_t *self)
{
    assert (self);
    return self->more;
}

// --------------------------------------------------------------------------
// Return frame zero copy indicator (1 or 0)

int
zframe_zero_copy (zframe_t *self)
{
    assert (self);
    return self->zero_copy;
}


//  --------------------------------------------------------------------------
//  Return true if two frames have identical size and data

bool
zframe_eq (zframe_t *self, zframe_t *other)
{
    if (!self || !other)
        return false;
    else
    if (zframe_size (self) == zframe_size (other)
    && memcmp (zframe_data (self), zframe_data (other),
               zframe_size (self)) == 0)
        return true;
    else
        return false;
}


//  --------------------------------------------------------------------------
//  Print contents of frame to stderr, prefix is ignored if null.

void
zframe_print (zframe_t *self, const char *prefix)
{
    assert (self);
    if (prefix)
        fprintf (stderr, "%s", prefix);
    byte *data = zframe_data (self);
    size_t size = zframe_size (self);

    int is_bin = 0;
    uint char_nbr;
    for (char_nbr = 0; char_nbr < size; char_nbr++)
        if (data [char_nbr] < 9 || data [char_nbr] > 127)
            is_bin = 1;

    fprintf (stderr, "[%03d] ", (int) size);
    size_t max_size = is_bin? 35: 70;
    const char *elipsis = "";
    if (size > max_size) {
        size = max_size;
        elipsis = "...";
    }
    for (char_nbr = 0; char_nbr < size; char_nbr++) {
        if (is_bin)
            fprintf (stderr, "%02X", (unsigned char) data [char_nbr]);
        else
            fprintf (stderr, "%c", data [char_nbr]);
    }
    fprintf (stderr, "%s\n", elipsis);
}


//  --------------------------------------------------------------------------
//  Set new contents for frame

void
zframe_reset (zframe_t *self, const void *data, size_t size)
{
    assert (self);
    assert (data);
    zmq_msg_close (&self->zmsg);
    zmq_msg_init_size (&self->zmsg, size);
    memcpy (zmq_msg_data (&self->zmsg), data, size);
}

void
zframe_freefn (zframe_t *self, zframe_free_fn *free_fn, void *arg)
{
    assert (self);
    assert (free_fn);

    self->free_fn = free_fn;
    self->free_arg = arg;
}

//  --------------------------------------------------------------------------
//  Selftest

static void
s_test_free_cb (void *data, void *arg)
{
    char cmp_buf [1024];

    int i;
    for (i = 0; i < 1024; i++)
        cmp_buf [i] = 'A';

    assert (memcmp (data, cmp_buf, 1024) == 0);
    free (data);
}

static void
s_test_free_frame_cb(void *frame, void *arg)
{
    assert (frame);
}

int
zframe_test (bool verbose)
{
    printf (" * zframe: ");
    int rc;

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);

    void *output = zsocket_new (ctx, ZMQ_PAIR);
    assert (output);
    zsocket_bind (output, "inproc://zframe.test");
    void *input = zsocket_new (ctx, ZMQ_PAIR);
    assert (input);
    zsocket_connect (input, "inproc://zframe.test");

    //  Send five different frames, test ZFRAME_MORE
    int frame_nbr;
    for (frame_nbr = 0; frame_nbr < 5; frame_nbr++) {
        zframe_t *frame = zframe_new ("Hello", 5);
        rc = zframe_send (&frame, output, ZFRAME_MORE);
        assert (rc == 0);
    }
    //  Send same frame five times, test ZFRAME_REUSE
    zframe_t *frame = zframe_new ("Hello", 5);
    assert (frame);
    for (frame_nbr = 0; frame_nbr < 5; frame_nbr++) {
        rc = zframe_send (&frame, output, ZFRAME_MORE + ZFRAME_REUSE);
        assert (rc == 0);
    }
    assert (frame);
    zframe_t *copy = zframe_dup (frame);
    assert (zframe_eq (frame, copy));
    zframe_destroy (&frame);
    assert (!zframe_eq (frame, copy));
    assert (zframe_size (copy) == 5);
    zframe_destroy (&copy);
    assert (!zframe_eq (frame, copy));

    //  Send END frame
    frame = zframe_new ("NOT", 3);
    assert (frame);
    zframe_reset (frame, "END", 3);
    char *string = zframe_strhex (frame);
    assert (streq (string, "454E44"));
    free (string);
    string = zframe_strdup (frame);
    assert (streq (string, "END"));
    free (string);
    rc = zframe_send (&frame, output, 0);
    assert (rc == 0);

    //  Read and count until we receive END
    frame_nbr = 0;
    for (frame_nbr = 0;; frame_nbr++) {
        zframe_t *frame = zframe_recv (input);
        if (zframe_streq (frame, "END")) {
            zframe_destroy (&frame);
            break;
        }
        assert (zframe_more (frame));
        zframe_destroy (&frame);
    }
    assert (frame_nbr == 10);
    frame = zframe_recv_nowait (input);
    assert (frame == NULL);

    // Test zero copy
    char *buffer = (char *) malloc (1024);
    int i;
    for (i = 0; i < 1024; i++)
        buffer [i] = 'A';

    frame = zframe_new_zero_copy (buffer, 1024, s_test_free_cb, NULL);
    zframe_t *frame_copy = zframe_dup (frame);

    assert (zframe_zero_copy (frame) == 1);
    assert (zframe_zero_copy (frame_copy) == 0);

    zframe_destroy (&frame);
    zframe_destroy (&frame_copy);

    frame = zframe_new ("callback", 8);
    zframe_freefn (frame, s_test_free_frame_cb, NULL);
    zframe_destroy (&frame);

    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
    return 0;
}
