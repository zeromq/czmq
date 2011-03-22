.set GIT=https://github.com/zeromq/zapi
.sub 0MQ=ØMQ

# zapi - High-level C binding for 0MQ

## Contents

.toc

## Overview

### Scope and Goals

zapi has these goals:

* To wrap the 0MQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of 0MQ.
* To provide a space for development of more sophisticated API semantics.

### Ownership and License

zapi is maintained by Pieter Hintjens and Mikko Koppanen (build system). Its other authors and contributors are listed in the AUTHORS file. It is held by the ZeroMQ organization at github.com.

The authors of zapi grant you use of this software under the terms of the GNU Lesser General Public License (LGPL). For details see the files `COPYING` and `COPYING.LESSER` in this directory.

### Contributing

To submit an issue use the [issue tracker](http://github.com/zeromq/zapi/issues). All discussion happens on the [zeromq-dev](zeromq-dev@lists.zeromq.org) list or #zeromq IRC channel at irc.freenode.net.

The proper way to submit patches is to clone this repository, make your changes, and use git to create a patch or a pull request. See http://www.zeromq.org/docs:contributing. All contributors are listed in AUTHORS.

All classes are maintained by a single person, who is the responsible editor for that class and who is named in the header as such. This is usually the originator of the class. When several people collaborate on a class, one single person is always the lead maintainer and the one to blame when it breaks.

The general rule is, if you contribute code to zapi you must be willing to maintain it as long as there are users of it. Code with no active maintainer will in general be deprecated and/or removed.

## Using zapi

### API Summary

#### zctx - working with 0MQ contexts

The zctx class wraps 0MQ contexts. It manages open sockets in the context and automatically closes these before terminating the context. It provides a simple way to set the linger timeout on sockets, and configure contexts for number of I/O threads. Sets-up signal (interrrupt) handling for the process.

    zctx_t *
        zctx = zctx_new (void);
    void
        zctx_destroy (zctx_t **self_p);
    void
        zctx_set_iothreads (zctx_t *self, int threads);
    void 
        zctx_set_linger (zctx_t *self, int msecs);
    void *
        zctx_context (zctx_t *self);
    void *
        zctx_socket_new (zctx_t *self, int type);
    void
        zctx_socket_destroy (zctx_t *self, void **socket);
    int
        zctx_test (int verbose);

#### zstr - sending and receiving strings

The zstr class provides utility functions for sending and receiving C strings across 0MQ sockets.

    char *
        zstr_recv (void *socket);
    int
        zstr_send (void *socket, const char *string);
    int
        zstr_sendf (void *socket, const char format, ...);

#### zframe - working with single message frames

The zframe class provides methods to send and receive single message frames across 0MQ sockets.

    zframe_t *
        zframe_new (const void *data, size_t size);
    void
        zframe_destroy (zframe_t **self_p);
    zframe_t *
        zframe_recv (void *socket, int flags);
    void
        zframe_send (zframe_t **self_p, void *socket, int flags);
    size_t
        zframe_size (zframe_t *self);
    void *
        zframe_data (zframe_t *self);
    int
        zframe_more (zframe_t *self);
    int
        zframe_test (int verbose);

#### zmsg - working with multipart messages

The zmsg class provides methods to send and receive multipart messages across 0MQ sockets.

    zmsg_t *
        zmsg_new (const void *data, size_t size);
    void
        zmsg_destroy (zmsg_t **self_p);
    zmsg_t *
        zmsg_recv (void *socket);
    void
        zmsg_send (zmsg_t **self, void *socket);
    size_t
        zmsg_size (zmsg_t *self);
    void
        zmsg_push (zmsg_t *self, const zframe_t *frame);
    void
        zmsg_queue (zmsg_t *self, const zframe_t *frame);
    void
        zmsg_pushmem (zmsg_t *self, const void *src, size_t size);
    void
        zmsg_queuemem (zmsg_t *self, const void *src, size_t size);
    zframe_t *
        zmsg_pop (zmsg_t *self);
    zframe_t *
        zmsg_remove (zmsg_t *self);
    zmsg_t *
        zmsg_load (FILE *file);
    void
        zmsg_save (zmsg_t **self_p, FILE *file);
    void
        zmsg_dump (zmsg_t *self);
    int
        zmsg_test (int verbose);

#### zloop - event-driven reactor

The zloop class provides an event-driven reactor pattern.

    zloop_t *
        zloop_new (void);
    void
        zloop_destroy (zloop_t **self_p);
    int
        zloop_register (zloop_t *self, void *socket, int flags, 
                        zloop_fn handler, void *argument);
    int
        zloop_alarm (zloop_t *self, size_t alarm_msecs,
                    zloop_fn handler, void *argument);
    int
        zloop_clock (zloop_t *self, size_t clock_msecs,
                    zloop_fn handler, void *argument);
    int
        zloop_start (zloop_t *self);
    int
        zloop_test (int verbose);

### Building and Installing

zapi uses autotools for packaging. To build from git (all example commands are for Linux):

    git clone git://github.com/zeromq/zapi.git
    cd zapi
    sh autogen.sh
    ./configure
    make all
    sudo make install
    sudo ldconfig

You will need the libtool and autotools packages. On FreeBSD, you may need to specify the default directories for configure:

    ./configure --with-zeromq=/usr/local

After building, you can run the zapi selftests:

    make check

### Linking with an Application

Include `zapi.h` in your application and link with libzapi. Here is a typical gcc link command:

    gcc -lzapi -lzmq myapp.c -o myapp

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
