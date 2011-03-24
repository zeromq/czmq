/*  =========================================================================
    zframe - working with single message frames

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of zapi, the C binding for 0MQ: http://zapi.zeromq.org.

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

#include "../include/zapi_prelude.h"
#include "../include/zctx.h"
#include "../include/zframe.h"

//  Structure of our class

struct _zframe_t {
    zmq_msg_t frame;            //  If so, zmq_msg_t for it
    int more;                   //  More flag, from last read
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
    if (size) {
        zmq_msg_init_size (&self->frame, size);
        if (data)
            memcpy (zmq_msg_data (&self->frame), data, size);
    }
    else
        zmq_msg_init (&self->frame);
    
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
        zmq_msg_close (&self->frame);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive frame from socket, returns zframe_t object or NULL if the recv
//  was interrupted. Does a blocking recv, if you want to not block then use
//  the zloop class or zmq_poll to check for socket input before receiving.

zframe_t *
zframe_recv (void *socket)
{
    assert (socket);
    zframe_t *self = zframe_new (NULL, 0);
    if (zmq_recv (socket, &self->frame, 0)) {
        zframe_destroy (&self);
        return NULL;            //  Interrupted or terminated
    }
    //  Check multipart framing
    int64_t more;
    size_t more_size = sizeof (more);
    zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &more_size);
    self->more = (int) more;
    return self;
}


//  --------------------------------------------------------------------------
//  Send frame to socket, destroy after sending unless ZFRAME_REUSE is set.

void
zframe_send (zframe_t **self_p, void *socket, int flags)
{
    assert (socket);
    assert (self_p);
    if (*self_p) {
        zframe_t *self = *self_p;
        if (flags & ZFRAME_REUSE) {
            zmq_msg_t copy;
            zmq_msg_init (&copy);
            zmq_msg_copy (&copy, &self->frame);
            zmq_send (socket, &copy, (flags & ZFRAME_MORE)? ZMQ_SNDMORE: 0);
            zmq_msg_close (&copy);
        }
        else {
            zmq_send (socket, &self->frame, (flags & ZFRAME_MORE)? ZMQ_SNDMORE: 0);
            zframe_destroy (self_p);
        }
    }
}


//  --------------------------------------------------------------------------
//  Return size of frame.

size_t
zframe_size (zframe_t *self)
{
    assert (self);
    return zmq_msg_size (&self->frame);
}


//  --------------------------------------------------------------------------
//  Return pointer to frame data.

void *
zframe_data (zframe_t *self)
{
    assert (self);
    return zmq_msg_data (&self->frame);
}


//  --------------------------------------------------------------------------
//  Return frame MORE indicator (1 or 0), set when reading from socket

int
zframe_more (zframe_t *self)
{
    assert (self);
    return self->more;
}


//  --------------------------------------------------------------------------
//  Selftest

int
zframe_test (Bool verbose)
{
    printf (" * zframe: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();

    void *output = zctx_socket_new (ctx, ZMQ_PAIR);
    zmq_bind (output, "inproc://zframe.test");
    void *input = zctx_socket_new (ctx, ZMQ_PAIR);
    zmq_connect (input, "inproc://zframe.test");
    
    //  Send five different frames, test ZFRAME_MORE
    int frame_nbr;
    for (frame_nbr = 0; frame_nbr < 5; frame_nbr++) {
        zframe_t *frame = zframe_new ("Hello", 5);
        zframe_send (&frame, output, ZFRAME_MORE);
    }
    //  Send same frame five times, test ZFRAME_REUSE
    zframe_t *frame = zframe_new ("Hello", 5);
    for (frame_nbr = 0; frame_nbr < 5; frame_nbr++) {
        zframe_send (&frame, output, ZFRAME_MORE + ZFRAME_REUSE);
    }
    assert (frame);
    zframe_destroy (&frame);
    
    //  Send END frame
    frame = zframe_new ("END", 3);
    zframe_send (&frame, output, 0);
    
    //  Read and count until we receive END
    frame_nbr = 0;
    for (frame_nbr = 0;; frame_nbr++) {
        zframe_t *frame = zframe_recv (input);
        if (zframe_size (frame) == 3
        &&  memcmp (zframe_data (frame), "END", 3) == 0) {
            zframe_destroy (&frame);
            break;
        }
        assert (zframe_more (frame));
        zframe_destroy (&frame);
    }
    assert (frame_nbr == 10);

    zctx_destroy (&ctx);
    //  @end
    printf ("OK\n");
    return 0;
}
