
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

**<a href="#toc2-220">Design Ideology</a>**
&emsp;<a href="#toc3-223">The Problem with C</a>
&emsp;<a href="#toc3-232">A Simple Class Model</a>
&emsp;<a href="#toc3-259">Naming Style</a>
&emsp;<a href="#toc3-268">Containers</a>
&emsp;<a href="#toc3-280">Portability</a>
&emsp;<a href="#toc3-306">Technical Aspects</a>

**<a href="#toc2-316">Under the Hood</a>**
&emsp;<a href="#toc3-319">Adding a New Class</a>
&emsp;<a href="#toc3-332">Coding Style</a>
&emsp;<a href="#toc3-351">Assertions</a>
&emsp;<a href="#toc3-369">Documentation</a>
&emsp;<a href="#toc3-408">Development</a>
&emsp;<a href="#toc3-418">Porting libzapi</a>
&emsp;<a href="#toc3-431">This Document</a>

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

<A name="toc2-220" title="Design Ideology" />
## Design Ideology

<A name="toc3-223" title="The Problem with C" />
### The Problem with C

C has the significant advantage of being a small language that, if we take a little care with formatting and naming, can be easily interchanged between developers. Every C developer will use much the same 90% of the language. Larger languages like C++ provide powerful abstractions like STL containers but at the cost of interchange. Every C++ developer will use a different 20% of the language.

The huge problem with C is that any realistic application needs packages of functionality to bring the language up to the levels we expect for the 21st century. Much can be done by using external libraries but every additional library is a dependency that makes the resulting applications harder to build and port. While C itself is a highly portable language (and can be made more so by careful use of the preprocessor), most C libraries consider themselves part of the operating system, and as such do not attempt to be portable.

The answer to this, as we learned from building enterprise-level C applications at iMatix from 1995-2005, is to create our own fully portable, high-quality libraries of pre-packaged functionality, in C. Doing this right solves both the requirements of richness of the language, and of portability of the final applications.

<A name="toc3-232" title="A Simple Class Model" />
### A Simple Class Model

C has no standard API style. It is one thing to write a useful component, but something else to provide an API that is consistent and obvious across many components. We learned from building [OpenAMQ](http://www.openamq.org), a messaging client and server of 0.5M LoC, that a consistent model for extending C makes life for the application developer much easier.

The general model is that of a class (the source package) that provides objects (in fact C structures). The application creates objects and then works with them. When done, the application destroys the object. In C, we tend to use the same name for the object as the class, when we can, and it looks like this (to take a fictitious libzapi class):

    zregexp_t *regexp = zregexp_new (regexp_string);
    if (!regexp)
        printf ("E: invalid regular expression: %s\n", regexp_string);
    else
    if (zregexp_match (regexp, input_buffer))
        printf ("I: successful match for %s\n", input buffer);
    zregexp_destroy (&regexp);

As far as the C program is concerned, the object is a reference to a structure (not a void pointer). We pass the object reference to all methods, since this is still C. We could do weird stuff like put method addresses into the structure so that we can emulate a C++ syntax but it's not worthwhile. The goal is not to emulate an OO system, it's simply to gain consistency. The constructor returns an object reference, or NULL if it fails. The destructor nullifies the class pointer, and is idempotent.

What we aim at here is the simplest possible consistent syntax.

No model is fully consistent, and classes can define their own rules if it helps make a better result. For example:

* Some classes may not be opaque. For example, we have cases of generated serialization classes that encode and decode structures to/from binary buffers. It feels clumsy to have to use methods to access the properties of these classes.

* While every class has a new method that is the formal constructor, some methods may also act as constructors. For example, a "dup" method might take one object and return a second object.

* While every class has a destroy method that is the formal destructor, some methods may also act as destructors. For example, a method that sends an object may also destroy the object (so that ownership of any buffers can passed to background threads). Such methods take the same "pointer to a reference" argument as the destroy method.

<A name="toc3-259" title="Naming Style" />
### Naming Style

libzapi aims for short, consistent names, following the theory that names we use most often should be shortest. Classes get one-word names, unless they are part of a family of classes in which case they may be two words, the first being the family name. Methods, similarly, get one-word names and we aim for consistency across classes (so a method that does something semantically similar in two classes will get the same name in both). So the canonical name for any method is:

    zclassname_methodname

And the reader can easily parse this without needing special syntax to separate the class name from the method name.

<A name="toc3-268" title="Containers" />
### Containers

After a long experiment with containers, we've decided that we need exactly two containers:

* A singly-linked list.
* A hash table using text keys.

These are zlist and zhash, respectively. Both store void pointers, with no attempt to manage the details of contained objects. You can use these containers to create lists of lists, hashes of lists, hashes of hashes, etc.

We assume that at some point we'll need to switch to a doubly-linked list.

<A name="toc3-280" title="Portability" />
### Portability

Creating a portable C application can be rewarding in terms of maintaining a single code base across many platforms, and keeping (expensive) system-specific knowledge separate from application developers. In most projects (like ØMQ core), there is no portability layer and application code does conditional compilation for all mixes of platforms. This leads to quite messy code.

libzapi is a portability layer, similar to but thinner than libraries like the [Apache Portable Runtime](http://apr.apache.org) (APR).

These are the places a C application is subject to arbitrary system differences:

* Different compilers may offer slightly different variants of the C language, often lacking specific types or using neat non-portable names. Windows is a big culprit here. We solve this by 'patching' the language in zapi_prelude.h, e.g. defining int64_t on Windows.
* System header files are inconsistent, i.e. you need to include different files depending on the OS type and version. We solve this by pulling in all necessary header files in zapi_prelude.h. This is a proven brute-force approach that increases recompilation times but eliminates a major source of pain.
* System libraries are inconsistent, i.e. you need to link with different libraries depending on the OS type and version. We solve this with an external compilation tool, 'C', which detects the OS type and version (at runtime) and builds the necessary link commands.
* System functions are inconsistent, i.e. you need to call different functions depending, again, on OS type and version. We solve this by building small abstract classes that handle specific areas of functionality, and doing conditional compilation in these.

An example of the last:

    #if (defined (__UNIX__))
        pid = GetCurrentProcessId();
    #elif (defined (__WINDOWS__))
        pid = getpid ();
    #else
        pid = 0;
    #endif

libzapi uses the GNU autotools system, so non-portable code can use the macros this defines. It can also use macros defined by the zapi_prelude.h header file.

<A name="toc3-306" title="Technical Aspects" />
### Technical Aspects

* *Thread safety*: the use of opaque structures is thread safe, though ØMQ applications should not share state between threads in any case.
* *Name spaces*: we prefix class names with `z`, which ensures that all exported functions are globally safe.
* *Library versioning*: we don't make any attempt to version the library at this stage. Classes are in our experience highly stable once they are built and tested, the only changes typically being added methods.
* *Performance*: for critical path processing, you may want to avoid creating and destroying classes. However on modern Linux systems the heap allocator is very fast. Individual classes can choose whether or not to nullify their data on allocation.
* *Self-testing*: every class has a `selftest` method that runs through the methods of the class. In theory, calling all selftest functions of all classes does a full unit test of the library. The `zapi_selftest` application does this.
* *Memory management*: libzapi classes do not use any special memory management techiques to detect leaks. We've done this in the past but it makes the code relatively complex. Instead, we do memory leak testing using tools like valgrind.

<A name="toc2-316" title="Under the Hood" />
## Under the Hood

<A name="toc3-319" title="Adding a New Class" />
### Adding a New Class

If you define a new libzapi class `myclass` you need to:

* Write the `zmyclass.c` and `zmyclass.h` source files, in `src` and `include` respectively.
* Add`#include <zmyclass.h>` to `include/zapi.h`.
* Add the myclass header and test call to `src/zapi_selftest.c`.
* Add a reference documentation to 'doc/zmyclass.txt'.
* Add myclass to 'src/Makefile.am` and `doc/Makefile.am`.

The `bin/newclass.sh` shell script will automate these steps for you.

<A name="toc3-332" title="Coding Style" />
### Coding Style

In general the zctx class defines the style for the whole library. The overriding rules for coding style are consistency, clarity, and ease of maintenance. We use the C99 standard for syntax including principally:

* The // comment style.
* Variables definitions placed in or before the code that uses them.

So while ANSI C code might say:

    zblob_t *file_buffer;       /*  Buffer for our file */
    ... (100 lines of code)
    file_buffer = zblob_new ();
    ...

The style in libzapi would be:

    zblob_t *file_buffer = zblob_new ();

<A name="toc3-351" title="Assertions" />
### Assertions

We use assertions heavily to catch bad argument values. The libzapi classes do not attempt to validate arguments and report errors; bad arguments are treated as fatal application programming errors.

We also use assertions heavily on calls to system functions that are never supposed to fail, where failure is to be treated as a fatal non-recoverable error (e.g. running out of memory).

Assertion code should always take this form:

    int rc = some_function (arguments);
    assert (rc == 0);

Rather than the side-effect form:

    assert (some_function (arguments) == 0);

Since assertions may be removed by an optimizing compiler.

<A name="toc3-369" title="Documentation" />
### Documentation

Man pages are generated from the class header and source files via the doc/mkman tool, and similar functionality in the gitdown tool (http://github.com/imatix/gitdown). The header file for a class must wrap its interface as follows (example is from include/zclock.h):

    //  @interface
    //  Sleep for a number of milliseconds
    void 
        zclock_sleep (int msecs);

    //  Return current system clock as milliseconds
    int64_t 
        zclock_time (void);

    //  Self test of this class
    int 
        zclock_test (Bool verbose);
    //  @end

The source file for a class must provide documentation as follows:

    /*  
    @header
    ...short explanation of class...
    @discuss
    ...longer discussion of how it works...
    @end
    */

The source file for a class then provides the self test example as follows:

    //  @selftest
    int64_t start = zclock_time ();
    zclock_sleep (10);
    assert ((zclock_time () - start) >= 10);
    //  @end

The template for man pages is in doc/mkman.

<A name="toc3-408" title="Development" />
### Development

libzapi is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

<A name="toc3-418" title="Porting libzapi" />
### Porting libzapi

When you try libzapi on an OS that it's not been used on (ever, or for a while), you will hit code that does not compile. In some cases the patches are trivial, in other cases (usually when porting to Windows), the work needed to build equivalent functionality may be quite heavy. In any case, the benefit is that once ported, the functionality is available to all applications.

Before attempting to patch code for portability, please read the `zapi_prelude.h` header file. There are several typical types of changes you may need to make to get functionality working on a specific operating system:

* Defining typedefs which are missing on that specific compiler: do this in zapi_prelude.h.
* Defining macros that rename exotic library functions to more conventional names: do this in zapi_prelude.h.
* Reimplementing specific methods to use a non-standard API: this is typically needed on Windows. Do this in the relevant class, using #ifdefs to properly differentiate code for different platforms.

The canonical 'standard operating system' for all libzapi code is Linux, gcc, POSIX.

<A name="toc3-431" title="This Document" />
### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
