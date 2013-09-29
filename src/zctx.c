/*  =========================================================================
    zctx - working with 0MQ contexts

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
@header
    The zctx class wraps 0MQ contexts. It manages open sockets in the context
    and automatically closes these before terminating the context. It provides
    a simple way to set the linger timeout on sockets, and configure contexts
    for number of I/O threads. Sets-up signal (interrupt) handling for the
    process.

    The zctx class has these main features:

    * Tracks all open sockets and automatically closes them before calling
    zmq_term(). This avoids an infinite wait on open sockets.

    * Automatically configures sockets with a ZMQ_LINGER timeout you can
    define, and which defaults to zero. The default behavior of zctx is
    therefore like 0MQ/2.0, immediate termination with loss of any pending
    messages. You can set any linger timeout you like by calling the
    zctx_set_linger() method.

    * Moves the iothreads configuration to a separate method, so that default
    usage is 1 I/O thread. Lets you configure this value.

    * Sets up signal (SIGINT and SIGTERM) handling so that blocking calls
    such as zmq_recv() and zmq_poll() will return when the user presses
    Ctrl-C.
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zctx_t {
    void *context;              //  Our 0MQ context
    zlist_t *sockets;           //  Sockets held by this thread
    zmutex_t* mutex;            //  Make zctx threadsafe
    bool shadow;                //  True if this is a shadow context
    int iothreads;              //  Number of IO threads, default 1
    int linger;                 //  Linger timeout, default 0
    int pipehwm;                //  Send/receive HWM for pipes
    int sndhwm;                 //  ZMQ_SNDHWM for normal sockets
    int rcvhwm;                 //  ZMQ_RCVHWM for normal sockets
};


//  ---------------------------------------------------------------------
//  Signal handling
//  This is a global variable accessible to CZMQ application code

volatile int zctx_interrupted = 0;

static void
s_signal_handler (int signal_value)
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
    if (!self)
        return NULL;

    self->sockets = zlist_new ();
    self->mutex = zmutex_new ();
    if (!self->sockets || !self->mutex) {
        zlist_destroy (&self->sockets);
        zmutex_destroy (&self->mutex);
        free (self);
        return NULL;
    }
    self->iothreads = 1;
    self->pipehwm = 1000;   
    self->sndhwm = 1000;
    self->rcvhwm = 1000;
    zsys_handler_set (s_signal_handler);
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

        //  Destroy all sockets
        while (zlist_size (self->sockets))
            zctx__socket_destroy (self, zlist_first (self->sockets));
        zlist_destroy (&self->sockets);
        zmutex_destroy (&self->mutex);

        //  ZMQ context may not yet be instantiated
        if (self->context && !self->shadow)
            zmq_term (self->context);

        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Create new shadow context, returns context object. Returns NULL if there
//  wasn't sufficient memory available.

zctx_t *
zctx_shadow (zctx_t *ctx)
{
    zctx_t
        *self;

    //  Shares same 0MQ context but has its own list of sockets so that
    //  we create, use, and destroy sockets only within a single thread.
    self = (zctx_t *) zmalloc (sizeof (zctx_t));
    if (!self)
        return NULL;

    self->sockets = zlist_new ();
    self->mutex = zmutex_new ();
    if (!self->sockets || !self->mutex) {
        zlist_destroy (&self->sockets);
        zmutex_destroy (&self->mutex);
        free (self);
        return NULL;
    }
    self->context = ctx->context;
    self->pipehwm = ctx->pipehwm;
    self->sndhwm = ctx->sndhwm;
    self->rcvhwm = ctx->rcvhwm;
    self->linger = ctx->linger;
    self->shadow = true;             //  This is a shadow context
    return self;
}


//  --------------------------------------------------------------------------
//  Configure number of I/O threads in context, only has effect if called
//  before creating first socket. Default I/O threads is 1, sufficient for
//  all except very high volume applications.

void
zctx_set_iothreads (zctx_t *self, int iothreads)
{
    assert (self);
    zmutex_lock (self->mutex);
    self->iothreads = iothreads;
    zmutex_unlock (self->mutex);
}


//  --------------------------------------------------------------------------
//  Configure linger timeout in msecs. Call this before destroying sockets or
//  context. Default is no linger, i.e. any pending messages or connects will
//  be lost.

void
zctx_set_linger (zctx_t *self, int linger)
{
    assert (self);
    zmutex_lock (self->mutex);
    self->linger = linger;
    zmutex_unlock (self->mutex);
}


//  --------------------------------------------------------------------------
//  Set initial high-water mark for inter-thread pipe sockets. Note that
//  this setting is separate from the default for normal sockets. You 
//  should change the default for pipe sockets *with care*. Too low values
//  will cause blocked threads, and an infinite setting can cause memory
//  exhaustion. The default, no matter the underlying ZeroMQ version, is
//  1,000.

void
zctx_set_pipehwm (zctx_t *self, int pipehwm)
{
    assert (self);
    zmutex_lock (self->mutex);
    self->pipehwm = pipehwm;
    zmutex_unlock (self->mutex);
}

    
//  --------------------------------------------------------------------------
//  Set initial send HWM for all new normal sockets created in context.
//  You can set this per-socket after the socket is created.
//  The default, no matter the underlying ZeroMQ version, is 1,000.

void
zctx_set_sndhwm (zctx_t *self, int sndhwm)
{
    assert (self);
    zmutex_lock (self->mutex);
    self->sndhwm = sndhwm;
    zmutex_unlock (self->mutex);
}

    
//  --------------------------------------------------------------------------
//  Set initial receive HWM for all new normal sockets created in context.
//  You can set this per-socket after the socket is created.
//  The default, no matter the underlying ZeroMQ version, is 1,000.

void
zctx_set_rcvhwm (zctx_t *self, int rcvhwm)
{
    assert (self);
    zmutex_lock (self->mutex);
    self->rcvhwm = rcvhwm;
    zmutex_unlock (self->mutex);
}


//  --------------------------------------------------------------------------
//  Return low-level 0MQ context object

void *
zctx_underlying (zctx_t *self)
{
    assert (self);
    return self->context;
}


//  --------------------------------------------------------------------------
//  Create socket within this context, for CZMQ use only

void *
zctx__socket_new (zctx_t *self, int type)
{
    //  Initialize context now if necessary
    assert (self);
    zmutex_lock (self->mutex);
    if (!self->context)
        self->context = zmq_init (self->iothreads);
    zmutex_unlock (self->mutex);
    if (!self->context)
        return NULL;

    //  Create and register socket
    void *zocket = zmq_socket (self->context, type);
    if (!zocket)
        return NULL;
    
#if (ZMQ_VERSION_MAJOR == 2)
    //  For ZeroMQ/2.x we use sndhwm for both send and receive
    zsocket_set_hwm (zocket, self->sndhwm);
#else
    //  For later versions we use separate SNDHWM and RCVHWM
    zsocket_set_sndhwm (zocket, self->sndhwm);
    zsocket_set_rcvhwm (zocket, self->rcvhwm);
#endif
    zmutex_lock (self->mutex);
    if (zlist_push (self->sockets, zocket)) {
        zmutex_unlock (self->mutex);
        zmq_close (zocket);
        return NULL;
    }
    zmutex_unlock (self->mutex);
    return zocket;
}


//  --------------------------------------------------------------------------
//  Create pipe socket within this context, for CZMQ use only

void *
zctx__socket_pipe (zctx_t *self)
{
    assert (self);
    void *pipe = zctx__socket_new (self, ZMQ_PAIR);
    if (pipe)
        zsocket_set_hwm (pipe, self->pipehwm);
    return pipe;
}


//  --------------------------------------------------------------------------
//  Destroy socket within this context, for CZMQ use only

void
zctx__socket_destroy (zctx_t *self, void *zocket)
{
    assert (self);
    assert (zocket);
    zsocket_set_linger (zocket, self->linger);
    zmq_close (zocket);

    zmutex_lock (self->mutex);
    zlist_remove (self->sockets, zocket);
    zmutex_unlock (self->mutex);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zctx_test (bool verbose)
{
    printf (" * zctx: ");

    //  @selftest
    //  Create and destroy a context without using it
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    zctx_destroy (&ctx);
    assert (ctx == NULL);

    //  Create a context with many busy sockets, destroy it
    ctx = zctx_new ();
    assert (ctx);
    zctx_set_iothreads (ctx, 1);
    zctx_set_linger (ctx, 5);       //  5 msecs
    void *s1 = zctx__socket_new (ctx, ZMQ_PAIR);
    void *s2 = zctx__socket_new (ctx, ZMQ_XREQ);
    void *s3 = zctx__socket_new (ctx, ZMQ_REQ);
    void *s4 = zctx__socket_new (ctx, ZMQ_REP);
    void *s5 = zctx__socket_new (ctx, ZMQ_PUB);
    void *s6 = zctx__socket_new (ctx, ZMQ_SUB);
    zsocket_connect (s1, "tcp://127.0.0.1:5555");
    zsocket_connect (s2, "tcp://127.0.0.1:5555");
    zsocket_connect (s3, "tcp://127.0.0.1:5555");
    zsocket_connect (s4, "tcp://127.0.0.1:5555");
    zsocket_connect (s5, "tcp://127.0.0.1:5555");
    zsocket_connect (s6, "tcp://127.0.0.1:5555");
    assert (zctx_underlying (ctx));
    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
