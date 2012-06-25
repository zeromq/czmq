/*  =========================================================================
    zsockopt - get/set 0MQ socket options

    GENERATED SOURCE CODE, DO NOT EDIT
    -------------------------------------------------------------------------
    Copyright (c) 1991-2012 iMatix Corporation <www.imatix.com>
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

//  @interface
#if (ZMQ_VERSION_MAJOR == 2)
//  Get socket options
int zsocket_hwm (void *socket);
int zsocket_swap (void *socket);
int zsocket_affinity (void *socket);
//  Returns freshly allocated string, free when done
char * zsocket_identity (void *socket);
int zsocket_rate (void *socket);
int zsocket_recovery_ivl (void *socket);
int zsocket_recovery_ivl_msec (void *socket);
int zsocket_mcast_loop (void *socket);
#   if (ZMQ_VERSION_MINOR == 2)
int zsocket_rcvtimeo (void *socket);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
int zsocket_sndtimeo (void *socket);
#   endif
int zsocket_sndbuf (void *socket);
int zsocket_rcvbuf (void *socket);
int zsocket_linger (void *socket);
int zsocket_reconnect_ivl (void *socket);
int zsocket_reconnect_ivl_max (void *socket);
int zsocket_backlog (void *socket);
int zsocket_type (void *socket);
int zsocket_rcvmore (void *socket);
int zsocket_fd (void *socket);
int zsocket_events (void *socket);

//  Set socket options
void zsocket_set_hwm (void *socket, int hwm);
void zsocket_set_swap (void *socket, int swap);
void zsocket_set_affinity (void *socket, int affinity);
void zsocket_set_identity (void *socket, char * identity);
void zsocket_set_rate (void *socket, int rate);
void zsocket_set_recovery_ivl (void *socket, int recovery_ivl);
void zsocket_set_recovery_ivl_msec (void *socket, int recovery_ivl_msec);
void zsocket_set_mcast_loop (void *socket, int mcast_loop);
#   if (ZMQ_VERSION_MINOR == 2)
void zsocket_set_rcvtimeo (void *socket, int rcvtimeo);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
void zsocket_set_sndtimeo (void *socket, int sndtimeo);
#   endif
void zsocket_set_sndbuf (void *socket, int sndbuf);
void zsocket_set_rcvbuf (void *socket, int rcvbuf);
void zsocket_set_linger (void *socket, int linger);
void zsocket_set_reconnect_ivl (void *socket, int reconnect_ivl);
void zsocket_set_reconnect_ivl_max (void *socket, int reconnect_ivl_max);
void zsocket_set_backlog (void *socket, int backlog);
void zsocket_set_subscribe (void *socket, char * subscribe);
void zsocket_set_unsubscribe (void *socket, char * unsubscribe);
#endif

#if (ZMQ_VERSION_MAJOR == 3)
//  Get socket options
int zsocket_type (void *socket);
int zsocket_sndhwm (void *socket);
int zsocket_rcvhwm (void *socket);
int zsocket_affinity (void *socket);
//  Returns freshly allocated string, free when done
char * zsocket_identity (void *socket);
int zsocket_rate (void *socket);
int zsocket_recovery_ivl (void *socket);
int zsocket_sndbuf (void *socket);
int zsocket_rcvbuf (void *socket);
int zsocket_linger (void *socket);
int zsocket_reconnect_ivl (void *socket);
int zsocket_reconnect_ivl_max (void *socket);
int zsocket_backlog (void *socket);
int zsocket_maxmsgsize (void *socket);
int zsocket_multicast_hops (void *socket);
int zsocket_rcvtimeo (void *socket);
int zsocket_sndtimeo (void *socket);
int zsocket_ipv4only (void *socket);
int zsocket_rcvmore (void *socket);
int zsocket_fd (void *socket);
int zsocket_events (void *socket);
//  Returns freshly allocated string, free when done
char * zsocket_last_endpoint (void *socket);

//  Set socket options
void zsocket_set_sndhwm (void *socket, int sndhwm);
void zsocket_set_rcvhwm (void *socket, int rcvhwm);
void zsocket_set_affinity (void *socket, int affinity);
void zsocket_set_subscribe (void *socket, char * subscribe);
void zsocket_set_unsubscribe (void *socket, char * unsubscribe);
void zsocket_set_identity (void *socket, char * identity);
void zsocket_set_rate (void *socket, int rate);
void zsocket_set_recovery_ivl (void *socket, int recovery_ivl);
void zsocket_set_sndbuf (void *socket, int sndbuf);
void zsocket_set_rcvbuf (void *socket, int rcvbuf);
void zsocket_set_linger (void *socket, int linger);
void zsocket_set_reconnect_ivl (void *socket, int reconnect_ivl);
void zsocket_set_reconnect_ivl_max (void *socket, int reconnect_ivl_max);
void zsocket_set_backlog (void *socket, int backlog);
void zsocket_set_maxmsgsize (void *socket, int maxmsgsize);
void zsocket_set_multicast_hops (void *socket, int multicast_hops);
void zsocket_set_rcvtimeo (void *socket, int rcvtimeo);
void zsocket_set_sndtimeo (void *socket, int sndtimeo);
void zsocket_set_ipv4only (void *socket, int ipv4only);
void zsocket_set_router_behavior (void *socket, int router_behavior);

//  Emulation of widely-used 2.x socket options
void zsocket_set_hwm (void *socket, int hwm);
#endif

//  Self test of this class
int zsockopt_test (Bool verbose);
//  @end

//  Deprecated function names
#if (ZMQ_VERSION_MAJOR == 2)
#define zsockopt_hwm zsocket_hwm
#define zsockopt_set_hwm zsocket_set_hwm
#define zsockopt_swap zsocket_swap
#define zsockopt_set_swap zsocket_set_swap
#define zsockopt_affinity zsocket_affinity
#define zsockopt_set_affinity zsocket_set_affinity
#define zsockopt_identity zsocket_identity
#define zsockopt_set_identity zsocket_set_identity
#define zsockopt_rate zsocket_rate
#define zsockopt_set_rate zsocket_set_rate
#define zsockopt_recovery_ivl zsocket_recovery_ivl
#define zsockopt_set_recovery_ivl zsocket_set_recovery_ivl
#define zsockopt_recovery_ivl_msec zsocket_recovery_ivl_msec
#define zsockopt_set_recovery_ivl_msec zsocket_set_recovery_ivl_msec
#define zsockopt_mcast_loop zsocket_mcast_loop
#define zsockopt_set_mcast_loop zsocket_set_mcast_loop
#   if (ZMQ_VERSION_MINOR == 2)
#define zsockopt_rcvtimeo zsocket_rcvtimeo
#define zsockopt_set_rcvtimeo zsocket_set_rcvtimeo
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
#define zsockopt_sndtimeo zsocket_sndtimeo
#define zsockopt_set_sndtimeo zsocket_set_sndtimeo
#   endif
#define zsockopt_sndbuf zsocket_sndbuf
#define zsockopt_set_sndbuf zsocket_set_sndbuf
#define zsockopt_rcvbuf zsocket_rcvbuf
#define zsockopt_set_rcvbuf zsocket_set_rcvbuf
#define zsockopt_linger zsocket_linger
#define zsockopt_set_linger zsocket_set_linger
#define zsockopt_reconnect_ivl zsocket_reconnect_ivl
#define zsockopt_set_reconnect_ivl zsocket_set_reconnect_ivl
#define zsockopt_reconnect_ivl_max zsocket_reconnect_ivl_max
#define zsockopt_set_reconnect_ivl_max zsocket_set_reconnect_ivl_max
#define zsockopt_backlog zsocket_backlog
#define zsockopt_set_backlog zsocket_set_backlog
#define zsockopt_set_subscribe zsocket_set_subscribe
#define zsockopt_set_unsubscribe zsocket_set_unsubscribe
#define zsockopt_type zsocket_type
#define zsockopt_rcvmore zsocket_rcvmore
#define zsockopt_fd zsocket_fd
#define zsockopt_events zsocket_events
#endif

//  Deprecated function names
#if (ZMQ_VERSION_MAJOR == 3)
#define zsockopt_type zsocket_type
#define zsockopt_sndhwm zsocket_sndhwm
#define zsockopt_set_sndhwm zsocket_set_sndhwm
#define zsockopt_rcvhwm zsocket_rcvhwm
#define zsockopt_set_rcvhwm zsocket_set_rcvhwm
#define zsockopt_affinity zsocket_affinity
#define zsockopt_set_affinity zsocket_set_affinity
#define zsockopt_set_subscribe zsocket_set_subscribe
#define zsockopt_set_unsubscribe zsocket_set_unsubscribe
#define zsockopt_identity zsocket_identity
#define zsockopt_set_identity zsocket_set_identity
#define zsockopt_rate zsocket_rate
#define zsockopt_set_rate zsocket_set_rate
#define zsockopt_recovery_ivl zsocket_recovery_ivl
#define zsockopt_set_recovery_ivl zsocket_set_recovery_ivl
#define zsockopt_sndbuf zsocket_sndbuf
#define zsockopt_set_sndbuf zsocket_set_sndbuf
#define zsockopt_rcvbuf zsocket_rcvbuf
#define zsockopt_set_rcvbuf zsocket_set_rcvbuf
#define zsockopt_linger zsocket_linger
#define zsockopt_set_linger zsocket_set_linger
#define zsockopt_reconnect_ivl zsocket_reconnect_ivl
#define zsockopt_set_reconnect_ivl zsocket_set_reconnect_ivl
#define zsockopt_reconnect_ivl_max zsocket_reconnect_ivl_max
#define zsockopt_set_reconnect_ivl_max zsocket_set_reconnect_ivl_max
#define zsockopt_backlog zsocket_backlog
#define zsockopt_set_backlog zsocket_set_backlog
#define zsockopt_maxmsgsize zsocket_maxmsgsize
#define zsockopt_set_maxmsgsize zsocket_set_maxmsgsize
#define zsockopt_multicast_hops zsocket_multicast_hops
#define zsockopt_set_multicast_hops zsocket_set_multicast_hops
#define zsockopt_rcvtimeo zsocket_rcvtimeo
#define zsockopt_set_rcvtimeo zsocket_set_rcvtimeo
#define zsockopt_sndtimeo zsocket_sndtimeo
#define zsockopt_set_sndtimeo zsocket_set_sndtimeo
#define zsockopt_ipv4only zsocket_ipv4only
#define zsockopt_set_ipv4only zsocket_set_ipv4only
#define zsockopt_set_router_behavior zsocket_set_router_behavior
#define zsockopt_rcvmore zsocket_rcvmore
#define zsockopt_fd zsocket_fd
#define zsockopt_events zsocket_events
#define zsockopt_last_endpoint zsocket_last_endpoint
#endif


#ifdef __cplusplus
}
#endif

#endif
