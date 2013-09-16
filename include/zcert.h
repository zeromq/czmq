/*  =========================================================================
    zcert - work with CURVE security certificates

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
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

#ifndef __ZCERT_H_INCLUDED__
#define __ZCERT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zcert_t zcert_t;

//  @interface

//  Create and initialize a new certificate in memory
CZMQ_EXPORT zcert_t *
    zcert_new (void);

//  Constructor, accepts public/secret key pair from caller
CZMQ_EXPORT zcert_t *
    zcert_new_from (byte *public_key, byte *secret_key);

//  Destroy a certificate in memory
CZMQ_EXPORT void
    zcert_destroy (zcert_t **self_p);

//  Return public part of key pair as 32-byte binary string
CZMQ_EXPORT byte *
    zcert_public_key (zcert_t *self);

//  Return secret part of key pair as 32-byte binary string
CZMQ_EXPORT byte *
    zcert_secret_key (zcert_t *self);

//  Return public part of key pair as Z85 armored string
CZMQ_EXPORT char *
    zcert_public_txt (zcert_t *self);

//  Return secret part of key pair as Z85 armored string
CZMQ_EXPORT char *
    zcert_secret_txt (zcert_t *self);

//  Set certificate header (metadata) from formatted string.
CZMQ_EXPORT void
    zcert_set_header (zcert_t *self, const char *name, const char *format, ...);

//  Get header value from certificate; if the header doesn't exist, returns
//  NULL.
CZMQ_EXPORT char *
    zcert_header (zcert_t *self, const char *name);

//  Save certificate to file for persistent storage
CZMQ_EXPORT int
    zcert_save (zcert_t *self, char *filename);

//  Load certificate from file (constructor)
CZMQ_EXPORT zcert_t *
    zcert_load (char *filename);

//  Return copy of certificate
CZMQ_EXPORT zcert_t *
    zcert_dup (zcert_t *self);

//  Return true if two certificates have the same keys
CZMQ_EXPORT bool
    zcert_eq (zcert_t *self, zcert_t *compare);

//  Dump certificate contents to stderr for debugging
CZMQ_EXPORT void
    zcert_dump (zcert_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zcert_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
