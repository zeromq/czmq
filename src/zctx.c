/*  =========================================================================
    zctx - working with 0MQ contexts (deprecated)

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
    NOTE: this class is deprecated in favor of zsock, which does not expose
    contexts in the API at all. All zsock instances use the same global
    context.
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


//  --------------------------------------------------------------------------
//  Constructor

zctx_t *
zctx_new (void)
{
    zctx_t *self = (zctx_t *) zmalloc (sizeof (zctx_t));
    if (!self)
        return NULL;

    self->sockets = zlist_new ();
    self->mutex = zmutex_new ();
    if (!self->sockets || !self->mutex) {
        zctx_destroy (&self);
        return NULL;
    }
    self->iothreads = 1;
    self->pipehwm = 1000;
    self->sndhwm = 1000;
    self->rcvhwm = 1000;

    //  Catch SIGINT and SIGTERM unless ZSYS_SIGHANDLER=false
    if (  getenv ("ZSYS_SIGHANDLER") == NULL
       || strneq (getenv ("ZSYS_SIGHANDLER"), "false"))
        zsys_catch_interrupts ();

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
        if (self->sockets)
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
    assert (ctx);
    // Initialize the original context now if necessary
    if (!ctx->context) {
        zctx__initialize_underlying (ctx);
        if (!ctx->context)
            return NULL;
    }
    zctx_t *self = zctx_shadow_zmq_ctx (ctx->context);
    if (self) {
        // copy high water marks and linger from old context
        self->pipehwm = ctx->pipehwm;
        self->sndhwm = ctx->sndhwm;
        self->rcvhwm = ctx->rcvhwm;
        self->linger = ctx->linger;
    }
    return self;
}

//  --------------------------------------------------------------------------
//  Create new context that shadows the given ZMQ context

zctx_t *
zctx_shadow_zmq_ctx (void *zmqctx)
{
    assert (zmqctx);

    //  Shares same 0MQ context but has its own list of sockets so that
    //  we create, use, and destroy sockets only within a single thread.
    zctx_t *self = (zctx_t *) zmalloc (sizeof (zctx_t));
    if (!self)
        return NULL;

    self->shadow = true;             //  This is a shadow context
    self->sockets = zlist_new ();
    self->mutex = zmutex_new ();
    if (!self->sockets || !self->mutex) {
        zctx_destroy (&self);
        return NULL;
    }
    self->context = zmqctx;
    self->pipehwm = 1000;
    self->sndhwm = 1000;
    self->rcvhwm = 1000;
    return self;
}


//  --------------------------------------------------------------------------
//  Configure number of I/O threads in context, only has effect if called
//  before creating first socket, or calling zctx_shadow. Default I/O
//  threads is 1, sufficient for all except very high volume applications.

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
//  Initialize the low-level 0MQ context object

void
zctx__initialize_underlying (zctx_t *self)
{
    assert (self);
    zmutex_lock (self->mutex);
    if (!self->context)
        self->context = zmq_init (self->iothreads);
    zmutex_unlock (self->mutex);
}


//  --------------------------------------------------------------------------
//  Create socket within this context, for CZMQ use only

void *
zctx__socket_new (zctx_t *self, int type)
{
    //  Initialize context now if necessary
    assert (self);
    if (!self->context) {
        zctx__initialize_underlying (self);
        if (!self->context)
            return NULL;
    }
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

void
zctx_test (bool verbose)
{
    printf (" * zctx (deprecated): ");

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
    assert (s1);
    void *s2 = zctx__socket_new (ctx, ZMQ_XREQ);
    assert (s2);
    void *s3 = zctx__socket_new (ctx, ZMQ_REQ);
    assert (s3);
    void *s4 = zctx__socket_new (ctx, ZMQ_REP);
    assert (s4);
    void *s5 = zctx__socket_new (ctx, ZMQ_PUB);
    assert (s5);
    void *s6 = zctx__socket_new (ctx, ZMQ_SUB);
    assert (s6);
    int rc = zsocket_connect (s1, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s2, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s3, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s4, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s5, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s6, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    assert (zctx_underlying (ctx));
    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
}
