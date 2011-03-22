/*  =========================================================================
    zctx - working with 0MQ contexts

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
#include "../include/zctx.h"

//  Structure of our class

struct _zctx_t {
    void *context;              //  0MQ context object
};


//  --------------------------------------------------------------------------
//  Constructor

zctx_t *
zctx_new (void)
{
    zctx_t
        *self;

    self = (zctx_t *) calloc (1, sizeof (zctx_t));
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zctx_destroy (zctx_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zctx_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Configure number of I/O threads in context, must call this before first
//  use of context. Default I/O threads is 1, sufficient for all except very
//  high volume applications.

void
zctx_set_iothreads (zctx_t *self, int threads) 
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Configure linger timeout. Call this before destroying sockets or context.
//  Default is no linger, i.e. any pending messages or connects will be lost.

void 
zctx_set_linger (zctx_t *self, int msecs)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Return 0MQ context for zctx

void *
zctx_context (zctx_t *self)
{
    assert (self);
    return self->context;
}


//  --------------------------------------------------------------------------
//  Create new 0MQ socket and register for this context. Use this instead of
//  the 0MQ core API method if you want automatic management of the socket 
//  at shutdown.

void *
zctx_socket_new (zctx_t *self, int type)
{
    assert (self);
    return NULL;
}


//  --------------------------------------------------------------------------
//  Destroy the socket. You must use this for any socket created via the 
//  zctx_socket_new method.

void
zctx_socket_destroy (zctx_t *self, void **socket)
{
    assert (self);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zctx_test (Bool verbose)
{
    zctx_t
        *ctx;

    printf (" * zctx: ");
    ctx = zctx_new ();
    assert (ctx);

    zctx_destroy (&ctx);
    assert (ctx == NULL);

    printf ("OK\n");
    return 0;
}
