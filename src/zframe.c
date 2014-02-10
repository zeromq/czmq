/*  =========================================================================
    zframe - working with single message frames

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
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

extern int errno;

//  Structure of our class

struct _zframe_t {
    zmq_msg_t zmsg;             //  zmq_msg_t blob for frame
    byte *needle;               //  Read/write pointer for serialization
    byte *ceiling;              //  Valid upper limit for read pointer
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
        if (data) {
            memcpy (zmq_msg_data (&self->zmsg), data, size);
            self->needle = zframe_data (self) + zframe_size (self);
            self->ceiling = self->needle + zframe_size (self);
        }
        else {
            self->needle = zframe_data (self);
            self->ceiling = self->needle + zframe_size (self);
        }
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
        self->needle = zframe_data (self);
        self->ceiling = self->needle + zframe_size (self);
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
            if (zmq_sendmsg (zocket, &copy, send_flags) == -1)
                return -1;
        }
        else {
            int rc = zmq_sendmsg (zocket, &self->zmsg, send_flags);
            zframe_destroy (self_p);
            if (rc == -1)
                return -1;
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
// Helper method which tests if there's 'bytes' space left on the frame

static bool
s_sufficient_data (zframe_t *self, size_t bytes)
{
    return (self->needle + bytes) <= self->ceiling;
}

//  --------------------------------------------------------------------------
//  Put a block of data to the frame payload. Returns 0 if successful else -1

int
zframe_put_block (zframe_t *self, byte *data, size_t size)
{
    assert (self);
    if (s_sufficient_data (self, size)) {
        memcpy (self->needle, data, size);
        self->needle += size;
        return 0;
    }
    else
        return -1;
}

//  --------------------------------------------------------------------------
//  Put one byte to frame payload. Returns 0 if successful else -1

int
zframe_put_uint8 (zframe_t *self, uint8_t data)
{
    assert (self);
    if (s_sufficient_data (self, 1)) {
        self->needle [0] = data;
        self->needle++;
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Put two bytes to frame payload. Returns 0 if successful else -1

int
zframe_put_uint16 (zframe_t *self, uint16_t data)
{
    assert (self);
    if (s_sufficient_data (self, 2)) {
        self->needle [0] = (byte) (data >> 8) & 255;
        self->needle [1] = (byte) (data & 255);
        self->needle += 2;
        return 0;
    }
    else
        return -1;
}

//  --------------------------------------------------------------------------
//  Put four bytes to frame payload. Returns 0 if successful else -1

int
zframe_put_uint32 (zframe_t *self, uint32_t data)
{
    assert (self);
    if (s_sufficient_data (self, 4)) {
        self->needle [0] = (byte) (data >> 24) & 255;
        self->needle [1] = (byte) (data >> 16) & 255;
        self->needle [2] = (byte) (data >>  8) & 255;
        self->needle [3] = (byte) (data & 255);
        self->needle += 4;
        return 0;
    }
    else
        return -1;
}

//  --------------------------------------------------------------------------
//  Put eight bytes to frame payload. Returns 0 if successful else -1

int
zframe_put_uint64 (zframe_t *self, uint64_t data)
{
    assert (self);
    if (s_sufficient_data (self, 8)) {
        self->needle [0] = (byte) (data >> 56) & 255;
        self->needle [1] = (byte) (data >> 48) & 255;
        self->needle [2] = (byte) (data >> 40) & 255;
        self->needle [3] = (byte) (data >> 32) & 255;
        self->needle [4] = (byte) (data >> 24) & 255;
        self->needle [5] = (byte) (data >> 16) & 255;
        self->needle [6] = (byte) (data >>  8) & 255;
        self->needle [7] = (byte) (data & 255);
        self->needle += 8;
        return 0;
    }
    else
        return -1;
}

//  --------------------------------------------------------------------------
//  Put a string to frame payload. Returns 0 if successful else -1
//  The string length limited to 2^16 - 1 for '\0' => 65535 characters.
//  For allocation purpose calculate: (string length + 2) bytes

int
zframe_put_string (zframe_t *self, char *data)
{
    assert (self);
    uint16_t size = strlen (data);
    if (s_sufficient_data (self, 2 + size)) {
        zframe_put_uint16 (self, size);
        memcpy (self->needle, data, size);
        self->needle += size;
        return 0;
    }
    else
        return -1;
}

//  --------------------------------------------------------------------------
//  Get a block of data from the frame payload by copying it to the 'data'
//  location. If there was insufficient data in the frame, returns -1

int
zframe_get_block (zframe_t *self, byte *data, size_t size)
{
    assert (self);  
    if (s_sufficient_data (self, size)) {
        memcpy (data, self->needle, size);
        self->needle += size;
        return 0;
    }
    else 
        return -1;
}

//  --------------------------------------------------------------------------
//  Get a 1-byte integer from the frame payload by copying it to the 'data'
//  location. If there was insufficient data in the frame, returns -1.

int
zframe_get_uint8 (zframe_t *self, uint8_t *data)
{
    assert (self);
    if (s_sufficient_data (self, 1)) {
        *data = *(byte *) self->needle;
        self->needle++;
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Get a 2-byte integer from the frame payload by copying it to the 'data'
//  location. If there was insufficient data in the frame, returns -1.

int 
zframe_get_uint16 (zframe_t *self, uint16_t *data)
{
    assert (self);
    if (s_sufficient_data (self, 2)) {
        *data = ((uint16_t) (self->needle [0]) << 8)
              +  (uint16_t) (self->needle [1]);
        self->needle += 2;
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Get a 4-byte integer from the frame payload by copying it to the 'data'
//  location. If there was insufficient data in the frame, returns -1.

int 
zframe_get_uint32 (zframe_t *self, uint32_t *data)
{
    assert (self);
    if (s_sufficient_data (self, 4)) {
        *data = ((uint32_t) (self->needle [0]) << 24)
              + ((uint32_t) (self->needle [1]) << 16)
              + ((uint32_t) (self->needle [2]) << 8)
              +  (uint32_t) (self->needle [3]);
        self->needle += 4;
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Get a 8-byte integer from the frame payload by copying it to the 'data'
//  location. If there was insufficient data in the frame, returns -1.

int
zframe_get_uint64 (zframe_t *self, uint64_t *data)
{
    assert (self);
    if (s_sufficient_data (self, 8)) {
        *data = ((uint64_t) (self->needle [0]) << 56)
              + ((uint64_t) (self->needle [1]) << 48)
              + ((uint64_t) (self->needle [2]) << 40)
              + ((uint64_t) (self->needle [3]) << 32)
              + ((uint64_t) (self->needle [4]) << 24)
              + ((uint64_t) (self->needle [5]) << 16)
              + ((uint64_t) (self->needle [6]) << 8)
              +  (uint64_t) (self->needle [7]);
        self->needle += 8;
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Get a newly allocated string from frame payload. Returns char pointer to 
//  a string. The max string size can be 2^16 and is 0 terminated. If there
//  was insufficient data in the frame, or the frame was badly formatted,
//  returns NULL.

char *
zframe_get_string (zframe_t *self)
{
    assert (self);
    uint16_t size;
    int rc = zframe_get_uint16 (self, &size);
    assert (rc == 0);
    if (s_sufficient_data (self, size)) {
        char *data = (char *) malloc (size + 1);
        memcpy (data, self->needle, size);
        data [size] = 0;
        self->needle += size;
        return data;
    }
    else
        return NULL;
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

    // Write custom frame
    frame = zframe_new (NULL, 45);
    size_t test_8bit  = 0xFF;
    size_t test_16bit = 0xFFFF;
    size_t test_32bit = 0xFFFFFFFF;
    uint64_t test_64bit = 0xFFFFFFFFFFFFFFFFUL;
    char *test_string = "Hello World!";
    zuuid_t *test_uuid = zuuid_new ();
    rc = zframe_put_uint8 (frame, test_64bit);
    assert (rc == 0);
    rc = zframe_put_uint16 (frame, test_64bit);
    assert (rc == 0);
    rc = zframe_put_uint32 (frame, test_64bit);
    assert (rc == 0);
    rc = zframe_put_uint64 (frame, test_64bit);
    assert (rc == 0);
    rc = zframe_put_string (frame, test_string);
    assert (rc == 0);
    rc = zframe_put_block (frame, zuuid_data (test_uuid), 16);
    assert (rc == 0);
    // one byte more than allocated, expect 1
    rc = zframe_put_uint8 (frame, test_64bit);
    assert (rc == -1);
    rc = zframe_send (&frame, output, 0);
    assert (rc == 0);

    // Read custom frame
    frame = zframe_recv (input);
    uint8_t bit8;
    rc = zframe_get_uint8 (frame, &bit8);
    assert (rc == 0);
    assert (bit8 == test_8bit);
    uint16_t bit16;
    rc = zframe_get_uint16 (frame, &bit16);
    assert (rc == 0);
    assert (bit16 == test_16bit);
    uint32_t bit32;
    rc = zframe_get_uint32 (frame, &bit32);
    assert (rc == 0);
    assert (bit32 == test_32bit);
    uint64_t bit64;
    rc = zframe_get_uint64 (frame, &bit64);
    assert (rc == 0);
    assert (bit64 == test_64bit);
    char *hello = zframe_get_string (frame);
    assert (streq (hello, test_string));
    zuuid_t *uuid = zuuid_new ();
    zframe_get_block (frame, zuuid_data (uuid), 16);
    assert (zuuid_eq (uuid, zuuid_data (test_uuid)));
    zframe_destroy (&frame);
    assert (frame == NULL);

    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
    return 0;
}
