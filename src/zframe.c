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

#include "../include/czmq.h"

//  Structure of our class

struct _zframe_t {
    zmq_msg_t zmsg;             //  zmq_msg_t blob for frame
    int more;                   //  More flag, from last read
};

//  --------------------------------------------------------------------------
//  Constructor; if size is >0, allocates frame with that size, and if data
//  is not null, copies data into frame.

zframe_t *
zframe_new (const void *data, size_t size)
{
    zframe_t *self = (zframe_t *) zmalloc (sizeof (zframe_t));
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
//  Constructor; Allocates a new empty (zero-sized) frame

zframe_t *
zframe_new_empty (void)
{
    zframe_t *self = (zframe_t *) zmalloc (sizeof (zframe_t));
    if (!self)
        return NULL;

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
        int send_flags = (flags & ZFRAME_MORE)? ZMQ_SNDMORE: 0;
        send_flags |= (flags & ZFRAME_DONTWAIT)? ZMQ_DONTWAIT: 0;
        if (flags & ZFRAME_REUSE) {
            zmq_msg_t copy;
            zmq_msg_init (&copy);
            if (zmq_msg_copy (&copy, &self->zmsg))
                return -1;
            if (zmq_sendmsg (zocket, &copy, send_flags) == -1) {
                zmq_msg_close (&copy);
                return -1;
            }
        }
        else {
            int rc = zmq_sendmsg (zocket, &self->zmsg, send_flags);
            zframe_destroy (self_p);
            if (rc < 0)
                return rc;
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
//  or by the zframe_set_more() method.

int
zframe_more (zframe_t *self)
{
    assert (self);
    return self->more;
}


//  --------------------------------------------------------------------------
//  Set frame MORE indicator (1 or 0). Note this is NOT used when sending
//  frame to socket, you have to specify flag explicitly.

void
zframe_set_more (zframe_t *self, int more)
{
    assert (self);
    assert (more == 0 || more == 1);
    self->more = more;
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
//  Print contents of frame to FILE stream, prefix is ignored if null.

void
zframe_fprint (zframe_t *self, const char *prefix, FILE *file)
{
    assert (self);
    if (prefix)
        fprintf (file, "%s", prefix);
    byte *data = zframe_data (self);
    size_t size = zframe_size (self);

    int is_bin = 0;
    uint char_nbr;
    for (char_nbr = 0; char_nbr < size; char_nbr++)
        if (data [char_nbr] < 9 || data [char_nbr] > 127)
            is_bin = 1;

    fprintf (file, "[%03d] ", (int) size);
    size_t max_size = is_bin? 35: 70;
    const char *ellipsis = "";
    if (size > max_size) {
        size = max_size;
        ellipsis = "...";
    }
    for (char_nbr = 0; char_nbr < size; char_nbr++) {
        if (is_bin)
            fprintf (file, "%02X", (unsigned char) data [char_nbr]);
        else
            fprintf (file, "%c", data [char_nbr]);
    }
    fprintf (file, "%s\n", ellipsis);
}


//  --------------------------------------------------------------------------
//  Print contents of frame to stderr, prefix is ignored if null.

void
zframe_print (zframe_t *self, const char *prefix)
{
    zframe_fprint (self, prefix, stderr);
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


//  --------------------------------------------------------------------------
//  Selftest

int
zframe_test (bool verbose)
{
    printf (" * zframe: ");
    int rc;
    zframe_t* frame;

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
        frame = zframe_new ("Hello", 5);
        rc = zframe_send (&frame, output, ZFRAME_MORE);
        assert (rc == 0);
    }
    //  Send same frame five times, test ZFRAME_REUSE
    frame = zframe_new ("Hello", 5);
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

    //  Test zframe_new_empty
    frame = zframe_new_empty ();
    assert (frame);
    assert (zframe_size (frame) == 0);
    zframe_destroy (&frame);

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
        zframe_set_more (frame, 0);
        assert (zframe_more (frame) == 0);
        zframe_destroy (&frame);
    }
    assert (frame_nbr == 10);
    frame = zframe_recv_nowait (input);
    assert (frame == NULL);

    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
    return 0;
}
