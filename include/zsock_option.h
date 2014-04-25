/*  =========================================================================
    zsock_option - get/set 0MQ socket options

            ****************************************************
            *   GENERATED SOURCE CODE, DO NOT EDIT!!           *
            *   TO CHANGE THIS, EDIT scripts/sockopts.gsl      *
            *   AND RUN ./generate in models/.                 *
            ****************************************************

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZSOCK_OPTION_H_INCLUDED__
#define __ZSOCK_OPTION_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
#if (ZMQ_VERSION_MAJOR == 4)
//  Get socket options
CZMQ_EXPORT int zsock_tos (zsock_t *self);
CZMQ_EXPORT int zsock_plain_server (zsock_t *self);
CZMQ_EXPORT char * zsock_plain_username (zsock_t *self);
CZMQ_EXPORT char * zsock_plain_password (zsock_t *self);
CZMQ_EXPORT int zsock_curve_server (zsock_t *self);
CZMQ_EXPORT char * zsock_curve_publickey (zsock_t *self);
CZMQ_EXPORT char * zsock_curve_secretkey (zsock_t *self);
CZMQ_EXPORT char * zsock_curve_serverkey (zsock_t *self);
CZMQ_EXPORT char * zsock_zap_domain (zsock_t *self);
CZMQ_EXPORT int zsock_mechanism (zsock_t *self);
CZMQ_EXPORT int zsock_ipv6 (zsock_t *self);
CZMQ_EXPORT int zsock_immediate (zsock_t *self);
CZMQ_EXPORT int zsock_ipv4only (zsock_t *self);
CZMQ_EXPORT int zsock_type (zsock_t *self);
CZMQ_EXPORT int zsock_sndhwm (zsock_t *self);
CZMQ_EXPORT int zsock_rcvhwm (zsock_t *self);
CZMQ_EXPORT int zsock_affinity (zsock_t *self);
CZMQ_EXPORT char * zsock_identity (zsock_t *self);
CZMQ_EXPORT int zsock_rate (zsock_t *self);
CZMQ_EXPORT int zsock_recovery_ivl (zsock_t *self);
CZMQ_EXPORT int zsock_sndbuf (zsock_t *self);
CZMQ_EXPORT int zsock_rcvbuf (zsock_t *self);
CZMQ_EXPORT int zsock_linger (zsock_t *self);
CZMQ_EXPORT int zsock_reconnect_ivl (zsock_t *self);
CZMQ_EXPORT int zsock_reconnect_ivl_max (zsock_t *self);
CZMQ_EXPORT int zsock_backlog (zsock_t *self);
CZMQ_EXPORT int zsock_maxmsgsize (zsock_t *self);
CZMQ_EXPORT int zsock_multicast_hops (zsock_t *self);
CZMQ_EXPORT int zsock_rcvtimeo (zsock_t *self);
CZMQ_EXPORT int zsock_sndtimeo (zsock_t *self);
CZMQ_EXPORT int zsock_tcp_keepalive (zsock_t *self);
CZMQ_EXPORT int zsock_tcp_keepalive_idle (zsock_t *self);
CZMQ_EXPORT int zsock_tcp_keepalive_cnt (zsock_t *self);
CZMQ_EXPORT int zsock_tcp_keepalive_intvl (zsock_t *self);
CZMQ_EXPORT char * zsock_tcp_accept_filter (zsock_t *self);
CZMQ_EXPORT int zsock_rcvmore (zsock_t *self);
CZMQ_EXPORT int zsock_fd (zsock_t *self);
CZMQ_EXPORT int zsock_events (zsock_t *self);
CZMQ_EXPORT char * zsock_last_endpoint (zsock_t *self);

//  Set socket options
CZMQ_EXPORT void zsock_set_tos (zsock_t *self, int tos);
CZMQ_EXPORT void zsock_set_router_handover (zsock_t *self, int router_handover);
CZMQ_EXPORT void zsock_set_router_mandatory (zsock_t *self, int router_mandatory);
CZMQ_EXPORT void zsock_set_probe_router (zsock_t *self, int probe_router);
CZMQ_EXPORT void zsock_set_req_relaxed (zsock_t *self, int req_relaxed);
CZMQ_EXPORT void zsock_set_req_correlate (zsock_t *self, int req_correlate);
CZMQ_EXPORT void zsock_set_conflate (zsock_t *self, int conflate);
CZMQ_EXPORT void zsock_set_plain_server (zsock_t *self, int plain_server);
CZMQ_EXPORT void zsock_set_plain_username (zsock_t *self, const char * plain_username);
CZMQ_EXPORT void zsock_set_plain_password (zsock_t *self, const char * plain_password);
CZMQ_EXPORT void zsock_set_curve_server (zsock_t *self, int curve_server);
CZMQ_EXPORT void zsock_set_curve_publickey (zsock_t *self, const char * curve_publickey);
CZMQ_EXPORT void zsock_set_curve_publickey_bin (zsock_t *self, const byte *curve_publickey);
CZMQ_EXPORT void zsock_set_curve_secretkey (zsock_t *self, const char * curve_secretkey);
CZMQ_EXPORT void zsock_set_curve_secretkey_bin (zsock_t *self, const byte *curve_secretkey);
CZMQ_EXPORT void zsock_set_curve_serverkey (zsock_t *self, const char * curve_serverkey);
CZMQ_EXPORT void zsock_set_curve_serverkey_bin (zsock_t *self, const byte *curve_serverkey);
CZMQ_EXPORT void zsock_set_zap_domain (zsock_t *self, const char * zap_domain);
CZMQ_EXPORT void zsock_set_ipv6 (zsock_t *self, int ipv6);
CZMQ_EXPORT void zsock_set_immediate (zsock_t *self, int immediate);
CZMQ_EXPORT void zsock_set_router_raw (zsock_t *self, int router_raw);
CZMQ_EXPORT void zsock_set_ipv4only (zsock_t *self, int ipv4only);
CZMQ_EXPORT void zsock_set_delay_attach_on_connect (zsock_t *self, int delay_attach_on_connect);
CZMQ_EXPORT void zsock_set_sndhwm (zsock_t *self, int sndhwm);
CZMQ_EXPORT void zsock_set_rcvhwm (zsock_t *self, int rcvhwm);
CZMQ_EXPORT void zsock_set_affinity (zsock_t *self, int affinity);
CZMQ_EXPORT void zsock_set_subscribe (zsock_t *self, const char * subscribe);
CZMQ_EXPORT void zsock_set_unsubscribe (zsock_t *self, const char * unsubscribe);
CZMQ_EXPORT void zsock_set_identity (zsock_t *self, const char * identity);
CZMQ_EXPORT void zsock_set_rate (zsock_t *self, int rate);
CZMQ_EXPORT void zsock_set_recovery_ivl (zsock_t *self, int recovery_ivl);
CZMQ_EXPORT void zsock_set_sndbuf (zsock_t *self, int sndbuf);
CZMQ_EXPORT void zsock_set_rcvbuf (zsock_t *self, int rcvbuf);
CZMQ_EXPORT void zsock_set_linger (zsock_t *self, int linger);
CZMQ_EXPORT void zsock_set_reconnect_ivl (zsock_t *self, int reconnect_ivl);
CZMQ_EXPORT void zsock_set_reconnect_ivl_max (zsock_t *self, int reconnect_ivl_max);
CZMQ_EXPORT void zsock_set_backlog (zsock_t *self, int backlog);
CZMQ_EXPORT void zsock_set_maxmsgsize (zsock_t *self, int maxmsgsize);
CZMQ_EXPORT void zsock_set_multicast_hops (zsock_t *self, int multicast_hops);
CZMQ_EXPORT void zsock_set_rcvtimeo (zsock_t *self, int rcvtimeo);
CZMQ_EXPORT void zsock_set_sndtimeo (zsock_t *self, int sndtimeo);
CZMQ_EXPORT void zsock_set_xpub_verbose (zsock_t *self, int xpub_verbose);
CZMQ_EXPORT void zsock_set_tcp_keepalive (zsock_t *self, int tcp_keepalive);
CZMQ_EXPORT void zsock_set_tcp_keepalive_idle (zsock_t *self, int tcp_keepalive_idle);
CZMQ_EXPORT void zsock_set_tcp_keepalive_cnt (zsock_t *self, int tcp_keepalive_cnt);
CZMQ_EXPORT void zsock_set_tcp_keepalive_intvl (zsock_t *self, int tcp_keepalive_intvl);
CZMQ_EXPORT void zsock_set_tcp_accept_filter (zsock_t *self, const char * tcp_accept_filter);
#endif

#if (ZMQ_VERSION_MAJOR == 3)
//  Get socket options
CZMQ_EXPORT int zsock_ipv4only (zsock_t *self);
CZMQ_EXPORT int zsock_type (zsock_t *self);
CZMQ_EXPORT int zsock_sndhwm (zsock_t *self);
CZMQ_EXPORT int zsock_rcvhwm (zsock_t *self);
CZMQ_EXPORT int zsock_affinity (zsock_t *self);
CZMQ_EXPORT char * zsock_identity (zsock_t *self);
CZMQ_EXPORT int zsock_rate (zsock_t *self);
CZMQ_EXPORT int zsock_recovery_ivl (zsock_t *self);
CZMQ_EXPORT int zsock_sndbuf (zsock_t *self);
CZMQ_EXPORT int zsock_rcvbuf (zsock_t *self);
CZMQ_EXPORT int zsock_linger (zsock_t *self);
CZMQ_EXPORT int zsock_reconnect_ivl (zsock_t *self);
CZMQ_EXPORT int zsock_reconnect_ivl_max (zsock_t *self);
CZMQ_EXPORT int zsock_backlog (zsock_t *self);
CZMQ_EXPORT int zsock_maxmsgsize (zsock_t *self);
CZMQ_EXPORT int zsock_multicast_hops (zsock_t *self);
CZMQ_EXPORT int zsock_rcvtimeo (zsock_t *self);
CZMQ_EXPORT int zsock_sndtimeo (zsock_t *self);
CZMQ_EXPORT int zsock_tcp_keepalive (zsock_t *self);
CZMQ_EXPORT int zsock_tcp_keepalive_idle (zsock_t *self);
CZMQ_EXPORT int zsock_tcp_keepalive_cnt (zsock_t *self);
CZMQ_EXPORT int zsock_tcp_keepalive_intvl (zsock_t *self);
CZMQ_EXPORT char * zsock_tcp_accept_filter (zsock_t *self);
CZMQ_EXPORT int zsock_rcvmore (zsock_t *self);
CZMQ_EXPORT int zsock_fd (zsock_t *self);
CZMQ_EXPORT int zsock_events (zsock_t *self);
CZMQ_EXPORT char * zsock_last_endpoint (zsock_t *self);

//  Set socket options
CZMQ_EXPORT void zsock_set_router_raw (zsock_t *self, int router_raw);
CZMQ_EXPORT void zsock_set_ipv4only (zsock_t *self, int ipv4only);
CZMQ_EXPORT void zsock_set_delay_attach_on_connect (zsock_t *self, int delay_attach_on_connect);
CZMQ_EXPORT void zsock_set_sndhwm (zsock_t *self, int sndhwm);
CZMQ_EXPORT void zsock_set_rcvhwm (zsock_t *self, int rcvhwm);
CZMQ_EXPORT void zsock_set_affinity (zsock_t *self, int affinity);
CZMQ_EXPORT void zsock_set_subscribe (zsock_t *self, const char * subscribe);
CZMQ_EXPORT void zsock_set_unsubscribe (zsock_t *self, const char * unsubscribe);
CZMQ_EXPORT void zsock_set_identity (zsock_t *self, const char * identity);
CZMQ_EXPORT void zsock_set_rate (zsock_t *self, int rate);
CZMQ_EXPORT void zsock_set_recovery_ivl (zsock_t *self, int recovery_ivl);
CZMQ_EXPORT void zsock_set_sndbuf (zsock_t *self, int sndbuf);
CZMQ_EXPORT void zsock_set_rcvbuf (zsock_t *self, int rcvbuf);
CZMQ_EXPORT void zsock_set_linger (zsock_t *self, int linger);
CZMQ_EXPORT void zsock_set_reconnect_ivl (zsock_t *self, int reconnect_ivl);
CZMQ_EXPORT void zsock_set_reconnect_ivl_max (zsock_t *self, int reconnect_ivl_max);
CZMQ_EXPORT void zsock_set_backlog (zsock_t *self, int backlog);
CZMQ_EXPORT void zsock_set_maxmsgsize (zsock_t *self, int maxmsgsize);
CZMQ_EXPORT void zsock_set_multicast_hops (zsock_t *self, int multicast_hops);
CZMQ_EXPORT void zsock_set_rcvtimeo (zsock_t *self, int rcvtimeo);
CZMQ_EXPORT void zsock_set_sndtimeo (zsock_t *self, int sndtimeo);
CZMQ_EXPORT void zsock_set_xpub_verbose (zsock_t *self, int xpub_verbose);
CZMQ_EXPORT void zsock_set_tcp_keepalive (zsock_t *self, int tcp_keepalive);
CZMQ_EXPORT void zsock_set_tcp_keepalive_idle (zsock_t *self, int tcp_keepalive_idle);
CZMQ_EXPORT void zsock_set_tcp_keepalive_cnt (zsock_t *self, int tcp_keepalive_cnt);
CZMQ_EXPORT void zsock_set_tcp_keepalive_intvl (zsock_t *self, int tcp_keepalive_intvl);
CZMQ_EXPORT void zsock_set_tcp_accept_filter (zsock_t *self, const char * tcp_accept_filter);
#endif

#if (ZMQ_VERSION_MAJOR == 2)
//  Get socket options
CZMQ_EXPORT int zsock_hwm (zsock_t *self);
CZMQ_EXPORT int zsock_swap (zsock_t *self);
CZMQ_EXPORT int zsock_affinity (zsock_t *self);
CZMQ_EXPORT char * zsock_identity (zsock_t *self);
CZMQ_EXPORT int zsock_rate (zsock_t *self);
CZMQ_EXPORT int zsock_recovery_ivl (zsock_t *self);
CZMQ_EXPORT int zsock_recovery_ivl_msec (zsock_t *self);
CZMQ_EXPORT int zsock_mcast_loop (zsock_t *self);
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT int zsock_rcvtimeo (zsock_t *self);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT int zsock_sndtimeo (zsock_t *self);
#   endif
CZMQ_EXPORT int zsock_sndbuf (zsock_t *self);
CZMQ_EXPORT int zsock_rcvbuf (zsock_t *self);
CZMQ_EXPORT int zsock_linger (zsock_t *self);
CZMQ_EXPORT int zsock_reconnect_ivl (zsock_t *self);
CZMQ_EXPORT int zsock_reconnect_ivl_max (zsock_t *self);
CZMQ_EXPORT int zsock_backlog (zsock_t *self);
CZMQ_EXPORT int zsock_type (zsock_t *self);
CZMQ_EXPORT int zsock_rcvmore (zsock_t *self);
CZMQ_EXPORT int zsock_fd (zsock_t *self);
CZMQ_EXPORT int zsock_events (zsock_t *self);

//  Set socket options
CZMQ_EXPORT void zsock_set_hwm (zsock_t *self, int hwm);
CZMQ_EXPORT void zsock_set_swap (zsock_t *self, int swap);
CZMQ_EXPORT void zsock_set_affinity (zsock_t *self, int affinity);
CZMQ_EXPORT void zsock_set_identity (zsock_t *self, const char * identity);
CZMQ_EXPORT void zsock_set_rate (zsock_t *self, int rate);
CZMQ_EXPORT void zsock_set_recovery_ivl (zsock_t *self, int recovery_ivl);
CZMQ_EXPORT void zsock_set_recovery_ivl_msec (zsock_t *self, int recovery_ivl_msec);
CZMQ_EXPORT void zsock_set_mcast_loop (zsock_t *self, int mcast_loop);
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT void zsock_set_rcvtimeo (zsock_t *self, int rcvtimeo);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT void zsock_set_sndtimeo (zsock_t *self, int sndtimeo);
#   endif
CZMQ_EXPORT void zsock_set_sndbuf (zsock_t *self, int sndbuf);
CZMQ_EXPORT void zsock_set_rcvbuf (zsock_t *self, int rcvbuf);
CZMQ_EXPORT void zsock_set_linger (zsock_t *self, int linger);
CZMQ_EXPORT void zsock_set_reconnect_ivl (zsock_t *self, int reconnect_ivl);
CZMQ_EXPORT void zsock_set_reconnect_ivl_max (zsock_t *self, int reconnect_ivl_max);
CZMQ_EXPORT void zsock_set_backlog (zsock_t *self, int backlog);
CZMQ_EXPORT void zsock_set_subscribe (zsock_t *self, const char * subscribe);
CZMQ_EXPORT void zsock_set_unsubscribe (zsock_t *self, const char * unsubscribe);
#endif

//  Self test of this class
CZMQ_EXPORT void zsock_option_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
