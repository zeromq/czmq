.set GIT=https://github.com/zeromq/zapi
.sub 0MQ=ØMQ

# zapi - High-level C binding for 0MQ

## Contents

.toc

## Overview

### Scope and Goals

zapi has these goals:

* To wrap the 0MQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of 0MQ, particularly 2.0, 2.1, and 3.0.
* To provide a space for development of more sophisticated API semantics.

zapi grew out of concepts developed in [ØMQ - The Guide](http://zguide.zeromq.org) and [ZFL](http://zfl.zeromq.org).

[diagram]
                              +---------------+
                              |               |
                              | C application |
                              |               |
                              +-----+---+-----+
                                    |   |
                                    |   |
                       +------------+   |
                       |                |
                       v                |
  Open context    +---------+           |
  Create sockets  |         |           |    Connect, bind sockets
  Close sockets   | libzapi |           |    get/set socket options
  Send/receive    |    cYEL |           |
  Multithreading  +----+----+           |
  Reactor pattern      |                |
  Hash container       +------------+   |
  List container                    |   |
  Close context                     v   v
                                 +---------+
                                 |         |
                                 | libzmq  |
                                 |         |
                                 +---------+

[/diagram]

### Highlights

* Single API hides differences between 0MQ/2.1, and 0MQ/3.0.
* Broken into easy-to-use classes: zctx, zstr, zframe, zmsg, zloop, zhash, and zlist.
* Work with messages as strings, individual frames, or multipart messages.
* Automatic closure of any open sockets at context termination.
* Automatic LINGER configuration of all sockets for context termination.
* Portable API for creating child threads and 0MQ pipes to talk to them.
* Simple reactor with one-off and repeated timers, and socket readers.
* Portable to Linux, UNIX, OS X, Windows (porting is not yet complete).
* Includes generic hash and list containers.
* Full selftests on all classes.

### Ownership and License

zapi is maintained by Pieter Hintjens and Mikko Koppanen (build system). Its other authors and contributors are listed in the AUTHORS file. It is held by the ZeroMQ organization at github.com.

The authors of zapi grant you use of this software under the terms of the GNU Lesser General Public License (LGPL). For details see the files `COPYING` and `COPYING.LESSER` in this directory.

### Contributing

To submit an issue use the [issue tracker](http://github.com/zeromq/zapi/issues). All discussion happens on the [zeromq-dev](zeromq-dev@lists.zeromq.org) list or #zeromq IRC channel at irc.freenode.net.

The proper way to submit patches is to clone this repository, make your changes, and use git to create a patch or a pull request. See http://www.zeromq.org/docs:contributing. All contributors are listed in AUTHORS.

The general rule is, if you contribute code to zapi you must be willing to maintain it as long as there are users of it. Code with no active maintainer will in general be deprecated and/or removed.

## Using zapi

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

### API Summary

#### zctx - working with 0MQ contexts

The zctx class wraps 0MQ contexts and replaces the core zmq_init(), zmq_term(), and zmq_socket() functions. The main purpose of this class is to automatically close sockets when terminating a context. The zctx class has these main features:

* Tracks all open sockets and automatically closes them before calling zmq_term(). This avoids an infinite wait on open sockets.
* Automatically configures sockets with a ZMQ_LINGER timeout you can define, and which defaults to zero. The default behavior of zctx is therefore like 0MQ/2.0, immediate termination with loss of any pending messages. You can set any linger timeout you like by calling the zctx_set_linger() method.
* Moves the iothreads configuration to a separate method, so that default usage is 1 I/O thread. Lets you configure this value.
* Sets up signal (SIGINT and SIGTERM) handling so that blocking calls such as zmq_recv() and zmq_poll() will return when the user presses Ctrl-C.
* Provides API to create child threads with a pipe (PAIR socket) to talk to them.

This is the class interface:

    //  Structure passed to threads created via this class
    typedef struct {
        zctx_t *ctx;                //  Context shared with parent thread
        void *pipe;                 //  Pipe to parent thread (PAIR)
        void *arg;                  //  Application argument
    } zthread_t;

    //  Create new context, returns context object, replaces zmq_init
    zctx_t *
        zctx_new (void);
    //  Destroy context and all sockets in it, replaces zmq_term
    void
        zctx_destroy (zctx_t **self_p);
    //  Raise default I/O threads from 1, for crazy heavy applications    
    void
        zctx_set_iothreads (zctx_t *self, int iothreads);
    //  Set msecs to flush sockets when closing them
    void 
        zctx_set_linger (zctx_t *self, int linger);
    //  Create socket within this context, replaces zmq_socket
    void *
        zctx_socket_new (zctx_t *self, int type);
    //  Destroy socket, replaces zmq_close
    void
        zctx_socket_destroy (zctx_t *self, void *socket);
    //  Create thread, return PAIR socket to talk to thread. The child thread
    //  receives a (zthread_t *) object including a zctx, a pipe back to the
    //  creating thread, and the arg passed in this call.
    void *
        zctx_thread_new (zctx_t *self, void *(*thread_fn) (void *), void *arg);
    //  Self test of this class
    int
        zctx_test (Bool verbose);

    //  Global signal indicator, TRUE when user presses Ctrl-C or the process
    //  gets a SIGTERM signal.
    int zctx_interrupted;

This class is an example of how to create a thread-safe object using 0MQ. Actually it's a little more work than it sounded at first. Let's start by agreeing on the problem here. We have a context wrapper, zctx, which does some neat work for us such as managing sockets automatically so we can just shut down without having to manually close each and every damn socket. All good, until we need to build a multithreaded application. Which is about 80% of interesting 0MQ applications. It is not safe to share a single object from multiple threads. They'll try to mess with the data structures concurrently, and it'll break nastily.

OK, the classic solution would be exclusion using semaphores, critical sections, etc. We're 0MQ fanatics so that's not even an option. Instead, we want to eat our own dogfood and do this using 0MQ.

The basic concept, which you'll see in this class, is that the real work is not done by the object we own, but by a separate object, running in its own thread. I call this the "agent". This is a nice pattern, and we see it in a few places, such as the flcliapi example from the Guide.

The slight difficulty here is bootstrapping. We have a separate agent thread, which we talk to over inproc, and which manages our context and sockets. This is problem number one, I'll get to problem two in a sec.

Part of zctx's magic is delaying the zmq_init call until it's really needed. This lets us first configure iothreads if needed. It's the agent that will create the 0MQ context by calling zmq_init. However we need sockets to talk to the agent. Solution: use two contexts, one for the pipes to/from the agent, and one for the application itself. Not many 0MQ applications create multiple contexts, but it's a valid and useful technique.

So we create a private context, two sockets, and then we pass one of those sockets to the agent. We can then talk to the agent by sending simple commands like IOTHREADS=100, SOCKET, CLOSE=0xff33344, and TERMINATE. BTW, do not set IOTHREADS to 100, that is insane. Anything above 1 is actually insane unless you know what you're doing.

Next problem is when our application needs child threads. If we simply use pthreads_create() we're faced with several issues. First, it's not portable to legacy OSes like win32. Second, how can a child thread get access to our zctx object? If we just pass it around, we'll end up sharing the pipe socket (which we use to talk to the agent) between threads, and that will then crash 0MQ. Sockets cannot be used from more than one thread at a time.

So each child thread needs its own pipe to the agent. For the agent, this is fine, it can talk to a million threads. But how do we create those pipes in the child thread? We can't, not without help from the main thread. The solution is to wrap thread creation, like we wrap socket creation. To create a new thread, the app calls zctx_thread_new() and this method creates a dedicated zctx object, with a pipe, and then it passes that object to the newly minted child thread.

The neat thing is we can hide non-portable aspects. Windows is really a mess when it comes to threads. Three different APIs, none of which is really right, so you have to do rubbish like manually cleaning up when a thread finishes. Anyhow, it's hidden in this class so you don't need to worry.

Second neat thing about wrapping thread creation is we can make it a more enriching experience for all involved. One thing I do often is use a PAIR-PAIR pipe to talk from a thread to/from its parent. So this class will automatically create such a pair for each thread you start.

That's it. We have a multithreaded class that is thread safe and also gives you major power for creating multithreaded applications, with a really simple API.

Now that's what I call a language binding.

#### zstr - sending and receiving strings

The zstr class provides utility functions for sending and receiving C strings across 0MQ sockets. It sends strings without a terminating null, and appends a null byte on received strings. This class is for simple message sending. 

[diagram]

           Memory                                 Wire
           +---+---+---+---+---+---+---+          +---+---+---+---+---+---+---+
    Send   | S | t | r | i | n | g | 0 |  ---->   | 6 | S | t | r | i | n | g |
           +---+---+---+---+---+---+---+          +---+---+---+---+---+---+---+


           Wire                                   Heap
           +---+---+---+---+---+---+---+          +---+---+---+---+---+---+---+
    Recv   | 6 | S | t | r | i | n | g |  ---->   | S | t | r | i | n | g | 0 |
           +---+---+---+---+---+---+---+          +---+---+---+---+---+---+---+

[/diagram]

This is the class interface:

    char *
        zstr_recv (void *socket);
    int
        zstr_send (void *socket, const char *string);
    int
        zstr_sendf (void *socket, const char *format, ...);

#### zframe - working with single message frames

The zframe class provides methods to send and receive single message frames across 0MQ sockets. A 'frame' corresponds to one zmq_msg_t. When you read a frame from a socket, the zframe_more() method indicates if the frame is part of an unfinished multipart message. The zframe_send method normally destroys the frame, but with the ZFRAME_REUSE flag, you can send the same frame many times. Frames are binary, and this class has no special support for text data.

This is the class interface:

    zframe_t *
        zframe_new (const void *data, size_t size);
    void
        zframe_destroy (zframe_t **self_p);
    zframe_t *
        zframe_recv (void *socket);
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

The zmsg class provides methods to send and receive multipart messages across 0MQ sockets. This class provides a list-like container interface, with methods to work with the overall container. zmsg_t messages are composed of zero or more zframe_t frames.

This is the class interface:

    zmsg_t *
        zmsg_new (void);
    void
        zmsg_destroy (zmsg_t **self_p);
    zmsg_t *
        zmsg_recv (void *socket);
    void
        zmsg_send (zmsg_t **self, void *socket);
    size_t
        zmsg_size (zmsg_t *self);
    void
        zmsg_push (zmsg_t *self, zframe_t *frame);
    void
        zmsg_append (zmsg_t *self, zframe_t *frame);
    void
        zmsg_pushmem (zmsg_t *self, const void *src, size_t size);
    void
        zmsg_appendmem (zmsg_t *self, const void *src, size_t size);
    zframe_t *
        zmsg_pop (zmsg_t *self);
    void
        zmsg_remove (zmsg_t *self, zframe_t *frame);
    zframe_t *
        zmsg_first (zmsg_t *self);
    zframe_t *
        zmsg_next (zmsg_t *self);
    zframe_t *
        zmsg_body (zmsg_t *self);
    void
        zmsg_save (zmsg_t *self, FILE *file);
    zmsg_t *
        zmsg_load (FILE *file);
    void
        zmsg_dump (zmsg_t *self);
    int
        zmsg_test (int verbose);

#### zloop - event-driven reactor

The zloop class provides an event-driven reactor pattern. The reactor handles socket readers (not writers in the current implementation), and once-off or repeated timers. Its resolution is 1 msec. It uses a tickless timer to reduce CPU interrupts in inactive processes.

This is the class interface:

    zloop_t *
        zloop_new (void);
    void
        zloop_destroy (zloop_t **self_p);
    int
        zloop_reader (zloop_t *self, void *socket, zloop_fn handler, void *args);
    void
        zloop_cancel (zloop_t *self, void *socket);
    int
        zloop_timer (zloop_t *self, size_t delay, size_t times, zloop_fn handler, void *args);
    int
        zloop_start (zloop_t *self);
    int
        zloop_test (Bool verbose);

#### zhash - expandable hash table container

Provides a generic hash container. Taken from the ZFL project zfl_hash class and provides identical functionality. Note that it's relatively slow (~50k insertions/deletes per second), so don't do inserts/updates on the critical path for message I/O.  It can do ~2.5M lookups per second for 16-char keys.  Timed on a 1.6GHz CPU.

This is the class interface:

    zhash_t *
        zhash_new (void);
    void
        zhash_destroy (zhash_t **self_p);
    int
        zhash_insert (zhash_t *self, char *key, void *value);
    void
        zhash_update (zhash_t *self, char *key, void *value);
    void
        zhash_delete (zhash_t *self, char *key);
    void *
        zhash_lookup (zhash_t *self, char *key);
    void *
        zhash_freefn (zhash_t *self, char *key, zhash_free_fn *free_fn);
    size_t
        zhash_size (zhash_t *self);
    int
        zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument);
    void
        zhash_test (int verbose);

#### zlist - singly-linked list container

Provides a generic list container. Taken from the ZFL project zlist class and provides identical functionality. You
can use this to construct multi-dimensional lists, and other structures together with other generic containers like zhash.

This is the class interface:

    zlist_t *
        zlist_new (void);
    void
        zlist_destroy (zlist_t **self_p);
    void *
        zlist_first (zlist_t *self);
    void *
        zlist_next (zlist_t *self);
    void
        zlist_append (zlist_t *self, void *value);
    void
        zlist_push (zlist_t *self, void *value);
    void *
        zlist_pop (zlist_t *self);
    void
        zlist_remove (zlist_t *self, void *value);
    zlist_t *
        zlist_copy (zlist_t *self);
    size_t
        zlist_size (zlist_t *self);
    void
        zlist_test (int verbose);

### Development

zapi is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
