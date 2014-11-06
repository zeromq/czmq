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
    The following codecs are implemented:
    * RFC 4648 (http://www.ietf.org/rfc/rfc4648.txt)
      - base64
      - base64url
      - base32
      - base32hex
      - base16
    Planned features:
    * z85
    * Padding
    * Breaking output into lines
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zarmour_t {
    zarmour_mode_t mode;        //  The current mode (std or url)
    bool pad;                   //  Should output be padded?
    char pad_char;              //  The pad character
    bool line_breaks;           //  Should output be broken into lines?
    size_t line_length;         //  The line length to use
};


//  Textual names of modes
const int _NUM_MODES = 5;
static char
s_codec_names[][16] = {
    "base64",
    "base64url",
    "base32",
    "base32hex",
    "base16"
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
    self->line_breaks = false;
    self->line_length = 72;

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
    zsys_debug ("   mode:        %s", zarmour_mode_str (self));
    zsys_debug ("   pad:         %s", self->pad? "true": "false");
    zsys_debug ("   pad_char:    '%c'", self->pad_char);
    zsys_debug ("   line_breaks: %s", self->line_breaks? "true": "false");
    zsys_debug ("   line_length: %d", self->line_length);
}


//  --------------------------------------------------------------------------
//  Get printable string for mode

const char *
zarmour_mode_str (zarmour_t *self)
{
    assert (self);
    assert ((int) self->mode >= 0 && (int) self->mode < _NUM_MODES);
    return s_codec_names[(int) self->mode];
}


//  --------------------------------------------------------------------------
//  Helper macros for encoding/decoding

#define _NO_CONVERT(c) (c)
#define _UPPER_CASE(c) ((c) & ((c) & 0x40? 0xdf: 0xff))

#define _NEXT_CHAR(i,n,c,a,u) \
while ((n) < (c) && !strchr ((a), (u(*(n))))) \
    ++(n); \
    (i) = (byte) ((n) < (c)? (strchr ((a), (u(*(n)))) - (a)): 0xff); \
    ++(n)


//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 4 (standard base64 alphabet)
static char  //        0----5----0----5----0----5----0----5----0----5----0----5----0---
s_base64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 5 (URL & filename friendly base64 alphabet)
static char  //           0----5----0----5----0----5----0----5----0----5----0----5----0---
s_base64url_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static char *
s_base64_encode (const byte *data, size_t length, const char *alphabet, bool pad, char pad_char)
{
    size_t extra_chars = ((length % 3)? length % 3 + 1: 0);
    size_t pad_chars = (pad && extra_chars)? 4 - extra_chars: 0;
    size_t str_chars = 4 * (length / 3) + extra_chars + pad_chars;
    char *str = (char *) zmalloc (str_chars + 1);
    char *enc = str;
    const byte *needle = data, *ceiling = data + length;
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
    while (pad && enc < str + str_chars)
        *enc++ = pad_char;
    *enc = 0;
    return str;
}


static byte *
s_base64_decode (const char *data, size_t *size, const char *alphabet)
{
    int length = strlen (data);
    while (length > 0 && !strchr (alphabet, data[length - 1])) --length;
    *size = 3 * (length / 4) + ((length % 4)? length % 4 - 1 : 0) + 1;
    byte *bytes = (byte *) zmalloc (*size);
    byte *dec = bytes;
    const byte *needle = (const byte *) data, *ceiling = (const byte *) (data + length);
    byte i1, i2, i3, i4;
    while (needle < ceiling) {
        _NEXT_CHAR(i1,needle,ceiling,alphabet,_NO_CONVERT);
        _NEXT_CHAR(i2,needle,ceiling,alphabet,_NO_CONVERT);
        if (i1 != 0xff && i2 != 0xff)
            *dec++ = i1 << 2 | i2 >> 4;
        _NEXT_CHAR(i3,needle,ceiling,alphabet,_NO_CONVERT);
        if (i2 != 0xff && i3 != 0xff)
            *dec++ = i2 << 4 | i3 >> 2;
        _NEXT_CHAR(i4,needle,ceiling,alphabet,_NO_CONVERT);
        if (i3 != 0xff && i4 != 0xff)
            *dec++ = i3 << 6 | i4;
    }
    *dec = 0;
    return bytes;
}


//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 6 (standard base32 alphabet)
static char  //        0----5----0----5----0----5----0-
s_base32_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 7 (base32hex alphabet)
static char  //           0----5----0----5----0----5----0-
s_base32hex_alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

static char *
s_base32_encode (const byte *data, size_t length, const char *alphabet, bool pad, char pad_char)
{
    size_t extra_bytes = length % 5, extra_chars = 0;
    switch (extra_bytes) {
        case 1: extra_chars = 2; break;
        case 2: extra_chars = 4; break;
        case 3: extra_chars = 5; break;
        case 4: extra_chars = 7; break;
    }
    size_t pad_chars = (pad && extra_chars)? 8 - extra_chars: 0;
    size_t str_chars = 8 * (length / 5) + extra_chars + pad_chars;
    char *str = (char *) zmalloc (str_chars + 1);
    char *enc = str;
    const byte *needle = data, *ceiling = data + length;
    while (needle < ceiling) {
        *enc++ = alphabet[(*needle) >> 3];
        if (needle + 1 < ceiling) {
            *enc++ = alphabet[((*needle << 2) & 0x1c) | (*(needle + 1) >> 6)];
            *enc++ = alphabet[(*(needle + 1) >> 1) & 0x1f];
            if (needle + 2 < ceiling) {
                *enc++ = alphabet[((*(needle + 1) << 4) & 0x10) | (*(needle + 2) >> 4)];
                if (needle + 3 < ceiling) {
                    *enc++ = alphabet[((*(needle + 2) << 1) & 0x1e) | (*(needle + 3) >> 7)];
                    *enc++ = alphabet[(*(needle + 3) >> 2) & 0x1f];
                    if (needle + 4 < ceiling) {
                        *enc++ = alphabet[((*(needle + 3) << 3) & 0x18) | (*(needle + 4) >> 5)];
                        *enc++ = alphabet[*(needle + 4) & 0x1f];
                    }
                    else
                        *enc++ = alphabet[(*(needle + 3) << 3) & 0x18];
                }
                else
                    *enc++ = alphabet[(*(needle + 2) << 1) & 0x1e];
            }
            else
                *enc++ = alphabet[(*(needle + 1) << 4) & 0x10];
        }
        else
            *enc++ = alphabet[(*needle << 2) & 0x1c];
        needle += 5;
    }
    while (enc < str + str_chars)
        *enc++ = pad_char;
    *enc = 0;
    return str;
}


static byte *
s_base32_decode (const char *data, size_t *size, const char *alphabet)
{
    size_t length = strlen (data);
    while (length > 0 && !strchr (alphabet, _UPPER_CASE(data[length - 1]))) --length;
    size_t extra_chars = length % 8, extra_bytes = 0;
    switch (extra_chars) {
        case 0: break;
        case 2: extra_bytes = 1; break;
        case 4: extra_bytes = 2; break;
        case 5: extra_bytes = 3; break;
        case 7: extra_bytes = 4; break;
        default: assert (false); break;
    }
    *size = 5 * (length / 8) + extra_bytes + 1;
    byte *bytes = (byte *) zmalloc (*size);
    byte *dec = bytes;
    const byte *needle = (const byte *) data, *ceiling = (const byte *) (data + length);
    byte i1, i2, i3 = 0, i4 = 0, i5 = 0, i6, i7 = 0, i8;
    while (needle < ceiling) {
        _NEXT_CHAR(i1,needle,ceiling,alphabet,_UPPER_CASE);
        _NEXT_CHAR(i2,needle,ceiling,alphabet,_UPPER_CASE);
        if (i1 != 0xff && i2 != 0xff)
            *dec++ = i1 << 3 | i2 >> 2;
        _NEXT_CHAR(i3,needle,ceiling,alphabet,_UPPER_CASE);
        _NEXT_CHAR(i4,needle,ceiling,alphabet,_UPPER_CASE);
        if (i2 != 0xff && i3 != 0xff && i4 != 0xff)
            *dec++ = i2 << 6 | i3 << 1 | i4 >> 4;
        _NEXT_CHAR(i5,needle,ceiling,alphabet,_UPPER_CASE);
        if (i4 != 0xff && i5 != 0xff)
            *dec++ = i4 << 4 | i5 >> 1;
        _NEXT_CHAR(i6,needle,ceiling,alphabet,_UPPER_CASE);
        _NEXT_CHAR(i7,needle,ceiling,alphabet,_UPPER_CASE);
        if (i5 != 0xff && i6 != 0xff && i7 != 0xff)
            *dec++ = i5 << 7 | i6 << 2 | i7 >> 3;
        _NEXT_CHAR(i8,needle,ceiling,alphabet,_UPPER_CASE);
        if (i7 != 0xff && i8 != 0xff)
            *dec++ = i7 << 5 | i8;
    }
    *dec = 0;
    return bytes;
}


//  ---------------------------------------------------------------------------
//  RFC 4648 Paragraph 8 (standard base16 alphabet)
static char  //        0----5----0----5
s_base16_alphabet[] = "0123456789ABCDEF";

static char *
s_base16_encode (const byte *data, size_t length, const char *alphabet)
{
    char *str = (char *) zmalloc (2 * length + 1);
    char *enc = str;
    const byte *needle = data, *ceiling = data + length;
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
    const byte *needle = (const byte *) data, *ceiling = (const byte *) (data + length);
    byte i1, i2;
    while (needle < ceiling) {
        i1 = (byte) (strchr (alphabet, _UPPER_CASE(*needle)) - alphabet);
        i2 = (byte) (strchr (alphabet, _UPPER_CASE(*(needle + 1))) - alphabet);
        *dec++ = i1 << 4 | i2;
        needle += 2;
    }
    *dec = 0;
    return bytes;
}


//  Definition of encode method
char *
zarmour_encode (zarmour_t *self, const byte *data, size_t data_size)
{
    assert (self);
    assert (data);

    switch (self->mode) {
        case ZARMOUR_MODE_BASE64_STD:
            return s_base64_encode (data, data_size, s_base64_alphabet, self->pad, self->pad_char);

        case ZARMOUR_MODE_BASE64_URL:
            return s_base64_encode (data, data_size, s_base64url_alphabet, self->pad, self->pad_char);

        case ZARMOUR_MODE_BASE32_STD:
            return s_base32_encode (data, data_size, s_base32_alphabet, self->pad, self->pad_char);

        case ZARMOUR_MODE_BASE32_HEX:
            return s_base32_encode (data, data_size, s_base32hex_alphabet, self->pad, self->pad_char);

        case ZARMOUR_MODE_BASE16:
            return s_base16_encode (data, data_size, s_base16_alphabet);
    }

    return NULL;
}


//  Definition of decode method
byte *
zarmour_decode (zarmour_t *self, const char *data, size_t *decode_size)
{
    assert (self);
    assert (data);
    assert (decode_size);

    switch (self->mode) {
        case ZARMOUR_MODE_BASE64_STD:
            return s_base64_decode (data, decode_size, s_base64_alphabet);

        case ZARMOUR_MODE_BASE64_URL:
            return s_base64_decode (data, decode_size, s_base64url_alphabet);

        case ZARMOUR_MODE_BASE32_STD:
            return s_base32_decode (data, decode_size, s_base32_alphabet);

        case ZARMOUR_MODE_BASE32_HEX:
            return s_base32_decode (data, decode_size, s_base32hex_alphabet);

        case ZARMOUR_MODE_BASE16:
            return s_base16_decode (data, decode_size, s_base16_alphabet);
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


//  --------------------------------------------------------------------------
//  Get/set the line_breaks property

bool
zarmour_line_breaks (zarmour_t *self)
{
    assert (self);
    return self->line_breaks;
}

void
zarmour_set_line_breaks (zarmour_t *self, bool line_breaks)
{
    assert (self);
    self->line_breaks = line_breaks;
}

//  --------------------------------------------------------------------------
//  Get/set the line_length property

size_t
zarmour_line_length (zarmour_t *self)
{
    assert (self);
    return self->line_length;
}

void
zarmour_set_line_length (zarmour_t *self, size_t line_length)
{
    assert (self);
    self->line_length = line_length;
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
    if (verbose)
        zsys_debug ("       encoded '%s' into '%s'", test_string, encoded);
    assert (strlen (encoded) == strlen (expected_result));
    assert (streq (encoded, expected_result));

    size_t size;
    char *decoded = (char *) zarmour_decode (self, encoded, &size);
    assert (decoded);
    if (verbose)
        zsys_debug ("       decoded '%s' into '%s'", encoded, decoded);
    assert (size == strlen (decoded) + 1);
    assert (streq (decoded, test_string));

    free (encoded);
    free (decoded);
}

static void
s_armour_decode (zarmour_t *self, const char *test_string, const char *expected_result, bool verbose)
{
    assert (self);
    assert (test_string);
    assert (expected_result);

    size_t size;
    char *decoded = (char *) zarmour_decode (self, test_string, &size);
    assert (decoded);
    if (verbose)
        zsys_debug ("       decoded '%s' into '%s'", test_string, decoded);
    assert (size == strlen (decoded) + 1);
    assert (streq (decoded, expected_result));

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

    assert (zarmour_pad (self));
    zarmour_set_pad (self, false);
    assert (!zarmour_pad (self));

    assert (zarmour_pad_char (self) == '=');
    zarmour_set_pad_char (self, '!');
    assert (zarmour_pad_char (self) == '!');
    zarmour_set_pad_char (self, '=');
    assert (zarmour_pad_char (self) == '=');

    assert (!zarmour_line_breaks (self));
    zarmour_set_line_breaks (self, true);
    assert (zarmour_line_breaks (self));

    assert (zarmour_line_length (self) == 72);
    zarmour_set_line_length (self, 64);
    assert (zarmour_line_length (self) == 64);

    //  Test against test vectors from RFC4648.
    zarmour_set_mode (self, ZARMOUR_MODE_BASE64_STD);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "Zg", verbose);
    s_armour_test (self, "fo", "Zm8", verbose);
    s_armour_test (self, "foo", "Zm9v", verbose);
    s_armour_test (self, "foob", "Zm9vYg", verbose);
    s_armour_test (self, "fooba", "Zm9vYmE", verbose);
    s_armour_test (self, "foobar", "Zm9vYmFy", verbose);
    zarmour_set_pad (self, true);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "Zg==", verbose);
    s_armour_test (self, "fo", "Zm8=", verbose);
    s_armour_test (self, "foo", "Zm9v", verbose);
    s_armour_test (self, "foob", "Zm9vYg==", verbose);
    s_armour_test (self, "fooba", "Zm9vYmE=", verbose);
    s_armour_test (self, "foobar", "Zm9vYmFy", verbose);

    zarmour_set_pad (self, false);
    zarmour_set_mode (self, ZARMOUR_MODE_BASE64_URL);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "Zg", verbose);
    s_armour_test (self, "fo", "Zm8", verbose);
    s_armour_test (self, "foo", "Zm9v", verbose);
    s_armour_test (self, "foob", "Zm9vYg", verbose);
    s_armour_test (self, "fooba", "Zm9vYmE", verbose);
    s_armour_test (self, "foobar", "Zm9vYmFy", verbose);
    zarmour_set_pad (self, true);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "Zg==", verbose);
    s_armour_test (self, "fo", "Zm8=", verbose);
    s_armour_test (self, "foo", "Zm9v", verbose);
    s_armour_test (self, "foob", "Zm9vYg==", verbose);
    s_armour_test (self, "fooba", "Zm9vYmE=", verbose);
    s_armour_test (self, "foobar", "Zm9vYmFy", verbose);

    zarmour_set_pad (self, false);
    zarmour_set_mode (self, ZARMOUR_MODE_BASE32_STD);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "MY", verbose);
    s_armour_test (self, "fo", "MZXQ", verbose);
    s_armour_test (self, "foo", "MZXW6", verbose);
    s_armour_test (self, "foob", "MZXW6YQ", verbose);
    s_armour_test (self, "fooba", "MZXW6YTB", verbose);
    s_armour_test (self, "foobar", "MZXW6YTBOI", verbose);
    s_armour_decode (self, "my", "f", verbose);
    s_armour_decode (self, "mzxq", "fo", verbose);
    s_armour_decode (self, "mzxw6", "foo", verbose);
    s_armour_decode (self, "mzxw6yq", "foob", verbose);
    s_armour_decode (self, "mzxw6ytb", "fooba", verbose);
    s_armour_decode (self, "mzxw6ytboi", "foobar", verbose);
    zarmour_set_pad (self, true);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "MY======", verbose);
    s_armour_test (self, "fo", "MZXQ====", verbose);
    s_armour_test (self, "foo", "MZXW6===", verbose);
    s_armour_test (self, "foob", "MZXW6YQ=", verbose);
    s_armour_test (self, "fooba", "MZXW6YTB", verbose);
    s_armour_test (self, "foobar", "MZXW6YTBOI======", verbose);
    s_armour_decode (self, "my======", "f", verbose);
    s_armour_decode (self, "mzxq====", "fo", verbose);
    s_armour_decode (self, "mzxw6===", "foo", verbose);
    s_armour_decode (self, "mzxw6yq=", "foob", verbose);
    s_armour_decode (self, "mzxw6ytb", "fooba", verbose);
    s_armour_decode (self, "mzxw6ytboi======", "foobar", verbose);

    zarmour_set_pad (self, false);
    zarmour_set_mode (self, ZARMOUR_MODE_BASE32_HEX);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "CO", verbose);
    s_armour_test (self, "fo", "CPNG", verbose);
    s_armour_test (self, "foo", "CPNMU", verbose);
    s_armour_test (self, "foob", "CPNMUOG", verbose);
    s_armour_test (self, "fooba", "CPNMUOJ1", verbose);
    s_armour_test (self, "foobar", "CPNMUOJ1E8", verbose);
    s_armour_decode (self, "co", "f", verbose);
    s_armour_decode (self, "cpng", "fo", verbose);
    s_armour_decode (self, "cpnmu", "foo", verbose);
    s_armour_decode (self, "cpnmuog", "foob", verbose);
    s_armour_decode (self, "cpnmuoj1", "fooba", verbose);
    s_armour_decode (self, "cpnmuoj1e8", "foobar", verbose);
    zarmour_set_pad (self, true);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "CO======", verbose);
    s_armour_test (self, "fo", "CPNG====", verbose);
    s_armour_test (self, "foo", "CPNMU===", verbose);
    s_armour_test (self, "foob", "CPNMUOG=", verbose);
    s_armour_test (self, "fooba", "CPNMUOJ1", verbose);
    s_armour_test (self, "foobar", "CPNMUOJ1E8======", verbose);
    s_armour_decode (self, "co======", "f", verbose);
    s_armour_decode (self, "cpng====", "fo", verbose);
    s_armour_decode (self, "cpnmu===", "foo", verbose);
    s_armour_decode (self, "cpnmuog=", "foob", verbose);
    s_armour_decode (self, "cpnmuoj1", "fooba", verbose);
    s_armour_decode (self, "cpnmuoj1e8======", "foobar", verbose);

    zarmour_set_mode (self, ZARMOUR_MODE_BASE16);
    if (verbose)
        zarmour_print (self);
    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "f", "66", verbose);
    s_armour_test (self, "fo", "666F", verbose);
    s_armour_test (self, "foo", "666F6F", verbose);
    s_armour_test (self, "foob", "666F6F62", verbose);
    s_armour_test (self, "fooba", "666F6F6261", verbose);
    s_armour_test (self, "foobar", "666F6F626172", verbose);
    s_armour_decode (self, "666f", "fo", verbose);
    s_armour_decode (self, "666f6f", "foo", verbose);
    s_armour_decode (self, "666f6f62", "foob", verbose);
    s_armour_decode (self, "666f6f6261", "fooba", verbose);
    s_armour_decode (self, "666f6f626172", "foobar", verbose);

    zarmour_destroy (&self);
    //  @end

    printf ("OK\n");
    return 0;
}
