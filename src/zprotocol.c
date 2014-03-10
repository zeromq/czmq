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
    The zprotocol simplifies message payload implementations for protocols.
@discuss
@end
*/
 
#include "../include/czmq.h"

//  Structure of our class

struct _zprotocol_t {
    int (*pack_message)(void *args);
    zprotocol_packmessage_fn *pack_fn;
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
//  The message is automatically destroyed by this method.
//  Returns the structure that has been unpacked from payload, otherwise NULL.

void *
zprotocol_recv (zprotocol_t *self, void *zocket)
{
    assert (self);
    assert (zocket);

    zmsg_t *msg = zmsg_recv (zocket);    
    if (msg) {
        void *data = (self->unpack_fn) (msg);
        zmsg_destroy (&msg);
        return data;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Receives a message from the provided zeromq socket and unpacks the
//  content of the message payload to some kind of structure. Doesn't go into
//  blocking mode, it returns immediatly if there are no messages queued.
//  The message is automatically destroyed by this method.
//  Returns the structure that has been unpacked from payload, otherwise NULL.

void *
zprotocol_recv_nowait (zprotocol_t *self, void *zocket)
{
    assert (self);
    assert (zocket);

    zmsg_t *msg = zmsg_recv_nowait (zocket);
    if (msg) {   
        void *data = (self->unpack_fn) (msg);
        zmsg_destroy (&msg);
        return data;
    }
    return NULL;
}


//  --------------------------------------------------------------------------
//  Test structs/methods

typedef struct {
    char *sender;
    int command;
    uint64_t some_value1;
    uint32_t some_value2;
} zprotocol_test_t;

zmsg_t *
s_test_packmessage_fn (void *args)
{
    assert (args);
    zprotocol_test_t *test = (zprotocol_test_t *) args;
 
    int size = 2 + strlen (test->sender) + 8 + 8 + 4;

    zmsg_t *msg = zmsg_new ();
    zframe_t *frame = zframe_new (NULL, size);
    zframe_put_string (frame, test->sender);
    zframe_put_uint64 (frame, test->command);
    zframe_put_uint64 (frame, test->some_value1);
    zframe_put_uint32 (frame, test->some_value2);
    zmsg_append (msg, &frame);

    return msg;
}

void *
s_test_unpackmessage_fn (zmsg_t *msg)
{
    assert (msg);
    zprotocol_test_t *test = malloc (sizeof (zprotocol_test_t));
    zframe_t *frame = zmsg_pop (msg);
    
    test->sender = zframe_get_string (frame);
    uint64_t command;
    zframe_get_uint64 (frame, &command);
    test->command = command;
    zframe_get_uint64 (frame, &test->some_value1);
    zframe_get_uint32 (frame, &test->some_value2);

    return test;
}

void
zprotocol_test (bool verbose)
{
    //  Setup
    void *ctx = zmq_ctx_new ();
    void *pipe1 = zmq_socket (ctx, ZMQ_PAIR);
    zmq_bind (pipe1, "inproc://zprotocol");
    void *pipe2 = zmq_socket (ctx, ZMQ_PAIR);
    zmq_connect (pipe2, "inproc://zprotocol");

    //  New Protocol Class
    zprotocol_t *protocol = zprotocol_new (s_test_packmessage_fn, s_test_unpackmessage_fn);
   
    //  Fill struct 
    zprotocol_test_t *test = malloc (sizeof (zprotocol_test_t));
    test->sender = "Paul";
    test->command = 0x5;
    test->some_value1 = 2046;
    test->some_value2 = 256;

    //  Pack/Send struct
    int rc = zprotocol_send (protocol, pipe1, test);
    assert (rc == 0);

    //  Assert actual
    zprotocol_test_t *actual = zprotocol_recv (protocol, pipe2);
    assert (streq (test->sender, actual->sender));
    assert (test->command == actual->command);
    assert (test->some_value1 == actual->some_value1);
    assert (test->some_value2 == actual->some_value2);

    //  Cleanup
    zmq_close (pipe1);
    zmq_close (pipe2);
    zmq_ctx_destroy (&ctx);
}

