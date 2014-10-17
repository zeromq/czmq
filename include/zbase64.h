/*  =========================================================================
    zbase64 - base64 encoding and decoding

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZBASE64_H_INCLUDED__
#define __ZBASE64_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface

//  Enumeration defining different encoding modes
typedef enum {
    ZBASE64_MODE_RFC4648_STD,
    ZBASE64_MODE_RFC4648_URL
} zbase64_mode_t;


//  Create a new zbase64
CZMQ_EXPORT zbase64_t *
    zbase64_new ();

//  Destroy the zbase64
CZMQ_EXPORT void
    zbase64_destroy (zbase64_t **self_p);

//  Print properties of object
CZMQ_EXPORT void
    zbase64_print (zbase64_t *self);

//  Encode a stream of bytes into a base64 string.
CZMQ_EXPORT char *
    zbase64_encode (zbase64_t *self, byte *data, size_t data_size);

//  Decode a base64 string into a string of bytes.
//  The decoded output is null-terminated, so it may be treated
//  as a string, if that's what it was prior to encoding.
CZMQ_EXPORT byte *
    zbase64_decode (zbase64_t *self, char *data, size_t *decode_size);

//  Get/set the mode property
CZMQ_EXPORT zbase64_mode_t
    zbase64_mode (zbase64_t *self);
CZMQ_EXPORT void
    zbase64_set_mode (zbase64_t *self, zbase64_mode_t mode);

//  Get/set the pad property
CZMQ_EXPORT bool
    zbase64_pad (zbase64_t *self);
CZMQ_EXPORT void
    zbase64_set_pad (zbase64_t *self, bool pad);

//  Get/set the pad_char property
CZMQ_EXPORT char
    zbase64_pad_char (zbase64_t *self);
CZMQ_EXPORT void
    zbase64_set_pad_char (zbase64_t *self, char pad_char);

//  Self test of this class
CZMQ_EXPORT int
    zbase64_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
