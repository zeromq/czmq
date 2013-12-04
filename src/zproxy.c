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
    I've modified zproxy to attempt to "do the right thing" depending
    on the underlying version of libzmq.  The functions to send commands
    to a steerable instance now return a -1 if czmq is built against a
    libzmq version that doesn't have steerable proxy.  Note - I'm not convinced
    I shouldn't just assert instead and am open to other approaches - just
    wanted to get it "working" first.  If built against libzmq 2.2, zproxy
    calls zmq_device with the correct arguments based on zproxy type.
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
    char control_addr [256];
    int control_type;
    void *control;
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
    self->control_type = ZMQ_SUB;

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
//  Command; Sends a pause command to the proxy
//  Returns 0 = ok
//  Returns -1 = command not implemented for current libzmq version

int
zproxy_pause (zproxy_t *self)
{
    assert (self);
    int rc = -1;
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    zstr_send (self->control, "PAUSE");
    rc = 0;
#endif
    return rc;
}

//  --------------------------------------------------------------------------
//  Command; Sends a resume command to the proxy
//  Returns 0 = ok
//  Returns -1 = command not implemented for current libzmq version

int
zproxy_resume (zproxy_t *self)
{
    assert (self);
    int rc = -1;
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    zstr_send (self->control, "RESUME");
    rc = 0;
#endif
    return rc;
}

//  --------------------------------------------------------------------------
//  Command; Sends a terminate command to the proxy
//  Returns 0 = ok
//  Returns -1 = command not implemented for current libzmq version
//      or proxy did not respond

int
zproxy_terminate (zproxy_t *self)
{
    assert (self);
    int rc = -1;
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    zstr_send (self->control, "TERMINATE");
    char *resp = zstr_recv (self->pipe);
    if (resp && streq ("TERMINATED", resp))
        rc = 0;
#endif
    return rc;
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
    const char *capture_addr,
    const char *control_addr)
{
    strncpy (self->frontend_addr, frontend_addr, 255);
    self->frontend_addr [255] = '\0';

    strncpy (self->backend_addr, backend_addr, 255);
    self->backend_addr [255] = '\0';

    strncpy (self->capture_addr, capture_addr, 255);
    self->capture_addr [255] = '\0';

    strncpy (self->control_addr, control_addr, 255);
    self->control_addr [255] = '\0';
    self->control = zsocket_new (self->ctx, ZMQ_PUB);
    zsocket_bind (self->control, self->control_addr);
    
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

//  --------------------------------------------------------------------------
//  Get the proxy control address

char *
zproxy_control_addr (zproxy_t *self)
{
    return self->control_addr;
}

//  --------------------------------------------------------------------------
//  Get the proxy control type

int
zproxy_control_type (zproxy_t *self)
{
    return self->control_type;
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
    const char *control_addr = "inproc://proxy_control";
    
    //  Create and start the proxy
    zctx_t *ctx = zctx_new ();
    zproxy_t *proxy = zproxy_new (ctx, ZPROXY_STREAMER);

    int rc = zproxy_bind (proxy, front_addr, back_addr, 
            capture_addr, control_addr);

    // Pause for 10ms 
    zclock_sleep (10);

    assert (rc == 0);
    
    assert (zproxy_type (proxy) == ZPROXY_STREAMER);
    assert (zproxy_frontend_type (proxy) == ZMQ_PULL);
    assert (zproxy_backend_type (proxy) == ZMQ_PUSH);
    assert (zproxy_capture_type (proxy) == ZMQ_PUB);
    assert (zproxy_control_type (proxy) == ZMQ_SUB);
    
    char *front_check = zproxy_frontend_addr (proxy);
    assert (streq (front_check, front_addr));
  
    char *back_check = zproxy_backend_addr (proxy);
    assert (streq (back_check, back_addr));

    char *capture_check = zproxy_capture_addr (proxy);
    assert (streq (capture_check, capture_addr));
   
    char *control_check = zproxy_control_addr (proxy);
    assert (streq (control_check, control_addr));

    //  Connect to the proxy front, back, and capture ports
    void *front_s = zsocket_new (ctx, ZMQ_PUSH);
    assert (front_s);
    rc = zsocket_connect (front_s, zproxy_frontend_addr (proxy));

    void *back_s = zsocket_new (ctx, ZMQ_PULL);
    assert (back_s);
    rc = zsocket_connect (back_s, zproxy_backend_addr (proxy));

#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)
    void *capture_s = zsocket_new (ctx, ZMQ_SUB);
    zsocket_set_subscribe (capture_s, "");
    rc = zsocket_connect (capture_s, zproxy_capture_addr (proxy));
#endif

    //  Send a message through the proxy and receive it
    zstr_send (front_s, "STREAMER_TEST");
    char *back_resp = zstr_recv (back_s);
    assert (back_resp);
    assert (streq ("STREAMER_TEST", back_resp));
    free (back_resp);
   
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)
    char *capture_resp = zstr_recv (capture_s);
    assert (capture_resp);
    assert (streq ("STREAMER_TEST", capture_resp));
    free (capture_resp);
#endif

    rc = zproxy_pause (proxy);
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    assert (rc == 0);
#else
    assert (rc == -1);
#endif

    rc = zproxy_resume (proxy);
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    assert (rc == 0);
#else
    assert (rc == -1);
#endif

    
    rc = zproxy_terminate (proxy);
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    assert (rc == 0);
#else
    assert (rc == -1);
#endif

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

    free (zstr_recv (pipe));
    zstr_send (pipe, "OK");

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

#if (ZMQ_VERSION <= ZPROXY_HAS_PROXY)
    if (zproxy_type (self) == ZPROXY_STREAMER) {
        zmq_device (ZMQ_STREAMER, self->frontend, self->backend);
    }
    else if (zproxy_type (self) == ZPROXY_QUEUE) {
        zmq_device (ZMQ_QUEUE, self->frontend, self->backend);
    }
    else if (zproxy_type (self)== ZPROXY_FORWARDER) {
        zmq_device (ZMQ_FORWARDER, self->frontend, self->backend);
    }
    else {
        assert (1==0);
    }
#else
    self->capture = zsocket_new (ctx, zproxy_capture_type (self));
    if (self->capture)
        zsocket_bind (self->capture, zproxy_capture_addr (self));
    else
        return;
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    void *control = zsocket_new (ctx, zproxy_control_type (self));
    if (control) {
        zsocket_set_subscribe (control, "");
        zsocket_connect (control, zproxy_control_addr (self));
    }
    else {
        return;
    }
    zmq_proxy_steerable (self->frontend, self->backend, self->capture, control);
    zstr_send (pipe, "TERMINATED");
#else
    zmq_proxy (self->frontend, self->backend, self->capture);
#endif
#endif
}
