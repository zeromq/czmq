/*  =========================================================================
    zmutex - working with mutexes (deprecated)

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
    The zmutex class provides a portable wrapper for mutexes. Please do not
    use this class to do multi-threading. It is for the rare case where you
    absolutely need thread-safe global state. This should happen in system
    code only. DO NOT USE THIS TO SHARE SOCKETS BETWEEN THREADS, OR DARK
    THINGS WILL HAPPEN TO YOUR CODE.
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zmutex_t {
#if defined (__UNIX__)
    pthread_mutex_t mutex;
#elif defined (__WINDOWS__)
    CRITICAL_SECTION mutex;
#endif
};


//  --------------------------------------------------------------------------
//  Constructor

zmutex_t *
zmutex_new (void)
{
    zmutex_t *self = (zmutex_t *) zmalloc (sizeof (zmutex_t));
    if (!self)
        return NULL;
#if defined (__UNIX__)
    if (pthread_mutex_init (&self->mutex, NULL) != 0) {
        free (self);
        return NULL;
    }
#elif defined (__WINDOWS__)
    InitializeCriticalSection (&self->mutex);
#endif
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zmutex_destroy (zmutex_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmutex_t *self = *self_p;
#if defined (__UNIX__)
        pthread_mutex_destroy (&self->mutex);
#elif defined (__WINDOWS__)
        DeleteCriticalSection (&self->mutex);
#endif
        free (self);
        *self_p = NULL;
    }
}



//  --------------------------------------------------------------------------
//  Lock mutex

void
zmutex_lock (zmutex_t *self)
{
    assert (self);
#if defined (__UNIX__)
    pthread_mutex_lock (&self->mutex);
#elif defined (__WINDOWS__)
    EnterCriticalSection (&self->mutex);
#endif
}


//  --------------------------------------------------------------------------
//  Unlock mutex

void
zmutex_unlock (zmutex_t *self)
{
    assert (self);
#if defined (__UNIX__)
    pthread_mutex_unlock (&self->mutex);
#elif defined (__WINDOWS__)
    LeaveCriticalSection (&self->mutex);
#endif
}

//  --------------------------------------------------------------------------
//  Try to lock mutex.
//  Returns:
//    0 if the mutex is already locked
//    1 if the mutex lock has successfully been acquired
//    -1 on error

int
zmutex_try_lock (zmutex_t *self)
{
    assert (self);
#if defined (__UNIX__)
    //  rc is either EBUSY or 0
    int rc = pthread_mutex_trylock (&self->mutex);
    return rc == EBUSY ? 0 : 1;
#elif defined (__WINDOWS__)
    //  rc is nonzero if the mutex lock has been acquired
    int rc = TryEnterCriticalSection (&self->mutex);
    return rc != 0 ? 1 : 0;
#endif
}

//  --------------------------------------------------------------------------
//  Selftest

void
zmutex_test (bool verbose)
{
    printf (" * zmutex (deprecated): ");

    //  @selftest
    zmutex_t *mutex = zmutex_new ();
    assert (mutex);
    zmutex_lock (mutex);
    zmutex_unlock (mutex);
    zmutex_destroy (&mutex);
    //  @end
    printf ("OK\n");
}
