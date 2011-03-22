/*  =========================================================================
    zframe - working with 0MQ contexts

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

#include "../include/zapi_prelude.h"
#include "../include/zframe.h"

//  Structure of our class

struct _zframe_t {
    void *context;              //  0MQ context object
};


//  --------------------------------------------------------------------------
//  Constructor

zframe_t *
zframe_new (const void *data, size_t size)
{
    zframe_t
        *self;

    self = (zframe_t *) calloc (1, sizeof (zframe_t));
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
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Receive frame from socket

zframe_t *
zframe_recv (void *socket, int flags)
{
    assert (socket);
    return NULL;
}


//  --------------------------------------------------------------------------
//  Send frame to socket, destroy after sending unless ZFRAME_REUSE is set.

void
zframe_send (zframe_t **self_p, void *socket, int flags)
{
    assert (socket);
}


//  --------------------------------------------------------------------------
//  Return size of frame.

size_t
zframe_size (zframe_t *self)
{
    assert (self);
    return 0;
}


//  --------------------------------------------------------------------------
//  Return pointer to frame data.

void *
zframe_data (zframe_t *self)
{
    assert (self);
    return NULL;
}


//  --------------------------------------------------------------------------
//  Return frame MORE indicator (1 or 0), set when reading from socket

int
zframe_more (zframe_t *self)
{
    assert (self);
    return 0;
}


//  --------------------------------------------------------------------------
//  Selftest

int
zframe_test (Bool verbose)
{
    zframe_t
        *frame;

    printf (" * zframe: ");
    frame = zframe_new ("Hello", 5);
    assert (frame);

    zframe_destroy (&frame);
    assert (frame == NULL);

    printf ("OK\n");
    return 0;
}
