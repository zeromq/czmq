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
CZMQ_EXPORT int zsock_tos (void *self);
CZMQ_EXPORT char * zsock_zap_domain (void *self);
CZMQ_EXPORT int zsock_mechanism (void *self);
CZMQ_EXPORT int zsock_plain_server (void *self);
CZMQ_EXPORT char * zsock_plain_username (void *self);
CZMQ_EXPORT char * zsock_plain_password (void *self);
CZMQ_EXPORT int zsock_curve_server (void *self);
CZMQ_EXPORT char * zsock_curve_publickey (void *self);
CZMQ_EXPORT char * zsock_curve_secretkey (void *self);
CZMQ_EXPORT char * zsock_curve_serverkey (void *self);
CZMQ_EXPORT int zsock_gssapi_server (void *self);
CZMQ_EXPORT int zsock_gssapi_plaintext (void *self);
CZMQ_EXPORT char * zsock_gssapi_principal (void *self);
CZMQ_EXPORT char * zsock_gssapi_service_principal (void *self);
CZMQ_EXPORT int zsock_ipv6 (void *self);
CZMQ_EXPORT int zsock_immediate (void *self);
CZMQ_EXPORT int zsock_ipv4only (void *self);
CZMQ_EXPORT int zsock_type (void *self);
CZMQ_EXPORT int zsock_sndhwm (void *self);
CZMQ_EXPORT int zsock_rcvhwm (void *self);
CZMQ_EXPORT int zsock_affinity (void *self);
CZMQ_EXPORT char * zsock_identity (void *self);
CZMQ_EXPORT int zsock_rate (void *self);
CZMQ_EXPORT int zsock_recovery_ivl (void *self);
CZMQ_EXPORT int zsock_sndbuf (void *self);
CZMQ_EXPORT int zsock_rcvbuf (void *self);
CZMQ_EXPORT int zsock_linger (void *self);
CZMQ_EXPORT int zsock_reconnect_ivl (void *self);
CZMQ_EXPORT int zsock_reconnect_ivl_max (void *self);
CZMQ_EXPORT int zsock_backlog (void *self);
CZMQ_EXPORT int zsock_maxmsgsize (void *self);
CZMQ_EXPORT int zsock_multicast_hops (void *self);
CZMQ_EXPORT int zsock_rcvtimeo (void *self);
CZMQ_EXPORT int zsock_sndtimeo (void *self);
CZMQ_EXPORT int zsock_tcp_keepalive (void *self);
CZMQ_EXPORT int zsock_tcp_keepalive_idle (void *self);
CZMQ_EXPORT int zsock_tcp_keepalive_cnt (void *self);
CZMQ_EXPORT int zsock_tcp_keepalive_intvl (void *self);
CZMQ_EXPORT char * zsock_tcp_accept_filter (void *self);
CZMQ_EXPORT int zsock_rcvmore (void *self);
CZMQ_EXPORT SOCKET zsock_fd (void *self);
CZMQ_EXPORT int zsock_events (void *self);
CZMQ_EXPORT char * zsock_last_endpoint (void *self);

//  Set socket options
CZMQ_EXPORT void zsock_set_tos (void *self, int tos);
CZMQ_EXPORT void zsock_set_router_handover (void *self, int router_handover);
CZMQ_EXPORT void zsock_set_router_mandatory (void *self, int router_mandatory);
CZMQ_EXPORT void zsock_set_probe_router (void *self, int probe_router);
CZMQ_EXPORT void zsock_set_req_relaxed (void *self, int req_relaxed);
CZMQ_EXPORT void zsock_set_req_correlate (void *self, int req_correlate);
CZMQ_EXPORT void zsock_set_conflate (void *self, int conflate);
CZMQ_EXPORT void zsock_set_zap_domain (void *self, const char * zap_domain);
CZMQ_EXPORT void zsock_set_plain_server (void *self, int plain_server);
CZMQ_EXPORT void zsock_set_plain_username (void *self, const char * plain_username);
CZMQ_EXPORT void zsock_set_plain_password (void *self, const char * plain_password);
CZMQ_EXPORT void zsock_set_curve_server (void *self, int curve_server);
CZMQ_EXPORT void zsock_set_curve_publickey (void *self, const char * curve_publickey);
CZMQ_EXPORT void zsock_set_curve_publickey_bin (void *self, const byte *curve_publickey);
CZMQ_EXPORT void zsock_set_curve_secretkey (void *self, const char * curve_secretkey);
CZMQ_EXPORT void zsock_set_curve_secretkey_bin (void *self, const byte *curve_secretkey);
CZMQ_EXPORT void zsock_set_curve_serverkey (void *self, const char * curve_serverkey);
CZMQ_EXPORT void zsock_set_curve_serverkey_bin (void *self, const byte *curve_serverkey);
CZMQ_EXPORT void zsock_set_gssapi_server (void *self, int gssapi_server);
CZMQ_EXPORT void zsock_set_gssapi_plaintext (void *self, int gssapi_plaintext);
CZMQ_EXPORT void zsock_set_gssapi_principal (void *self, const char * gssapi_principal);
CZMQ_EXPORT void zsock_set_gssapi_service_principal (void *self, const char * gssapi_service_principal);
CZMQ_EXPORT void zsock_set_ipv6 (void *self, int ipv6);
CZMQ_EXPORT void zsock_set_immediate (void *self, int immediate);
CZMQ_EXPORT void zsock_set_router_raw (void *self, int router_raw);
CZMQ_EXPORT void zsock_set_ipv4only (void *self, int ipv4only);
CZMQ_EXPORT void zsock_set_delay_attach_on_connect (void *self, int delay_attach_on_connect);
CZMQ_EXPORT void zsock_set_sndhwm (void *self, int sndhwm);
CZMQ_EXPORT void zsock_set_rcvhwm (void *self, int rcvhwm);
CZMQ_EXPORT void zsock_set_affinity (void *self, int affinity);
CZMQ_EXPORT void zsock_set_subscribe (void *self, const char * subscribe);
CZMQ_EXPORT void zsock_set_unsubscribe (void *self, const char * unsubscribe);
CZMQ_EXPORT void zsock_set_identity (void *self, const char * identity);
CZMQ_EXPORT void zsock_set_rate (void *self, int rate);
CZMQ_EXPORT void zsock_set_recovery_ivl (void *self, int recovery_ivl);
CZMQ_EXPORT void zsock_set_sndbuf (void *self, int sndbuf);
CZMQ_EXPORT void zsock_set_rcvbuf (void *self, int rcvbuf);
CZMQ_EXPORT void zsock_set_linger (void *self, int linger);
CZMQ_EXPORT void zsock_set_reconnect_ivl (void *self, int reconnect_ivl);
CZMQ_EXPORT void zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max);
CZMQ_EXPORT void zsock_set_backlog (void *self, int backlog);
CZMQ_EXPORT void zsock_set_maxmsgsize (void *self, int maxmsgsize);
CZMQ_EXPORT void zsock_set_multicast_hops (void *self, int multicast_hops);
CZMQ_EXPORT void zsock_set_rcvtimeo (void *self, int rcvtimeo);
CZMQ_EXPORT void zsock_set_sndtimeo (void *self, int sndtimeo);
CZMQ_EXPORT void zsock_set_xpub_verbose (void *self, int xpub_verbose);
CZMQ_EXPORT void zsock_set_tcp_keepalive (void *self, int tcp_keepalive);
CZMQ_EXPORT void zsock_set_tcp_keepalive_idle (void *self, int tcp_keepalive_idle);
CZMQ_EXPORT void zsock_set_tcp_keepalive_cnt (void *self, int tcp_keepalive_cnt);
CZMQ_EXPORT void zsock_set_tcp_keepalive_intvl (void *self, int tcp_keepalive_intvl);
CZMQ_EXPORT void zsock_set_tcp_accept_filter (void *self, const char * tcp_accept_filter);
#endif

#if (ZMQ_VERSION_MAJOR == 3)
//  Get socket options
CZMQ_EXPORT int zsock_ipv4only (void *self);
CZMQ_EXPORT int zsock_type (void *self);
CZMQ_EXPORT int zsock_sndhwm (void *self);
CZMQ_EXPORT int zsock_rcvhwm (void *self);
CZMQ_EXPORT int zsock_affinity (void *self);
CZMQ_EXPORT char * zsock_identity (void *self);
CZMQ_EXPORT int zsock_rate (void *self);
CZMQ_EXPORT int zsock_recovery_ivl (void *self);
CZMQ_EXPORT int zsock_sndbuf (void *self);
CZMQ_EXPORT int zsock_rcvbuf (void *self);
CZMQ_EXPORT int zsock_linger (void *self);
CZMQ_EXPORT int zsock_reconnect_ivl (void *self);
CZMQ_EXPORT int zsock_reconnect_ivl_max (void *self);
CZMQ_EXPORT int zsock_backlog (void *self);
CZMQ_EXPORT int zsock_maxmsgsize (void *self);
CZMQ_EXPORT int zsock_multicast_hops (void *self);
CZMQ_EXPORT int zsock_rcvtimeo (void *self);
CZMQ_EXPORT int zsock_sndtimeo (void *self);
CZMQ_EXPORT int zsock_tcp_keepalive (void *self);
CZMQ_EXPORT int zsock_tcp_keepalive_idle (void *self);
CZMQ_EXPORT int zsock_tcp_keepalive_cnt (void *self);
CZMQ_EXPORT int zsock_tcp_keepalive_intvl (void *self);
CZMQ_EXPORT char * zsock_tcp_accept_filter (void *self);
CZMQ_EXPORT int zsock_rcvmore (void *self);
CZMQ_EXPORT SOCKET zsock_fd (void *self);
CZMQ_EXPORT int zsock_events (void *self);
CZMQ_EXPORT char * zsock_last_endpoint (void *self);

//  Set socket options
CZMQ_EXPORT void zsock_set_router_raw (void *self, int router_raw);
CZMQ_EXPORT void zsock_set_ipv4only (void *self, int ipv4only);
CZMQ_EXPORT void zsock_set_delay_attach_on_connect (void *self, int delay_attach_on_connect);
CZMQ_EXPORT void zsock_set_sndhwm (void *self, int sndhwm);
CZMQ_EXPORT void zsock_set_rcvhwm (void *self, int rcvhwm);
CZMQ_EXPORT void zsock_set_affinity (void *self, int affinity);
CZMQ_EXPORT void zsock_set_subscribe (void *self, const char * subscribe);
CZMQ_EXPORT void zsock_set_unsubscribe (void *self, const char * unsubscribe);
CZMQ_EXPORT void zsock_set_identity (void *self, const char * identity);
CZMQ_EXPORT void zsock_set_rate (void *self, int rate);
CZMQ_EXPORT void zsock_set_recovery_ivl (void *self, int recovery_ivl);
CZMQ_EXPORT void zsock_set_sndbuf (void *self, int sndbuf);
CZMQ_EXPORT void zsock_set_rcvbuf (void *self, int rcvbuf);
CZMQ_EXPORT void zsock_set_linger (void *self, int linger);
CZMQ_EXPORT void zsock_set_reconnect_ivl (void *self, int reconnect_ivl);
CZMQ_EXPORT void zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max);
CZMQ_EXPORT void zsock_set_backlog (void *self, int backlog);
CZMQ_EXPORT void zsock_set_maxmsgsize (void *self, int maxmsgsize);
CZMQ_EXPORT void zsock_set_multicast_hops (void *self, int multicast_hops);
CZMQ_EXPORT void zsock_set_rcvtimeo (void *self, int rcvtimeo);
CZMQ_EXPORT void zsock_set_sndtimeo (void *self, int sndtimeo);
CZMQ_EXPORT void zsock_set_xpub_verbose (void *self, int xpub_verbose);
CZMQ_EXPORT void zsock_set_tcp_keepalive (void *self, int tcp_keepalive);
CZMQ_EXPORT void zsock_set_tcp_keepalive_idle (void *self, int tcp_keepalive_idle);
CZMQ_EXPORT void zsock_set_tcp_keepalive_cnt (void *self, int tcp_keepalive_cnt);
CZMQ_EXPORT void zsock_set_tcp_keepalive_intvl (void *self, int tcp_keepalive_intvl);
CZMQ_EXPORT void zsock_set_tcp_accept_filter (void *self, const char * tcp_accept_filter);
#endif

#if (ZMQ_VERSION_MAJOR == 2)
//  Get socket options
CZMQ_EXPORT int zsock_hwm (void *self);
CZMQ_EXPORT int zsock_swap (void *self);
CZMQ_EXPORT int zsock_affinity (void *self);
CZMQ_EXPORT char * zsock_identity (void *self);
CZMQ_EXPORT int zsock_rate (void *self);
CZMQ_EXPORT int zsock_recovery_ivl (void *self);
CZMQ_EXPORT int zsock_recovery_ivl_msec (void *self);
CZMQ_EXPORT int zsock_mcast_loop (void *self);
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT int zsock_rcvtimeo (void *self);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT int zsock_sndtimeo (void *self);
#   endif
CZMQ_EXPORT int zsock_sndbuf (void *self);
CZMQ_EXPORT int zsock_rcvbuf (void *self);
CZMQ_EXPORT int zsock_linger (void *self);
CZMQ_EXPORT int zsock_reconnect_ivl (void *self);
CZMQ_EXPORT int zsock_reconnect_ivl_max (void *self);
CZMQ_EXPORT int zsock_backlog (void *self);
CZMQ_EXPORT int zsock_type (void *self);
CZMQ_EXPORT int zsock_rcvmore (void *self);
CZMQ_EXPORT SOCKET zsock_fd (void *self);
CZMQ_EXPORT int zsock_events (void *self);

//  Set socket options
CZMQ_EXPORT void zsock_set_hwm (void *self, int hwm);
CZMQ_EXPORT void zsock_set_swap (void *self, int swap);
CZMQ_EXPORT void zsock_set_affinity (void *self, int affinity);
CZMQ_EXPORT void zsock_set_identity (void *self, const char * identity);
CZMQ_EXPORT void zsock_set_rate (void *self, int rate);
CZMQ_EXPORT void zsock_set_recovery_ivl (void *self, int recovery_ivl);
CZMQ_EXPORT void zsock_set_recovery_ivl_msec (void *self, int recovery_ivl_msec);
CZMQ_EXPORT void zsock_set_mcast_loop (void *self, int mcast_loop);
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT void zsock_set_rcvtimeo (void *self, int rcvtimeo);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
CZMQ_EXPORT void zsock_set_sndtimeo (void *self, int sndtimeo);
#   endif
CZMQ_EXPORT void zsock_set_sndbuf (void *self, int sndbuf);
CZMQ_EXPORT void zsock_set_rcvbuf (void *self, int rcvbuf);
CZMQ_EXPORT void zsock_set_linger (void *self, int linger);
CZMQ_EXPORT void zsock_set_reconnect_ivl (void *self, int reconnect_ivl);
CZMQ_EXPORT void zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max);
CZMQ_EXPORT void zsock_set_backlog (void *self, int backlog);
CZMQ_EXPORT void zsock_set_subscribe (void *self, const char * subscribe);
CZMQ_EXPORT void zsock_set_unsubscribe (void *self, const char * unsubscribe);
#endif

//  Self test of this class
CZMQ_EXPORT void zsock_option_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
