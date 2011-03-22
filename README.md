
<A name="toc1-3" title="zapi - High-level C binding for ØMQ" />
# zapi - High-level C binding for ØMQ

<A name="toc2-6" title="Contents" />
## Contents


**<a href="#toc2-11">Overview</a>**
&emsp;<a href="#toc3-14">Scope and Goals</a>
&emsp;<a href="#toc3-23">Ownership and License</a>
&emsp;<a href="#toc3-30">Contributing</a>

**<a href="#toc2-41">Using zapi</a>**
&emsp;<a href="#toc3-44">API Summary</a>
&emsp;<a href="#toc4-47">zctx - working with ØMQ contexts</a>
&emsp;<a href="#toc4-69">zstr - sending and receiving strings</a>
&emsp;<a href="#toc4-81">zframe - working with single message frames</a>
&emsp;<a href="#toc4-103">zmsg - working with multipart messages</a>
&emsp;<a href="#toc4-139">zloop - event-driven reactor</a>
&emsp;<a href="#toc3-162">Building and Installing</a>
&emsp;<a href="#toc3-183">Linking with an Application</a>
&emsp;<a href="#toc3-190">This Document</a>

<A name="toc2-11" title="Overview" />
## Overview

<A name="toc3-14" title="Scope and Goals" />
### Scope and Goals

zapi has these goals:

* To wrap the ØMQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of ØMQ.
* To provide a space for development of more sophisticated API semantics.

<A name="toc3-23" title="Ownership and License" />
### Ownership and License

zapi is maintained by Pieter Hintjens and Mikko Koppanen (build system). Its other authors and contributors are listed in the AUTHORS file. It is held by the ZeroMQ organization at github.com.

The authors of zapi grant you use of this software under the terms of the GNU Lesser General Public License (LGPL). For details see the files `COPYING` and `COPYING.LESSER` in this directory.

<A name="toc3-30" title="Contributing" />
### Contributing

To submit an issue use the [issue tracker](http://github.com/zeromq/zapi/issues). All discussion happens on the [zeromq-dev](zeromq-dev@lists.zeromq.org) list or #zeromq IRC channel at irc.freenode.net.

The proper way to submit patches is to clone this repository, make your changes, and use git to create a patch or a pull request. See http://www.zeromq.org/docs:contributing. All contributors are listed in AUTHORS.

All classes are maintained by a single person, who is the responsible editor for that class and who is named in the header as such. This is usually the originator of the class. When several people collaborate on a class, one single person is always the lead maintainer and the one to blame when it breaks.

The general rule is, if you contribute code to zapi you must be willing to maintain it as long as there are users of it. Code with no active maintainer will in general be deprecated and/or removed.

<A name="toc2-41" title="Using zapi" />
## Using zapi

<A name="toc3-44" title="API Summary" />
### API Summary

<A name="toc4-47" title="zctx - working with ØMQ contexts" />
#### zctx - working with ØMQ contexts

The zctx class wraps ØMQ contexts. It manages open sockets in the context and automatically closes these before terminating the context. It provides a simple way to set the linger timeout on sockets, and configure contexts for number of I/O threads. Sets-up signal (interrrupt) handling for the process.

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

<A name="toc4-69" title="zstr - sending and receiving strings" />
#### zstr - sending and receiving strings

The zstr class provides utility functions for sending and receiving C strings across ØMQ sockets.

    char *
        zstr_recv (void *socket);
    int
        zstr_send (void *socket, const char *string);
    int
        zstr_sendf (void *socket, const char format, ...);

<A name="toc4-81" title="zframe - working with single message frames" />
#### zframe - working with single message frames

The zframe class provides methods to send and receive single message frames across ØMQ sockets.

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

<A name="toc4-103" title="zmsg - working with multipart messages" />
#### zmsg - working with multipart messages

The zmsg class provides methods to send and receive multipart messages across ØMQ sockets.

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

<A name="toc4-139" title="zloop - event-driven reactor" />
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

<A name="toc3-162" title="Building and Installing" />
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

<A name="toc3-183" title="Linking with an Application" />
### Linking with an Application

Include `zapi.h` in your application and link with libzapi. Here is a typical gcc link command:

    gcc -lzapi -lzmq myapp.c -o myapp

<A name="toc3-190" title="This Document" />
### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
