
<A name="toc1-3" title="libzapi - High-level C binding for ØMQ" />
# libzapi - High-level C binding for ØMQ

<A name="toc2-6" title="Contents" />
## Contents


**<a href="#toc2-11">Overview</a>**
&emsp;<a href="#toc3-14">Scope and Goals</a>
&emsp;<a href="#toc3-53">Highlights</a>
&emsp;<a href="#toc3-68">Ownership and License</a>
&emsp;<a href="#toc3-75">Contributing</a>

**<a href="#toc2-84">Using libzapi</a>**
&emsp;<a href="#toc3-87">Building and Installing</a>
&emsp;<a href="#toc3-108">Linking with an Application</a>
&emsp;<a href="#toc3-115">API Summary</a>
&emsp;<a href="#toc4-118">zctx - working with ØMQ contexts</a>
&emsp;<a href="#toc4-129">zstr - sending and receiving strings</a>
&emsp;<a href="#toc4-154">zframe - working with single message frames</a>
&emsp;<a href="#toc4-165">zmsg - working with multipart messages</a>
&emsp;<a href="#toc4-176">zloop - event-driven reactor</a>
&emsp;<a href="#toc4-187">zhash - expandable hash table container</a>
&emsp;<a href="#toc4-198">zlist - singly-linked list container</a>
&emsp;<a href="#toc4-209">zclock - millisecond clocks and delays</a>
&emsp;<a href="#toc3-220">Development</a>
&emsp;<a href="#toc3-230">This Document</a>

<A name="toc2-11" title="Overview" />
## Overview

<A name="toc3-14" title="Scope and Goals" />
### Scope and Goals

libzapi has these goals:

* To wrap the ØMQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of ØMQ, particularly 2.0, 2.1, and 3.0.
* To provide a space for development of more sophisticated API semantics.

libzapi grew out of concepts developed in [ØMQ - The Guide](http://zguide.zeromq.org) and [ZFL](http://zfl.zeromq.org).

<center>
<img src="https://github.com/zeromq/libzapi/raw/master/images/README_1.png" alt="1">
</center>

<A name="toc3-53" title="Highlights" />
### Highlights

* Single API hides differences between ØMQ/2.1, and ØMQ/3.0.
* Broken into easy-to-use classes: zctx, zstr, zframe, zmsg, zloop, zhash, and zlist.
* Work with messages as strings, individual frames, or multipart messages.
* Automatic closure of any open sockets at context termination.
* Automatic LINGER configuration of all sockets for context termination.
* Portable API for creating child threads and ØMQ pipes to talk to them.
* Simple reactor with one-off and repeated timers, and socket readers.
* System clock functions for sleeping and calculating timers.
* Portable to Linux, UNIX, OS X, Windows (porting is not yet complete).
* Includes generic hash and list containers.
* Full selftests on all classes.

<A name="toc3-68" title="Ownership and License" />
### Ownership and License

libzapi is maintained by Pieter Hintjens and Mikko Koppanen (build system). Its other authors and contributors are listed in the AUTHORS file. It is held by the ZeroMQ organization at github.com.

The authors of libzapi grant you use of this software under the terms of the GNU Lesser General Public License (LGPL). For details see the files `COPYING` and `COPYING.LESSER` in this directory.

<A name="toc3-75" title="Contributing" />
### Contributing

To submit an issue use the [issue tracker](http://github.com/zeromq/libzapi/issues). All discussion happens on the [zeromq-dev](zeromq-dev@lists.zeromq.org) list or #zeromq IRC channel at irc.freenode.net.

The proper way to submit patches is to clone this repository, make your changes, and use git to create a patch or a pull request. See http://www.zeromq.org/docs:contributing. All contributors are listed in AUTHORS.

The general rule is, if you contribute code to libzapi you must be willing to maintain it as long as there are users of it. Code with no active maintainer will in general be deprecated and/or removed.

<A name="toc2-84" title="Using libzapi" />
## Using libzapi

<A name="toc3-87" title="Building and Installing" />
### Building and Installing

libzapi uses autotools for packaging. To build from git (all example commands are for Linux):

    git clone git://github.com/zeromq/libzapi.git
    cd libzapi
    sh autogen.sh
    ./configure
    make all
    sudo make install
    sudo ldconfig

You will need the libtool and autotools packages. On FreeBSD, you may need to specify the default directories for configure:

    ./configure --with-zeromq=/usr/local

After building, you can run the libzapi selftests:

    make check

<A name="toc3-108" title="Linking with an Application" />
### Linking with an Application

Include `zapi.h` in your application and link with libzapi. Here is a typical gcc link command:

    gcc -lzapi -lzmq myapp.c -o myapp

<A name="toc3-115" title="API Summary" />
### API Summary

<A name="toc4-118" title="zctx - working with ØMQ contexts" />
#### zctx - working with ØMQ contexts

The zctx class wraps ØMQ contexts. It manages open sockets in the context 
and automatically closes these before terminating the context. It provides
a simple way to set the linger timeout on sockets, and configure contexts
for number of I/O threads. Sets-up signal (interrrupt) handling for the
process.

The zctx class has these main features:

* Tracks all open sockets and automatically closes them before calling 
zmq_term(). This avoids an infinite wait on open sockets.

* Automatically configures sockets with a ZMQ_LINGER timeout you can 
define, and which defaults to zero. The default behavior of zctx is 
therefore like ØMQ/2.0, immediate termination with loss of any pending 
messages. You can set any linger timeout you like by calling the 
zctx_set_linger() method.

* Moves the iothreads configuration to a separate method, so that default 
usage is 1 I/O thread. Lets you configure this value.

* Sets up signal (SIGINT and SIGTERM) handling so that blocking calls 
such as zmq_recv() and zmq_poll() will return when the user presses 
Ctrl-C.

* Provides API to create child threads with a pipe (PAIR socket) to talk 
to them.

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
    extern int zctx_interrupted;

This class is an example of how to create a thread-safe object using
ØMQ. Actually it's a little more work than it sounded at first. Let's
start by agreeing on the problem here. We have a context wrapper, zctx,
which does some neat work for us such as managing sockets automatically
so we can just shut down without having to manually close each and
every damn socket. All good, until we need to build a multithreaded
application. Which is about 80% of interesting ØMQ applications. It is
not safe to share a single object from multiple threads. They'll try to
mess with the data structures concurrently, and it'll break nastily.

OK, the classic solution would be exclusion using semaphores, critical
sections, etc. We're ØMQ fanatics so that's not even an option. Instead,
we want to eat our own dogfood and do this using ØMQ.

The basic concept, which you'll see in this class, is that the real work
is not done by the object we own, but by a separate object, running in
its own thread. I call this the "agent". This is a nice pattern, and we
see it in a few places, such as the flcliapi example from the Guide.

The slight difficulty here is bootstrapping. We have a separate agent
thread, which we talk to over inproc, and which manages our context and
sockets. This is problem number one, I'll get to problem two in a sec.

Part of zctx's magic is delaying the zmq_init call until it's really
needed. This lets us first configure iothreads if needed. It's the agent
that will create the ØMQ context by calling zmq_init. However we need
sockets to talk to the agent. Solution: use two contexts, one for the
pipes to/from the agent, and one for the application itself. Not many
ØMQ applications create multiple contexts, but it's a valid and useful
technique.

So we create a private context, two sockets, and then we pass one of
those sockets to the agent. We can then talk to the agent by sending
simple commands like IOTHREADS=100, SOCKET, CLOSE=0xff33344, and
TERMINATE. BTW, do not set IOTHREADS to 100, that is insane. Anything
above 1 is actually insane unless you know what you're doing.

Next problem is when our application needs child threads. If we simply
use pthreads_create() we're faced with several issues. First, it's not
portable to legacy OSes like win32. Second, how can a child thread get
access to our zctx object? If we just pass it around, we'll end up 
sharing the pipe socket (which we use to talk to the agent) between 
threads, and that will then crash ØMQ. Sockets cannot be used from more
than one thread at a time.

So each child thread needs its own pipe to the agent. For the agent, 
this is fine, it can talk to a million threads. But how do we create 
those pipes in the child thread? We can't, not without help from the
main thread. The solution is to wrap thread creation, like we wrap
socket creation. To create a new thread, the app calls zctx_thread_new()
and this method creates a dedicated zctx object, with a pipe, and then
it passes that object to the newly minted child thread.

The neat thing is we can hide non-portable aspects. Windows is really a
mess when it comes to threads. Three different APIs, none of which is
really right, so you have to do rubbish like manually cleaning up when
a thread finishes. Anyhow, it's hidden in this class so you don't need
to worry.

Second neat thing about wrapping thread creation is we can make it a 
more enriching experience for all involved. One thing I do often is use
a PAIR-PAIR pipe to talk from a thread to/from its parent. So this class
will automatically create such a pair for each thread you start.

That's it. We have a multithreaded class that is thread safe and also
gives you major power for creating multithreaded applications, with a 
really simple API.

Now that's what I call a language binding.

<A name="toc4-129" title="zstr - sending and receiving strings" />
#### zstr - sending and receiving strings

The zstr class provides utility functions for sending and receiving C 
strings across ØMQ sockets. It sends strings without a terminating null, 
and appends a null byte on received strings. This class is for simple 
message sending. 

<center>
<img src="https://github.com/zeromq/libzapi/raw/master/images/README_2.png" alt="2">
</center>

This is the class interface:

    //  Receive a string off a socket, caller must free it
    char *
        zstr_recv (void *socket);
    
    //  Send a string to a socket in ØMQ string format
    int
        zstr_send (void *socket, const char *string);
    
    //  Send a formatted string to a socket
    int
        zstr_sendf (void *socket, const char *format, ...);
    
    //  Self test of this class
    int
        zstr_test (Bool verbose);


<A name="toc4-154" title="zframe - working with single message frames" />
#### zframe - working with single message frames

The zframe class provides methods to send and receive single message 
frames across ØMQ sockets. A 'frame' corresponds to one zmq_msg_t. When 
you read a frame from a socket, the zframe_more() method indicates if the 
frame is part of an unfinished multipart message. The zframe_send method 
normally destroys the frame, but with the ZFRAME_REUSE flag, you can send 
the same frame many times. Frames are binary, and this class has no 
special support for text data.

This is the class interface:

    #define ZFRAME_MORE     1
    #define ZFRAME_REUSE    2
    
    //  Create a new frame with optional size, and optional data
    zframe_t *
        zframe_new (const void *data, size_t size);
    
    //  Destroy a frame
    void
        zframe_destroy (zframe_t **self_p);
    
    //  Receive a new frame off the socket
    zframe_t *
        zframe_recv (void *socket);
    
    //  Send a frame to a socket, destroy frame after sending
    void
        zframe_send (zframe_t **self_p, void *socket, int flags);
    
    //  Return number of bytes in frame data
    size_t
        zframe_size (zframe_t *self);
    
    //  Return address of frame data
    void *
        zframe_data (zframe_t *self);
    
    //  Return frame 'more' property
    int
        zframe_more (zframe_t *self);
    
    //  Self test of this class
    int
        zframe_test (Bool verbose);


<A name="toc4-165" title="zmsg - working with multipart messages" />
#### zmsg - working with multipart messages

The zmsg class provides methods to send and receive multipart messages 
across ØMQ sockets. This class provides a list-like container interface, 
with methods to work with the overall container. zmsg_t messages are 
composed of zero or more zframe_t frames.

This is the class interface:

    //  Create a new empty message object
    zmsg_t *
        zmsg_new (void);
    
    //  Destroy a message object and all frames it contains
    void
        zmsg_destroy (zmsg_t **self_p);
        
    //  Read 1 or more frames off the socket, into a new message object
    zmsg_t *
        zmsg_recv (void *socket);
        
    //  Send a message to the socket, and then destroy it
    void
        zmsg_send (zmsg_t **self_p, void *socket);
        
    //  Return number of frames in message
    size_t
        zmsg_size (zmsg_t *self);
    
    //  Push frame to front of message, before first frame
    void
        zmsg_push (zmsg_t *self, zframe_t *frame);
    
    //  Append frame to end of message, after last frame
    void
        zmsg_append (zmsg_t *self, zframe_t *frame);
    
    //  Push block of memory as new frame to front of message
    void
        zmsg_pushmem (zmsg_t *self, const void *src, size_t size);
    
    //  Push block of memory as new frame to end of message
    void
        zmsg_appendmem (zmsg_t *self, const void *src, size_t size);
    
    //  Pop frame off front of message, caller now owns frame
    zframe_t *
        zmsg_pop (zmsg_t *self);
    
    //  Remove frame from message, at any position, caller owns it
    void
        zmsg_remove (zmsg_t *self, zframe_t *frame);
    
    //  Return first frame in message, or null
    zframe_t *
        zmsg_first (zmsg_t *self);
    
    //  Return next frame in message, or null
    zframe_t *
        zmsg_next (zmsg_t *self);
    
    //  Return first body frame, i.e. after first null frame
    zframe_t *
        zmsg_body (zmsg_t *self);
    
    //  Save message to an open file
    void
        zmsg_save (zmsg_t *self, FILE *file);
    
    //  Load a message from an open file
    zmsg_t *
        zmsg_load (FILE *file);
    
    //  Print message to stderr, for debugging
    void
        zmsg_dump (zmsg_t *self);
    
    //  Self test of this class
    int
        zmsg_test (Bool verbose);


<A name="toc4-176" title="zloop - event-driven reactor" />
#### zloop - event-driven reactor

The zloop class provides an event-driven reactor pattern. The reactor 
handles socket readers (not writers in the current implementation), and 
once-off or repeated timers. Its resolution is 1 msec. It uses a tickless 
timer to reduce CPU interrupts in inactive processes.

This is the class interface:

    //  Callback function for reactor events
    typedef int (zloop_fn) (zloop_t *loop, void *socket, void *args);
    
    //  Create a new zloop reactor
    zloop_t *
        zloop_new (void);
    
    //  Destroy a reactor
    void
        zloop_destroy (zloop_t **self_p);
    
    //  Register a socket reader, on one socket
    int
        zloop_reader (zloop_t *self, void *socket, zloop_fn handler, void *args);
    
    //  Cancel the reader on the specified socket, if any
    void
        zloop_cancel (zloop_t *self, void *socket);
    
    //  Register a timer that will go off after 'delay' msecs, and will
    //  repeat 'times' times, unless 'times' is zero, meaning repeat forever.
    int
        zloop_timer (zloop_t *self, size_t delay, size_t times, zloop_fn handler, void *args);
    
    //  Start the reactor, ends if a callback function returns -1, or the process
    //  received SIGINT or SIGTERM.
    int
        zloop_start (zloop_t *self);
    
    //  Self test of this class
    int
        zloop_test (Bool verbose);


<A name="toc4-187" title="zhash - expandable hash table container" />
#### zhash - expandable hash table container

Expandable hash table container

This is the class interface:

    //  Callback function for zhash_foreach method
    typedef int (zhash_foreach_fn) (char *key, void *item, void *argument);
    //  Callback function for zhash_freefn method
    typedef void (zhash_free_fn) (void *data);
    
    //  Create a new, empty hash container
    zhash_t *
        zhash_new (void);
    
    //  Destroy a hash container and all items in it
    void
        zhash_destroy (zhash_t **self_p);
    
    //  Insert an item into the hash container using the specified key
    int
        zhash_insert (zhash_t *self, char *key, void *item);
    
    //  Insert or update the item for the specified key
    void
        zhash_update (zhash_t *self, char *key, void *item);
    
    //  Destroy the item at the specified key, if any
    void
        zhash_delete (zhash_t *self, char *key);
    
    //  Return the item at the specified key, or null
    void *
        zhash_lookup (zhash_t *self, char *key);
    
    //  Set a free function for the item at the specified key
    void *
        zhash_freefn (zhash_t *self, char *key, zhash_free_fn *free_fn);
    
    //  Return the number of keys/items in the hash table
    size_t
        zhash_size (zhash_t *self);
    
    //  Iterate over the hash table and apply the function to each item
    int
        zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument);
    
    //  Self test of this class
    void
        zhash_test (int verbose);

Note that it's relatively slow (~50k insertions/deletes per second), so
don't do inserts/updates on the critical path for message I/O.  It can
do ~2.5M lookups per second for 16-char keys.  Timed on a 1.6GHz CPU.

<A name="toc4-198" title="zlist - singly-linked list container" />
#### zlist - singly-linked list container

Provides a generic container implementing a fast singly-linked list. You
can use this to construct multi-dimensional lists, and other structures
together with other generic containers like zhash.

This is the class interface:

    //  Create a new list container
    zlist_t *
        zlist_new (void);
    
    //  Destroy a list container
    void
        zlist_destroy (zlist_t **self_p);
    
    //  Return first item in the list, or null
    void *
        zlist_first (zlist_t *self);
    
    //  Return next item in the list, or null
    void *
        zlist_next (zlist_t *self);
    
    //  Append an item to the end of the list
    void
        zlist_append (zlist_t *self, void *item);
    
    //  Push an item to the start of the list
    void
        zlist_push (zlist_t *self, void *item);
    
    //  Pop the item off the start of the list, if any
    void *
        zlist_pop (zlist_t *self);
    
    //  Remove the specified item from the list if present
    void
        zlist_remove (zlist_t *self, void *item);
    
    //  Copy the entire list, return the copy
    zlist_t *
        zlist_copy (zlist_t *self);
    
    //  Return number of items in the list
    size_t
        zlist_size (zlist_t *self);
    
    //  Self test of this class
    void
        zlist_test (int verbose);


<A name="toc4-209" title="zclock - millisecond clocks and delays" />
#### zclock - millisecond clocks and delays

The zclock class provides essential sleep and system time functions, used 
to slow down threads for testing, and calculate timers for polling. Wraps 
the non-portable system calls in a simple portable API.

This is the class interface:

    //  Sleep for a number of milliseconds
    void 
        zclock_sleep (int msecs);
    
    //  Return current system clock as milliseconds
    int64_t 
        zclock_time (void);
    
    //  Self test of this class
    int 
        zclock_test (Bool verbose);

This class contains some small surprises. Most amazing, win32 did an API
better than POSIX. The win32 Sleep() call is not only a neat 1-liner, it
also sleeps for milliseconds, whereas the POSIX call asks us to think in
terms of nanoseconds, which is insane. I've decided every single man page
for this library will say "insane" at least once. Anyhow, milliseconds 
are a concept we can deal with. Seconds are too fat, nanoseconds too 
tiny, but milliseconds are just right for slices of time we want to work
with at the ØMQ scale. zclock doesn't give you objects to work with, we
like the zapi class model but we're not insane. There, got it in again.

<A name="toc3-220" title="Development" />
### Development

libzapi is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

<A name="toc3-230" title="This Document" />
### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
