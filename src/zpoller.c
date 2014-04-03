/*  =========================================================================
    zpoller - trivial socket poller class

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================*/

/*
@header
    The zpoller class provides a minimalist interface to ZeroMQ's zmq_poll
    API, for the very common case of reading from a number of sockets.
    It does not provide polling for output, nor polling on file handles.
    If you need either of these, use the zmq_poll API directly.
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zpoller_t {
    zlist_t *readers;           //  List of sockets to read from
    zmq_pollitem_t *poll_set;   //  Current zmq_poll set
    size_t poll_size;           //  Size of poll set
    bool need_rebuild;          //  Does pollset needs rebuilding?
    bool expired;               //  Did poll timer expire?
    bool terminated;            //  Did poll call end with EINTR?
};

static int s_rebuild_poll_set (zpoller_t *self);


//  --------------------------------------------------------------------------
//  Constructor
//  Create new poller from a list of readers (end in NULL)

zpoller_t *
zpoller_new (void *reader, ...)
{
    zpoller_t *self = (zpoller_t *) zmalloc (sizeof (zpoller_t));
    assert (self);
    self->readers = zlist_new ();
    self->need_rebuild = true;

    va_list args;
    va_start (args, reader);
    while (reader) {
        zlist_append (self->readers, reader);
        reader = va_arg (args, void *);
    }
    va_end (args);
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a poller

void
zpoller_destroy (zpoller_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zpoller_t *self = *self_p;
        zlist_destroy (&self->readers);
        free (self->poll_set);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Add a reader to be polled

int
zpoller_add (zpoller_t *self, void *reader)
{
    assert (self);
    assert (reader);
    int rc = zlist_append (self->readers, reader);
    if (rc != -1)
        self->need_rebuild = true;
    return rc;
}


//  --------------------------------------------------------------------------
//  Poll the registered readers for I/O, return first socket that has input.
//  This means the order that sockets are defined in the poll list affects
//  their priority. If you need a balanced poll, use the low level zmq_poll
//  method directly. If the poll call was interrupted (SIGINT), or the ZMQ
//  context was destroyed, or the timeout expired, returns NULL. You can
//  test the actual exit condition by calling zpoller_expired () and 
//  zpoller_terminated (). Timeout is in msec.

void *
zpoller_wait (zpoller_t *self, int timeout)
{
    self->expired = false;
    self->terminated = false;
    if (self->need_rebuild)
        s_rebuild_poll_set (self);
        
    int rc = zmq_poll (self->poll_set, (int) self->poll_size,
                       timeout * ZMQ_POLL_MSEC);
    if (rc > 0) {
        uint reader = 0;
        for (reader = 0; reader < self->poll_size; reader++)
            if (self->poll_set [reader].revents & ZMQ_POLLIN)
                return self->poll_set [reader].socket;
    }
    else
    if (rc == 0)
        self->expired = true;
    else
    if (rc == -1 || zctx_interrupted)
        self->terminated = true;

    return NULL;
}


static int
s_rebuild_poll_set (zpoller_t *self)
{
    free (self->poll_set);
    self->poll_set = NULL;

    self->poll_size = zlist_size (self->readers);
    self->poll_set = (zmq_pollitem_t *)
        zmalloc (self->poll_size * sizeof (zmq_pollitem_t));
    if (!self->poll_set)
        return -1;

    uint reader_nbr = 0;
    void *reader = zlist_first (self->readers);
    while (reader) {
        self->poll_set [reader_nbr].socket = reader;
        self->poll_set [reader_nbr].events = ZMQ_POLLIN;
        reader_nbr++;
        reader = zlist_next (self->readers);
    }
    self->need_rebuild = false;
    return 0;
}


//  --------------------------------------------------------------------------
//  Return true if the last zpoller_wait () call ended because the timeout
//  expired, without any error.

bool
zpoller_expired (zpoller_t *self)
{
    assert (self);
    return self->expired;
}


//  --------------------------------------------------------------------------
//  Return true if the last zpoller_wait () call ended because the process
//  was interrupted, or the parent context was destroyed.

bool
zpoller_terminated (zpoller_t *self)
{
    assert (self);
    return self->terminated;
}


//  --------------------------------------------------------------------------
//  Self test of this class

int
zpoller_test (bool verbose)
{
    printf (" * zpoller: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();

    //  Create a few sockets
    void *vent = zsocket_new (ctx, ZMQ_PUSH);
    int rc = zsocket_bind (vent, "tcp://*:9000");
    assert (rc != -1);
    void *sink = zsocket_new (ctx, ZMQ_PULL);
    rc = zsocket_connect (sink, "tcp://localhost:9000");
    assert (rc != -1);
    void *bowl = zsocket_new (ctx, ZMQ_PULL);
    void *dish = zsocket_new (ctx, ZMQ_PULL);

    //  Set-up poller
    zpoller_t *poller = zpoller_new (bowl, dish, NULL);
    assert (poller);

    // Add a reader the existing poller
    rc = zpoller_add (poller, sink);
    assert (rc != -1);

    zstr_send (vent, "Hello, World");

    //  We expect a message only on the sink
    void *which = zpoller_wait (poller, -1);
    assert (which == sink);
    assert (zpoller_expired (poller) == false);
    assert (zpoller_terminated (poller) == false);
    char *message = zstr_recv (which);
    assert (streq (message, "Hello, World"));
    zstr_free (&message);

    //  Destroy poller and context
    zpoller_destroy (&poller);
    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
