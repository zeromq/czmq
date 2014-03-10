/*  =========================================================================
    zprotocol - work repeating message payloads

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
    The zprotocol simplifies the implementation of a protocol where the 
    payload of a message is reoccurring.
@discuss
@end
*/
 
#include "../include/czmq.h"

//  Structure of our class

struct _zprotocol_t {
    int (*pack_message)(void *args);
    zprotocol_packmessage_fn *pack_fn;;
    zprotocol_unpackmessage_fn *unpack_fn;
};


//  --------------------------------------------------------------------------
//  Constructor. Takes two callback methods.
//  The first to add data to a message's payload.
//  The seccond to get data from a message's payload

zprotocol_t *
zprotocol_new (zprotocol_packmessage_fn *pack_fn, zprotocol_unpackmessage_fn *unpack_fn)
{
    zprotocol_t *self = (zprotocol_t *) zmalloc (sizeof (zprotocol_t));
    assert (self);

    self->pack_fn = pack_fn;
    self->unpack_fn = unpack_fn;
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zprotocol_destroy (zprotocol_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zprotocol_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Adds the contents passed with args to a message's payload and sends 
//  over the provided zeromq socket. Returns 0 if successful, otherwise -1

int
zprotocol_send (zprotocol_t *self, void *zocket, void *args)
{
    assert (self);
    assert (zocket);

    zmsg_t *msg = (self->pack_fn) (args);
    int rc = 0;
    if (msg) {
        rc = zmsg_send (&msg, zocket);
    }
    else {
        rc = -1;
    } 
    return rc;
}

//  --------------------------------------------------------------------------
//  Receives a message from the provided zeromq socket and unpacks the
//  content of the message payload to some kind of structure.
//  Returns the structure that has been unpacked from payload, otherwise NULL.

void *
zprotocol_recv (zprotocol_t *self, void *zocket)
{
    assert (self);
    assert (zocket);

    zmsg_t *msg = zmsg_recv (zocket);    
    if (msg) {   
        return (self->unpack_fn) (msg);
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Receives a message from the provided zeromq socket and unpacks the
//  content of the message payload to some kind of structure. Doesn't go into
//  blocking mode, it returns immediatly if there are no messages queued.
//  Returns the structure that has been unpacked from payload, otherwise NULL.

void *
zprotocol_recv_nowait (zprotocol_t *self, void *zocket)
{
    assert (self);
    assert (zocket);

    zmsg_t *msg = zmsg_recv_nowait (zocket);
    if (msg) {   
        return (self->unpack_fn) (msg);
    }
    return NULL;
}

zmsg_t *
s_test_packmessage_fn (void *args)
{
    return NULL;
}

void *
s_test_unpackmessage_fn (zmsg_t *msg)
{
    return NULL;
}

typedef struct {
    char *a;
    int b;
    uint8_t c;
    uint32_t d;
} zprotocol_test_t;

void
zprotocol_test (bool verbose)
{
    zctx_t *ctx = zctx_new ();

    zprotocol_t *protocol = zprotocol_new (s_test_packmessage_fn, s_test_unpackmessage_fn);
    
    zprotocol_test_t *test1 = malloc (sizeof (zprotocol_test_t));
    test1->a = "Hello World!";
    test1->b = 42;
    test1->c = 0xFA;
    test1->d = 0xFAFAFAFA;

    int rc = zprotocol_send (protocol, NULL, test1);
}

