/*  =========================================================================
    zloop - working with 0MQ contexts

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
#include "../include/zloop.h"

//  Structure of our class

struct _zloop_t {
    void *context;              //  0MQ context object
};


//  --------------------------------------------------------------------------
//  Constructor

zloop_t *
zloop_new (void)
{
    zloop_t
        *self;

    self = (zloop_t *) calloc (1, sizeof (zloop_t));
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zloop_destroy (zloop_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zloop_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Register a socket with the reactor. You can register the socket for either
//  input or output or both. When the socket is ready, will call the handler
//  passing the argument.

int
zloop_register (zloop_t *self, void *socket, int flags, zloop_fn handler, 
                void *argument)
{
    assert (self);
    return 0;
}


//  --------------------------------------------------------------------------
//  Register a single timer to expire within some msecs. When the timer
//  expires, will call the handler passing the argument.

int
zloop_alarm (zloop_t *self, size_t alarm_msecs, zloop_fn handler, void *argument)
{
    assert (self);
    return 0;
}


//  --------------------------------------------------------------------------
//  Register a repeated timer to expire every some msecs. Each time the timer
//  expires, will call the handler passing the argument.

int
zloop_clock (zloop_t *self, size_t clock_msecs, zloop_fn handler, void *argument)
{
    assert (self);
    return 0;
}


//  --------------------------------------------------------------------------
//  Start the reactor. Takes control of the thread and returns only when the 
//  0MQ context is terminated or the process is interrupted.

int
zloop_start (zloop_t *self)
{
    assert (self);
    return 0;
}


//  --------------------------------------------------------------------------
//  Selftest

int
zloop_test (Bool verbose)
{
    zloop_t
        *loop;

    printf (" * zloop: ");
    loop = zloop_new ();
    assert (loop);

    zloop_destroy (&loop);
    assert (loop == NULL);

    printf ("OK\n");
    return 0;
}
