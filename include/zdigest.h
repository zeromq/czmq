/*  =========================================================================
    zdigest - provides hashing functions (SHA-1 at present)

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZDIGEST_H_INCLUDED__
#define __ZDIGEST_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Constructor - creates new digest object, which you use to build up a
//  digest by repeatedly calling zdigest_update() on chunks of data.
CZMQ_EXPORT zdigest_t *
    zdigest_new (void);
    
//  Destroy a digest object
CZMQ_EXPORT void
    zdigest_destroy (zdigest_t **self_p);
    
//  Add buffer into digest calculation
CZMQ_EXPORT void
    zdigest_update (zdigest_t *self, byte *buffer, size_t length);
    
//  Return final digest hash data. If built without crypto support, returns
//  NULL.
CZMQ_EXPORT byte *
    zdigest_data (zdigest_t *self);
    
//  Return final digest hash size
CZMQ_EXPORT size_t
    zdigest_size (zdigest_t *self);
    
//  Return digest as printable hex string; caller should not modify nor
//  free this string. After calling this, you may not use zdigest_update()
//  on the same digest. If built without crypto support, returns NULL.
CZMQ_EXPORT char *
    zdigest_string (zdigest_t *self);
    
//  Self test of this class
CZMQ_EXPORT void
    zdigest_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
