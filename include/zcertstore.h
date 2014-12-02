/*  =========================================================================
    zcertstore - work with CURVE security certificate stores

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZCERTSTORE_H_INCLUDED__
#define __ZCERTSTORE_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

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

//  Print list of certificates in store to logging facility
CZMQ_EXPORT void
    zcertstore_print (zcertstore_t *self);

//  DEPRECATED as incompatible with centralized logging
//  Print list of certificates in store to open stream
CZMQ_EXPORT void
    zcertstore_fprint (zcertstore_t *self, FILE *file);

//  Self test of this class
CZMQ_EXPORT void
    zcertstore_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

//  Deprecated method aliases
#define zcertstore_dump(s) zcertstore_print(s)

#endif
