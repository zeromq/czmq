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
    I've modified zproxy to "do the right thing" depending
    on both the underlying version of libzmq and arguments passed 
    to zproxy_bind.  While this introduces quite a 
    bit of complexity in this czmq class, the hope is that it
    reduces a lot of complexity on behalf of the actual czmq user.

    Note that because the zproxy_bind function signature changes
    based on version of libzmq compiled against, swapping out the
    underlying libzmq for an older version than compiled against
    will cause code to fail.  This is intended behavior.

    I have tested swapping out the underlying libzmq for newer
    versions than compiled against, and this works.

    The behavior for various cases is as follows:

    ------------------------------------------
    ZMQ_VERSION >= ZPROXY_HAS_STEERABLE 
    
    int
    zproxy_bind (zproxy_t *self, char *frontend_addr,
            char *backend_addr, char *capture_addr,
            char *control_addr);

    capture? | control?  | should call
    YES      | YES       | zmq_proxy_steerable
    NO       | YES       | zmq_proxy_steerable
    YES      | NO        | zmq_proxy
    NO       | NO        | zmq_proxy
   
    ------------------------------------------
    ZMQ_VERSION >= ZPROXY_HAS_PROXY &&
    ZMQ_VERSION < ZPROXY_HAS_STEERABLE

    int
    zproxy_bind (zproxy_t *self, char *frontend_addr,
            char *backend_addr, char *capture_addr);

    capture? | should call
    YES      | zmq_proxy
    NO       | zmq_proxy

    ------------------------------------------
    ZMQ_VERSION < ZPROXY_HAS_PROXY

    int
    zproxy_bind (zproxy_t *self, char *frontend_addr,
            char *backend_addr);

    should call
    zmq_proxy
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zproxy_t {
    zctx_t *ctx;
    void *pipe;
    int type;
    char *frontend_identity;
    char *frontend_addr;
    int frontend_type;
    void *frontend;
    char *backend_identity;
    char *backend_addr;
    int backend_type;
    void *backend;
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)
    char *capture_addr;
    int capture_type;
    void *capture;
#endif
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    char *control_addr;
    int control_type;
    void *control;
#endif
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
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)
    self->capture_type = ZMQ_PUB;
#endif
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    self->control_type = ZMQ_SUB;
#endif

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
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)

//  --------------------------------------------------------------------------
//  Command; Sends a pause command to the proxy

void
zproxy_pause (zproxy_t *self)
{
    assert (self);
    zstr_send (self->control, "PAUSE");

}

//  --------------------------------------------------------------------------
//  Command; Sends a resume command to the proxy

void
zproxy_resume (zproxy_t *self)
{
    assert (self);
    zstr_send (self->control, "RESUME");
}

//  --------------------------------------------------------------------------
//  Command; Sends a terminate command to the proxy

void
zproxy_terminate (zproxy_t *self)
{
    assert (self);
    zpoller_t *poller = zpoller_new (self->pipe, NULL);
    assert (poller);
    zstr_send (self->control, "TERMINATE");
    void *pipe_sock = zpoller_wait (poller, 1000);
    char *resp = zstr_recv_nowait (pipe_sock);
    assert (streq (resp, "TERMINATED"));
    free (resp);
}
#endif 

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
    char *frontend_addr,
    char *backend_addr
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)
    ,char *capture_addr
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    ,char *control_addr
#endif
#endif
)
{
    self->frontend_addr = frontend_addr? strdup(frontend_addr): NULL;
    self->backend_addr = backend_addr? strdup(backend_addr): NULL;
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)
    self->capture_addr = capture_addr? strdup(capture_addr): NULL;
#endif
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    self->control_addr = control_addr? strdup(control_addr): NULL;
    if (self->control_addr) {
        self->control = zsocket_new (self->ctx, ZMQ_PUB);
        zsocket_bind (self->control, self->control_addr);
    }
#endif

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

void
zproxy_set_frontend_identity (zproxy_t *self, char *frontend_identity)
{
    self->frontend_identity = frontend_identity? strdup(frontend_identity): NULL;
}

void
zproxy_set_backend_identity (zproxy_t *self, char *backend_identity)
{
    self->backend_identity = backend_identity? strdup(backend_identity): NULL;
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
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)

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
#endif
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)

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
#endif

// --------------------------------------------------------------------------
//  Selftest

// Forward declaration of test runner.  This lets us test zproxy with a 
// matrix of different input combinations without repeating a lot
// of code.   
static void
s_zproxy_test_run (zproxy_t *proxy, char *frontend_addr,
        char *backend_addr, char *capture_addr, char *control_addr);

int
zproxy_test (bool verbose)
{
    printf (" * zproxy: ");
    
    //  @selftest
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)

    char *frontend_addr = "inproc://proxy_front";
    char *backend_addr = "inproc://proxy_back";
    char *capture_addr = "inproc://proxy_capture";
    char *control_addr = "inproc://proxy_control";

    // Defining both a capture address and a control address
    // should create a zmq_proxy_steerable proxy that we
    // can control with a control port and capture data from
    // using the capture port.
    zctx_t *ctx = zctx_new ();
    zproxy_t *proxy = zproxy_new (ctx, ZPROXY_STREAMER);

    int rc = zproxy_bind (proxy, frontend_addr, backend_addr, 
            capture_addr, control_addr);

    assert (rc == 0);

    // Run the actual test
    s_zproxy_test_run (proxy, frontend_addr, backend_addr,
            capture_addr, control_addr);

    // Destroy the proxy
    zctx_destroy (&ctx);
    zproxy_destroy (&proxy);

    // Defining a control address and no capture address
    // should create a zmq_proxy_steerable proxy that we
    // can control with a control port with no capture
    // port.
    ctx = zctx_new ();
    proxy = zproxy_new (ctx, ZPROXY_STREAMER);

    rc = zproxy_bind (proxy, frontend_addr, backend_addr,
            NULL, control_addr);
    
    assert (rc == 0);

    // Run the actual test
    s_zproxy_test_run (proxy, frontend_addr, backend_addr,
            capture_addr, control_addr);

    // Destroy the proxy
    zctx_destroy (&ctx);
    zproxy_destroy (&proxy);

    // Defining a capture address and no control address
    // should create a zmq_proxy proxy that we
    // can capture data from
    ctx = zctx_new ();
    proxy = zproxy_new (ctx, ZPROXY_STREAMER);

    rc = zproxy_bind (proxy, frontend_addr, backend_addr, 
            capture_addr, NULL);
    
    assert (rc == 0);

    // Run the actual test
    s_zproxy_test_run (proxy, frontend_addr, backend_addr,
            capture_addr, control_addr);

    // Destroy the proxy
    zctx_destroy (&ctx);
    zproxy_destroy (&proxy);

    // Leaving both capture and control undefined
    // should create a zmq_proxy proxy that is
    // not steerable and has no capture port.
    ctx = zctx_new ();
    proxy = zproxy_new (ctx, ZPROXY_STREAMER);

    rc = zproxy_bind (proxy, frontend_addr, backend_addr, 
            NULL, NULL);

    assert (rc == 0);

    // Run the actual test
    s_zproxy_test_run (proxy, frontend_addr, backend_addr,
            capture_addr, control_addr);

    // Destroy the proxy
    zctx_destroy (&ctx);
    zproxy_destroy (&proxy);

#endif
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY && ZMQ_VERSION < ZPROXY_HAS_STEERABLE)

    char *frontend_addr = "inproc://proxy_front";
    char *backend_addr = "inproc://proxy_back";
    char *capture_addr = "inproc://proxy_capture";

    // Defining a capture address and no control address
    // should create a zmq_proxy proxy that we
    // can capture data from
    zctx_t *ctx = zctx_new ();
    zproxy_t *proxy = zproxy_new (ctx, ZPROXY_STREAMER);

    int rc = zproxy_bind (proxy, frontend_addr, backend_addr, 
            capture_addr);

    assert (rc == 0);

    // Run the actual test
    s_zproxy_test_run (proxy, frontend_addr, backend_addr,
            capture_addr, NULL);

    // Destroy the proxy
    zctx_destroy (&ctx);
    zproxy_destroy (&proxy);
#endif
#if (ZMQ_VERSION < ZPROXY_HAS_PROXY)

    char *frontend_addr = "inproc://proxy_front";
    char *backend_addr = "inproc://proxy_back";

    zctx_t *ctx = zctx_new ();
    zproxy_t *proxy = zproxy_new (ctx, ZPROXY_STREAMER);

    int rc = zproxy_bind (proxy, frontend_addr, backend_addr);

    assert (rc == 0);

    // Run the actual test
    s_zproxy_test_run (proxy, frontend_addr, backend_addr,
            NULL, NULL);

    // Destroy the proxy
    zctx_destroy (&ctx);
    zproxy_destroy (&proxy);
#endif

    // Keep valgrind happy
    zclock_sleep (100);

    //  @end
    printf ("OK\n");
    return 0;
}

static void
s_zproxy_test_run (zproxy_t *proxy, char *frontend_addr,
        char *backend_addr, char *capture_addr, char *control_addr)
{
    // Pause for 10ms 
    zclock_sleep (10);

    // Check proxy type
    assert (zproxy_type (proxy) == ZPROXY_STREAMER);

    // Check frontend type and address
    assert (zproxy_frontend_type (proxy) == ZMQ_PULL);
    assert (streq (zproxy_frontend_addr (proxy), frontend_addr));

    // Check backend type and address 
    assert (zproxy_backend_type (proxy) == ZMQ_PUSH);
    assert (streq (zproxy_backend_addr (proxy), backend_addr));
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY)

    // Check capture type and address    
    if (zproxy_capture_addr (proxy)) {
        assert (zproxy_capture_type (proxy) == ZMQ_PUB);
        assert (streq (zproxy_capture_addr (proxy), capture_addr));
    }
#endif
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE) 
    // Check control type and address 
    if (zproxy_control_addr (proxy)) {
        assert (zproxy_control_type (proxy) == ZMQ_SUB);
        assert (streq (zproxy_control_addr (proxy), control_addr));
    }
#endif 

    // Connect to front of proxy
    void *front_s = zsocket_new (proxy->ctx, ZMQ_PUSH);
    assert (front_s);
    zsocket_connect (front_s, zproxy_frontend_addr (proxy));

    // Connect to back of proxy
    void *back_s = zsocket_new (proxy->ctx, ZMQ_PULL);
    assert (back_s);
    zsocket_connect (back_s, zproxy_backend_addr (proxy));
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY) 

    // Create a capture socket in case we need it
    void *capture_s = zsocket_new (proxy->ctx, ZMQ_SUB);
    assert (capture_s);
    zsocket_set_subscribe (capture_s, "");

    // if proxy has a capture port connect to it
    if (zproxy_capture_addr (proxy))
        zsocket_connect (capture_s, zproxy_capture_addr (proxy));
#endif

    // Send a message through the proxy and receive it
    zstr_send (front_s, "STREAMER_TEST");
    char *back_resp = zstr_recv (back_s);
    assert (back_resp);
    assert (streq ("STREAMER_TEST", back_resp));
    free (back_resp);
#if (ZMQ_VERSION >= ZPROXY_HAS_PROXY) 

    // If the proxy has a capture address, make sure the
    // data sent through the proxy comes through the
    // capture socket.
    if (zproxy_capture_addr (proxy)) {
        char *capture_resp = zstr_recv (capture_s);
        assert (capture_resp);
        assert (streq ("STREAMER_TEST", capture_resp));
        free (capture_resp);
    }
#endif
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE) 

    // If the proxy has a control address, call
    // pause, resume, and destroy
    if (zproxy_control_addr (proxy)) {
        zproxy_pause (proxy);
        zproxy_resume (proxy);
        zproxy_terminate (proxy);
    }
#endif
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
    if (self->frontend) {
        if (self->frontend_identity)
            zsocket_set_identity (self->frontend, self->frontend_identity);

        zsocket_bind (self->frontend, zproxy_frontend_addr (self));
    }
    else {
        return;
    }

    self->backend = zsocket_new (ctx, zproxy_backend_type (self));
    if (self->backend) {
        if (self->backend_identity)
            zsocket_set_identity (self->backend, self->backend_identity);

        zsocket_bind (self->backend, zproxy_backend_addr (self));
    }
    else {
        return;
    }
#if (ZMQ_VERSION < ZPROXY_HAS_PROXY)

    switch (zproxy_type (self)) {
        case (ZPROXY_STREAMER):
            zmq_device (ZMQ_STREAMER, self->frontend, self->backend);
            break;
        case (ZPROXY_QUEUE):
            zmq_device (ZMQ_QUEUE, self->frontend, self->backend);
            break;
        case (ZPROXY_FORWARDER):
            zmq_device (ZMQ_FORWARDER, self->frontend, self->backend);
            break;
        default:
            assert (1==0);
    }
#else

    if (zproxy_capture_addr (self)) {
        self->capture = zsocket_new (ctx, zproxy_capture_type (self));
        if (self->capture)
            zsocket_bind (self->capture, zproxy_capture_addr (self));
        else
            return;
    }
#if (ZMQ_VERSION >= ZPROXY_HAS_STEERABLE)
    if (zproxy_control_addr (self)) {
        void *control = zsocket_new (ctx, zproxy_control_type (self));
        if (control) {
            zsocket_set_subscribe (control, "");
            zsocket_connect (control, zproxy_control_addr (self));
        }
        else 
            return;
        zmq_proxy_steerable (self->frontend, self->backend, self->capture, control);
        zstr_send (pipe, "TERMINATED");
    }
    else
        zmq_proxy (self->frontend, self->backend, self->capture);
#else
    zmq_proxy (self->frontend, self->backend, self->capture);
#endif
#endif
}
