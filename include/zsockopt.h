/*  =========================================================================
    zsockopt - get/set 0MQ socket options

            ****************************************************
            *   GENERATED SOURCE CODE, DO NOT EDIT!!           *
            *   TO CHANGE THIS, EDIT scripts/sockopts.gsl      *
            *   AND RUN ./generate in models/.                 *
            ****************************************************
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

#ifndef __ZSOCKOPT_H_INCLUDED__
#define __ZSOCKOPT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
#if (ZMQ_VERSION_MAJOR == 3)
//  Get socket options
CZMQ_EXPORT int zsocket_type (void *zocket);
CZMQ_EXPORT int zsocket_sndhwm (void *zocket);
CZMQ_EXPORT int zsocket_rcvhwm (void *zocket);
CZMQ_EXPORT int zsocket_affinity (void *zocket);
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
CZMQ_EXPORT int zsocket_ipv6 (void *zocket);
CZMQ_EXPORT int zsocket_probe_router (void *zocket);
CZMQ_EXPORT int zsocket_tcp_keepalive (void *zocket);
CZMQ_EXPORT int zsocket_tcp_keepalive_idle (void *zocket);
CZMQ_EXPORT int zsocket_tcp_keepalive_cnt (void *zocket);
CZMQ_EXPORT int zsocket_tcp_keepalive_intvl (void *zocket);
CZMQ_EXPORT char * zsocket_tcp_accept_filter (void *zocket);
CZMQ_EXPORT int zsocket_plain_server (void *zocket);
CZMQ_EXPORT char * zsocket_plain_username (void *zocket);
CZMQ_EXPORT char * zsocket_plain_password (void *zocket);
CZMQ_EXPORT int zsocket_curve_server (void *zocket);
CZMQ_EXPORT char * zsocket_curve_publickey (void *zocket);
CZMQ_EXPORT char * zsocket_curve_secretkey (void *zocket);
CZMQ_EXPORT char * zsocket_curve_serverkey (void *zocket);
CZMQ_EXPORT int zsocket_rcvmore (void *zocket);
CZMQ_EXPORT int zsocket_fd (void *zocket);
CZMQ_EXPORT int zsocket_events (void *zocket);
CZMQ_EXPORT char * zsocket_last_endpoint (void *zocket);
CZMQ_EXPORT int zsocket_ipv4only (void *zocket);

//  Set socket options
CZMQ_EXPORT void zsocket_set_sndhwm (void *zocket, int sndhwm);
CZMQ_EXPORT void zsocket_set_rcvhwm (void *zocket, int rcvhwm);
CZMQ_EXPORT void zsocket_set_affinity (void *zocket, int affinity);
CZMQ_EXPORT void zsocket_set_subscribe (void *zocket, const char * subscribe);
CZMQ_EXPORT void zsocket_set_unsubscribe (void *zocket, const char * unsubscribe);
CZMQ_EXPORT void zsocket_set_identity (void *zocket, const char * identity);
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
CZMQ_EXPORT void zsocket_set_ipv6 (void *zocket, int ipv6);
CZMQ_EXPORT void zsocket_set_immediate (void *zocket, int immediate);
CZMQ_EXPORT void zsocket_set_router_mandatory (void *zocket, int router_mandatory);
CZMQ_EXPORT void zsocket_set_xpub_verbose (void *zocket, int xpub_verbose);
CZMQ_EXPORT void zsocket_set_tcp_keepalive (void *zocket, int tcp_keepalive);
CZMQ_EXPORT void zsocket_set_tcp_keepalive_idle (void *zocket, int tcp_keepalive_idle);
CZMQ_EXPORT void zsocket_set_tcp_keepalive_cnt (void *zocket, int tcp_keepalive_cnt);
CZMQ_EXPORT void zsocket_set_tcp_keepalive_intvl (void *zocket, int tcp_keepalive_intvl);
CZMQ_EXPORT void zsocket_set_tcp_accept_filter (void *zocket, const char * tcp_accept_filter);
CZMQ_EXPORT void zsocket_set_plain_server (void *zocket, int plain_server);
CZMQ_EXPORT void zsocket_set_plain_username (void *zocket, const char * plain_username);
CZMQ_EXPORT void zsocket_set_plain_password (void *zocket, const char * plain_password);
CZMQ_EXPORT void zsocket_set_curve_server (void *zocket, int curve_server);
CZMQ_EXPORT void zsocket_set_curve_publickey (void *zocket, const char * curve_publickey);
CZMQ_EXPORT void zsocket_set_curve_secretkey (void *zocket, const char * curve_secretkey);
CZMQ_EXPORT void zsocket_set_curve_serverkey (void *zocket, const char * curve_serverkey);
CZMQ_EXPORT void zsocket_set_router_raw (void *zocket, int router_raw);
CZMQ_EXPORT void zsocket_set_ipv4only (void *zocket, int ipv4only);
CZMQ_EXPORT void zsocket_set_delay_attach_on_connect (void *zocket, int delay_attach_on_connect);

//  Emulation of widely-used 2.x socket options
void zsocket_set_hwm (void *zocket, int hwm);
//  Patch in case we're on older libzmq
#ifndef ZMQ_STREAM
#define ZMQ_STREAM 11
#endif
#endif

#if (ZMQ_VERSION_MAJOR == 2)
//  Get socket options
CZMQ_EXPORT int zsocket_hwm (void *zocket);
CZMQ_EXPORT int zsocket_swap (void *zocket);
CZMQ_EXPORT int zsocket_affinity (void *zocket);
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
CZMQ_EXPORT void zsocket_set_identity (void *zocket, const char * identity);
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
CZMQ_EXPORT void zsocket_set_subscribe (void *zocket, const char * subscribe);
CZMQ_EXPORT void zsocket_set_unsubscribe (void *zocket, const char * unsubscribe);
#endif

//  Self test of this class
int zsockopt_test (bool verbose);
//  @end

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
#define zsockopt_ipv6 zsocket_ipv6
#define zsockopt_set_ipv6 zsocket_set_ipv6
#define zsockopt_set_immediate zsocket_set_immediate
#define zsockopt_set_router_mandatory zsocket_set_router_mandatory
#define zsockopt_probe_router zsocket_probe_router
#define zsockopt_set_xpub_verbose zsocket_set_xpub_verbose
#define zsockopt_tcp_keepalive zsocket_tcp_keepalive
#define zsockopt_set_tcp_keepalive zsocket_set_tcp_keepalive
#define zsockopt_tcp_keepalive_idle zsocket_tcp_keepalive_idle
#define zsockopt_set_tcp_keepalive_idle zsocket_set_tcp_keepalive_idle
#define zsockopt_tcp_keepalive_cnt zsocket_tcp_keepalive_cnt
#define zsockopt_set_tcp_keepalive_cnt zsocket_set_tcp_keepalive_cnt
#define zsockopt_tcp_keepalive_intvl zsocket_tcp_keepalive_intvl
#define zsockopt_set_tcp_keepalive_intvl zsocket_set_tcp_keepalive_intvl
#define zsockopt_tcp_accept_filter zsocket_tcp_accept_filter
#define zsockopt_set_tcp_accept_filter zsocket_set_tcp_accept_filter
#define zsockopt_plain_server zsocket_plain_server
#define zsockopt_set_plain_server zsocket_set_plain_server
#define zsockopt_plain_username zsocket_plain_username
#define zsockopt_set_plain_username zsocket_set_plain_username
#define zsockopt_plain_password zsocket_plain_password
#define zsockopt_set_plain_password zsocket_set_plain_password
#define zsockopt_curve_server zsocket_curve_server
#define zsockopt_set_curve_server zsocket_set_curve_server
#define zsockopt_curve_publickey zsocket_curve_publickey
#define zsockopt_set_curve_publickey zsocket_set_curve_publickey
#define zsockopt_curve_secretkey zsocket_curve_secretkey
#define zsockopt_set_curve_secretkey zsocket_set_curve_secretkey
#define zsockopt_curve_serverkey zsocket_curve_serverkey
#define zsockopt_set_curve_serverkey zsocket_set_curve_serverkey
#define zsockopt_rcvmore zsocket_rcvmore
#define zsockopt_fd zsocket_fd
#define zsockopt_events zsocket_events
#define zsockopt_last_endpoint zsocket_last_endpoint
#define zsockopt_set_router_raw zsocket_set_router_raw
#define zsockopt_ipv4only zsocket_ipv4only
#define zsockopt_set_ipv4only zsocket_set_ipv4only
#define zsockopt_set_delay_attach_on_connect zsocket_set_delay_attach_on_connect
#endif

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


#ifdef __cplusplus
}
#endif

#endif
