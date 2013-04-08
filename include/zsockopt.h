/*  =========================================================================
    zsockopt - get/set 0MQ socket options

    GENERATED SOURCE CODE, DO NOT EDIT
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

#ifndef __ZSOCKOPT_H_INCLUDED__
#define __ZSOCKOPT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
#if (ZMQ_VERSION_MAJOR == 2)
//  Get socket options
CZMQ_EXPORT int zsocket_hwm (void *zocket);
CZMQ_EXPORT int zsocket_swap (void *zocket);
CZMQ_EXPORT int zsocket_affinity (void *zocket);
//  Returns freshly allocated string, free when done
CZMQ_EXPORT char * zsocket_identity (void *zocket);
CZMQ_EXPORT int zsocket_rate (void *zocket);
CZMQ_EXPORT int zsocket_recovery_ivl (void *zocket);
CZMQ_EXPORT int zsocket_recovery_ivl_msec (void *zocket);
CZMQ_EXPORT int zsocket_mcast_loop (void *zocket);
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT int zsocket_rcvtimeo (void *zocket);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT int zsocket_sndtimeo (void *zocket);
#   endif
CZMQ_EXPORT int zsocket_sndbuf (void *zocket);
CZMQ_EXPORT int zsocket_rcvbuf (void *zocket);
CZMQ_EXPORT int zsocket_linger (void *zocket);
CZMQ_EXPORT int zsocket_reconnect_ivl (void *zocket);
CZMQ_EXPORT int zsocket_reconnect_ivl_max (void *zocket);
CZMQ_EXPORT int zsocket_backlog (void *zocket);
CZMQ_EXPORT int zsocket_type (void *zocket);
CZMQ_EXPORT int zsocket_rcvmore (void *zocket);
CZMQ_EXPORT int zsocket_fd (void *zocket);
CZMQ_EXPORT int zsocket_events (void *zocket);

//  Set socket options
CZMQ_EXPORT void zsocket_set_hwm (void *zocket, int hwm);
CZMQ_EXPORT void zsocket_set_swap (void *zocket, int swap);
CZMQ_EXPORT void zsocket_set_affinity (void *zocket, int affinity);
CZMQ_EXPORT void zsocket_set_identity (void *zocket, char * identity);
CZMQ_EXPORT void zsocket_set_rate (void *zocket, int rate);
CZMQ_EXPORT void zsocket_set_recovery_ivl (void *zocket, int recovery_ivl);
CZMQ_EXPORT void zsocket_set_recovery_ivl_msec (void *zocket, int recovery_ivl_msec);
CZMQ_EXPORT void zsocket_set_mcast_loop (void *zocket, int mcast_loop);
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT void zsocket_set_rcvtimeo (void *zocket, int rcvtimeo);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT void zsocket_set_sndtimeo (void *zocket, int sndtimeo);
#   endif
CZMQ_EXPORT void zsocket_set_sndbuf (void *zocket, int sndbuf);
CZMQ_EXPORT void zsocket_set_rcvbuf (void *zocket, int rcvbuf);
CZMQ_EXPORT void zsocket_set_linger (void *zocket, int linger);
CZMQ_EXPORT void zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl);
CZMQ_EXPORT void zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max);
CZMQ_EXPORT void zsocket_set_backlog (void *zocket, int backlog);
CZMQ_EXPORT void zsocket_set_subscribe (void *zocket, char * subscribe);
CZMQ_EXPORT void zsocket_set_unsubscribe (void *zocket, char * unsubscribe);
#endif

#if (ZMQ_VERSION_MAJOR == 3)
//  Get socket options
CZMQ_EXPORT int zsocket_type (void *zocket);
CZMQ_EXPORT int zsocket_sndhwm (void *zocket);
CZMQ_EXPORT int zsocket_rcvhwm (void *zocket);
CZMQ_EXPORT int zsocket_affinity (void *zocket);
//  Returns freshly allocated string, free when done
CZMQ_EXPORT char * zsocket_identity (void *zocket);
CZMQ_EXPORT int zsocket_rate (void *zocket);
CZMQ_EXPORT int zsocket_recovery_ivl (void *zocket);
CZMQ_EXPORT int zsocket_sndbuf (void *zocket);
CZMQ_EXPORT int zsocket_rcvbuf (void *zocket);
CZMQ_EXPORT int zsocket_linger (void *zocket);
CZMQ_EXPORT int zsocket_reconnect_ivl (void *zocket);
CZMQ_EXPORT int zsocket_reconnect_ivl_max (void *zocket);
CZMQ_EXPORT int zsocket_backlog (void *zocket);
CZMQ_EXPORT int zsocket_maxmsgsize (void *zocket);
CZMQ_EXPORT int zsocket_multicast_hops (void *zocket);
CZMQ_EXPORT int zsocket_rcvtimeo (void *zocket);
CZMQ_EXPORT int zsocket_sndtimeo (void *zocket);
CZMQ_EXPORT int zsocket_ipv4only (void *zocket);
CZMQ_EXPORT int zsocket_rcvmore (void *zocket);
CZMQ_EXPORT int zsocket_fd (void *zocket);
CZMQ_EXPORT int zsocket_events (void *zocket);
//  Returns freshly allocated string, free when done
CZMQ_EXPORT char * zsocket_last_endpoint (void *zocket);

//  Set socket options
CZMQ_EXPORT void zsocket_set_sndhwm (void *zocket, int sndhwm);
CZMQ_EXPORT void zsocket_set_rcvhwm (void *zocket, int rcvhwm);
CZMQ_EXPORT void zsocket_set_affinity (void *zocket, int affinity);
CZMQ_EXPORT void zsocket_set_subscribe (void *zocket, char * subscribe);
CZMQ_EXPORT void zsocket_set_unsubscribe (void *zocket, char * unsubscribe);
CZMQ_EXPORT void zsocket_set_identity (void *zocket, char * identity);
CZMQ_EXPORT void zsocket_set_rate (void *zocket, int rate);
CZMQ_EXPORT void zsocket_set_recovery_ivl (void *zocket, int recovery_ivl);
CZMQ_EXPORT void zsocket_set_sndbuf (void *zocket, int sndbuf);
CZMQ_EXPORT void zsocket_set_rcvbuf (void *zocket, int rcvbuf);
CZMQ_EXPORT void zsocket_set_linger (void *zocket, int linger);
CZMQ_EXPORT void zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl);
CZMQ_EXPORT void zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max);
CZMQ_EXPORT void zsocket_set_backlog (void *zocket, int backlog);
CZMQ_EXPORT void zsocket_set_maxmsgsize (void *zocket, int maxmsgsize);
CZMQ_EXPORT void zsocket_set_multicast_hops (void *zocket, int multicast_hops);
CZMQ_EXPORT void zsocket_set_rcvtimeo (void *zocket, int rcvtimeo);
CZMQ_EXPORT void zsocket_set_sndtimeo (void *zocket, int sndtimeo);
CZMQ_EXPORT void zsocket_set_ipv4only (void *zocket, int ipv4only);
CZMQ_EXPORT void zsocket_set_delay_attach_on_connect (void *zocket, int delay_attach_on_connect);
CZMQ_EXPORT void zsocket_set_router_mandatory (void *zocket, int router_mandatory);
CZMQ_EXPORT void zsocket_set_router_raw (void *zocket, int router_raw);
CZMQ_EXPORT void zsocket_set_xpub_verbose (void *zocket, int xpub_verbose);

//  Emulation of widely-used 2.x socket options
void zsocket_set_hwm (void *zocket, int hwm);
#endif

//  Self test of this class
CZMQ_EXPORT int zsockopt_test (bool verbose);
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
#define zsockopt_set_delay_attach_on_connect zsocket_set_delay_attach_on_connect
#define zsockopt_set_router_mandatory zsocket_set_router_mandatory
#define zsockopt_set_router_raw zsocket_set_router_raw
#define zsockopt_set_xpub_verbose zsocket_set_xpub_verbose
#define zsockopt_rcvmore zsocket_rcvmore
#define zsockopt_fd zsocket_fd
#define zsockopt_events zsocket_events
#define zsockopt_last_endpoint zsocket_last_endpoint
#endif


#ifdef __cplusplus
}
#endif

#endif
