/*  =========================================================================
    zproxy - work with zmq_proxy_steerable instances

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
    The zproxy class simplifies working with the zmq_proxy_steerable API.
@discuss
    There's no fallback to the older zmq_proxy, since that does not allow
    dynamic control. If you need zmq_proxy (if you are using an older
    version of libzmq), use that directly in your code.
@end
*/
#if HAVE_ZMQ_PROXY_STREERABLE
#endif
#include "../include/czmq.h"
#include "platform.h"

//  Structure of our class

struct _zproxy_t {
    zctx_t *ctx;
    void *frontend;
    void *backend;
    void *capture;
    void *pipe;
};


//  The proxy runs in a background thread
static void
    s_proxy_task (void *args, zctx_t *ctx, void *pipe);

//  --------------------------------------------------------------------------
//  Constructor
//  Create a new zproxy object. You must create the frontend and backend
//  sockets, configure them, and connect or bind them, before you pass them
//  to the constructor. Do NOT use the sockets again, after passing them to
//  this method.

zproxy_t *
zproxy_new (zctx_t *ctx, void *frontend, void *backend)
{
    assert (ctx);
#if HAVE_ZMQ_PROXY_STEERABLE
    zproxy_t *self;
    self = (zproxy_t *) zmalloc (sizeof (zproxy_t));
    if (self) {
        self->ctx = ctx;
        self->frontend = frontend;
        self->backend = backend;
        self->pipe = zthread_fork (ctx, s_proxy_task, self);
        if (self->pipe) {
            char *reply = zstr_recv (self->pipe);
            zstr_free (&reply);
        }
        else {
            free (self);
            self = NULL;
        }
    }
    return self;
#else
    return NULL;
#endif
}


//  --------------------------------------------------------------------------
//  Destroy a zproxy object; note this first stops the proxy.

void
zproxy_destroy (zproxy_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zproxy_t *self = *self_p;
        zstr_send (self->pipe, "TERMINATE");
        char *reply = zstr_recv (self->pipe);
        zstr_free (&reply);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Copy all proxied messages to specified endpoint; if this is NULL, any
//  in-progress capturing will be stopped. You must already have bound the
//  endpoint to a PULL socket.

void
zproxy_capture (zproxy_t *self, char *endpoint)
{
    assert (self);

    //  Stop running proxy, and reconfigure new proxy with capture
    zstr_send (self->pipe, "TERMINATE");
    char *reply = zstr_recv (self->pipe);
    zstr_free (&reply);

    //  Capture flow is always PUSH-to-PULL
    self->capture = zsocket_new (self->ctx, ZMQ_PUSH);
    assert (self->capture);
    int rc = zsocket_connect (self->capture, endpoint);
    assert (rc == 0);

    //  Restart proxy with new configuration
    self->pipe = zthread_fork (self->ctx, s_proxy_task, self);
    assert (self->pipe);
    reply = zstr_recv (self->pipe);
    zstr_free (&reply);
}


//  --------------------------------------------------------------------------
//  Pauses a zproxy object; a paused proxy will cease processing messages,
//  causing them to be queued up and potentially hit the high-water mark on
//  the frontend socket, causing messages to be dropped, or writing
//  applications to block.

void
zproxy_pause (zproxy_t *self)
{
    assert (self);
    zstr_send (self->pipe, "PAUSE");
}


//  --------------------------------------------------------------------------
//  Resume a zproxy object

void
zproxy_resume (zproxy_t *self)
{
    assert (self);
    zstr_send (self->pipe, "RESUME");
}


//  --------------------------------------------------------------------------
//  Backend proxy task

static void
s_proxy_task (void *args, zctx_t *ctx, void *pipe)
{
    //  Confirm to API that we've initialized
    zstr_send (pipe, "INITIALIZED");

#if HAVE_ZMQ_PROXY_STEERABLE
    //  Start proxy
    zproxy_t *self = (zproxy_t *) args;
    zmq_proxy_steerable (self->frontend, self->backend, self->capture, pipe);
#endif
    //  Proxy has ended; we now confirm to the API that we've terminated
    zstr_send (pipe, "TERMINATED");
}


// --------------------------------------------------------------------------
//  Selftest

void
zproxy_test (bool verbose)
{
    printf (" * zproxy: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    void *frontend = zsocket_new (ctx, ZMQ_PULL);
    int rc = zsocket_bind (frontend, "inproc://frontend");
    assert (rc == 0);
    void *backend = zsocket_new (ctx, ZMQ_PUSH);
    rc = zsocket_bind (backend, "inproc://backend");
    assert (rc == 0);
    zproxy_t *proxy = zproxy_new (ctx, frontend, backend);

#if HAVE_ZMQ_PROXY_STEERABLE
    //  Connect application sockets to proxy
    void *faucet = zsocket_new (ctx, ZMQ_PUSH);
    rc = zsocket_connect (faucet, "inproc://frontend");
    assert (rc == 0);
    void *sink = zsocket_new (ctx, ZMQ_PULL);
    rc = zsocket_connect (sink, "inproc://backend");
    assert (rc == 0);

    //  Create capture socket, must be a PULL socket
    void *capture = zsocket_new (ctx, ZMQ_PULL);
    rc = zsocket_bind (capture, "inproc://capture");
    assert (rc == 0);

    //  Send some messages and check they arrived
    zstr_send (faucet, "Hello");
    char *string = zstr_recv (sink);
    assert (streq (string, "Hello"));
    zstr_free (&string);
    
    zproxy_pause (proxy);
    zstr_send (faucet, "World");
    zproxy_resume (proxy);
    string = zstr_recv (sink);
    assert (streq (string, "World"));
    zstr_free (&string);
    
    //  Switch on capturing, check that it works
    zproxy_capture (proxy, "inproc://capture");
    zstr_send (faucet, "Hello");
    
    string = zstr_recv (sink);
    assert (streq (string, "Hello"));
    zstr_free (&string);
    
    string = zstr_recv (capture);
    assert (streq (string, "Hello"));
    zstr_free (&string);
    zproxy_destroy (&proxy);
#else
    assert (proxy == NULL);
#endif
    zctx_destroy (&ctx);
    
    //  @end
    printf ("OK\n");
}
