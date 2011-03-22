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
#include "../include/zlist.h"
#include "../include/zctx.h"

//  Structure of our class

struct _zctx_t {
    void *context;              //  0MQ context object
    zlist_t *sockets;           //  List of registered sockets
    int iothreads;              //  Number of io threads for zmq_init
    int linger;                 //  Linger setting
};


//  ---------------------------------------------------------------------
//  Signal handling
//

int zctx_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    zctx_interrupted = 1;
}


//  --------------------------------------------------------------------------
//  Constructor

zctx_t *
zctx_new (void)
{
    zctx_t
        *self;

    self = (zctx_t *) zmalloc (sizeof (zctx_t));
    self->sockets = zlist_new ();

    //  Install signal handler for SIGINT and SIGTERM
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
    
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
        if (self->context) {
            //  Close all sockets still registered with context
            while (zlist_size (self->sockets))
                zctx_socket_destroy (self, zlist_first (self->sockets));
            zmq_term (self->context);
        }
        zlist_destroy (&self->sockets);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Configure number of I/O threads in context, must call this before first
//  use of context. Default I/O threads is 1, sufficient for all except very
//  high volume applications.

void
zctx_set_iothreads (zctx_t *self, int iothreads) 
{
    assert (self);
    assert (!self->context);
    self->iothreads = iothreads;
}


//  --------------------------------------------------------------------------
//  Configure linger timeout. Call this before destroying sockets or context.
//  Default is no linger, i.e. any pending messages or connects will be lost.

void 
zctx_set_linger (zctx_t *self, int linger)
{
    assert (self);
    self->linger = linger;
}


//  --------------------------------------------------------------------------
//  Return 0MQ context for zctx; if the context has not yet been initialized,
//  this method will initialize it.

void *
zctx_context (zctx_t *self)
{
    assert (self);
    if (self->context == NULL)
        self->context = zmq_init (self->iothreads);
    assert (self->context);
    return self->context;
}


//  --------------------------------------------------------------------------
//  Create new 0MQ socket and register for this context. Use this instead of
//  the 0MQ core API method if you want automatic management of the socket 
//  at shutdown. If the context has not yet been initialized, this method will
//  initialize it.

void *
zctx_socket_new (zctx_t *self, int type)
{
    assert (self);
    if (self->context == NULL)
        self->context = zmq_init (self->iothreads);
    assert (self->context);
    void *socket = zmq_socket (self->context, type);
    assert (socket);
    zlist_push (self->sockets, socket);
    return socket;
}


//  --------------------------------------------------------------------------
//  Destroy the socket. You must use this for any socket created via the 
//  zctx_socket_new method.

void
zctx_socket_destroy (zctx_t *self, void *socket)
{
    assert (self);
    assert (socket);
    zmq_setsockopt (socket, ZMQ_LINGER, &self->linger, sizeof (int));
    zmq_close (socket);
    zlist_remove (self->sockets, socket);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zctx_test (Bool verbose)
{
    printf (" * zctx: ");
    
    //  Create and destroy a context without using it
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    zctx_destroy (&ctx);
    assert (ctx == NULL);

    //  Create a context with many busy sockets, destroy it
    ctx = zctx_new ();
    void *s1 = zctx_socket_new (ctx, ZMQ_PAIR);
    void *s2 = zctx_socket_new (ctx, ZMQ_XREQ);
    void *s3 = zctx_socket_new (ctx, ZMQ_REQ);
    void *s4 = zctx_socket_new (ctx, ZMQ_REP);
    void *s5 = zctx_socket_new (ctx, ZMQ_PUB);
    void *s6 = zctx_socket_new (ctx, ZMQ_SUB);
    zmq_connect (s1, "tcp://127.0.0.1:5555");
    zmq_connect (s2, "tcp://127.0.0.1:5555");
    zmq_connect (s3, "tcp://127.0.0.1:5555");
    zmq_connect (s4, "tcp://127.0.0.1:5555");
    zmq_connect (s5, "tcp://127.0.0.1:5555");
    zmq_connect (s6, "tcp://127.0.0.1:5555");
    zctx_destroy (&ctx);
    
    printf ("OK\n");
    return 0;
}
