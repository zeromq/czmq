/*  =========================================================================
    zmsg - working with 0MQ contexts

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
#include "../include/zmsg.h"

//  Structure of our class

struct _zmsg_t {
    void *context;              //  0MQ context object
};


//  --------------------------------------------------------------------------
//  Constructor

zmsg_t *
zmsg_new (const void *data, size_t size)
{
    zmsg_t
        *self;

    self = (zmsg_t *) calloc (1, sizeof (zmsg_t));
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
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  

zmsg_t *
zmsg_recv (void *socket)
{
    assert (socket);
    return NULL;
}


//  --------------------------------------------------------------------------
//  

void
zmsg_send (zmsg_t **self_p, void *socket)
{
    assert (self_p);
    assert (socket);
}


//  --------------------------------------------------------------------------
//  

size_t
zmsg_size (zmsg_t *self)
{
    assert (self);
    return 0;
}


//  --------------------------------------------------------------------------
//  

void
zmsg_push (zmsg_t *self, const zframe_t *frame)
{
    assert (self);
    assert (frame);
}


//  --------------------------------------------------------------------------
//  

void
zmsg_queue (zmsg_t *self, const zframe_t *frame)
{
    assert (self);
    assert (frame);
}


//  --------------------------------------------------------------------------
//  

void
zmsg_pushmem (zmsg_t *self, const void *src, size_t size)
{
    assert (self);
    assert (src);
}


//  --------------------------------------------------------------------------
//  

void
zmsg_queuemem (zmsg_t *self, const void *src, size_t size)
{
    assert (self);
    assert (src);
}


//  --------------------------------------------------------------------------
//  

zframe_t *
zmsg_pop (zmsg_t *self)
{
    assert (self);
    return NULL;
}

//  --------------------------------------------------------------------------
//  

zframe_t *
zmsg_remove (zmsg_t *self)
{
    assert (self);
    return NULL;
}


//  --------------------------------------------------------------------------
//  

zmsg_t *
zmsg_load (FILE *file)
{
    assert (file);
    return NULL;
}


//  --------------------------------------------------------------------------
//  

void
zmsg_save (zmsg_t **self_p, FILE *file)
{
    assert (self_p);
    assert (file);
}


//  --------------------------------------------------------------------------
//  

void
zmsg_dump (zmsg_t *self)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zmsg_test (Bool verbose)
{
    zmsg_t
        *msg;

    printf (" * zmsg: ");
    msg = zmsg_new ("Hello", 5);
    assert (msg);

    zmsg_destroy (&msg);
    assert (msg == NULL);

    printf ("OK\n");
    return 0;
}
