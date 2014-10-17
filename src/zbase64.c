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

/*
@header
    zbase64 - base64 encoding and decoding
@discuss
    The zbase64 class implements encoding and decoding of base64 data
    based on the definitions in RFC4648. Both the standard alphabet from
    paragraph 4 and the URL and filename friendly one from paragraph 5
    are supported. The class API defines a flag to specify whether to
    pad out the encoded string to a multiple of 4 chars, but this is
    not implemented yet. The API also defines accessors for the character
    to use for padding. Breaking the output into lines is not implemented.
@end
*/

#include "../include/czmq.h"
#include "../include/zbase64.h"

//  Structure of our class

struct _zbase64_t {
    zbase64_mode_t mode;                //  The current mode (std or url)
    bool pad;                           //  Should output be padded?
    char pad_char;                      //  The pad character
};


//  --------------------------------------------------------------------------
//  Create a new zbase64

zbase64_t *
zbase64_new ()
{
    zbase64_t *self = (zbase64_t *) zmalloc (sizeof (zbase64_t));
    assert (self);

    //  Setup default as RFC4648 paragraph 4
    self->mode = ZBASE64_MODE_RFC4648_STD;
    self->pad = true;
    self->pad_char = '=';

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zbase64

void
zbase64_destroy (zbase64_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zbase64_t *self = *self_p;

        //  Free class properties

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Print properties of object

void
zbase64_print (zbase64_t *self)
{
    assert (self);

    zsys_debug ("zbase64:");
    zsys_debug ("   mode=%d", self->mode);
    zsys_debug ("   pad=%s", self->pad? "true": "false");
    zsys_debug ("   pad_char=%c", self->pad_char);
}


//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 4 (standard alphabet)
static char  //        0----5----0----5----0----5----0----5----0----5----0----5----0---
s_rfc4648p4_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789/+";

//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 5 (URL & filename friendly alphabet)
static char  //        0----5----0----5----0----5----0----5----0----5----0----5----0---
s_rfc4648p5_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static char *
s_base64_encode (byte *data, size_t length, const char *alphabet)
{
    char *str = (char *) zmalloc (4 * (length / 3) + ((length % 3)? length % 3 + 1: 0) + 1);
    char *enc = str;
    byte *needle = data, *ceiling = data + length;
    while (needle < ceiling) {
        *enc++ = alphabet[(*needle) >> 2];
        if (needle + 1 < ceiling) {
            *enc++ = alphabet[((*needle << 4) & 0x30) | (*(needle + 1) >> 4)];
            if (needle + 2 < ceiling) {
                *enc++ = alphabet[((*(needle + 1) << 2) & 0x3c) | (*(needle + 2) >> 6)];
                *enc++ = alphabet[*(needle + 2) & 0x3f];
            }
            else
                *enc++ = alphabet[(*(needle + 1) << 2) & 0x3c];
        }
        else
            *enc++ = alphabet[(*needle << 4) & 0x30];
        needle += 3;
    }
    *enc = 0;
    return str;
}

static byte *
s_base64_decode (const char *data, size_t *size, const char *alphabet)
{
    int length = strlen (data);
    *size = 3 * (length / 4) + ((length % 4)? length % 4 - 1 : 0) + 1;
    byte *bytes = (byte *) zmalloc (*size);
    byte *dec = bytes;
    byte *needle = (byte *) data, *ceiling = (byte *) (data + length);
    byte i1, i2, i3 = 0, i4;
    while (needle < ceiling) {
        i1 = (byte) (strchr (alphabet, *needle) - alphabet);
        i2 = (byte) (strchr (alphabet, *(needle + 1)) - alphabet);
        *dec++ = i1 << 2 | i2 >> 4;
        if (needle + 2 < ceiling) {
            i3 = (byte) (strchr (alphabet, *(needle + 2)) - alphabet);
            *dec++ = i2 << 4 | i3 >> 2;
        }
        if (needle + 3 < ceiling) {
            i4 = (byte) (strchr (alphabet, *(needle + 3)) - alphabet);
            *dec++ = i3 << 6 | i4;
        }
        needle += 4;
    }
    *dec = 0;
    return bytes;
}


//  Definition of encode method
char *
zbase64_encode (zbase64_t *self, byte *data, size_t data_size)
{
    assert (self);
    assert (data);
    const char *alphabet = self->mode == ZBASE64_MODE_RFC4648_STD? s_rfc4648p4_alphabet: s_rfc4648p5_alphabet;
    return s_base64_encode (data, data_size, alphabet);
}


//  Definition of decode method
byte *
zbase64_decode (zbase64_t *self, char *data, size_t *decode_size)
{
    assert (self);
    assert (data);
    assert (decode_size);
    const char *alphabet = self->mode == ZBASE64_MODE_RFC4648_STD? s_rfc4648p4_alphabet: s_rfc4648p5_alphabet;
    return s_base64_decode (data, decode_size, alphabet);
}


//  --------------------------------------------------------------------------
//  Get/set the mode property

zbase64_mode_t
zbase64_mode (zbase64_t *self)
{
    assert (self);
    return self->mode;
}

void
zbase64_set_mode (zbase64_t *self, zbase64_mode_t mode)
{
    assert (self);
    self->mode = mode;
}

//  --------------------------------------------------------------------------
//  Get/set the pad property

bool
zbase64_pad (zbase64_t *self)
{
    assert (self);
    return self->pad;
}

void
zbase64_set_pad (zbase64_t *self, bool pad)
{
    assert (self);
    self->pad = pad;
}

//  --------------------------------------------------------------------------
//  Get/set the pad_char property

char
zbase64_pad_char (zbase64_t *self)
{
    assert (self);
    return self->pad_char;
}

void
zbase64_set_pad_char (zbase64_t *self, char pad_char)
{
    assert (self);
    self->pad_char = pad_char;
}


//  ---------------------------------------------------------------------------
//  base64 test utility

static void
s_base64_test (zbase64_t *self, const char *test_string, const char *expected_result, bool verbose)
{
    assert (self);
    assert (test_string);
    assert (expected_result);

    char *encoded = zbase64_encode (self, (byte *) test_string, strlen (test_string));
    assert (encoded);
    assert (strlen (encoded) == strlen (expected_result));
    assert (streq (encoded, expected_result));

    if (verbose)
        zsys_debug ("       encoded '%s' into '%s'", test_string, encoded);

    size_t size;
    char *decoded = (char *) zbase64_decode (self, encoded, &size);
    assert (decoded);
    assert (size == strlen (decoded) + 1);
    assert (streq (decoded, test_string));

    if (verbose)
        zsys_debug ("       decoded '%s' into '%s'", encoded, decoded);

    free (encoded);
    free (decoded);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zbase64_test (bool verbose)
{
    printf (" * zbase64: ");

    if (verbose)
        printf ("\n");

    //  @selftest
    zbase64_t *self = zbase64_new ();
    assert (self);

    zbase64_mode_t mode = zbase64_mode (self);
    assert (mode == ZBASE64_MODE_RFC4648_STD);

    zbase64_set_mode (self, ZBASE64_MODE_RFC4648_URL);
    mode = zbase64_mode (self);
    assert (mode == ZBASE64_MODE_RFC4648_URL);

    bool pad = zbase64_pad (self);
    assert (pad);

    zbase64_set_pad (self, false);
    pad = zbase64_pad (self);
    assert (!pad);

    char pad_char = zbase64_pad_char (self);
    assert (pad_char == '=');

    if (verbose)
        zbase64_print (self);

    zbase64_set_pad_char (self, '-');
    pad_char = zbase64_pad_char (self);
    assert (pad_char == '-');

    //  Test against test vectors from RFC4648.
    s_base64_test (self, "", "", verbose);
    s_base64_test (self, "f", "Zg", verbose);
    s_base64_test (self, "fo", "Zm8", verbose);
    s_base64_test (self, "foo", "Zm9v", verbose);
    s_base64_test (self, "foob", "Zm9vYg", verbose);
    s_base64_test (self, "fooba", "Zm9vYmE", verbose);
    s_base64_test (self, "foobar", "Zm9vYmFy", verbose);

    zbase64_destroy (&self);
    //  @end

    printf ("OK\n");
    return 0;
}
