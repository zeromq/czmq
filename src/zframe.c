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

//  zframe_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZFRAME_TAG              0x0002cafe

//  Structure of our class

struct _zframe_t {
    uint32_t tag;               //  Object tag for runtime detection
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
    if (self) {
        self->tag = ZFRAME_TAG;
        if (size) {
            zmq_msg_init_size (&self->zmsg, size);
            if (data)
                memcpy (zmq_msg_data (&self->zmsg), data, size);
        }
        else
            zmq_msg_init (&self->zmsg);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Constructor; Allocates a new empty (zero-sized) frame

zframe_t *
zframe_new_empty (void)
{
    zframe_t *self = (zframe_t *) zmalloc (sizeof (zframe_t));
    if (self) {
        self->tag = ZFRAME_TAG;
        zmq_msg_init (&self->zmsg);
    }
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
        assert (zframe_is (self));
        zmq_msg_close (&self->zmsg);
        self->tag = 0xDeadBeef;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive frame from socket, returns zframe_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  zpoller or zloop.

zframe_t *
zframe_recv (void *source)
{
    assert (source);
    void *handle = zsock_resolve (source);
    zframe_t *self = zframe_new (NULL, 0);
    if (self) {
        if (zmq_recvmsg (handle, &self->zmsg, 0) < 0) {
            zframe_destroy (&self);
            return NULL;            //  Interrupted or terminated
        }
        self->more = zsock_rcvmore (handle);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Send frame to socket, destroy after sending unless ZFRAME_REUSE is
//  set or the attempt to send the message errors out.

int
zframe_send (zframe_t **self_p, void *dest, int flags)
{
    assert (dest);
    assert (self_p);

    void *handle = zsock_resolve (dest);
    if (*self_p) {
        zframe_t *self = *self_p;
        assert (zframe_is (self));

        int send_flags = (flags & ZFRAME_MORE) ? ZMQ_SNDMORE : 0;
        send_flags |= (flags & ZFRAME_DONTWAIT) ? ZMQ_DONTWAIT : 0;
        if (flags & ZFRAME_REUSE) {
            zmq_msg_t copy;
            zmq_msg_init (&copy);
            if (zmq_msg_copy (&copy, &self->zmsg))
                return -1;
            if (zmq_sendmsg (handle, &copy, send_flags) == -1) {
                zmq_msg_close (&copy);
                return -1;
            }
        }
        else {
            int rc = zmq_sendmsg (handle, &self->zmsg, send_flags);
            zframe_destroy (self_p);
            if (rc == -1)
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
    assert (zframe_is (self));

    return zmq_msg_size (&self->zmsg);
}


//  --------------------------------------------------------------------------
//  Return pointer to frame data.

byte *
zframe_data (zframe_t *self)
{
    assert (self);
    assert (zframe_is (self));

    return (byte *) zmq_msg_data (&self->zmsg);
}


//  --------------------------------------------------------------------------
//  Create a new frame that duplicates an existing frame. If frame is null,
//  or memory was exhausted, returns null.

zframe_t *
zframe_dup (zframe_t *self)
{
    if (self) {
        assert (zframe_is (self));
        return zframe_new (zframe_data (self), zframe_size (self));
    }
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return frame data encoded as printable hex string, useful for 0MQ UUIDs.
//  Caller must free string when finished with it.

char *
zframe_strhex (zframe_t *self)
{
    assert (self);
    assert (zframe_is (self));

    static const char
        hex_char [] = "0123456789ABCDEF";

    size_t size = zframe_size (self);
    byte *data = zframe_data (self);
    char *hex_str = (char *) zmalloc (size * 2 + 1);
    if (!hex_str)
        return NULL;

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
    assert (self);
    assert (zframe_is (self));

    size_t size = zframe_size (self);
    char *string = (char *) malloc (size + 1);
    if (string) {
        memcpy (string, zframe_data (self), size);
        string [size] = 0;
    }
    return string;
}


//  --------------------------------------------------------------------------
//  Return true if frame body is equal to string, excluding terminator

bool
zframe_streq (zframe_t *self, const char *string)
{
    assert (self);
    assert (zframe_is (self));

    if (  zframe_size (self) == strlen (string)
       && memcmp (zframe_data (self), string, strlen (string)) == 0)
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
    assert (zframe_is (self));

    return self->more;
}


//  --------------------------------------------------------------------------
//  Set frame MORE indicator (1 or 0). Note this is NOT used when sending
//  frame to socket, you have to specify flag explicitly.

void
zframe_set_more (zframe_t *self, int more)
{
    assert (self);
    assert (zframe_is (self));
    assert (more == 0 || more == 1);

    self->more = more;
}


//  --------------------------------------------------------------------------
//  Return true if two frames have identical size and data

bool
zframe_eq (zframe_t *self, zframe_t *other)
{
    if (!self || !other)
        return false;           //  Tolerate null references here
    else {
        assert (zframe_is (self));
        assert (zframe_is (other));

        if (  zframe_size (self) == zframe_size (other)
           && memcmp (zframe_data (self),
                      zframe_data (other),
                      zframe_size (self)) == 0)
            return true;
        else
            return false;
    }
}


//  --------------------------------------------------------------------------
//  Set new contents for frame

void
zframe_reset (zframe_t *self, const void *data, size_t size)
{
    assert (self);
    assert (zframe_is (self));
    assert (data);

    zmq_msg_close (&self->zmsg);
    zmq_msg_init_size (&self->zmsg, size);
    memcpy (zmq_msg_data (&self->zmsg), data, size);
}


//  --------------------------------------------------------------------------
//  Send message to zsys log sink (may be stdout, or system facility as
//  configured by zsys_set_logstream). Prefix shows before frame, if not null.

void
zframe_print (zframe_t *self, const char *prefix)
{
    assert (self);
    assert (zframe_is (self));

    byte *data = zframe_data (self);
    size_t size = zframe_size (self);

    //  Probe data to check if it looks like unprintable binary
    int is_bin = 0;
    uint char_nbr;
    for (char_nbr = 0; char_nbr < size; char_nbr++)
        if (data [char_nbr] < 9 || data [char_nbr] > 127)
            is_bin = 1;

    char buffer [256] = "";
    snprintf (buffer, 30, "%s[%03d] ", prefix ? prefix : "", (int) size);
    size_t max_size = is_bin ? 35 : 70;
    const char *ellipsis = "";
    if (size > max_size) {
        size = max_size;
        ellipsis = "...";
    }
    for (char_nbr = 0; char_nbr < size; char_nbr++) {
        if (is_bin)
            sprintf (buffer + strlen (buffer), "%02X", (unsigned char) data [char_nbr]);
        else
            sprintf (buffer + strlen (buffer), "%c", data [char_nbr]);
    }
    strcat (buffer, ellipsis);
    zsys_debug (buffer);
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zframe_t.

bool
zframe_is (void *self)
{
    assert (self);
    return ((zframe_t *) self)->tag == ZFRAME_TAG;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as poor style -- callers should use zloop or zpoller
//  Receive a new frame off the socket. Returns newly allocated frame, or
//  NULL if there was no input waiting, or if the read was interrupted.

zframe_t *
zframe_recv_nowait (void *source)
{
    assert (source);
    void *handle = zsock_resolve (source);

    zframe_t *self = zframe_new (NULL, 0);
    if (self) {
        if (zmq_recvmsg (handle, &self->zmsg, ZMQ_DONTWAIT) < 0) {
            zframe_destroy (&self);
            return NULL;            //  Interrupted or terminated
        }
        self->more = zsock_rcvmore (handle);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  DEPRECATED as inconsistent; breaks principle that logging should all go
//  to a single destination.
//  Print contents of frame to FILE stream, prefix is ignored if null.

void
zframe_fprint (zframe_t *self, const char *prefix, FILE *file)
{
    assert (self);
    assert (zframe_is (self));

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
    size_t max_size = is_bin ? 35 : 70;
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
//  Selftest

void
zframe_test (bool verbose)
{
    printf (" * zframe: ");
    int rc;
    zframe_t* frame;

    //  @selftest
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new_pair ("@inproc://zframe.test");
    assert (output);
    zsock_t *input = zsock_new_pair (">inproc://zframe.test");
    assert (input);

    //  Send five different frames, test ZFRAME_MORE
    int frame_nbr;
    for (frame_nbr = 0; frame_nbr < 5; frame_nbr++) {
        frame = zframe_new ("Hello", 5);
        assert (frame);
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

    zsock_destroy (&input);
    zsock_destroy (&output);

    //  @end
    printf ("OK\n");
}
