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
    The class implements the poller using the zmq_poller API if that exists,
    else does the work itself.
@end
*/

#include "czmq_classes.h"

//  Structure of our class

struct _zpoller_t {
#ifdef ZMQ_HAVE_POLLER
    void *zmq_poller;           //  ZMQ poller structure
#else
    zlist_t *reader_list;       //  List of sockets to read from
    zmq_pollitem_t *poll_set;   //  Current zmq_poll set
    void **poll_readers;        //  Matching table of socket readers
    size_t poll_size;           //  Size of poll set
    bool need_rebuild;          //  Does pollset need rebuilding?
#endif
    bool expired;               //  Did poll timer expire?
    bool terminated;            //  Did poll call end with EINTR?
    bool nonstop;               //  Don't stop running on Ctrl-C
};


#ifndef ZMQ_HAVE_POLLER
static int
s_rebuild_poll_set (zpoller_t *self)
{
    free (self->poll_set);
    self->poll_set = NULL;
    free (self->poll_readers);
    self->poll_readers = NULL;

    self->poll_size = zlist_size (self->reader_list);
    self->poll_set = (zmq_pollitem_t *)
                      zmalloc (self->poll_size * sizeof (zmq_pollitem_t));
    self->poll_readers = (void **) zmalloc (self->poll_size * sizeof (void *));
    if (!self->poll_set || !self->poll_readers)
        return -1;

    uint reader_nbr = 0;
    void *reader = zlist_first (self->reader_list);
    while (reader) {
        self->poll_readers [reader_nbr] = reader;
        void *socket = zsock_resolve (reader);
        if (socket == NULL) {
            self->poll_set [reader_nbr].socket = NULL;
            self->poll_set [reader_nbr].fd = *(SOCKET *) reader;
        }
        else
            self->poll_set [reader_nbr].socket = socket;
        self->poll_set [reader_nbr].events = ZMQ_POLLIN;

        reader_nbr++;
        reader = zlist_next (self->reader_list);
    }
    self->need_rebuild = false;
    return 0;
}
#endif


//  --------------------------------------------------------------------------
//  Create new poller, specifying zero or more readers. The list of
//  readers ends in a NULL. Each reader can be a zsock_t instance, a
//  zactor_t instance, a libzmq socket (void *), or a file handle.

zpoller_t *
zpoller_new (void *reader, ...)
{
    zpoller_t *self = (zpoller_t *) zmalloc (sizeof (zpoller_t));
    assert (self);
#ifdef ZMQ_HAVE_POLLER
    self->zmq_poller = zmq_poller_new ();
    assert (self->zmq_poller);
#else
    self->reader_list = zlist_new ();
    assert (self->reader_list);
#endif
    va_list args;
    va_start (args, reader);
    while (reader) {
        if (zpoller_add (self, reader)) {
            zpoller_destroy (&self);
            break;
        }
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
#ifdef ZMQ_HAVE_POLLER
        zmq_poller_destroy (&self->zmq_poller);
#else
        zlist_destroy (&self->reader_list);
        free (self->poll_readers);
        free (self->poll_set);
#endif
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
    int rc = 0;
#ifdef ZMQ_HAVE_POLLER
    void *socket = zsock_resolve (reader);
    if (socket)
        rc = zmq_poller_add (self->zmq_poller, socket, reader, ZMQ_POLLIN);
    else
        rc = zmq_poller_add_fd (self->zmq_poller, *(SOCKET *) reader, reader, ZMQ_POLLIN);
#else
    zlist_append (self->reader_list, reader);
    self->need_rebuild = true;
#endif
    return rc;
}


//  --------------------------------------------------------------------------
//  Remove a reader from the poller; returns 0 if OK, -1 on failure. The reader
//  must have been passed during construction, or in an zpoller_add () call.

int
zpoller_remove (zpoller_t *self, void *reader)
{
    assert (self);
    assert (reader);
    int rc = 0;
#ifdef ZMQ_HAVE_POLLER
    void *socket = zsock_resolve (reader);
    if (socket)
        rc = zmq_poller_remove (self->zmq_poller, socket);
    else
        rc = zmq_poller_remove_fd (self->zmq_poller, *(SOCKET *) reader);
#else
    zlist_remove (self->reader_list, reader);
    self->need_rebuild = true;
#endif
    return rc;
}


//  --------------------------------------------------------------------------
//  By default the poller stops if the process receives a SIGINT or SIGTERM
//  signal. This makes it impossible to shut-down message based architectures
//  like zactors. This method lets you switch off break handling. The default
//  nonstop setting is off (false).

void
zpoller_set_nonstop (zpoller_t *self, bool nonstop)
{
    assert (self);
    self->nonstop = nonstop;
}


//  --------------------------------------------------------------------------
//  Poll the registered readers for I/O, return first reader that has input.
//  The reader will be a libzmq void * socket, or a zsock_t or zactor_t
//  instance as specified in zpoller_new/zpoller_add. The timeout should be
//  zero or greater, or -1 to wait indefinitely. Socket priority is defined
//  by their order in the poll list. If you need a balanced poll, use the low
//  level zmq_poll method directly. If the poll call was interrupted (SIGINT),
//  or the ZMQ context was destroyed, or the timeout expired, returns NULL.
//  You can test the actual exit condition by calling zpoller_expired () and
//  zpoller_terminated (). The timeout is in msec.

void *
zpoller_wait (zpoller_t *self, int timeout)
{
    assert (self);
    self->expired = false;
    if (zsys_interrupted && !self->nonstop) {
        self->terminated = true;
        return NULL;
    }
    else
        self->terminated = false;

#ifdef ZMQ_HAVE_POLLER
    zmq_poller_event_t event;
    if (!zmq_poller_wait (self->zmq_poller, &event, timeout * ZMQ_POLL_MSEC))
        return event.user_data;
    else
    if (errno == ETIMEDOUT)
        self->expired = true;
    else
    if (zsys_interrupted && !self->nonstop)
        self->terminated = true;

    return NULL;
#else
    if (self->need_rebuild)
        s_rebuild_poll_set (self);
    int rc = zmq_poll (self->poll_set, (int) self->poll_size, timeout * ZMQ_POLL_MSEC);
    if (rc > 0) {
        uint reader = 0;
        for (reader = 0; reader < self->poll_size; reader++)
            if (self->poll_set [reader].revents & ZMQ_POLLIN)
                return self->poll_readers [reader];
    }
    else
    if (rc == -1 || (zsys_interrupted && !self->nonstop))
        self->terminated = true;
    else
    if (rc == 0)
        self->expired = true;

    return NULL;
#endif
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

    //  Set up poller
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
    SOCKET fd = zsock_fd (bowl);
    rc = zpoller_add (poller, (void *) &fd);
    assert (rc != -1);
    zstr_send (vent, "Hello again, world");
    assert (zpoller_wait (poller, 500) == &fd);

    // Check zpoller_set_nonstop ()
    zsys_interrupted = 1;
    zpoller_wait (poller, 0);
    assert (zpoller_terminated (poller));
    zpoller_set_nonstop (poller, true);
    zpoller_wait (poller, 0);
    assert (!zpoller_terminated (poller));
    zsys_interrupted = 0;

    zpoller_destroy (&poller);
    zsock_destroy (&vent);
    zsock_destroy (&sink);
    zsock_destroy (&bowl);
    zsock_destroy (&dish);

#ifdef ZMQ_SERVER
    //  Check thread safe sockets
    zpoller_destroy (&poller);
    zsock_t *client = zsock_new (ZMQ_CLIENT);
    assert (client);
    zsock_t *server = zsock_new (ZMQ_SERVER);
    assert (server);
    poller = zpoller_new (client, server, NULL);
    assert (poller);
    port_nbr = zsock_bind (server, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    rc = zsock_connect (client, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc != -1);

    zstr_send (client, "Hello, World");

    //  We expect a message only on the server
    which = (zsock_t *) zpoller_wait (poller, -1);
    assert (which == server);
    assert (zpoller_expired (poller) == false);
    assert (zpoller_terminated (poller) == false);
    message = zstr_recv (which);
    assert (streq (message, "Hello, World"));
    zstr_free (&message);

    zpoller_destroy (&poller);
    zsock_destroy (&client);
    zsock_destroy (&server);
#endif
    //  @end

    printf ("OK\n");
}
