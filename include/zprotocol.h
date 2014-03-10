/*  =========================================================================
    zprotocol - work with repeating message payloads

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

#ifndef __ZPROTOCOL_H_INCLUDED__
#define __ZPROTOCOL_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zprotocol_t zprotocol_t;

//  @interface
//  Callback function for zprotocol pack message method
typedef zmsg_t * (zprotocol_packmessage_fn) (void *args);
//  Callback function for zprotocol unpack message method
typedef void * (zprotocol_unpackmessage_fn) (zmsg_t *msg);


//  Constructor. Takes two callback methods.
//  The first to add data to a message's payload.
//  The seccond to get data from a message's payload
CZMQ_EXPORT zprotocol_t *
    zprotocol_new (zprotocol_packmessage_fn *pack_fn, zprotocol_unpackmessage_fn *unpack_fn);

//  Destructor
CZMQ_EXPORT void
    zprotocol_destroy (zprotocol_t **self_p);

//  Adds the contents passed with args to a message's payload and sends 
//  over the provided zeromq socket. Returns 0 if successful, otherwise -1

CZMQ_EXPORT int
    zprotocol_send (zprotocol_t *self, void *zocket, void *args);

//  Receives a message from the provided zeromq socket and unpacks the
//  content of the message payload to some kind of structure.
//  The message is automatically destroyed by this method.
//  Returns the structure that has been unpacked from payload, otherwise NULL.

CZMQ_EXPORT void *
    zprotocol_recv (zprotocol_t *self, void *zocket);

//  Receives a message from the provided zeromq socket and unpacks the
//  content of the message payload to some kind of structure. Doesn't go into
//  blocking mode, it returns immediatly if there are no messages queued.
//  The message is automatically destroyed by this method.
//  Returns the structure that has been unpacked from payload, otherwise NULL.

CZMQ_EXPORT void *
    zprotocol_recv_nowait (zprotocol_t *self, void *zocket);


CZMQ_EXPORT void
    zprotocol_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif

