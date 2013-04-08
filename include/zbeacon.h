/*  =========================================================================
    zbeacon - LAN service announcement and discovery
    
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

#ifndef __ZBEACON_H_INCLUDED__
#define __ZBEACON_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zbeacon_t zbeacon_t;

//  @interface
//  Create a new beacon on a certain UDP port
CZMQ_EXPORT zbeacon_t *
    zbeacon_new (int port_nbr);
    
//  Destroy a beacon
CZMQ_EXPORT void
    zbeacon_destroy (zbeacon_t **self_p);

//  Return our own IP address as printable string
CZMQ_EXPORT char *
    zbeacon_hostname (zbeacon_t *self);

//  Set broadcast interval in milliseconds (default is 1000 msec)
CZMQ_EXPORT void
    zbeacon_set_interval (zbeacon_t *self, int interval);

//  Filter out any beacon that looks exactly like ours
CZMQ_EXPORT void
    zbeacon_noecho (zbeacon_t *self);

//  Start broadcasting beacon to peers at the specified interval
CZMQ_EXPORT void
    zbeacon_publish (zbeacon_t *self, byte *transmit, size_t size);
    
//  Stop broadcasting beacons
CZMQ_EXPORT void
    zbeacon_silence (zbeacon_t *self);

//  Start listening to other peers; zero-sized filter means get everything
CZMQ_EXPORT void
    zbeacon_subscribe (zbeacon_t *self, byte *filter, size_t size);

//  Stop listening to other peers
CZMQ_EXPORT void
    zbeacon_unsubscribe (zbeacon_t *self);

//  Get beacon pipe, for polling or receiving messages
CZMQ_EXPORT void *
    zbeacon_pipe (zbeacon_t *self);

//  Self test of this class
CZMQ_EXPORT void
    zbeacon_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
