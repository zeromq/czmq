/*  =========================================================================
    zpoller - trivial socket poller class

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

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

#ifndef __zpoller_H_INCLUDED__
#define __zpoller_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zpoller_t zpoller_t;

//  @interface
//  Create new poller
CZMQ_EXPORT zpoller_t *
    zpoller_new (void *reader, ...);

//  Destroy a poller
CZMQ_EXPORT void
    zpoller_destroy (zpoller_t **self_p);

//  Add a reader to be polled
CZMQ_EXPORT int
    zpoller_add (zpoller_t *self, void *reader);

//  Poll the registered readers for I/O, return first socket that has input.
//  This means the order that sockets are defined in the poll list affects
//  their priority. If you need a balanced poll, use the low level zmq_poll
//  method directly. If the poll call was interrupted (SIGINT), or the ZMQ
//  context was destroyed, or the timeout expired, returns NULL. You can
//  test the actual exit condition by calling zpoller_expired () and
//  zpoller_terminated (). Timeout is in msec.
CZMQ_EXPORT void *
    zpoller_wait (zpoller_t *self, int timeout);

//  Return true if the last zpoller_wait () call ended because the timeout
//  expired, without any error.
CZMQ_EXPORT bool
    zpoller_expired (zpoller_t *self);

//  Return true if the last zpoller_wait () call ended because the process
//  was interrupted, or the parent context was destroyed.
CZMQ_EXPORT bool
    zpoller_terminated (zpoller_t *self);

//  Self test of this class
CZMQ_EXPORT int
    zpoller_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif

