/*  =========================================================================
    zsockopt - get/set 0MQ socket options

    GENERATED SOURCE CODE, DO NOT EDIT
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

#ifndef __ZSOCKOPT_H_INCLUDED__
#define __ZSOCKOPT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Set socket ZMQ_HWM value
void
    zsockopt_set_hwm (void *socket, int hwm);

//  Return socket ZMQ_HWM value
int
    zsockopt_hwm (void *socket);

//  Set socket ZMQ_SWAP value
void
    zsockopt_set_swap (void *socket, int swap);

//  Return socket ZMQ_SWAP value
int
    zsockopt_swap (void *socket);

//  Set socket ZMQ_AFFINITY value
void
    zsockopt_set_affinity (void *socket, int affinity);

//  Return socket ZMQ_AFFINITY value
int
    zsockopt_affinity (void *socket);

//  Set socket ZMQ_IDENTITY value
void
    zsockopt_set_identity (void *socket, char * identity);

//  Set socket ZMQ_RATE value
void
    zsockopt_set_rate (void *socket, int rate);

//  Return socket ZMQ_RATE value
int
    zsockopt_rate (void *socket);

//  Set socket ZMQ_RECOVERY_IVL value
void
    zsockopt_set_recovery_ivl (void *socket, int recovery_ivl);

//  Return socket ZMQ_RECOVERY_IVL value
int
    zsockopt_recovery_ivl (void *socket);

//  Set socket ZMQ_RECOVERY_IVL_MSEC value
void
    zsockopt_set_recovery_ivl_msec (void *socket, int recovery_ivl_msec);

//  Return socket ZMQ_RECOVERY_IVL_MSEC value
int
    zsockopt_recovery_ivl_msec (void *socket);

//  Set socket ZMQ_MCAST_LOOP value
void
    zsockopt_set_mcast_loop (void *socket, int mcast_loop);

//  Return socket ZMQ_MCAST_LOOP value
int
    zsockopt_mcast_loop (void *socket);

//  Set socket ZMQ_SNDBUF value
void
    zsockopt_set_sndbuf (void *socket, int sndbuf);

//  Return socket ZMQ_SNDBUF value
int
    zsockopt_sndbuf (void *socket);

//  Set socket ZMQ_RCVBUF value
void
    zsockopt_set_rcvbuf (void *socket, int rcvbuf);

//  Return socket ZMQ_RCVBUF value
int
    zsockopt_rcvbuf (void *socket);

//  Set socket ZMQ_LINGER value
void
    zsockopt_set_linger (void *socket, int linger);

//  Return socket ZMQ_LINGER value
int
    zsockopt_linger (void *socket);

//  Set socket ZMQ_RECONNECT_IVL value
void
    zsockopt_set_reconnect_ivl (void *socket, int reconnect_ivl);

//  Return socket ZMQ_RECONNECT_IVL value
int
    zsockopt_reconnect_ivl (void *socket);

//  Set socket ZMQ_RECONNECT_IVL_MAX value
void
    zsockopt_set_reconnect_ivl_max (void *socket, int reconnect_ivl_max);

//  Return socket ZMQ_RECONNECT_IVL_MAX value
int
    zsockopt_reconnect_ivl_max (void *socket);

//  Set socket ZMQ_BACKLOG value
void
    zsockopt_set_backlog (void *socket, int backlog);

//  Return socket ZMQ_BACKLOG value
int
    zsockopt_backlog (void *socket);

//  Set socket ZMQ_SUBSCRIBE value
void
    zsockopt_set_subscribe (void *socket, char * subscribe);

//  Set socket ZMQ_UNSUBSCRIBE value
void
    zsockopt_set_unsubscribe (void *socket, char * unsubscribe);

//  Return socket ZMQ_TYPE value
int
    zsockopt_type (void *socket);

//  Return socket ZMQ_RCVMORE value
int
    zsockopt_rcvmore (void *socket);

//  Return socket ZMQ_FD value
int
    zsockopt_fd (void *socket);

//  Return socket ZMQ_EVENTS value
int
    zsockopt_events (void *socket);

//  Self test of this class
int
    zsockopt_test (Bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
