/*  =========================================================================
    zarmour - armoured text encoding and decoding

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZARMOUR_H_INCLUDED__
#define __ZARMOUR_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface

//  Enumeration defining different encoding modes
typedef enum {
      ZARMOUR_MODE_BASE64_STD   //  Standard base 64
    , ZARMOUR_MODE_BASE64_URL   //  URL and filename friendly base 64
    , ZARMOUR_MODE_BASE32_STD   //  Standard base 32
    , ZARMOUR_MODE_BASE32_HEX   //  Extended hex base 32
    , ZARMOUR_MODE_BASE16       //  Standard base 16
#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 3, 0))
    , ZARMOUR_MODE_Z85          //  Z85 from ZeroMQ RFC 32
#endif
} zarmour_mode_t;


//  Create a new zarmour
CZMQ_EXPORT zarmour_t *
    zarmour_new ();

//  Destroy the zarmour
CZMQ_EXPORT void
    zarmour_destroy (zarmour_t **self_p);

//  Print properties of object
CZMQ_EXPORT void
    zarmour_print (zarmour_t *self);

//  Get printable string for mode
CZMQ_EXPORT const char *
    zarmour_mode_str (zarmour_t *self);

//  Encode a stream of bytes into an armoured string.
CZMQ_EXPORT char *
    zarmour_encode (zarmour_t *self, const byte *data, size_t data_size);

//  Decode an armoured string into a string of bytes.
//  The decoded output is null-terminated, so it may be treated
//  as a string, if that's what it was prior to encoding.
CZMQ_EXPORT byte *
    zarmour_decode (zarmour_t *self, const char *data, size_t *decode_size);

//  Get/set the mode property
CZMQ_EXPORT zarmour_mode_t
    zarmour_mode (zarmour_t *self);
CZMQ_EXPORT void
    zarmour_set_mode (zarmour_t *self, zarmour_mode_t mode);

//  Get/set the pad property
CZMQ_EXPORT bool
    zarmour_pad (zarmour_t *self);
CZMQ_EXPORT void
    zarmour_set_pad (zarmour_t *self, bool pad);

//  Get/set the pad_char property
CZMQ_EXPORT char
    zarmour_pad_char (zarmour_t *self);
CZMQ_EXPORT void
    zarmour_set_pad_char (zarmour_t *self, char pad_char);

//  Get/set the line_breaks property
CZMQ_EXPORT bool
    zarmour_line_breaks (zarmour_t *self);
CZMQ_EXPORT void
    zarmour_set_line_breaks (zarmour_t *self, bool line_breaks);

//  Get/set the line_length property
CZMQ_EXPORT size_t
    zarmour_line_length (zarmour_t *self);
CZMQ_EXPORT void
    zarmour_set_line_length (zarmour_t *self, size_t line_length);

//  Self test of this class
CZMQ_EXPORT int
    zarmour_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
