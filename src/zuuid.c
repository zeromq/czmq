/*  =========================================================================
    zuuid - UUID support class

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
    The zuuid class generates UUIDs and provides methods for working with
    them. If you build CZMQ with libuuid, on Unix/Linux, it will use that
    library. On Windows it will use UuidCreate(). Otherwise it will use a
    random number generator to produce convincing imitations of UUIDs.
@discuss
@end
*/

#include "czmq_classes.h"

//  Structure of our class

struct _zuuid_t {
    byte uuid [ZUUID_LEN];              //  Binary UUID
    char str [ZUUID_STR_LEN + 1];       //  Printable UUID
    char *str_canonical;                //  Canonical UUID, if any
};


//  --------------------------------------------------------------------------
//  Constructor

zuuid_t *
zuuid_new (void)
{
    zuuid_t *self = (zuuid_t *) zmalloc (sizeof (zuuid_t));
    assert (self);

#if defined (__WINDOWS__)
    //  Windows always has UUID support
    UUID uuid;
    assert (sizeof (uuid) == ZUUID_LEN);
    UuidCreate (&uuid);
    zuuid_set (self, (byte *) &uuid);
#elif defined (__UTYPE_ANDROID) || !defined (HAVE_UUID)
    //  No UUID system calls, so generate a random string
    byte uuid [ZUUID_LEN];

    int fd = open ("/dev/urandom", O_RDONLY);
    if (fd != -1) {
        ssize_t bytes_read = read (fd, uuid, ZUUID_LEN);
        assert (bytes_read == ZUUID_LEN);
        close (fd);
        zuuid_set (self, uuid);
    }
    else {
        //  We couldn't read /dev/urandom and we have no alternative
        //  strategy
        zsys_error (strerror (errno));
        assert (false);
    }
#elif defined (__UTYPE_OPENBSD) || defined (__UTYPE_FREEBSD) || defined (__UTYPE_NETBSD)
    uuid_t uuid;
    uint32_t status = 0;
    uuid_create (&uuid, &status);
    if (status != uuid_s_ok) {
        zuuid_destroy (&self);
        return NULL;
    }
    byte buffer [ZUUID_LEN];
    uuid_enc_be (&buffer, &uuid);
    zuuid_set (self, buffer);
#elif defined (__UTYPE_LINUX) || defined (__UTYPE_OSX) || defined (__UTYPE_GNU)
    uuid_t uuid;
    assert (sizeof (uuid) == ZUUID_LEN);
    uuid_generate (uuid);
    zuuid_set (self, (byte *) uuid);
#else
#   error "Unknow UNIX TYPE"
#endif
    return self;
}


//  -----------------------------------------------------------------
//  Destructor

void
zuuid_destroy (zuuid_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zuuid_t *self = *self_p;
        free (self->str_canonical);
        free (self);
        *self_p = NULL;
    }
}


//  -----------------------------------------------------------------
//  Create UUID object from supplied ZUUID_LEN-octet value

zuuid_t *
zuuid_new_from (const byte *source)
{
    zuuid_t *self = (zuuid_t *) zmalloc (sizeof (zuuid_t));
    assert (self);
    zuuid_set (self, source);
    return self;
}


//  -----------------------------------------------------------------
//  Set UUID to new supplied ZUUID_LEN-octet value

void
zuuid_set (zuuid_t *self, const byte *source)
{
    assert (self);
    memcpy (self->uuid, source, ZUUID_LEN);
    char hex_char [] = "0123456789ABCDEF";
    int byte_nbr;
    for (byte_nbr = 0; byte_nbr < ZUUID_LEN; byte_nbr++) {
        uint val = (self->uuid) [byte_nbr];
        self->str [byte_nbr * 2 + 0] = hex_char [val >> 4];
        self->str [byte_nbr * 2 + 1] = hex_char [val & 15];
    }
    self->str [ZUUID_LEN * 2] = 0;
    zstr_free (&self->str_canonical);
}


//  -----------------------------------------------------------------
//  Set UUID to new supplied string value skipping '-' and '{' '}'
//  optional delimiters. Return 0 if OK, else returns -1.

int
zuuid_set_str (zuuid_t *self, const char *source)
{
    assert (self);
    assert (source);

    uint byte_nbr = 0;
    while (*source) {
        if (*source == '-' || *source == '{' || *source == '}')
            source++;
        else {
            //  Get two hex digits
            uint value;
            if (sscanf (source, "%02x", &value) != 1)
                return -1;
            if (byte_nbr < ZUUID_LEN) {
                self->uuid [byte_nbr] = (byte) value;
                self->str [byte_nbr * 2 + 0] = toupper(*source++);
                self->str [byte_nbr * 2 + 1] = toupper(*source++);
                byte_nbr++;
            }
            else
                return -1;
        }
    }
    zstr_free (&self->str_canonical);
    return 0;
}


//  -----------------------------------------------------------------
//  Return UUID binary data

const byte *
zuuid_data (zuuid_t *self)
{
    assert (self);
    return self->uuid;
}


//  -----------------------------------------------------------------
//  Return UUID binary size

size_t
zuuid_size (zuuid_t *self)
{
    assert (self);
    return ZUUID_LEN;
}


//  -----------------------------------------------------------------
//  Returns UUID as string

const char *
zuuid_str (zuuid_t *self)
{
    assert (self);
    return self->str;
}


//  -----------------------------------------------------------------
//  Return UUID in the canonical string format: 8-4-4-4-12, in lower
//  case. Caller does not modify or free returned value. See
//  http://en.wikipedia.org/wiki/Universally_unique_identifier

const char *
zuuid_str_canonical (zuuid_t *self)
{
    assert (self);
    if (!self->str_canonical)
        self->str_canonical = (char *) zmalloc (8 + 4 + 4 + 4 + 12 + 5);
    *self->str_canonical = 0;
    strncat (self->str_canonical, self->str, 8);
    strcat  (self->str_canonical, "-");
    strncat (self->str_canonical, self->str + 8, 4);
    strcat  (self->str_canonical, "-");
    strncat (self->str_canonical, self->str + 12, 4);
    strcat  (self->str_canonical, "-");
    strncat (self->str_canonical, self->str + 16, 4);
    strcat  (self->str_canonical, "-");
    strncat (self->str_canonical, self->str + 20, 12);

    int char_nbr;
    for (char_nbr = 0; char_nbr < 36; char_nbr++)
        self->str_canonical [char_nbr] = tolower (self->str_canonical [char_nbr]);
    return self->str_canonical;
}


//  -----------------------------------------------------------------
//  Store UUID blob into a target array

void
zuuid_export (zuuid_t *self, byte *target)
{
    assert (self);
    memcpy (target, self->uuid, ZUUID_LEN);
}


//  -----------------------------------------------------------------
//  Check if UUID is same as supplied value

bool
zuuid_eq (zuuid_t *self, const byte *compare)
{
    assert (self);
    return (memcmp (self->uuid, compare, ZUUID_LEN) == 0);
}


//  -----------------------------------------------------------------
//  Check if UUID is different from supplied value

bool
zuuid_neq (zuuid_t *self, const byte *compare)
{
    assert (self);
    return (memcmp (self->uuid, compare, ZUUID_LEN) != 0);
}


//  --------------------------------------------------------------------------
//  Make copy of UUID object; if uuid is null, or memory was exhausted,
//  returns null.

zuuid_t *
zuuid_dup (zuuid_t *self)
{
    if (self)
        return zuuid_new_from (zuuid_data (self));
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Print properties of the zuuid object.

void
zuuid_print (zuuid_t *self)
{
    printf ("%s", zuuid_str_canonical (self));
}


//  --------------------------------------------------------------------------
//  Selftest

void
zuuid_test (bool verbose)
{
    printf (" * zuuid: ");

    //  @selftest
    //  Simple create/destroy test
    assert (ZUUID_LEN == 16);
    assert (ZUUID_STR_LEN == 32);

    zuuid_t *uuid = zuuid_new ();
    assert (uuid);
    assert (zuuid_size (uuid) == ZUUID_LEN);
    assert (strlen (zuuid_str (uuid)) == ZUUID_STR_LEN);
    zuuid_t *copy = zuuid_dup (uuid);
    assert (streq (zuuid_str (uuid), zuuid_str (copy)));

    //  Check set/set_str/export methods
    const char *myuuid = "8CB3E9A9649B4BEF8DE225E9C2CEBB38";
    const char *myuuid2 = "8CB3E9A9-649B-4BEF-8DE2-25E9C2CEBB38";
    const char *myuuid3 = "{8CB3E9A9-649B-4BEF-8DE2-25E9C2CEBB38}";
    const char *myuuid4 = "8CB3E9A9649B4BEF8DE225E9C2CEBB3838";
    int rc = zuuid_set_str (uuid, myuuid);
    assert (rc == 0);
    assert (streq (zuuid_str (uuid), myuuid));
    rc = zuuid_set_str (uuid, myuuid2);
    assert (rc == 0);
    assert (streq (zuuid_str (uuid), myuuid));
    rc = zuuid_set_str (uuid, myuuid3);
    assert (rc == 0);
    assert (streq (zuuid_str (uuid), myuuid));
    rc = zuuid_set_str (uuid, myuuid4);
    assert (rc == -1);
    byte copy_uuid [ZUUID_LEN];
    zuuid_export (uuid, copy_uuid);
    zuuid_set (uuid, copy_uuid);
    assert (streq (zuuid_str (uuid), myuuid));

    //  Check the canonical string format
    assert (streq (zuuid_str_canonical (uuid),
                   "8cb3e9a9-649b-4bef-8de2-25e9c2cebb38"));

    zuuid_destroy (&uuid);
    zuuid_destroy (&copy);
    //  @end

    printf ("OK\n");
}
