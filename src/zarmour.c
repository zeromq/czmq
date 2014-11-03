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

/*
@header
    zarmour - armoured text encoding and decoding
@discuss
    The zarmour class implements encoding and decoding of armoured text data.
    Currently, only base64 from RFC4648 (both the standard alphabet from
    paragraph 4 and the URL and filename friendly one from paragraph 5) is
    supported. The class API defines a flag to specify whether to
    pad out the encoded string to the multiple of chars as specified by the
    applicable standard, but this is not implemented yet. The API also defines
    accessors for the character to use for padding. Breaking the output into
    lines is not implemented.
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zarmour_t {
    zarmour_mode_t mode;                //  The current mode (std or url)
    bool pad;                           //  Should output be padded?
    char pad_char;                      //  The pad character
};


//  --------------------------------------------------------------------------
//  Create a new zarmour

zarmour_t *
zarmour_new ()
{
    zarmour_t *self = (zarmour_t *) zmalloc (sizeof (zarmour_t));
    assert (self);

    //  Setup default as RFC4648 paragraph 4
    self->mode = ZARMOUR_MODE_BASE64_STD;
    self->pad = true;
    self->pad_char = '=';

    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zarmour

void
zarmour_destroy (zarmour_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zarmour_t *self = *self_p;

        //  Free class properties

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Print properties of object

void
zarmour_print (zarmour_t *self)
{
    assert (self);

    zsys_debug ("zarmour:");
    zsys_debug ("   mode=%d", self->mode);
    zsys_debug ("   pad=%s", self->pad? "true": "false");
    zsys_debug ("   pad_char=%c", self->pad_char);
}


//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 4 (standard base64 alphabet)
static char  //        0----5----0----5----0----5----0----5----0----5----0----5----0---
s_rfc4648p4_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 5 (URL & filename friendly base64 alphabet)
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


//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 8 (standard base16 alphabet)
static char  //           0----5----0----5
s_rfc4648p8_alphabet[] = "0123456789ABCDEF";

static char *
s_base16_encode (byte *data, size_t length, const char *alphabet)
{
    char *str = (char *) zmalloc (2 * length + 1);
    char *enc = str;
    byte *needle = data, *ceiling = data + length;
    while (needle < ceiling) {
        *enc++ = alphabet[(*needle) >> 4];
        *enc++ = alphabet[(*needle++) & 0x0f];
    }
    *enc = 0;
    return str;
}

static byte *
s_base16_decode (const char *data, size_t *size, const char *alphabet)
{
    int length = strlen (data);
    *size = length / 2 + 1;
    byte *bytes = (byte *) zmalloc (*size);
    byte *dec = bytes;
    byte *needle = (byte *) data, *ceiling = (byte *) (data + length);
    byte i1, i2;
    while (needle < ceiling) {
        i1 = (byte) (strchr (alphabet, *needle) - alphabet);
        i2 = (byte) (strchr (alphabet, *(needle + 1)) - alphabet);
        *dec++ = i1 << 4 | i2;
        needle += 2;
    }
    *dec = 0;
    return bytes;
}


//  Definition of encode method
char *
zarmour_encode (zarmour_t *self, byte *data, size_t data_size)
{
    assert (self);
    assert (data);

    switch (self->mode) {
        case ZARMOUR_MODE_BASE64_STD:
            return s_base64_encode (data, data_size, s_rfc4648p4_alphabet);

        case ZARMOUR_MODE_BASE64_URL:
            return s_base64_encode (data, data_size, s_rfc4648p5_alphabet);

        case ZARMOUR_MODE_BASE16:
            return s_base16_encode (data, data_size, s_rfc4648p8_alphabet);
    }

    return NULL;
}


//  Definition of decode method
byte *
zarmour_decode (zarmour_t *self, char *data, size_t *decode_size)
{
    assert (self);
    assert (data);
    assert (decode_size);

    switch (self->mode) {
        case ZARMOUR_MODE_BASE64_STD:
            return s_base64_decode (data, decode_size, s_rfc4648p4_alphabet);

        case ZARMOUR_MODE_BASE64_URL:
            return s_base64_decode (data, decode_size, s_rfc4648p5_alphabet);

        case ZARMOUR_MODE_BASE16:
            return s_base16_decode (data, decode_size, s_rfc4648p8_alphabet);
    }

    return NULL;
}


//  --------------------------------------------------------------------------
//  Get/set the mode property

zarmour_mode_t
zarmour_mode (zarmour_t *self)
{
    assert (self);
    return self->mode;
}

void
zarmour_set_mode (zarmour_t *self, zarmour_mode_t mode)
{
    assert (self);
    self->mode = mode;
}

//  --------------------------------------------------------------------------
//  Get/set the pad property

bool
zarmour_pad (zarmour_t *self)
{
    assert (self);
    return self->pad;
}

void
zarmour_set_pad (zarmour_t *self, bool pad)
{
    assert (self);
    self->pad = pad;
}

//  --------------------------------------------------------------------------
//  Get/set the pad_char property

char
zarmour_pad_char (zarmour_t *self)
{
    assert (self);
    return self->pad_char;
}

void
zarmour_set_pad_char (zarmour_t *self, char pad_char)
{
    assert (self);
    self->pad_char = pad_char;
}


//  ---------------------------------------------------------------------------
//  armour test utility

static void
s_armour_test (zarmour_t *self, const char *test_string, const char *expected_result, bool verbose)
{
    assert (self);
    assert (test_string);
    assert (expected_result);

    char *encoded = zarmour_encode (self, (byte *) test_string, strlen (test_string));
    assert (encoded);
    assert (strlen (encoded) == strlen (expected_result));
    assert (streq (encoded, expected_result));

    if (verbose)
        zsys_debug ("       encoded '%s' into '%s'", test_string, encoded);

    size_t size;
    char *decoded = (char *) zarmour_decode (self, encoded, &size);
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
zarmour_test (bool verbose)
{
    printf (" * zarmour: ");

    if (verbose)
        printf ("\n");

    //  @selftest
    zarmour_t *self = zarmour_new ();
    assert (self);

    zarmour_mode_t mode = zarmour_mode (self);
    assert (mode == ZARMOUR_MODE_BASE64_STD);

    zarmour_set_mode (self, ZARMOUR_MODE_BASE64_URL);
    mode = zarmour_mode (self);
    assert (mode == ZARMOUR_MODE_BASE64_URL);

    bool pad = zarmour_pad (self);
    assert (pad);

    zarmour_set_pad (self, false);
    pad = zarmour_pad (self);
    assert (!pad);

    char pad_char = zarmour_pad_char (self);
    assert (pad_char == '=');

    if (verbose)
        zarmour_print (self);

    zarmour_set_pad_char (self, '-');
    pad_char = zarmour_pad_char (self);
    assert (pad_char == '-');

    //  Test against test vectors from RFC4648.
    zarmour_set_mode (self, ZARMOUR_MODE_BASE64_STD);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "Zg", verbose);
    s_armour_test (self, "fo", "Zm8", verbose);
    s_armour_test (self, "foo", "Zm9v", verbose);
    s_armour_test (self, "foob", "Zm9vYg", verbose);
    s_armour_test (self, "fooba", "Zm9vYmE", verbose);
    s_armour_test (self, "foobar", "Zm9vYmFy", verbose);

    zarmour_set_mode (self, ZARMOUR_MODE_BASE64_URL);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "Zg", verbose);
    s_armour_test (self, "fo", "Zm8", verbose);
    s_armour_test (self, "foo", "Zm9v", verbose);
    s_armour_test (self, "foob", "Zm9vYg", verbose);
    s_armour_test (self, "fooba", "Zm9vYmE", verbose);
    s_armour_test (self, "foobar", "Zm9vYmFy", verbose);

    zarmour_set_mode (self, ZARMOUR_MODE_BASE16);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "66", verbose);
    s_armour_test (self, "fo", "666F", verbose);
    s_armour_test (self, "foo", "666F6F", verbose);
    s_armour_test (self, "foob", "666F6F62", verbose);
    s_armour_test (self, "fooba", "666F6F6261", verbose);
    s_armour_test (self, "foobar", "666F6F626172", verbose);

    zarmour_destroy (&self);
    //  @end

    printf ("OK\n");
    return 0;
}
