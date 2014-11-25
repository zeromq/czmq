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
    zlistx_t *reader_list;      //  List of sockets to read from
    zmq_pollitem_t *poll_set;   //  Current zmq_poll set
    void **poll_readers;        //  Matching table of socket readers
    size_t poll_size;           //  Size of poll set
    bool need_rebuild;          //  Does pollset needs rebuilding?
    bool expired;               //  Did poll timer expire?
    bool terminated;            //  Did poll call end with EINTR?
};

static int s_rebuild_poll_set (zpoller_t *self);


//  --------------------------------------------------------------------------
//  Constructor
//  Create new poller; the reader can be a libzmq socket (void *), a zsock_t
//  instance, or a zactor_t instance.

zpoller_t *
zpoller_new (void *reader, ...)
{
    zpoller_t *self = (zpoller_t *) zmalloc (sizeof (zpoller_t));
    if (self) {
        self->reader_list = zlistx_new ();
        if (self->reader_list) {
            self->need_rebuild = true;

            va_list args;
            va_start (args, reader);
            while (reader) {
                if (zlistx_add_end (self->reader_list, reader) == NULL) {
                    zpoller_destroy (&self);
                    break;
                }
                reader = va_arg (args, void *);
            }
            va_end (args);
        }
        else
            zpoller_destroy (&self);
    }
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
        zlistx_destroy (&self->reader_list);
        free (self->poll_readers);
        free (self->poll_set);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Add a reader to be polled. Returns 0 if OK, -1 on failure. The reader may
//  be a libzmq void * socket, a zsock_t instance, or a zactor_t instance.

int
zpoller_add (zpoller_t *self, void *reader)
{
    assert (self);
    assert (reader);
    int rc = zlistx_add_end (self->reader_list, reader)? 0: -1;
    self->need_rebuild = true;
    return rc;
}


//  --------------------------------------------------------------------------
//  Remove a reader from the poller; returns 0 if OK, -1 on failure. The
//  reader may be a libzmq void * socket, a zsock_t instance, or a zactor_t
//  instance.

int
zpoller_remove (zpoller_t *self, void *reader)
{
    assert (self);
    assert (reader);
    void *handle = zlistx_find (self->reader_list, reader);
    if (handle)
        zlistx_delete (self->reader_list, handle);
    self->need_rebuild = true;
    return 0;
}


//  --------------------------------------------------------------------------
//  Poll the registered readers for I/O, return first reader that has input.
//  The reader will be a libzmq void * socket, or a zsock_t or zactor_t
//  instance as specified in zpoller_new/zpoller_add. The order that
//  sockets are defined in the poll list affects their priority. If you
//  need a balanced poll, use the low level zmq_poll method directly. If
//  the poll call was interrupted (SIGINT), or the ZMQ context was
//  destroyed, or the timeout expired, returns NULL. You can test the
//  actual exit condition by calling zpoller_expired () and
//  zpoller_terminated (). The timeout is in msec.

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
                return self->poll_readers [reader];
    }
    else
    if (rc == 0)
        self->expired = true;
    else
    if (rc == -1 || zsys_interrupted)
        self->terminated = true;

    return NULL;
}


static int
s_rebuild_poll_set (zpoller_t *self)
{
    free (self->poll_set);
    self->poll_set = NULL;
    free (self->poll_readers);
    self->poll_readers = NULL;

    self->poll_size = zlistx_size (self->reader_list);
    self->poll_set = (zmq_pollitem_t *)
                     zmalloc (self->poll_size * sizeof (zmq_pollitem_t));
    self->poll_readers = (void **) zmalloc (self->poll_size * sizeof (void *));
    if (!self->poll_set || !self->poll_readers)
        return -1;

    uint reader_nbr = 0;
    void *reader = zlistx_first (self->reader_list);
    while (reader) {
        self->poll_readers [reader_nbr] = reader;
        void *socket = zsock_resolve (reader);
        if (socket == NULL) {
            self->poll_set [reader_nbr].socket = NULL;
#ifdef _WIN32
            self->poll_set [reader_nbr].fd = *(SOCKET *) reader;
#else
            self->poll_set [reader_nbr].fd = *(int *) reader;
#endif
        }
        else
            self->poll_set [reader_nbr].socket = socket;
        self->poll_set [reader_nbr].events = ZMQ_POLLIN;

        reader_nbr++;
        reader = zlistx_next (self->reader_list);
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

void
zpoller_test (bool verbose)
{
    printf (" * zpoller: ");

    //  @selftest
    //  Create a few sockets
    zsock_t *vent = zsock_new (ZMQ_PUSH);
    assert (vent);
    int port_nbr = zsock_bind (vent, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    zsock_t *sink = zsock_new (ZMQ_PULL);
    assert (sink);
    int rc = zsock_connect (sink, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc != -1);
    zsock_t *bowl = zsock_new (ZMQ_PULL);
    assert (bowl);
    zsock_t *dish = zsock_new (ZMQ_PULL);
    assert (dish);

    //  Set-up poller
    zpoller_t *poller = zpoller_new (bowl, dish, NULL);
    assert (poller);

    // Add a reader to the existing poller
    rc = zpoller_add (poller, sink);
    assert (rc == 0);

    zstr_send (vent, "Hello, World");

    //  We expect a message only on the sink
    zsock_t *which = (zsock_t *) zpoller_wait (poller, -1);
    assert (which == sink);
    assert (zpoller_expired (poller) == false);
    assert (zpoller_terminated (poller) == false);
    char *message = zstr_recv (which);
    assert (streq (message, "Hello, World"));
    zstr_free (&message);

    //  Stop polling reader
    rc = zpoller_remove (poller, sink);
    assert (rc == 0);

    //  Check we can poll an FD
    rc = zsock_connect (bowl, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc != -1);
    int fd = zsock_fd (bowl);
    rc = zpoller_add (poller, (void *) &fd);
    assert (rc != -1);
    zstr_send (vent, "Hello again, world");
    assert (zpoller_wait (poller, 500) == &fd);

    //  Destroy poller and sockets
    zpoller_destroy (&poller);

    zsock_destroy (&vent);
    zsock_destroy (&sink);
    zsock_destroy (&bowl);
    zsock_destroy (&dish);
    //  @end

    printf ("OK\n");
}
