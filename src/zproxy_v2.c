/*  =========================================================================
    zproxy_v2 - run a steerable proxy in the background (deprecated)

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
    The zproxy class provides an equivalent to the ZMQ steerable proxy, on
    all versions of ZeroMQ.
@discuss
    This class is deprecated in CZMQ v3; it works together with zctx, zsocket,
    and other deprecated V2 classes. New applications should use the V3 zproxy
    interface, based on zactor, together with the zsock class for sockets.
@end
*/

#include "platform.h"
#include "../include/czmq.h"

//  The proxy runs in a background thread
static void
s_proxy_task (void *args, zctx_t *ctx, void *pipe);

//  Structure of our class; note that this structure is shared between
//  the API thread and the proxy task.

struct _zproxy_t {
    zctx_t *ctx;                //  Context, allows inproc capture
    void *pipe;                 //  Pipe used by API thread
    void *frontend;             //  Frontend socket for proxy switching
    void *backend;              //  Backend socket for proxy switching
};


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
    zproxy_t *self = (zproxy_t *) zmalloc (sizeof (zproxy_t));
    if (self) {
        self->ctx = ctx;
        self->frontend = frontend;
        self->backend = backend;
        self->pipe = zthread_fork (ctx, s_proxy_task, self);
        if (self->pipe)
            zsocket_wait (self->pipe);
        else {
            //  If we ran out of sockets, signal failure to caller
            zproxy_destroy (&self);
        }
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a zproxy object; note this first stops the proxy.

void
zproxy_destroy (zproxy_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zproxy_t *self = *self_p;
        if (self->pipe) {
            zstr_send (self->pipe, "STOP");
            zsocket_wait (self->pipe);
        }
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Copy all proxied messages to specified endpoint; if this is NULL, any
//  in-progress capturing will be stopped. You must already have bound the
//  endpoint to a PULL socket.

void
zproxy_capture (zproxy_t *self, const char *endpoint)
{
    assert (self);
    zstr_sendm (self->pipe, "CAPTURE");
    zstr_send (self->pipe, endpoint);
    zsocket_wait (self->pipe);
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
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Resume a zproxy object

void
zproxy_resume (zproxy_t *self)
{
    assert (self);
    zstr_send (self->pipe, "RESUME");
    zsocket_wait (self->pipe);
}


//  --------------------------------------------------------------------------
//  Backend proxy task

static void
s_proxy_task (void *args, zctx_t *ctx, void *command_pipe)
{
    //  Confirm to API that we've started up
    zsocket_signal (command_pipe);
    zproxy_t *self = (zproxy_t *) args;
    //  Capture socket, if not NULL, receives all data
    void *capture = NULL;

    //  Create poller to work on all three sockets
    zpoller_t *poller = zpoller_new (self->frontend, self->backend, command_pipe, NULL);
    assert (poller);

    bool stopped = false;
    while (!stopped) {
        //  Wait for activity on any polled socket, and read incoming message
        void *which = zpoller_wait (poller, -1);
        zmq_msg_t msg;
        zmq_msg_init (&msg);
        int send_flags;         //  Flags for outgoing message

        if (which && zmq_recvmsg (which, &msg, 0) != -1) {
            send_flags = zsocket_rcvmore (which) ? ZMQ_SNDMORE : 0;
            if (which == self->frontend || which == self->backend) {
                void *output = which == self->frontend ?
                               self->backend : self->frontend;

                //  Loop on all waiting messages, since polling adds a
                //  non-trivial cost per message, especially on OS/X
                while (true) {
                    if (capture) {
                        zmq_msg_t dup;
                        zmq_msg_init (&dup);
                        zmq_msg_copy (&dup, &msg);
                        if (zmq_sendmsg (capture, &dup, send_flags) == -1)
                            zmq_msg_close (&dup);
                    }
                    if (zmq_sendmsg (output, &msg, send_flags) == -1) {
                        zmq_msg_close (&msg);
                        break;
                    }
                    if (zmq_recvmsg (which, &msg, ZMQ_DONTWAIT) == -1)
                        break;      //  Presumably EAGAIN
                    send_flags = zsocket_rcvmore (which) ? ZMQ_SNDMORE : 0;
                }
            }
            else
            if (which == command_pipe) {
                char command [10] = { 0 };
                assert (zmq_msg_size (&msg) < 10);
                memcpy (command, zmq_msg_data (&msg), zmq_msg_size (&msg));

                //  Execute API command
                if (streq (command, "PAUSE")) {
                    zpoller_destroy (&poller);
                    poller = zpoller_new (command_pipe, NULL);
                    assert (poller);
                }
                else
                if (streq (command, "RESUME")) {
                    zpoller_destroy (&poller);
                    poller = zpoller_new (self->frontend, self->backend, command_pipe, NULL);
                    assert (poller);
                }
                else
                if (streq (command, "CAPTURE")) {
                    //  Capture flow is always PUSH-to-PULL
                    capture = zsocket_new (self->ctx, ZMQ_PUSH);
                    char *endpoint = zstr_recv (command_pipe);
                    assert (endpoint);
                    if (capture) {
                        int rc = zsocket_connect (capture, "%s", endpoint);
                        assert (rc == 0);
                    }
                    zstr_free (&endpoint);
                }
                else
                if (streq (command, "STOP"))
                    stopped = true;
                else
                    assert (0); //  Cannot happen, so die

                //  Signal to caller that we processed the command
                zsocket_signal (command_pipe);
            }
            else
                assert (0);     //  Cannot happen, so die
        }
        else
            break;              //  Interrupted
    }
    zpoller_destroy (&poller);
}


// --------------------------------------------------------------------------
//  Selftest

void
zproxy_v2_test (bool verbose)
{
    printf (" * zproxy (deprecated): ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *frontend = zsocket_new (ctx, ZMQ_PULL);
    assert (frontend);
    int rc = zsocket_bind (frontend, "inproc://frontend");
    assert (rc == 0);
    void *backend = zsocket_new (ctx, ZMQ_PUSH);
    assert (backend);
    rc = zsocket_bind (backend, "inproc://backend");
    assert (rc == 0);
    zproxy_t *proxy = zproxy_new (ctx, frontend, backend);
    assert (proxy);

    //  Connect application sockets to proxy
    void *faucet = zsocket_new (ctx, ZMQ_PUSH);
    assert (faucet);
    rc = zsocket_connect (faucet, "inproc://frontend");
    assert (rc == 0);
    void *sink = zsocket_new (ctx, ZMQ_PULL);
    assert (sink);
    rc = zsocket_connect (sink, "inproc://backend");
    assert (rc == 0);

    //  Send some messages and check they arrived
    zstr_send (faucet, "Hello");
    char *string = zstr_recv (sink);
    assert (streq (string, "Hello"));
    zstr_free (&string);

    //  Check pause/resume functionality
    zproxy_pause (proxy);
    zstr_send (faucet, "World");

    zproxy_resume (proxy);
    string = zstr_recv (sink);
    assert (streq (string, "World"));
    zstr_free (&string);

    //  Create capture socket, must be a PULL socket
    void *capture = zsocket_new (ctx, ZMQ_PULL);
    assert (capture);
    rc = zsocket_bind (capture, "inproc://capture");
    assert (rc == 0);

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
    zctx_destroy (&ctx);

    //  @end
    printf ("OK\n");
}
