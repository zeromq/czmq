/*  =========================================================================
    zsockopt - get/set 0MQ socket options

    GENERATED SOURCE CODE, DO NOT EDIT
    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
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

#ifndef __ZSOCKOPT_H_INCLUDED__
#define __ZSOCKOPT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

// Uniform zsocket_set_* API

#define zsocket_set_hwm zsockopt_set_hwm
#define zsocket_set_swap zsockopt_set_swap
#define zsocket_set_affinity zsockopt_set_affinity
#define zsocket_set_identity zsockopt_set_identity
#define zsocket_set_rate zsockopt_set_rate
#define zsocket_set_recovery_ivl zsockopt_set_recovery_ivl
#define zsocket_set_recovery_ivl_msec zsockopt_set_recovery_ivl_msec
#define zsocket_set_mcast_loop zsockopt_set_mcast_loop
#define zsocket_set_sndbuf zsockopt_set_sndbuf
#define zsocket_set_rcvbuf zsockopt_set_rcvbuf
#define zsocket_set_linger zsockopt_set_linger
#define zsocket_set_reconnect_ivl zsockopt_set_reconnect_ivl
#define zsocket_set_reconnect_ivl_max zsockopt_set_reconnect_ivl_max
#define zsocket_set_backlog zsockopt_set_backlog
#define zsocket_set_subscribe zsockopt_set_subscribe
#define zsocket_set_unsubscribe zsockopt_set_unsubscribe
#define zsocket_set_sndhwm zsockopt_set_sndhwm
#define zsocket_set_rcvhwm zsockopt_set_rcvhwm
#define zsocket_set_backlog zsockopt_set_backlog
#define zsocket_set_maxmsgsize zsockopt_set_maxmsgsize

// Uniform zsocket_* API

#define zsocket_hwm zsockopt_hwm
#define zsocket_swap zsockopt_swap
#define zsocket_affinity zsockopt_affinity
#define zsocket_identity zsockopt_identity
#define zsocket_rate zsockopt_rate
#define zsocket_recovery_ivl zsockopt_recovery_ivl
#define zsocket_recovery_ivl_msec zsockopt_recovery_ivl_msec
#define zsocket_mcast_loop zsockopt_mcast_loop
#define zsocket_sndbuf zsockopt_sndbuf
#define zsocket_rcvbuf zsockopt_rcvbuf
#define zsocket_linger zsockopt_linger
#define zsocket_reconnect_ivl zsockopt_reconnect_ivl
#define zsocket_reconnect_ivl_max zsockopt_reconnect_ivl_max
#define zsocket_backlog zsockopt_backlog
#define zsocket_subscribe zsockopt_subscribe
#define zsocket_unsubscribe zsockopt_unsubscribe
#define zsocket_sndhwm zsockopt_sndhwm
#define zsocket_rcvhwm zsockopt_rcvhwm
#define zsocket_backlog zsockopt_backlog
#define zsocket_maxmsgsize zsockopt_maxmsgsize
#define zsocket_events zsocketopt_events
#define zsocket_rcvmore zsocketopt_rcvmore
#define zsocket_type zsocketopt_type
#define zsocket_fd zsocketopt_fd

//  @interface
#if (ZMQ_VERSION_MAJOR == 2)
//  Get socket options
int  zsockopt_hwm (void *socket);
int  zsockopt_swap (void *socket);
int  zsockopt_affinity (void *socket);
int  zsockopt_rate (void *socket);
int  zsockopt_recovery_ivl (void *socket);
int  zsockopt_recovery_ivl_msec (void *socket);
int  zsockopt_mcast_loop (void *socket);
int  zsockopt_sndbuf (void *socket);
int  zsockopt_rcvbuf (void *socket);
int  zsockopt_linger (void *socket);
int  zsockopt_reconnect_ivl (void *socket);
int  zsockopt_reconnect_ivl_max (void *socket);
int  zsockopt_backlog (void *socket);
int  zsockopt_type (void *socket);
int  zsockopt_rcvmore (void *socket);
int  zsockopt_fd (void *socket);
int  zsockopt_events (void *socket);

//  Set socket options
void zsockopt_set_hwm (void *socket, int hwm);
void zsockopt_set_swap (void *socket, int swap);
void zsockopt_set_affinity (void *socket, int affinity);
void zsockopt_set_identity (void *socket, char * identity);
void zsockopt_set_rate (void *socket, int rate);
void zsockopt_set_recovery_ivl (void *socket, int recovery_ivl);
void zsockopt_set_recovery_ivl_msec (void *socket, int recovery_ivl_msec);
void zsockopt_set_mcast_loop (void *socket, int mcast_loop);
void zsockopt_set_sndbuf (void *socket, int sndbuf);
void zsockopt_set_rcvbuf (void *socket, int rcvbuf);
void zsockopt_set_linger (void *socket, int linger);
void zsockopt_set_reconnect_ivl (void *socket, int reconnect_ivl);
void zsockopt_set_reconnect_ivl_max (void *socket, int reconnect_ivl_max);
void zsockopt_set_backlog (void *socket, int backlog);
void zsockopt_set_subscribe (void *socket, char * subscribe);
void zsockopt_set_unsubscribe (void *socket, char * unsubscribe);
#endif

#if (ZMQ_VERSION_MAJOR == 3)
//  Get socket options
int  zsockopt_sndhwm (void *socket);
int  zsockopt_rcvhwm (void *socket);
int  zsockopt_affinity (void *socket);
int  zsockopt_rate (void *socket);
int  zsockopt_recovery_ivl (void *socket);
int  zsockopt_sndbuf (void *socket);
int  zsockopt_rcvbuf (void *socket);
int  zsockopt_linger (void *socket);
int  zsockopt_reconnect_ivl (void *socket);
int  zsockopt_reconnect_ivl_max (void *socket);
int  zsockopt_backlog (void *socket);
int  zsockopt_maxmsgsize (void *socket);
int  zsockopt_type (void *socket);
int  zsockopt_rcvmore (void *socket);
int  zsockopt_fd (void *socket);
int  zsockopt_events (void *socket);

//  Set socket options
void zsockopt_set_sndhwm (void *socket, int sndhwm);
void zsockopt_set_rcvhwm (void *socket, int rcvhwm);
void zsockopt_set_affinity (void *socket, int affinity);
void zsockopt_set_identity (void *socket, char * identity);
void zsockopt_set_rate (void *socket, int rate);
void zsockopt_set_recovery_ivl (void *socket, int recovery_ivl);
void zsockopt_set_sndbuf (void *socket, int sndbuf);
void zsockopt_set_rcvbuf (void *socket, int rcvbuf);
void zsockopt_set_linger (void *socket, int linger);
void zsockopt_set_reconnect_ivl (void *socket, int reconnect_ivl);
void zsockopt_set_reconnect_ivl_max (void *socket, int reconnect_ivl_max);
void zsockopt_set_backlog (void *socket, int backlog);
void zsockopt_set_maxmsgsize (void *socket, int maxmsgsize);
void zsockopt_set_subscribe (void *socket, char * subscribe);
void zsockopt_set_unsubscribe (void *socket, char * unsubscribe);

//  Emulation of widely-used 2.x socket options
void zsockopt_set_hwm (void *socket, int hwm);
#endif

#if (ZMQ_VERSION_MAJOR == 4)
//  Get socket options
int  zsockopt_sndhwm (void *socket);
int  zsockopt_rcvhwm (void *socket);
int  zsockopt_affinity (void *socket);
int  zsockopt_rate (void *socket);
int  zsockopt_recovery_ivl (void *socket);
int  zsockopt_sndbuf (void *socket);
int  zsockopt_rcvbuf (void *socket);
int  zsockopt_linger (void *socket);
int  zsockopt_reconnect_ivl (void *socket);
int  zsockopt_reconnect_ivl_max (void *socket);
int  zsockopt_backlog (void *socket);
int  zsockopt_maxmsgsize (void *socket);
int  zsockopt_type (void *socket);
int  zsockopt_rcvmore (void *socket);
int  zsockopt_fd (void *socket);
int  zsockopt_events (void *socket);

//  Set socket options
void zsockopt_set_sndhwm (void *socket, int sndhwm);
void zsockopt_set_rcvhwm (void *socket, int rcvhwm);
void zsockopt_set_affinity (void *socket, int affinity);
void zsockopt_set_rate (void *socket, int rate);
void zsockopt_set_recovery_ivl (void *socket, int recovery_ivl);
void zsockopt_set_sndbuf (void *socket, int sndbuf);
void zsockopt_set_rcvbuf (void *socket, int rcvbuf);
void zsockopt_set_linger (void *socket, int linger);
void zsockopt_set_reconnect_ivl (void *socket, int reconnect_ivl);
void zsockopt_set_reconnect_ivl_max (void *socket, int reconnect_ivl_max);
void zsockopt_set_backlog (void *socket, int backlog);
void zsockopt_set_maxmsgsize (void *socket, int maxmsgsize);
void zsockopt_set_subscribe (void *socket, char * subscribe);
void zsockopt_set_unsubscribe (void *socket, char * unsubscribe);

//  Emulation of widely-used 2.x socket options
void zsockopt_set_hwm (void *socket, int hwm);
#endif

//  Self test of this class
int zsockopt_test (Bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
