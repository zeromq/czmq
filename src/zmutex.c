/*  =========================================================================
    zmutex - working with mutexes

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
    zmutex_t
        *self;

    self = (zmutex_t *) zmalloc (sizeof (zmutex_t));
#if defined (__UNIX__)
    pthread_mutex_init (&self->mutex, NULL);
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
#if defined (__UNIX__)
    pthread_mutex_unlock (&self->mutex);
#elif defined (__WINDOWS__)
    LeaveCriticalSection (&self->mutex);
#endif
}


//  --------------------------------------------------------------------------
//  Selftest

int
zmutex_test (bool verbose)
{
    printf (" * zmutex: ");

    //  @selftest
    zmutex_t *mutex = zmutex_new ();
    zmutex_lock (mutex);
    zmutex_unlock (mutex);
    zmutex_destroy (&mutex);
    //  @end
    printf ("OK\n");
    return 0;
}
