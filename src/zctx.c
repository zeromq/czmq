/*  =========================================================================
    zctx - working with 0MQ contexts

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of zapi, the C binding for 0MQ: http://zapi.zeromq.org.

    This is free software; you can redistribute it and/or modify it under the
    terms of the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your option)
    any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
    This class is an example of how to create a thread-safe object using
    0MQ. Actually it's a little more work than it sounded at first. Let's
    start by agreeing on the problem here. We have a context wrapper, zctx,
    which does some neat work for us such as managing sockets automatically
    so we can just shut down without having to manually close each and
    every damn socket. All good, until we need to build a multithreaded
    application. Which is about 80% of interesting 0MQ applications. It is
    not safe to share a single object from multiple threads. They'll try to
    mess with the data structures concurrently, and it'll break nastily.
    
    OK, the classic solution would be exclusion using semaphores, critical
    sections, etc. We're 0MQ fanatics so that's not even an option. Instead,
    we want to eat our own dogfood and do this using 0MQ.
    
    The basic concept, which you'll see in this class, is that the real work
    is not done by the object we own, but by a separate object, running in
    its own thread. I call this the "agent". This is a nice pattern, and we
    see it in a few places, such as the flcliapi example from the Guide.
    
    The slight difficulty here is bootstrapping. We have a separate agent
    thread, which we talk to over inproc, and which manages our context and
    sockets. This is problem number one, I'll get to problem two in a sec.
    
    Part of zctx's magic is delaying the zmq_init call until it's really
    needed. This lets us first configure iothreads if needed. It's the agent
    that will create the 0MQ context by calling zmq_init. However we need
    sockets to talk to the agent. Solution: use two contexts, one for the
    pipes to/from the agent, and one for the application itself. Not many
    0MQ applications create multiple contexts, but it's a valid and useful
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
    threads, and that will then crash 0MQ. Sockets cannot be used from more
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
 */

#include "../include/zapi_prelude.h"
#include "../include/zlist.h"
#include "../include/zstr.h"
#include "../include/zframe.h"
#include "../include/zctx.h"

//  Structure of our class

struct _zctx_t {
    //  This is the agent thread we create
    pthread_t agent;
    //  Generated endpoint means we can have many zctx instances
    char endpoint [64];
    //  Private context used between us and agent
    void *context;
    //  This is the socket we use to talk to the agent thread
    void *pipe;
};


//  ---------------------------------------------------------------------
//  Context agent
//  This thread does the real work of creating and destroying sockets.
//  It accepts the following string requests over its inproc socket:
//
//    IOTHREADS=%d   Set iothreads to specified threads
//    LINGER=%d      Set linger to specified msecs
//    SOCKET=%d      Create socket of specified type, reply="%p"
//    CLOSE=%p       Close specified socket
//    TERMINATE      Destroy sockets, terminate context

static void *
agent_thread (void *pipe)
{
    zlist_t *sockets;           //  List of registered sockets
    void *context = NULL;       //  Context for applications
    int iothreads = 1;          //  Number of io threads for zmq_init
    int linger = 0;             //  Linger setting
    Bool terminated = FALSE;
    
    //  Initialize agent
    sockets = zlist_new ();

    while (!terminated) {
        //  ROUTER socket gives us two frames, [address] [request]
        zframe_t *address = zframe_recv (pipe);
        assert (zframe_more (address));
        //  Read command as string
        char *command = zstr_recv (pipe);
        char *value = strchr (command, '=');
        if (value)
            *value++ = 0;

        if (streq (command, "IOTHREADS")) {
            assert (value);
            iothreads = atoi (value);
        }
        else
        if (streq (command, "LINGER")) {
            assert (value);
            linger = atoi (value);
        }
        else
        if (streq (command, "SOCKET")) {
            assert (value);
            //  Initialize context now if necessary
            if (context == NULL)
                context = zmq_init (iothreads);
            assert (context);
            //  Create and register socket
            void *socket = zmq_socket (context, atoi (value));
            assert (socket);
            zlist_push (sockets, socket);
            //  Return socket reference to caller
            zframe_send (&address, pipe, ZFRAME_MORE);
            zstr_sendf (pipe, "%p", socket);
        }
        else
        if (streq (command, "CLOSE")) {
            assert (value);
            void *socket;
            int rc = sscanf (value, "%p", &socket);
            assert (rc == 1);
            zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof (int));
            zmq_close (socket);
            zlist_remove (sockets, socket);
        }
        else
        if (streq (command, "TERMINATE")) {
            while (zlist_size (sockets)) {
                void *socket = zlist_first (sockets);
                zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof (int));
                zmq_close (socket);
                zlist_remove (sockets, socket);
            }
            zlist_destroy (&sockets);
            if (context)
                zmq_term (context);
            terminated = TRUE;
        }
        else {
            fprintf (stderr, "E: bad command (%s)\n", command);
            assert (0);
        }
        free (command);
        zframe_destroy (&address);
    }
    //  Terminate agent
    zmq_close (pipe);
    return NULL;
}


//  ---------------------------------------------------------------------
//  Signal handling
//

//  This is a global variable accessible to zapi application code
int zctx_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    zctx_interrupted = 1;
}


//  --------------------------------------------------------------------------
//  Constructor

zctx_t *
zctx_new (void)
{
    zctx_t
        *self;

    self = (zctx_t *) zmalloc (sizeof (zctx_t));
    self->context = zmq_init (1);
    int rc = snprintf (self->endpoint, sizeof (self->endpoint),
        "inproc://zctx-pipe-%p", self->context);
    assert (rc < sizeof (self->endpoint));

    //  Create bound socket and pass it to agent
    void *remote = zmq_socket (self->context, ZMQ_ROUTER);
    rc = zmq_bind (remote, self->endpoint);
    assert (rc == 0);
    pthread_create (&self->agent, NULL, agent_thread, remote);

    //  Now connect ourselves to that endpoint
    self->pipe = zmq_socket (self->context, ZMQ_DEALER);
    rc = zmq_connect (self->pipe, self->endpoint);
    assert (rc == 0);

    //  Install signal handler for SIGINT and SIGTERM
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);

    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zctx_destroy (zctx_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zctx_t *self = *self_p;
        //  The main context has an agent, child threads don't
        if (self->agent) {
            zstr_sendf (self->pipe, "TERMINATE");
            pthread_join (self->agent, NULL);
        }
        zmq_close (self->pipe);
        if (self->agent)
            zmq_term (self->context);
        
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Configure number of I/O threads in context, only has effect if called 
//  before creating first socket. Default I/O threads is 1, sufficient for 
//  all except very high volume applications.

void
zctx_set_iothreads (zctx_t *self, int iothreads) 
{
    assert (self);
    zstr_sendf (self->pipe, "IOTHREADS=%d", iothreads);
}


//  --------------------------------------------------------------------------
//  Configure linger timeout in msecs. Call this before destroying sockets or 
//  context. Default is no linger, i.e. any pending messages or connects will 
//  be lost.

void 
zctx_set_linger (zctx_t *self, int linger)
{
    assert (self);
    zstr_sendf (self->pipe, "LINGER=%d", linger);
}


//  --------------------------------------------------------------------------
//  Thread creation code, taken from ZFL's zfl_thread class and customized.

typedef struct {
    void *(*thread_fn) (void *);
    void *args;
#if defined (__WINDOWS__)
    HANDLE handle;
#endif
} shim_t;

#if defined (__UNIX__)
//  Thread shim for UNIX calls the real thread and cleans up afterwards.

void *
s_call_thread_fn (void *args)
{
    assert (args);
    shim_t *shim = (shim_t *) args;
    shim->thread_fn (shim->args);

    zthread_t *zthread = (zthread_t *) shim->args;
    zctx_destroy (&zthread->ctx);
    free (zthread);
    free (shim);
    return NULL;
}

#elif defined (__WINDOWS__)
//  Thread shim for Windows that wraps a POSIX-style thread handler
//  and does the _endthreadex for us automatically.

unsigned __stdcall 
s_call_thread_fn (void *args)
{
    assert (args);
    shim_t *shim = (shim_t *) args;
    shim->thread_fn (shim->args);
    _endthreadex (0);
    CloseHandle (shim->handle);
    
    zthread_t *zthread = (zthread_t *) shim->args;
    zctx_destroy (&zthread->ctx);
    free (zthread);
    free (shim);
    return 0;
}
#endif


//  --------------------------------------------------------------------------
//  Create a child thread able to speak to this thread over inproc sockets.
//  The child thread receives a zthread_t structure as argument. Returns a
//  PAIR socket that is connected to the child thread. You can ignore the
//  socket if you don't need it.

void *
zctx_thread_new (zctx_t *self, void *(*thread_fn) (void *), void *arg)
{
    //  This is what we're going to pass to the child thread
    zthread_t *args = (zthread_t *) zmalloc (sizeof (zthread_t));
    args->arg = arg;
    
    //  Create pipe from our thread to child
    void *pipe = zctx_socket_new (self, ZMQ_PAIR);
    args->pipe = zctx_socket_new (self, ZMQ_PAIR);
    char endpoint [64];
    int rc = snprintf (endpoint, 64, "inproc://zctx-pipe-%p", pipe);
    assert (rc < 64);
    rc = zmq_bind (pipe, endpoint);
    assert (rc == 0);
    rc = zmq_connect (args->pipe, endpoint);
    assert (rc == 0);

    //  Create clone of ourselves, for safe use by child thread
    //  We're going to create a new pipe through to the agent
    args->ctx = (zctx_t *) zmalloc (sizeof (zctx_t));
    args->ctx->context = self;
    args->ctx->pipe = zmq_socket (self->context, ZMQ_DEALER);
    rc = zmq_connect (args->ctx->pipe, self->endpoint);
    assert (rc == 0);

    //  Now start child thread, passing our arguments
    shim_t *shim = (shim_t *) zmalloc (sizeof (shim_t));
    shim->thread_fn = thread_fn;
    shim->args = args;
    
#if defined (__UNIX__)
    pthread_t thread;
    pthread_create (&thread, NULL, s_call_thread_fn, shim);
    pthread_detach (thread);
    
#elif defined (__WINDOWS__)
    shim->handle = (HANDLE)_beginthreadex(
        NULL,                   //  Handle is private to this process
        0,                      //  Use a default stack size for new thread
        &s_call_thread_fn,      //  Start real thread function via this shim
        shim,                   //  Which gets the current object as argument
        CREATE_SUSPENDED,       //  Set thread priority before starting it
        NULL);                  //  We don't use the thread ID

    assert (shim->handle);
    //  Set child thread priority to same as current
    int priority = GetThreadPriority (GetCurrentThread ());
    SetThreadPriority (shim->handle, priority);
    //  Now start thread
    ResumeThread (shim->handle);
#endif
    
    return pipe;
}


//  --------------------------------------------------------------------------
//  Create new 0MQ socket and register for this context. Use this instead of
//  the 0MQ core API method if you want automatic management of the socket 
//  at shutdown. If the context has not yet been initialized, this method will
//  initialize it.

void *
zctx_socket_new (zctx_t *self, int type)
{
    assert (self);
    zstr_sendf (self->pipe, "SOCKET=%d", type);
    char *reply = zstr_recv (self->pipe);
    void *socket;
    int rc = sscanf (reply, "%p", &socket);
    assert (rc == 1);
    free (reply);
    return socket;
}


//  --------------------------------------------------------------------------
//  Destroy the socket. You must use this for any socket created via the 
//  zctx_socket_new method.

void
zctx_socket_destroy (zctx_t *self, void *socket)
{
    assert (self);
    assert (socket);
    zstr_sendf (self->pipe, "CLOSE=%p", socket);
}


//  --------------------------------------------------------------------------
//  Selftest

static void *
s_test_thread (void *args_ptr)
{
    zthread_t *args = (zthread_t *) args_ptr;
    
    //  Create a socket to check it'll be properly deleted at exit
    zctx_socket_new (args->ctx, ZMQ_PUSH);
    
    //  Wait for our parent to ping us, and pong back
    char *ping = zstr_recv (args->pipe);
    free (ping);
    zstr_send (args->pipe, "pong");
    return NULL;
}


int
zctx_test (Bool verbose)
{
    printf (" * zctx: ");
    
    //  Create and destroy a context without using it
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    zctx_destroy (&ctx);
    assert (ctx == NULL);

    //  Create a context with many busy sockets, destroy it
    ctx = zctx_new ();
    zctx_set_iothreads (ctx, 1);
    zctx_set_linger (ctx, 5);       //  5 msecs
    void *s1 = zctx_socket_new (ctx, ZMQ_PAIR);
    void *s2 = zctx_socket_new (ctx, ZMQ_XREQ);
    void *s3 = zctx_socket_new (ctx, ZMQ_REQ);
    void *s4 = zctx_socket_new (ctx, ZMQ_REP);
    void *s5 = zctx_socket_new (ctx, ZMQ_PUB);
    void *s6 = zctx_socket_new (ctx, ZMQ_SUB);
    zmq_connect (s1, "tcp://127.0.0.1:5555");
    zmq_connect (s2, "tcp://127.0.0.1:5555");
    zmq_connect (s3, "tcp://127.0.0.1:5555");
    zmq_connect (s4, "tcp://127.0.0.1:5555");
    zmq_connect (s5, "tcp://127.0.0.1:5555");
    zmq_connect (s6, "tcp://127.0.0.1:5555");

    //  Create a child thread, check it's safely alive
    void *pipe = zctx_thread_new (ctx, s_test_thread, NULL);
    zstr_send (pipe, "ping");
    char *pong = zstr_recv (pipe);
    assert (streq (pong, "pong"));
    free (pong);

    //  Everything should be cleanly closed now
    zctx_destroy (&ctx);

    printf ("OK\n");
    return 0;
}
