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
    * Z85 (http://rfc.zeromq.org/spec:32)
    All RFC4648 base64 and base32 variants support padding the output. The pad
    character is configurable. Default is padding on, with character '='.
    Additionally, in some cases (e.g. MIME), splitting the output into lines of a
    specific length is required. This feature is also supported, though
    turned off by default.
    The z85 mode does neither padding nor line breaks; it is merely a wrapping
    of the corresponding libzmq methods. Encoding will assert if input length is
    not divisible by 4 and decoding will assert if input length is not
    divisible by 5.
@end
*/

#include "czmq_classes.h"

#if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 3, 0))
#   define _INCLUDE_Z85
#endif

//  Structure of our class

struct _zarmour_t {
    int mode;                   //  The current mode (Base64/32/16/Z85, STD/URL)
    bool pad;                   //  Should output be padded?
    char pad_char;              //  The pad character
    bool line_breaks;           //  Should output be broken into lines?
    size_t line_length;         //  The line length to use
    char *line_end;
};


//  Textual names of modes

static char
*s_mode_names [] = {
    "base64", "base64url", "base32", "base32hex", "base16", "z85"
};


//  --------------------------------------------------------------------------
//  Create a new zarmour

zarmour_t *
zarmour_new (void)
{
    zarmour_t *self = (zarmour_t *) zmalloc (sizeof (zarmour_t));
    assert (self);

    //  Setup default as RFC4648 paragraph 4
    self->mode = ZARMOUR_MODE_BASE64_STD;
    self->pad = true;
    self->pad_char = '=';
    self->line_breaks = false;
    self->line_length = 72;
    self->line_end = strdup ("\n");
    assert (self->line_end);
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
        freen (self->line_end);
        freen (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Helper macros for encoding/decoding

#define _NO_CONVERT(c) (c)
#define _UPPER_CASE(c) ((c) & ((c) & 0x40? 0xdf: 0xff))

#define _NEXT_CHAR(i, n, c, a, u) \
    while ((n) < (c) && !strchr ((a), (u (*(n))))) \
        ++ (n); \
    (i) = (byte) ((n) < (c)? (strchr ((a), (u (*(n)))) - (a)): 0xff); \
    ++ (n)


//  RFC 4648 Paragraph 4 (standard base64 alphabet)
static char  //        0----5----0----5----0----5----0----5----0----5----0----5----0---
s_base64_alphabet [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//  RFC 4648 Paragraph 5 (URL & filename friendly base64 alphabet)
static char  //           0----5----0----5----0----5----0----5----0----5----0----5----0---
s_base64url_alphabet [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static char *
s_base64_encode (const byte *data, size_t length, const char *alphabet, bool pad, char pad_char)
{
    size_t extra_chars = ((length % 3)? length % 3 + 1: 0);
    size_t pad_chars = (pad && extra_chars)? 4 - extra_chars: 0;
    size_t str_chars = 4 *(length / 3) + extra_chars + pad_chars;
    char *str = (char *) zmalloc (str_chars + 1);
    if (!str)
        return NULL;

    char *enc = str;
    const byte *needle = data, *ceiling = data + length;
    while (needle < ceiling) {
        *enc++ = alphabet [(*needle) >> 2];
        if (needle + 1 < ceiling) {
            *enc++ = alphabet [((*needle << 4) & 0x30) | (*(needle + 1) >> 4)];
            if (needle + 2 < ceiling) {
                *enc++ = alphabet [((*(needle + 1) << 2) & 0x3c) | (*(needle + 2) >> 6)];
                *enc++ = alphabet [*(needle + 2) & 0x3f];
            }
            else
                *enc++ = alphabet [(*(needle + 1) << 2) & 0x3c];
        }
        else
            *enc++ = alphabet [(*needle << 4) & 0x30];
        needle += 3;
    }
    while (pad && enc < str + str_chars)
        *enc++ = pad_char;
    *enc = 0;
    return str;
}


static byte *
s_base64_decode (const char *data, size_t *size, const char *alphabet, size_t linebreakchars)
{
    size_t length = strlen (data);
    while (length > 0 && !strchr (alphabet, data [length - 1]))
        --length;
    const byte *needle = (const byte *) data;
    const byte *ceiling = (const byte *) (data + length);

    length -= linebreakchars;
    *size = 3 * (length / 4) + ((length % 4)? length % 4 - 1 : 0) + 1;
    byte *bytes = (byte *) zmalloc (*size);
    if (!bytes)
        return NULL;

    byte *dec = bytes;
    byte i1, i2, i3, i4;
    while (needle < ceiling) {
        _NEXT_CHAR (i1, needle, ceiling, alphabet, _NO_CONVERT);
        _NEXT_CHAR (i2, needle, ceiling, alphabet, _NO_CONVERT);
        if (i1 != 0xff && i2 != 0xff)
            *dec++ = i1 << 2 | i2 >> 4;
        _NEXT_CHAR (i3, needle, ceiling, alphabet, _NO_CONVERT);
        if (i2 != 0xff && i3 != 0xff)
            *dec++ = i2 << 4 | i3 >> 2;
        _NEXT_CHAR (i4, needle, ceiling, alphabet, _NO_CONVERT);
        if (i3 != 0xff && i4 != 0xff)
            *dec++ = i3 << 6 | i4;
    }
    *dec = 0;
    return bytes;
}


//  RFC 4648 Paragraph 6 (standard base32 alphabet)

static char  //        0----5----0----5----0----5----0-
s_base32_alphabet [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

//  RFC 4648 Paragraph 7 (base32hex alphabet)

static char  //           0----5----0----5----0----5----0-
s_base32hex_alphabet [] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

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
    size_t str_chars = 8 *(length / 5) + extra_chars + pad_chars;
    char *str = (char *) zmalloc (str_chars + 1);
    if (!str)
        return NULL;

    char *enc = str;
    const byte *needle = data, *ceiling = data + length;
    while (needle < ceiling) {
        *enc++ = alphabet [(*needle) >> 3];
        if (needle + 1 < ceiling) {
            *enc++ = alphabet [((*needle << 2) & 0x1c) | (*(needle + 1) >> 6)];
            *enc++ = alphabet [(*(needle + 1) >> 1) & 0x1f];
            if (needle + 2 < ceiling) {
                *enc++ = alphabet [((*(needle + 1) << 4) & 0x10) | (*(needle + 2) >> 4)];
                if (needle + 3 < ceiling) {
                    *enc++ = alphabet [((*(needle + 2) << 1) & 0x1e) | (*(needle + 3) >> 7)];
                    *enc++ = alphabet [(*(needle + 3) >> 2) & 0x1f];
                    if (needle + 4 < ceiling) {
                        *enc++ = alphabet [((*(needle + 3) << 3) & 0x18) | (*(needle + 4) >> 5)];
                        *enc++ = alphabet [*(needle + 4) & 0x1f];
                    }
                    else
                        *enc++ = alphabet [(*(needle + 3) << 3) & 0x18];
                }
                else
                    *enc++ = alphabet [(*(needle + 2) << 1) & 0x1e];
            }
            else
                *enc++ = alphabet [(*(needle + 1) << 4) & 0x10];
        }
        else
            *enc++ = alphabet [(*needle << 2) & 0x1c];
        needle += 5;
    }
    while (enc < str + str_chars)
        *enc++ = pad_char;
    *enc = 0;
    return str;
}


static byte *
s_base32_decode (const char *data, size_t *size, const char *alphabet, size_t linebreakchars)
{
    size_t length = strlen (data);
    while (length > 0 && !strchr (alphabet, _UPPER_CASE (data [length - 1]))) --length;
    const byte *needle = (const byte *) data, *ceiling = (const byte *) (data + length);
    length -= linebreakchars;
    size_t extra_chars = length % 8, extra_bytes = 0;
    switch (extra_chars) {
        case 0: break;
        case 2: extra_bytes = 1; break;
        case 4: extra_bytes = 2; break;
        case 5: extra_bytes = 3; break;
        case 7: extra_bytes = 4; break;
        default: assert (false); break;
    }
    *size = 5 *(length / 8) + extra_bytes + 1;
    byte *bytes = (byte *) zmalloc (*size);
    if (!bytes)
        return NULL;

    byte *dec = bytes;
    byte i1, i2, i3, i4, i5, i6, i7, i8;
    while (needle < ceiling) {
        _NEXT_CHAR (i1, needle, ceiling, alphabet, _UPPER_CASE);
        _NEXT_CHAR (i2, needle, ceiling, alphabet, _UPPER_CASE);
        if (i1 != 0xff && i2 != 0xff)
            *dec++ = i1 << 3 | i2 >> 2;
        _NEXT_CHAR (i3, needle, ceiling, alphabet, _UPPER_CASE);
        _NEXT_CHAR (i4, needle, ceiling, alphabet, _UPPER_CASE);
        if (i2 != 0xff && i3 != 0xff && i4 != 0xff)
            *dec++ = i2 << 6 | i3 << 1 | i4 >> 4;
        _NEXT_CHAR (i5, needle, ceiling, alphabet, _UPPER_CASE);
        if (i4 != 0xff && i5 != 0xff)
            *dec++ = i4 << 4 | i5 >> 1;
        _NEXT_CHAR (i6, needle, ceiling, alphabet, _UPPER_CASE);
        _NEXT_CHAR (i7, needle, ceiling, alphabet, _UPPER_CASE);
        if (i5 != 0xff && i6 != 0xff && i7 != 0xff)
            *dec++ = i5 << 7 | i6 << 2 | i7 >> 3;
        _NEXT_CHAR (i8, needle, ceiling, alphabet, _UPPER_CASE);
        if (i7 != 0xff && i8 != 0xff)
            *dec++ = i7 << 5 | i8;
    }
    *dec = 0;
    return bytes;
}


//  RFC 4648 Paragraph 8 (standard base16 alphabet)

static char  //        0----5----0----5
s_base16_alphabet [] = "0123456789ABCDEF";

static char *
s_base16_encode (const byte *data, size_t length, const char *alphabet)
{
    char *str = (char *) zmalloc (2 * length + 1);
    if (!str)
        return NULL;

    char *enc = str;
    const byte *needle = data, *ceiling = data + length;
    while (needle < ceiling) {
        *enc++ = alphabet [(*needle) >> 4];
        *enc++ = alphabet [(*needle++) & 0x0f];
    }
    *enc = 0;
    return str;
}

static byte *
s_base16_decode (const char *data, size_t *size, const char *alphabet, size_t linebreakchars)
{
    size_t length = strlen (data);
    const byte *needle = (const byte *) data, *ceiling = (const byte *) (data + length);
    length -= linebreakchars;
    *size = length / 2 + 1;
    byte *bytes = (byte *) zmalloc (*size);
    if (!bytes)
        return NULL;

    byte *dec = bytes;
    byte i1, i2;
    while (needle < ceiling) {
        _NEXT_CHAR (i1, needle, ceiling, alphabet, _UPPER_CASE);
        _NEXT_CHAR (i2, needle, ceiling, alphabet, _UPPER_CASE);
        if (i1 != 0xff && i2 != 0xff)
            *dec++ = i1 << 4 | i2;
    }
    *dec = 0;
    return bytes;
}


//  Z85 encoding/decoding

static char *
s_z85_encode (const byte *data, size_t length)
{
    assert (data != NULL);
    assert (length % 4 == 0);
#ifdef _INCLUDE_Z85
    char *str = (char *) zmalloc (5 * length / 4 + 1);
    char *result = zmq_z85_encode (str, (uint8_t *) data, length);
    if (result == NULL) {
        freen (str);
        str = NULL;
    }
    return str;
#else
    return NULL;
#endif
}

static byte *
s_z85_decode (const char *data, size_t *size)
{
    assert (data);
    assert (size);
#ifdef _INCLUDE_Z85
    size_t length = strlen (data);
    assert (length % 5 == 0);
    *size = 4 * length / 5 + 1;
    byte *bytes = (byte *) zmalloc (*size);
    uint8_t *result = zmq_z85_decode (bytes, (char *) data);
    if (result == NULL) {
        freen (bytes);
        bytes = NULL;
    }
    return bytes;
#else
    return NULL;
#endif
}


//  --------------------------------------------------------------------------
//  Decode an armoured string into a string of bytes.
//  The decoded output is null-terminated, so it may be treated
//  as a string, if that's what it was prior to encoding.
//  The caller is responsible for destroying the return value when finished with it.

char *
zarmour_encode (zarmour_t *self, const byte *data, size_t data_size)
{
    assert (self);
    assert (data);

    char *encoded = NULL;
    switch (self->mode) {
        case ZARMOUR_MODE_BASE64_STD:
            encoded = s_base64_encode (data, data_size, s_base64_alphabet, self->pad, self->pad_char);
            break;
        case ZARMOUR_MODE_BASE64_URL:
            encoded = s_base64_encode (data, data_size, s_base64url_alphabet, self->pad, self->pad_char);
            break;
        case ZARMOUR_MODE_BASE32_STD:
            encoded = s_base32_encode (data, data_size, s_base32_alphabet, self->pad, self->pad_char);
            break;
        case ZARMOUR_MODE_BASE32_HEX:
            encoded = s_base32_encode (data, data_size, s_base32hex_alphabet, self->pad, self->pad_char);
            break;
        case ZARMOUR_MODE_BASE16:
            encoded = s_base16_encode (data, data_size, s_base16_alphabet);
            break;
        case ZARMOUR_MODE_Z85:
            encoded = s_z85_encode (data, data_size);
            break;
    }
    if (!encoded)
        return NULL;

    if (self->line_breaks
    &&  self->line_length > 0
    &&  strlen (encoded) > self->line_length
    &&  self->mode != ZARMOUR_MODE_Z85) {
        char *line_end = self->line_end;
        size_t nbr_lines = strlen (encoded) / self->line_length;
        size_t new_length =
            nbr_lines *(self->line_length + strlen (line_end)) +
            strlen (encoded) % self->line_length;
        char *src = encoded;
        char *temp = encoded;
        encoded = (char *) zmalloc (new_length + 1);

        char *dest = encoded;
        while (strlen (src) >= self->line_length) {
            memcpy (dest, src, self->line_length);
            src += self->line_length;
            dest += self->line_length;
            if (*src) {
                memcpy (dest, line_end, strlen (line_end));
                dest += strlen (line_end);
            }
        }
        if (*src) {
            memcpy (dest, src, strlen (src));
            dest += strlen (src);
        }
        freen (temp);
        *dest = 0;
    }
    return encoded;
}


//  --------------------------------------------------------------------------
//  Decode an armoured string into a chunk. The decoded output is
//  null-terminated, so it may be treated as a string, if that's what
//  it was prior to encoding.
//  Caller owns return value and must destroy it when done.

zchunk_t *
zarmour_decode (zarmour_t *self, const char *data)
{
    assert (self);
    assert (data);

    size_t linebreakchars = 0;
    char *line_end = self->line_end;
    const char *pos = data;
    while ((pos = strstr (pos, line_end))) {
        linebreakchars += strlen (line_end);
        pos += strlen (line_end);
    }
    byte *decoded = NULL;
    size_t size = 0;

    switch (self->mode) {
        case ZARMOUR_MODE_BASE64_STD:
            decoded = s_base64_decode (data, &size, s_base64_alphabet, linebreakchars);
            break;
        case ZARMOUR_MODE_BASE64_URL:
            decoded = s_base64_decode (data, &size, s_base64url_alphabet, linebreakchars);
            break;
        case ZARMOUR_MODE_BASE32_STD:
            decoded = s_base32_decode (data, &size, s_base32_alphabet, linebreakchars);
            break;
        case ZARMOUR_MODE_BASE32_HEX:
            decoded = s_base32_decode (data, &size, s_base32hex_alphabet, linebreakchars);
            break;
        case ZARMOUR_MODE_BASE16:
            decoded = s_base16_decode (data, &size, s_base16_alphabet, linebreakchars);
            break;
        case ZARMOUR_MODE_Z85:
            decoded = s_z85_decode (data, &size);
            break;
    }
    zchunk_t *ret = zchunk_new (decoded, size);
    freen (decoded);
    return ret;
}


//  --------------------------------------------------------------------------
//  Get the mode property.

int
zarmour_mode (zarmour_t *self)
{
    assert (self);
    return self->mode;
}


//  --------------------------------------------------------------------------
//  Get printable string for mode.

const char *
zarmour_mode_str (zarmour_t *self)
{
    assert (self);
    return s_mode_names [(int) self->mode];
}


//  --------------------------------------------------------------------------
//  Set the mode property.

void
zarmour_set_mode (zarmour_t *self, int mode)
{
    assert (self);
    self->mode = mode;
}


//  --------------------------------------------------------------------------
//  Return true if padding is turned on.

bool
zarmour_pad (zarmour_t *self)
{
    assert (self);
    return self->pad;
}

//  --------------------------------------------------------------------------
//  Turn padding on or off. Default is on.

void
zarmour_set_pad (zarmour_t *self, bool pad)
{
    assert (self);
    self->pad = pad;
}


//  --------------------------------------------------------------------------
//  Get the padding character.

char
zarmour_pad_char (zarmour_t *self)
{
    assert (self);
    return self->pad_char;
}


//  --------------------------------------------------------------------------
//  Set the padding character.

void
zarmour_set_pad_char (zarmour_t *self, char pad_char)
{
    assert (self);
    self->pad_char = pad_char;
}


//  --------------------------------------------------------------------------
//  Return if splitting output into lines is turned on. Default is off.

bool
zarmour_line_breaks (zarmour_t *self)
{
    assert (self);
    return self->line_breaks;
}


//  --------------------------------------------------------------------------
//  Turn splitting output into lines on or off.

void
zarmour_set_line_breaks (zarmour_t *self, bool line_breaks)
{
    assert (self);
    self->line_breaks = line_breaks;
}


//  --------------------------------------------------------------------------
//  Get the line length used for splitting lines.

size_t
zarmour_line_length (zarmour_t *self)
{
    assert (self);
    return self->line_length;
}


//  --------------------------------------------------------------------------
//  Set the line length used for splitting lines.

void
zarmour_set_line_length (zarmour_t *self, size_t line_length)
{
    assert (self);
    self->line_length = line_length;
}


//  --------------------------------------------------------------------------
//  Print properties of object

void
zarmour_print (zarmour_t *self)
{
    assert (self);

    zsys_debug ("zarmour:");
    zsys_debug ("    mode:        %s", zarmour_mode_str (self));
    zsys_debug ("    pad:         %s", self->pad? "true": "false");
    zsys_debug ("    pad_char:    '%c'", self->pad_char);
    zsys_debug ("    line_breaks: %s", self->line_breaks? "true": "false");
    zsys_debug ("    line_length: %d", self->line_length);
}


//  ---------------------------------------------------------------------------
//  Self test support functions

static void
s_armour_decode (zarmour_t *self, const char *test_string, const char *expected, bool verbose)
{
    assert (self);
    assert (test_string);
    assert (expected);

    zchunk_t *chunk = zarmour_decode (self, test_string);
    assert (chunk);
    if (verbose)
        zsys_debug ("    decoded '%s' into '%s'", test_string, (char *) zchunk_data (chunk));
    assert (zchunk_size (chunk) == strlen (expected) + 1
            || (strlen (expected) == 0 && zchunk_size (chunk) == 0));
    assert ((strlen (expected) == 0 && zchunk_size (chunk) == 0)
            || streq ((char *) zchunk_data (chunk), expected));
    zchunk_destroy (&chunk);
}

static void
s_armour_test (zarmour_t *self, const char *test_string, const char *expected, bool verbose)
{
    assert (self);
    assert (test_string);
    assert (expected);

    char *encoded = zarmour_encode (self, (byte *) test_string, strlen (test_string));
    assert (encoded);
    if (verbose)
        zsys_debug ("    encoded '%s' into '%s' ('%s')", test_string, encoded, expected);
    assert (strlen (encoded) == strlen (expected));
    assert (streq (encoded, expected));
    s_armour_decode (self, encoded, test_string, verbose);
    freen (encoded);
}

static void
s_armour_test_long (zarmour_t *self, byte *test_data, size_t length, bool verbose)
{
    if (verbose)
        zarmour_print (self);

    char *test_string = zarmour_encode (self, test_data, length);
    assert (test_string);
    if (verbose)
        zsys_debug ("    encoded %d bytes array to:\n%s", length, test_string);

    zchunk_t *chunk = zarmour_decode (self, test_string);
    assert (chunk);
    assert (zchunk_size (chunk) == length + 1);
    uint index;
    for (index = 0; index < length; index++)
        assert (zchunk_data (chunk)[index] == index);
    zchunk_destroy (&chunk);
    if (verbose)
        zsys_debug ("    decoded %d bytes, all match", length);
    freen (test_string);
}


//  --------------------------------------------------------------------------
//  Selftest

void
zarmour_test (bool verbose)
{
    printf (" * zarmour: ");

    if (verbose)
        printf ("\n");

    //  @selftest
    zarmour_t *self = zarmour_new ();
    assert (self);

    int mode = zarmour_mode (self);
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
    zarmour_set_pad (self, true);

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

#ifdef _INCLUDE_Z85
    //  Z85 test is homemade; using 0, 4 and 8 bytes, with precalculated
    //  test vectors created with a libzmq test.
    //  ----------------------------------------------------------------

    //  Make a fake curve key from hex (base16) string, making sure
    //  there are no null bytes inside, so we can use our test utility
    zarmour_set_mode (self, ZARMOUR_MODE_BASE16);
    zarmour_set_line_breaks (self, false);

    zchunk_t *chunk = zarmour_decode (self,
        "4E6F87E2FB6EB22A1EF5E257B75D79124949565F0B8B36A878A4F03111C96E0B");
    assert (chunk);

    zarmour_set_mode (self, ZARMOUR_MODE_Z85);  //  Z85 mode does not support padding or line breaks
    zarmour_set_pad (self, false);              //  so these two are superfluous;
    zarmour_set_line_breaks (self, false);      //  just for consistency
    if (verbose)
        zarmour_print (self);

    s_armour_test (self, "", "", verbose);
    s_armour_test (self, "foob", "w]zP%", verbose);
    s_armour_test (self, "foobar!!", "w]zP%vr9Im", verbose);
    s_armour_test (self, (char *) zchunk_data (chunk),
                   "ph+{E}!&X?9}!I]W{sm(nL8@&3Yu{wC+<*-5Y[[#", verbose);
    zchunk_destroy (&chunk);
#endif

    //  Armouring longer byte array to test line breaks
    zarmour_set_pad (self, true);
    zarmour_set_line_breaks (self, true);
    byte test_data [256];
    int index;
    for (index = 0; index < 256; index++)
        test_data [index] = index;

    zarmour_set_mode (self, ZARMOUR_MODE_BASE64_STD);
    s_armour_test_long (self, test_data, 256, verbose);
    zarmour_set_mode (self, ZARMOUR_MODE_BASE64_URL);
    s_armour_test_long (self, test_data, 256, verbose);
    zarmour_set_mode (self, ZARMOUR_MODE_BASE32_STD);
    s_armour_test_long (self, test_data, 256, verbose);
    zarmour_set_mode (self, ZARMOUR_MODE_BASE32_HEX);
    s_armour_test_long (self, test_data, 256, verbose);
    zarmour_set_mode (self, ZARMOUR_MODE_BASE16);
    s_armour_test_long (self, test_data, 256, verbose);
#ifdef _INCLUDE_Z85
    zarmour_set_mode (self, ZARMOUR_MODE_Z85);
    s_armour_test_long (self, test_data, 256, verbose);
#endif

    zarmour_destroy (&self);

#if defined (__WINDOWS__)
    zsys_shutdown();
#endif
    //  @end

    printf ("OK\n");
}
