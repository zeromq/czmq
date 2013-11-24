/*  =========================================================================
    zproxy - convenient zmq_proxy API

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
    The zproxy class simplifies working with the zmq_proxy API.
@discuss
    If we used a steerable proxy we could do termination better, and
    allow creation/termination of proxies without destroying contexts.
    Then, zproxy_destroy would send the proxy task a KILL message and
    wait for an OK response, then return to the caller.
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zproxy_t {
    zctx_t *ctx;
    void *pipe;
    int type;
    char frontend_addr [256];
    int frontend_type;
    void *frontend;
    char backend_addr [256];
    int backend_type;
    void *backend;
    char capture_addr [256];
    int capture_type;
    void *capture;
};

// Background task that runs the proxy
static void
    s_agent_task (void *args, zctx_t *ctx, void *pipe);


//  --------------------------------------------------------------------------
//  Constructor

zproxy_t *
zproxy_new (zctx_t *ctx, int zproxy_type)
{
    assert (ctx);
    assert (zproxy_type > 0 && zproxy_type < 4);
    
    zproxy_t *self;
    self = (zproxy_t *) zmalloc (sizeof (zproxy_t));
    assert (self);
    self->ctx = ctx;
    self->type = zproxy_type;
    self->capture_type = ZMQ_PUB;
    
    switch (self->type) {
        case (ZPROXY_QUEUE):
            self->frontend_type = ZMQ_ROUTER;
            self->backend_type = ZMQ_DEALER;
            break;
        case (ZPROXY_FORWARDER):
            self->frontend_type = ZMQ_XSUB;
            self->backend_type = ZMQ_XPUB;
            break;
        case (ZPROXY_STREAMER):
            self->frontend_type = ZMQ_PULL;
            self->backend_type = ZMQ_PUSH;
            break;
        default:
            break;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor; call this after destroying the parent context. Since the
//  pipe to the proxy task will be dead, we cannot synchronize 

void
zproxy_destroy (zproxy_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zproxy_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Start and zmq_proxy in an attached thread, binding to endpoints
//  Returns 0 if OK, -1 if there was an error

int
zproxy_bind (zproxy_t *self,
    const char *frontend_addr,
    const char *backend_addr,
    const char *capture_addr)
{
    strncpy (self->frontend_addr, frontend_addr, 255);
    self->frontend_addr [255] = '\0';

    strncpy (self->backend_addr, backend_addr, 255);
    self->frontend_addr [255] = '\0';

    strncpy (self->capture_addr, capture_addr, 255);
    self->frontend_addr [255] = '\0';

    self->pipe = zthread_fork (self->ctx, s_agent_task, self);
    int rc;
    if (self->pipe) {
        zstr_send (self->pipe, "START");
        char *response = zstr_recv (self->pipe);
        rc = streq (response, "OK")? 0: -1;
        free (response);
    }
    else
        rc = -1;
    return rc;
}


//  --------------------------------------------------------------------------
//  Get the proxy type 

int
zproxy_type (zproxy_t *self)
{
    return self->type;
}


//  --------------------------------------------------------------------------
//  Get the proxy frontend address

char *
zproxy_frontend_addr (zproxy_t *self)
{
    return self->frontend_addr;
}


//  --------------------------------------------------------------------------
//  Get the proxy frontend type

int
zproxy_frontend_type (zproxy_t *self)
{
    return self->frontend_type;
}


//  --------------------------------------------------------------------------
//  Get the proxy backend address

char *
zproxy_backend_addr (zproxy_t *self)
{
    return self->backend_addr;
}


//  --------------------------------------------------------------------------
//  Get the proxy backend type

int
zproxy_backend_type (zproxy_t *self)
{
    return self->backend_type;
}


//  --------------------------------------------------------------------------
//  Get the proxy capture address

char *
zproxy_capture_addr (zproxy_t *self)
{
    return self->capture_addr;
}


//  --------------------------------------------------------------------------
//  Get the proxy capture type

int
zproxy_capture_type (zproxy_t *self)
{
    return self->capture_type;
}


// --------------------------------------------------------------------------
//  Selftest

int
zproxy_test (bool verbose)
{
    printf (" * zproxy: ");

    //  @selftest
    const char *front_addr = "inproc://proxy_front";
    const char *back_addr = "inproc://proxy_back";
    const char *capture_addr = "inproc://proxy_capture";

    //  Create and start the proxy
    zctx_t *ctx = zctx_new ();
    zproxy_t *proxy = zproxy_new (ctx, ZPROXY_STREAMER);
    int rc = zproxy_bind (proxy, front_addr, back_addr, capture_addr);
    assert (rc == 0);

    //  Test the accessor methods
    assert (zproxy_type (proxy) == ZPROXY_STREAMER);
    assert (zproxy_frontend_type (proxy) == ZMQ_PULL);
    assert (zproxy_backend_type (proxy) == ZMQ_PUSH);
    assert (zproxy_capture_type (proxy) == ZMQ_PUB);

    char *front_check = zproxy_frontend_addr (proxy);
    assert (streq (front_check, front_addr));
    char *back_check = zproxy_backend_addr (proxy);
    assert (streq (back_check, back_addr));
    char *capture_check = zproxy_capture_addr (proxy);
    assert (streq (capture_check, capture_addr));

    //  Connect to the proxy front, back, and capture ports
    void *front_s = zsocket_new (ctx, ZMQ_PUSH);
    assert (front_s);
    zsocket_connect (front_s, zproxy_frontend_addr (proxy));

    void *back_s = zsocket_new (ctx, ZMQ_PULL);
    assert (back_s);
    zsocket_connect (back_s, zproxy_backend_addr (proxy));

    void *capture_s = zsocket_new (ctx, ZMQ_SUB);
    zsocket_set_subscribe (capture_s, "");
    assert (back_s);
    zsocket_connect (capture_s, zproxy_capture_addr (proxy));

    //  Send a message through the proxy and receive it
    zstr_send (front_s, "STREAMER_TEST");
    
    char *back_resp = zstr_recv (back_s);
    assert (back_resp);
    assert (streq ("STREAMER_TEST", back_resp));
    free (back_resp);
    
    char *capture_resp = zstr_recv (capture_s);
    assert (capture_resp);
    assert (capture_resp);
    assert (streq ("STREAMER_TEST", capture_resp));
    free (capture_resp);
   
    //  Destroying the context will stop the proxy, see note at
    //  start of source about using zmq_proxy_steerable instead.
    //  The sleep here is to ensure memory is freed so valgrind
    //  will be happy.
    zctx_destroy (&ctx);
    zproxy_destroy (&proxy);
    zclock_sleep (100);
    
    //  @end
    printf ("OK\n");
    return 0;
}


//  --------------------------------------------------------------------------
//  Backend agent implementation

static void
s_agent_task (void *args, zctx_t *ctx, void *pipe)
{   
    zproxy_t *self = (zproxy_t*) args;
    self->frontend = zsocket_new (ctx, zproxy_frontend_type (self));
    if (self->frontend)
        zsocket_bind (self->frontend, zproxy_frontend_addr (self));
    else
        return;

    self->backend = zsocket_new (ctx, zproxy_backend_type (self));
    if (self->backend)
        zsocket_bind (self->backend, zproxy_backend_addr (self));
    else
        return;

    self->capture = zsocket_new (ctx, zproxy_capture_type (self));
    if (self->capture)
        zsocket_bind (self->capture, zproxy_capture_addr (self));
    else
        return;

    free (zstr_recv (pipe));
    zstr_send (pipe, "OK");

#if (ZMQ_VERSION_MAJOR >= 30201)
    //  zmq_proxy was introduced in libzmq 3.2.1
    zmq_proxy (self->frontend, self->backend, self->capture);
#else
    zmq_device (ZMQ_QUEUE, self->frontend, self->backend);
#endif
}
