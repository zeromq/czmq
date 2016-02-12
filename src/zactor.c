/*  =========================================================================
    zactor - simple actor framework

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The zactor class provides a simple actor framework. It replaces the
    CZMQ zthread class, which had a complex API that did not fit the CLASS
    standard. A CZMQ actor is implemented as a thread plus a PAIR-PAIR
    pipe. The constructor and destructor are always synchronized, so the
    caller can be sure all resources are created, and destroyed, when these
    calls complete. (This solves a major problem with zthread, that a caller
    could not be sure when a child thread had finished.)

    A zactor_t instance acts like a zsock_t and you can pass it to any CZMQ
    method that would take a zsock_t argument, including methods in zframe,
    zmsg, zstr, and zpoller. (zloop somehow escaped and needs catching.)

    An actor function MUST call zsock_signal (pipe) when initialized
    and MUST listen to pipe and exit on $TERM command.
@discuss
@end
*/

#include "czmq_classes.h"

//  zactor_t instances always have this tag as the first 4 octets of
//  their data, which lets us do runtime object typing & validation.
#define ZACTOR_TAG          0x0005cafe

//  Structure of our class

struct _zactor_t {
    uint32_t tag;               //  Object tag for runtime detection
    zsock_t *pipe;              //  Front-end pipe through to actor
};


//  This shims the OS thread APIs

typedef struct {
    zactor_fn *handler;
    zsock_t *pipe;              //  Pipe back to parent
    void *args;                 //  Application arguments
} shim_t;


//  --------------------------------------------------------------------------
//  Thread creation code, wrapping POSIX and Win32 thread APIs

#if defined (__UNIX__)
//  Thread shim for UNIX calls the actual thread and cleans up afterwards.
static void *
s_thread_shim (void *args)
{
    assert (args);
    shim_t *shim = (shim_t *) args;
    shim->handler (shim->pipe, shim->args);
    //  Do not block, if the other end of the pipe is already deleted
    zsock_set_sndtimeo (shim->pipe, 0);
    zsock_signal (shim->pipe, 0);
    zsock_destroy (&shim->pipe);
    free (shim);
    return NULL;
}

#elif defined (__WINDOWS__)
//  Thread shim for Windows that wraps a POSIX-style thread handler
//  and does the _endthreadex for us automatically.
static unsigned __stdcall
s_thread_shim (void *args)
{
    assert (args);
    shim_t *shim = (shim_t *) args;
    shim->handler (shim->pipe, shim->args);
    //  Do not block, if the other end of the pipe is already deleted
    zsock_set_sndtimeo (shim->pipe, 0);
    zsock_signal (shim->pipe, 0);
    zsock_destroy (&shim->pipe);
    free (shim);
    _endthreadex (0);           //  Terminates thread
    return 0;
}
#endif


//  --------------------------------------------------------------------------
//  Create a new actor.

zactor_t *
zactor_new (zactor_fn actor, void *args)
{
    zactor_t *self = (zactor_t *) zmalloc (sizeof (zactor_t));
    assert (self);
    self->tag = ZACTOR_TAG;

    shim_t *shim = (shim_t *) zmalloc (sizeof (shim_t));
    assert (shim);
    shim->pipe = zsys_create_pipe (&self->pipe);
    assert (shim->pipe);
    shim->handler = actor;
    shim->args = args;

#if defined (__UNIX__)
    pthread_t thread;
    pthread_create (&thread, NULL, s_thread_shim, shim);
    pthread_detach (thread);

#elif defined (__WINDOWS__)
    HANDLE handle = (HANDLE) _beginthreadex (
        NULL,                   //  Handle is private to this process
        0,                      //  Use a default stack size for new thread
        &s_thread_shim,         //  Start real thread function via this shim
        shim,                   //  Which gets arguments shim
        CREATE_SUSPENDED,       //  Set thread priority before starting it
        NULL);                  //  We don't use the thread ID
    assert (handle);

    //  Set child thread priority to same as current
    int priority = GetThreadPriority (GetCurrentThread ());
    SetThreadPriority (handle, priority);

    //  Start thread & release resources
    ResumeThread (handle);
    CloseHandle (handle);
#endif

    //  Mandatory handshake for new actor so that constructor returns only
    //  when actor has also initialized. This eliminates timing issues at
    //  application start up.
    zsock_wait (self->pipe);
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the actor.

void
zactor_destroy (zactor_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zactor_t *self = *self_p;
        assert (zactor_is (self));

        //  Signal the actor to end and wait for the thread exit code
        //  If the pipe isn't connected any longer, assume child thread
        //  has already quit due to other reasons and don't collect the
        //  exit signal.
        if (self->pipe) {
            zsock_set_sndtimeo (self->pipe, 0);
            if (zstr_send (self->pipe, "$TERM") == 0)
                zsock_wait (self->pipe);
            zsock_destroy (&self->pipe);
        }
        self->tag = 0xDeadBeef;
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Send a zmsg message to the actor, take ownership of the message
//  and destroy when it has been sent.

int
zactor_send (zactor_t *self, zmsg_t **msg_p)
{
    return zmsg_send (msg_p, self);
}


//  --------------------------------------------------------------------------
//  Receive a zmsg message from the actor. Returns NULL if the actor
//  was interrupted before the message could be received, or if there
//  was a timeout on the actor.

zmsg_t *
zactor_recv (zactor_t *self)
{
    return zmsg_recv (self);
}


//  --------------------------------------------------------------------------
//  Probe the supplied object, and report if it looks like a zactor_t.

bool
zactor_is (void *self)
{
    assert (self);
    return ((zactor_t *) self)->tag == ZACTOR_TAG;
}


//  --------------------------------------------------------------------------
//  Probe the supplied reference. If it looks like a zactor_t instance,
//  return the underlying libzmq actor handle; else if it looks like
//  a libzmq actor handle, return the supplied value.

void *
zactor_resolve (void *self)
{
    assert (self);
    if (zactor_is (self))
        return zsock_resolve (((zactor_t *) self)->pipe);
    else
        return self;
}


//  --------------------------------------------------------------------------
//  Return the actor's zsock handle. Use this when you absolutely need
//  to work with the zsock instance rather than the actor.

zsock_t *
zactor_sock (zactor_t *self)
{
    assert (self);
    assert (zactor_is (self));
    return self->pipe;
}


//  --------------------------------------------------------------------------
//  Actor
//  must call zsock_signal (pipe) when initialized
//  must listen to pipe and exit on $TERM command

static void
echo_actor (zsock_t *pipe, void *args)
{
    //  Do some initialization
    assert (streq ((char *) args, "Hello, World"));
    zsock_signal (pipe, 0);

    bool terminated = false;
    while (!terminated) {
        zmsg_t *msg = zmsg_recv (pipe);
        if (!msg)
            break;              //  Interrupted
        char *command = zmsg_popstr (msg);
        //  All actors must handle $TERM in this way
        if (streq (command, "$TERM"))
            terminated = true;
        else
        //  This is an example command for our test actor
        if (streq (command, "ECHO"))
            zmsg_send (&msg, pipe);
        else {
            puts ("E: invalid message to actor");
            assert (false);
        }
        free (command);
        zmsg_destroy (&msg);
    }
}


//  --------------------------------------------------------------------------
//  Selftest

void
zactor_test (bool verbose)
{
    printf (" * zactor: ");

    //  @selftest
    zactor_t *actor = zactor_new (echo_actor, "Hello, World");
    assert (actor);
    zstr_sendx (actor, "ECHO", "This is a string", NULL);
    char *string = zstr_recv (actor);
    assert (streq (string, "This is a string"));
    free (string);
    zactor_destroy (&actor);
    //  @end

    printf ("OK\n");
}
