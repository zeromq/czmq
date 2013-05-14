/*  =========================================================================
    zcurve - CurveZMQ security engine (rfc.zeromq.org/spec:26)

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

#ifndef __ZCURVE_H_INCLUDED__
#define __ZCURVE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zcurve_t zcurve_t;

//  @interface
//  Constructor; to create a client instance, pass the server public
//  key. If you pass null, you create a server instance.
CZMQ_EXPORT zcurve_t *
    zcurve_new (byte *server_key);
    
//  Destructor
CZMQ_EXPORT void
    zcurve_destroy (zcurve_t **self_p);

//  Long-term key management for servers; generate a new key pair
CZMQ_EXPORT void
    zcurve_keypair_new (zcurve_t *self);

//  Save long-term key pair to disk; not confidential
CZMQ_EXPORT int
    zcurve_keypair_save (zcurve_t *self);

//  Load long-term key pair from disk
CZMQ_EXPORT int
    zcurve_keypair_load (zcurve_t *self);

//  Return public part of key pair
CZMQ_EXPORT byte *
    zcurve_keypair_public (zcurve_t *self);
    
//  Set a metadata property; these are sent to the peer after the
//  security handshake. Property values are strings.
CZMQ_EXPORT void
    zcurve_metadata_set (zcurve_t *self, char *name, char *value);

//  Accept input command from peer. If the command is invalid, it is
//  discarded silently. May return a blob to send to the peer, or NULL
//  if there is nothing to send.
CZMQ_EXPORT zframe_t *
    zcurve_execute (zcurve_t *self, zframe_t *input);

//  Encode clear-text message to peer. Returns a blob ready to send
//  on the wire.
CZMQ_EXPORT zframe_t *
    zcurve_encode (zcurve_t *self, zframe_t *clear);

//  Decode blob into message from peer.
CZMQ_EXPORT zframe_t *
    zcurve_decode (zcurve_t *self, zframe_t *input);

//  Indicate whether handshake is still in progress
CZMQ_EXPORT bool
    zcurve_connected (zcurve_t *self);

//  Self test of this class
void
    zcurve_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
