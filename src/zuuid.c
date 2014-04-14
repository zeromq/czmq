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
    random number generator to produce convincing imitations of uuids.
@discuss
@end
*/

#include "../include/czmq.h"
#include "platform.h"

#if defined (__WINDOWS__) && !defined (HAVE_LIBUUID)
#define HAVE_LIBUUID 1
#endif

#if defined (__UTYPE_OSX) && !defined (HAVE_LIBUUID)
#define HAVE_LIBUUID 1
#endif

#if defined (HAVE_LIBUUID)
#if defined (__UTYPE_FREEBSD) || defined (__UTYPE_NETBSD)
#   include <uuid.h>
#elif defined __UTYPE_HPUX
#   include <dce/uuid.h>
#elif defined (__UNIX__)
#   include <uuid/uuid.h>
#endif
#endif

//  Structure of our class

struct _zuuid_t {
    byte uuid [ZUUID_LEN];              //  Binary UUID
    char str [ZUUID_LEN * 2 + 1];       //  Printable UUID
};


//  --------------------------------------------------------------------------
//  Constructor

zuuid_t *
zuuid_new (void)
{
    zuuid_t *self = (zuuid_t *) zmalloc (sizeof (zuuid_t));
#if defined (HAVE_LIBUUID)
#   if defined (__WINDOWS__)
    UUID uuid;
    assert (sizeof (uuid) == ZUUID_LEN);
    UuidCreate (&uuid);
    zuuid_set (self, (byte *) &uuid);
#   else
    uuid_t uuid;
    assert (sizeof (uuid) == ZUUID_LEN);
    uuid_generate (uuid);
    zuuid_set (self, (byte *) uuid);
#   endif
#else
    //  No UUID system calls, so generate a random string
    byte uuid [ZUUID_LEN];
    int fd = open ("/dev/urandom", O_RDONLY);
    if (fd != -1) {
        ssize_t bytes_read = read (fd, uuid, ZUUID_LEN);
        assert (bytes_read == ZUUID_LEN);
        close (fd);
    }
    zuuid_set (self, uuid);
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
        free (self);
        *self_p = NULL;
    }
}


//  -----------------------------------------------------------------
//  Set UUID to new supplied value

void
zuuid_set (zuuid_t *self, byte *source)
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
}


//  -----------------------------------------------------------------
//  Return UUID binary data

byte *
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

char *
zuuid_str (zuuid_t *self)
{
    assert (self);
    return self->str;
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
zuuid_eq (zuuid_t *self, byte *compare)
{
    assert (self);
    return (memcmp (self->uuid, compare, ZUUID_LEN) == 0);
}


//  -----------------------------------------------------------------
//  Check if UUID is different from supplied value

bool
zuuid_neq (zuuid_t *self, byte *compare)
{
    assert (self);
    return (memcmp (self->uuid, compare, ZUUID_LEN) != 0);
}


//  --------------------------------------------------------------------------
//  Make copy of UUID object

zuuid_t *
zuuid_dup (zuuid_t *self)
{
    if (!self)
        return NULL;

    zuuid_t *copy = zuuid_new ();
    if (copy)
        zuuid_set (copy, zuuid_data (self));
    return copy;
}


//  --------------------------------------------------------------------------
//  Selftest

int
zuuid_test (bool verbose)
{
    printf (" * zuuid: ");

    //  @selftest
    //  Simple create/destroy test
    zuuid_t *uuid = zuuid_new ();
    assert (uuid);
    assert (zuuid_size (uuid) == 16);
    assert (strlen (zuuid_str (uuid)) == 32);
    zuuid_t *copy = zuuid_dup (uuid);
    assert (streq (zuuid_str (uuid), zuuid_str (copy)));
    zuuid_destroy (&uuid);
    zuuid_destroy (&copy);
    //  @end

    printf ("OK\n");
    return 0;
}
