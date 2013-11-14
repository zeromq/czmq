/*  =========================================================================
    zuuid - UUID support class

    -------------------------------------------------------------------------
    Copyright (c) 1991-2012 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of Zyre, an open-source framework for proximity-based
    peer-to-peer applications -- See http://zyre.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
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
//  Returns UUID as string

char *
zuuid_str (zuuid_t *self)
{
    assert (self);
    return self->str;
}


//  -----------------------------------------------------------------
//  Store UUID blob in target array

void
zuuid_cpy (zuuid_t *self, byte *target)
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
//  Selftest

int
zuuid_test (bool verbose)
{
    printf (" * zuuid: ");

    //  @selftest
    //  Simple create/destroy test
    zuuid_t *self = zuuid_new ();
    assert (self);
    assert (strlen (zuuid_str (self)) == 32);
    zuuid_destroy (&self);
    //  @end

    printf ("OK\n");
    return 0;
}
