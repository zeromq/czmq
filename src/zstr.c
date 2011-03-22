/*  =========================================================================
    zstr - working with 0MQ contexts

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of zapi, the C binding for 0MQ: http://zapi.zeromq.org.

    This is free software; you can redistribute it and/or modify it under the
    terms of the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your option)
    any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#include "../include/zapi_prelude.h"
#include "../include/zstr.h"


//  --------------------------------------------------------------------------
//  Receive C string from socket

char *
zstr_recv (void *socket)
{
    assert (socket);
    return NULL;
}


//  --------------------------------------------------------------------------
//  Send C string to socket

int
zstr_send (void *socket, const char *string)
{
    assert (socket);
    return 0;
}


//  --------------------------------------------------------------------------
//  Send formatted C string to socket

int
zstr_sendf (void *socket, const char format, ...)
{
    assert (socket);
    return 0;
}

//  --------------------------------------------------------------------------
//  Selftest

int
zstr_test (Bool verbose)
{
    printf (" * zstr: ");
    printf ("OK\n");
    return 0;
}
