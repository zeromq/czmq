/*  =========================================================================
    zloop - event-driven reactor

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of libzapi, the high-level C binding for 0MQ:
    http://libzapi.zeromq.org.

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

#ifndef __ZLOOP_H_INCLUDED__
#define __ZLOOP_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zloop_t zloop_t;

//  @interface
//  Callback function for reactor events
typedef int (zloop_fn) (zloop_t *loop, void *socket, void *arg);

//  Create a new zloop reactor
zloop_t *
    zloop_new (void);

//  Destroy a reactor
void
    zloop_destroy (zloop_t **self_p);

//  Register a socket reader, on one socket
int
    zloop_reader (zloop_t *self, void *socket, zloop_fn handler, void *arg);

//  Cancel the reader on the specified socket, if any
void
    zloop_cancel (zloop_t *self, void *socket);

//  Register a timer that will go off after 'delay' msecs, and will
//  repeat 'times' times, unless 'times' is zero, meaning repeat forever.
int
    zloop_timer (zloop_t *self, size_t delay, size_t times, zloop_fn handler, void *arg);

//  Set verbose tracing of reactor on/off
void
    zloop_set_verbose (zloop_t *self, Bool verbose);

//  Start the reactor, ends if a callback function returns -1, or the process
//  received SIGINT or SIGTERM.
int
    zloop_start (zloop_t *self);

//  Self test of this class
int
    zloop_test (Bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
