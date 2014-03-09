/*  =========================================================================
    zcertstore - work with CURVE security certificate stores

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

#ifndef __ZCERTSTORE_H_INCLUDED__
#define __ZCERTSTORE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zcertstore_t zcertstore_t;

//  @interface

//  Create a new certificate store from a disk directory, loading and 
//  indexing all certificates in that location. The directory itself may be
//  absent, and created later, or modified at any time. The certificate store 
//  is automatically refreshed on any zcertstore_lookup() call. If the 
//  location is specified as NULL, creates a pure-memory store, which you 
//  can work with by inserting certificates at runtime.
CZMQ_EXPORT zcertstore_t *
    zcertstore_new (const char *location);

//  Destroy a certificate store object in memory. Does not affect anything
//  stored on disk.
CZMQ_EXPORT void
    zcertstore_destroy (zcertstore_t **self_p);

//  Look up certificate by public key, returns zcert_t object if found, 
//  else returns NULL. The public key is provided in Z85 text format.
CZMQ_EXPORT zcert_t *
    zcertstore_lookup (zcertstore_t *self, const char *public_key);

//  Insert certificate into certificate store in memory. Note that this 
//  does not save the certificate to disk. To do that, use zcert_save()
//  directly on the certificate. Takes ownership of zcert_t object.
CZMQ_EXPORT void
    zcertstore_insert (zcertstore_t *self, zcert_t **cert_p);

//  Print list of certificates in store to open stream
CZMQ_EXPORT void
    zcertstore_fprint (zcertstore_t *self, FILE *file);

//  Print list of certificates in store to stdout
CZMQ_EXPORT void
    zcertstore_print (zcertstore_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zcertstore_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

//  Deprecated method aliases
#define zcertstore_dump(s) zcertstore_print(s)

#endif
