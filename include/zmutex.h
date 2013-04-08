/*  =========================================================================
    zmutex - working with mutexes

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or (at
    your option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZMUTEX_H_INCLUDED__
#define __ZMUTEX_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zmutex_t zmutex_t;

//  @interface
//  Create a new mutex container
CZMQ_EXPORT zmutex_t *
    zmutex_new (void);

//  Destroy a mutex container
CZMQ_EXPORT void
    zmutex_destroy (zmutex_t **self_p);

//  Lock mutex
CZMQ_EXPORT void
    zmutex_lock (zmutex_t *self);

//  Unlock mutex
CZMQ_EXPORT void
    zmutex_unlock (zmutex_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zmutex_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
