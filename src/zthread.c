/*  =========================================================================
    zthread - working with system threads

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the 
    Free Software Foundation; either version 3 of the License, or (at your 
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General 
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License 
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

/*
@header
    The zthread class wraps OS thread creation. It creates detached threads
    that look like normal OS threads, or attached threads that share the
    caller's 0MQ context, and get an inproc pipe to talk back to the parent
    thread. Detached threads create their own 0MQ contexts as needed.
@discuss
    We have several use cases for multiple threads. One is to simulate many
    processes, so we can test 0MQ designs and flows more easily. Another is
    to create APIs that can send and receive 0MQ messages in the background.

    zthread solves these two use cases separately, using the zthread_new
    and zthead_fork methods respectively. These methods wrap the native
    system calls needed to start threads, so your code can remain fully
    portable.

    Detached threads follow the POSIX pthreads API; they accept a void *
    argument and return a void * result (always NULL in our case).

    Attached thread receive a void * argument, a zctx_t context, and a pipe
    socket. The pipe socket is a PAIR socket that is connected back to the
    caller. When you call zthread_fork, it returns you a PAIR socket that
    is the other end of this pipe. Thus attached threads can talk back to
    their parent threads over the pipe. We use this very heavily when making
    so-called "asynchronous" APIs, which you can see in the Guide examples
    like 'clone'.

    To recap some rules about threading: do not share sockets between
    threads or your code will crash. You can migrate a socket from one
    thread to a child thread, if you stop using it in the parent thread
    immediately after creating the child thread. If you want to connect
    sockets over inproc:// they must share the same 0MQ context, i.e. be
    attached threads. You should always use zthread_fork to create an
    attached thread; it is not sufficient to pass a zctx_t structure to
    a detached thread (this will crash).

    If you want to communicate over ipc:// or tcp:// you may be sharing
    the same context, or use separate contexts. Thus, every detached thread
    usually starts by creating its own zctx_t instance.    
@end
*/

#include "../include/czmq.h"

//  --------------------------------------------------------------------------
//  Thread creation code, wrapping POSIX and Win32 thread APIs

typedef struct {
    //  Two thread handlers, one will be set, one NULL
    zthread_attached_fn *attached;
    zthread_detached_fn *detached;
    void *args;                 //  Application arguments
    zctx_t *ctx;                //  Context object if any
    void *pipe;                 //  Pipe, if any, back to parent
#if defined (__WINDOWS__)
    HANDLE handle;              //  Win32 thread handle
#endif
} shim_t;

#if defined (__UNIX__)
//  Thread shim for UNIX calls the real thread and cleans up afterwards.
void *
s_thread_shim (void *args)
{
    assert (args);
    shim_t *shim = (shim_t *) args;
    if (shim->attached)
        shim->attached (shim->args, shim->ctx, shim->pipe);
    else
        shim->detached (shim->args);

    zctx_destroy (&shim->ctx);
    free (shim);
    return NULL;
}

#elif defined (__WINDOWS__)
//  Thread shim for Windows that wraps a POSIX-style thread handler
//  and does the _endthreadex for us automatically.
unsigned __stdcall
s_thread_shim (void *args)
{
    assert (args);
    shim_t *shim = (shim_t *) args;
    if (shim->attached)
        shim->attached (shim->args, shim->ctx, shim->pipe);
    else
        shim->detached (shim->args);

    zctx_destroy (&shim->ctx);  //  Close any dangling sockets
    free (shim);
    _endthreadex (0);           //  Terminates thread
    return 0;
}
#endif

static void
s_thread_start (shim_t *shim)
{
#if defined (__UNIX__)
    pthread_t thread;
    pthread_create (&thread, NULL, s_thread_shim, shim);
    pthread_detach (thread);

#elif defined (__WINDOWS__)
    shim->handle = (HANDLE)_beginthreadex(
        NULL,                   //  Handle is private to this process
        0,                      //  Use a default stack size for new thread
        &s_thread_shim,         //  Start real thread function via this shim
        shim,                   //  Which gets the current object as argument
        CREATE_SUSPENDED,       //  Set thread priority before starting it
        NULL);                  //  We don't use the thread ID

    assert (shim->handle);
    //  Set child thread priority to same as current
    int priority = GetThreadPriority (GetCurrentThread ());
    SetThreadPriority (shim->handle, priority);
    //  Start thread
    ResumeThread (shim->handle);
    //  Release resources
    CloseHandle (shim->handle);
    shim->handle = 0;
#endif
}


//  --------------------------------------------------------------------------
//  Create a detached thread. A detached thread operates autonomously
//  and is used to simulate a separate process. It gets no ctx, and no
//  pipe. Returns 0 on success, -1 if insufficient memory.

int
zthread_new (zthread_detached_fn *thread_fn, void *args)
{
    //  Prepare argument shim for child thread
    shim_t *shim = (shim_t *) zmalloc (sizeof (shim_t));
    if (shim) {
        shim->detached = thread_fn;
        shim->args = args;
        s_thread_start (shim);
        return 0;
    }
    return -1;
}


//  --------------------------------------------------------------------------
//  Create an attached thread. An attached thread gets a ctx and a PAIR
//  pipe back to its parent. It must monitor its pipe, and exit if the
//  pipe becomes unreadable. Returns pipe, or NULL if there was an error.

void *
zthread_fork (zctx_t *ctx, zthread_attached_fn *thread_fn, void *args)
{
    shim_t *shim = NULL;
    //  Create our end of the pipe
    void *pipe = zctx__socket_pipe (ctx);
    if (pipe)
        zsocket_bind (pipe, "inproc://zctx-pipe-%p", pipe);
    else
        return NULL;
    
    //  Prepare argument shim for child thread
    shim = (shim_t *) zmalloc (sizeof (shim_t));
    if (shim) {
        shim->attached = thread_fn;
        shim->args = args;
        shim->ctx = zctx_shadow (ctx);
        if (!shim->ctx)
            return NULL;
    }
    else
        return NULL;
    
    //  Connect child pipe to our pipe
    shim->pipe = zctx__socket_pipe (shim->ctx);
    if (!shim->pipe)
        return NULL;
    zsocket_connect (shim->pipe, "inproc://zctx-pipe-%p", pipe);
    
    s_thread_start (shim);
    return pipe;
}


//  --------------------------------------------------------------------------
//  Selftest

//  @selftest
static void *
s_test_detached (void *args)
{
    //  Create a socket to check it'll be automatically deleted
    zctx_t *ctx = zctx_new ();
    assert (ctx);

    void *push = zsocket_new (ctx, ZMQ_PUSH);
    assert (push);
    zctx_destroy (&ctx);
    return NULL;
}

static void
s_test_attached (void *args, zctx_t *ctx, void *pipe)
{
    //  Create a socket to check it'll be automatically deleted
    zsocket_new (ctx, ZMQ_PUSH);
    //  Wait for our parent to ping us, and pong back
    free (zstr_recv (pipe));
    zstr_send (pipe, "pong");
}

//  @end

int
zthread_test (bool verbose)
{
    printf (" * zthread: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    int rc = 0;

    //  Create a detached thread, let it run
    rc = zthread_new (s_test_detached, NULL);
    assert (rc == 0);
    zclock_sleep (100);

    //  Create an attached thread, check it's safely alive
    void *pipe = zthread_fork (ctx, s_test_attached, NULL);
    assert (pipe);
    zstr_send (pipe, "ping");
    char *pong = zstr_recv (pipe);
    assert (streq (pong, "pong"));
    free (pong);

    //  Everything should be cleanly closed now
    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
