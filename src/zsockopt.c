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

/*
@header
    The zsockopt class provides access to the 0MQ getsockopt/setsockopt API.
@discuss
    This class is generated, using the GSL code generator. See the sockopts
    XML file, which provides the metadata, and the sockopts.gsl template,
    which does the work.
@end
*/

#include "../include/czmq.h"

#if (ZMQ_VERSION_MAJOR == 3)
//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_type (void *zocket)
{
#   if defined (ZMQ_TYPE)
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_TYPE, &type, &option_len);
    return type;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_sndhwm (void *zocket, int sndhwm)
{
#   if defined (ZMQ_SNDHWM)
    int rc = zmq_setsockopt (zocket, ZMQ_SNDHWM, &sndhwm, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_sndhwm (void *zocket)
{
#   if defined (ZMQ_SNDHWM)
    int sndhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_SNDHWM, &sndhwm, &option_len);
    return sndhwm;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_rcvhwm (void *zocket, int rcvhwm)
{
#   if defined (ZMQ_RCVHWM)
    int rc = zmq_setsockopt (zocket, ZMQ_RCVHWM, &rcvhwm, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rcvhwm (void *zocket)
{
#   if defined (ZMQ_RCVHWM)
    int rcvhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVHWM, &rcvhwm, &option_len);
    return rcvhwm;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_affinity (void *zocket, int affinity)
{
#   if defined (ZMQ_AFFINITY)
    uint64_t value = affinity;
    int rc = zmq_setsockopt (zocket, ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_affinity (void *zocket)
{
#   if defined (ZMQ_AFFINITY)
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_subscribe (void *zocket, const char * subscribe)
{
#   if defined (ZMQ_SUBSCRIBE)
    if (zsockopt_type (zocket) != ZMQ_SUB) {
        printf ("ZMQ_SUBSCRIBE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_unsubscribe (void *zocket, const char * unsubscribe)
{
#   if defined (ZMQ_UNSUBSCRIBE)
    if (zsockopt_type (zocket) != ZMQ_SUB) {
        printf ("ZMQ_UNSUBSCRIBE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_identity (void *zocket, const char * identity)
{
#   if defined (ZMQ_IDENTITY)
    if (zsockopt_type (zocket) != ZMQ_REQ
    &&  zsockopt_type (zocket) != ZMQ_REP
    &&  zsockopt_type (zocket) != ZMQ_DEALER
    &&  zsockopt_type (zocket) != ZMQ_ROUTER
    &&  zsockopt_type (zocket) != ZMQ_STREAM) {
        printf ("ZMQ_IDENTITY is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_identity (void *zocket)
{
#   if defined (ZMQ_IDENTITY)
    size_t option_len = 255;
    char *identity = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_IDENTITY, identity, &option_len);
    return (char *) identity;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_rate (void *zocket, int rate)
{
#   if defined (ZMQ_RATE)
    int rc = zmq_setsockopt (zocket, ZMQ_RATE, &rate, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rate (void *zocket)
{
#   if defined (ZMQ_RATE)
    int rate;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RATE, &rate, &option_len);
    return rate;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_recovery_ivl (void *zocket, int recovery_ivl)
{
#   if defined (ZMQ_RECOVERY_IVL)
    int rc = zmq_setsockopt (zocket, ZMQ_RECOVERY_IVL, &recovery_ivl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_recovery_ivl (void *zocket)
{
#   if defined (ZMQ_RECOVERY_IVL)
    int recovery_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return recovery_ivl;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_sndbuf (void *zocket, int sndbuf)
{
#   if defined (ZMQ_SNDBUF)
    int rc = zmq_setsockopt (zocket, ZMQ_SNDBUF, &sndbuf, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_sndbuf (void *zocket)
{
#   if defined (ZMQ_SNDBUF)
    int sndbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_SNDBUF, &sndbuf, &option_len);
    return sndbuf;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_rcvbuf (void *zocket, int rcvbuf)
{
#   if defined (ZMQ_RCVBUF)
    int rc = zmq_setsockopt (zocket, ZMQ_RCVBUF, &rcvbuf, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rcvbuf (void *zocket)
{
#   if defined (ZMQ_RCVBUF)
    int rcvbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVBUF, &rcvbuf, &option_len);
    return rcvbuf;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_linger (void *zocket, int linger)
{
#   if defined (ZMQ_LINGER)
    int rc = zmq_setsockopt (zocket, ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_linger (void *zocket)
{
#   if defined (ZMQ_LINGER)
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_LINGER, &linger, &option_len);
    return linger;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl)
{
#   if defined (ZMQ_RECONNECT_IVL)
    int rc = zmq_setsockopt (zocket, ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_reconnect_ivl (void *zocket)
{
#   if defined (ZMQ_RECONNECT_IVL)
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max)
{
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int rc = zmq_setsockopt (zocket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_reconnect_ivl_max (void *zocket)
{
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_backlog (void *zocket, int backlog)
{
#   if defined (ZMQ_BACKLOG)
    int rc = zmq_setsockopt (zocket, ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_backlog (void *zocket)
{
#   if defined (ZMQ_BACKLOG)
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MAXMSGSIZE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_maxmsgsize (void *zocket, int maxmsgsize)
{
#   if defined (ZMQ_MAXMSGSIZE)
    int64_t value = maxmsgsize;
    int rc = zmq_setsockopt (zocket, ZMQ_MAXMSGSIZE, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MAXMSGSIZE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_maxmsgsize (void *zocket)
{
#   if defined (ZMQ_MAXMSGSIZE)
    int64_t maxmsgsize;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_MAXMSGSIZE, &maxmsgsize, &option_len);
    return (int) maxmsgsize;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MULTICAST_HOPS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_multicast_hops (void *zocket, int multicast_hops)
{
#   if defined (ZMQ_MULTICAST_HOPS)
    int rc = zmq_setsockopt (zocket, ZMQ_MULTICAST_HOPS, &multicast_hops, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MULTICAST_HOPS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_multicast_hops (void *zocket)
{
#   if defined (ZMQ_MULTICAST_HOPS)
    int multicast_hops;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_MULTICAST_HOPS, &multicast_hops, &option_len);
    return multicast_hops;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_rcvtimeo (void *zocket, int rcvtimeo)
{
#   if defined (ZMQ_RCVTIMEO)
    int rc = zmq_setsockopt (zocket, ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rcvtimeo (void *zocket)
{
#   if defined (ZMQ_RCVTIMEO)
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_sndtimeo (void *zocket, int sndtimeo)
{
#   if defined (ZMQ_SNDTIMEO)
    int rc = zmq_setsockopt (zocket, ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_sndtimeo (void *zocket)
{
#   if defined (ZMQ_SNDTIMEO)
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IPV6 value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_ipv6 (void *zocket, int ipv6)
{
#   if defined (ZMQ_IPV6)
    int rc = zmq_setsockopt (zocket, ZMQ_IPV6, &ipv6, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IPV6 value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_ipv6 (void *zocket)
{
#   if defined (ZMQ_IPV6)
    int ipv6;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_IPV6, &ipv6, &option_len);
    return ipv6;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IMMEDIATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_immediate (void *zocket, int immediate)
{
#   if defined (ZMQ_IMMEDIATE)
    int rc = zmq_setsockopt (zocket, ZMQ_IMMEDIATE, &immediate, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_ROUTER_MANDATORY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_router_mandatory (void *zocket, int router_mandatory)
{
#   if defined (ZMQ_ROUTER_MANDATORY)
    if (zsockopt_type (zocket) != ZMQ_ROUTER) {
        printf ("ZMQ_ROUTER_MANDATORY is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_ROUTER_MANDATORY, &router_mandatory, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_PROBE_ROUTER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_probe_router (void *zocket)
{
#   if defined (ZMQ_PROBE_ROUTER)
    int probe_router;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_PROBE_ROUTER, &probe_router, &option_len);
    return probe_router;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_XPUB_VERBOSE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_xpub_verbose (void *zocket, int xpub_verbose)
{
#   if defined (ZMQ_XPUB_VERBOSE)
    if (zsockopt_type (zocket) != ZMQ_XPUB) {
        printf ("ZMQ_XPUB_VERBOSE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_XPUB_VERBOSE, &xpub_verbose, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_tcp_keepalive (void *zocket, int tcp_keepalive)
{
#   if defined (ZMQ_TCP_KEEPALIVE)
    int rc = zmq_setsockopt (zocket, ZMQ_TCP_KEEPALIVE, &tcp_keepalive, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_tcp_keepalive (void *zocket)
{
#   if defined (ZMQ_TCP_KEEPALIVE)
    int tcp_keepalive;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_TCP_KEEPALIVE, &tcp_keepalive, &option_len);
    return tcp_keepalive;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_IDLE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_tcp_keepalive_idle (void *zocket, int tcp_keepalive_idle)
{
#   if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    int rc = zmq_setsockopt (zocket, ZMQ_TCP_KEEPALIVE_IDLE, &tcp_keepalive_idle, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_IDLE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_tcp_keepalive_idle (void *zocket)
{
#   if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    int tcp_keepalive_idle;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_TCP_KEEPALIVE_IDLE, &tcp_keepalive_idle, &option_len);
    return tcp_keepalive_idle;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_CNT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_tcp_keepalive_cnt (void *zocket, int tcp_keepalive_cnt)
{
#   if defined (ZMQ_TCP_KEEPALIVE_CNT)
    int rc = zmq_setsockopt (zocket, ZMQ_TCP_KEEPALIVE_CNT, &tcp_keepalive_cnt, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_CNT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_tcp_keepalive_cnt (void *zocket)
{
#   if defined (ZMQ_TCP_KEEPALIVE_CNT)
    int tcp_keepalive_cnt;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_TCP_KEEPALIVE_CNT, &tcp_keepalive_cnt, &option_len);
    return tcp_keepalive_cnt;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_INTVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_tcp_keepalive_intvl (void *zocket, int tcp_keepalive_intvl)
{
#   if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    int rc = zmq_setsockopt (zocket, ZMQ_TCP_KEEPALIVE_INTVL, &tcp_keepalive_intvl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_INTVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_tcp_keepalive_intvl (void *zocket)
{
#   if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    int tcp_keepalive_intvl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_TCP_KEEPALIVE_INTVL, &tcp_keepalive_intvl, &option_len);
    return tcp_keepalive_intvl;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_ACCEPT_FILTER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_tcp_accept_filter (void *zocket, const char * tcp_accept_filter)
{
#   if defined (ZMQ_TCP_ACCEPT_FILTER)
    int rc = zmq_setsockopt (zocket, ZMQ_TCP_ACCEPT_FILTER, tcp_accept_filter, strlen (tcp_accept_filter));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_ACCEPT_FILTER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_tcp_accept_filter (void *zocket)
{
#   if defined (ZMQ_TCP_ACCEPT_FILTER)
    size_t option_len = 255;
    char *tcp_accept_filter = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_TCP_ACCEPT_FILTER, tcp_accept_filter, &option_len);
    return (char *) tcp_accept_filter;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_PLAIN_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_plain_server (void *zocket, int plain_server)
{
#   if defined (ZMQ_PLAIN_SERVER)
    int rc = zmq_setsockopt (zocket, ZMQ_PLAIN_SERVER, &plain_server, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_PLAIN_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_plain_server (void *zocket)
{
#   if defined (ZMQ_PLAIN_SERVER)
    int plain_server;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_PLAIN_SERVER, &plain_server, &option_len);
    return plain_server;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_PLAIN_USERNAME value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_plain_username (void *zocket, const char * plain_username)
{
#   if defined (ZMQ_PLAIN_USERNAME)
    int rc = zmq_setsockopt (zocket, ZMQ_PLAIN_USERNAME, plain_username, strlen (plain_username));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_PLAIN_USERNAME value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_plain_username (void *zocket)
{
#   if defined (ZMQ_PLAIN_USERNAME)
    size_t option_len = 255;
    char *plain_username = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_PLAIN_USERNAME, plain_username, &option_len);
    return (char *) plain_username;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_PLAIN_PASSWORD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_plain_password (void *zocket, const char * plain_password)
{
#   if defined (ZMQ_PLAIN_PASSWORD)
    int rc = zmq_setsockopt (zocket, ZMQ_PLAIN_PASSWORD, plain_password, strlen (plain_password));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_PLAIN_PASSWORD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_plain_password (void *zocket)
{
#   if defined (ZMQ_PLAIN_PASSWORD)
    size_t option_len = 255;
    char *plain_password = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_PLAIN_PASSWORD, plain_password, &option_len);
    return (char *) plain_password;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_curve_server (void *zocket, int curve_server)
{
#   if defined (ZMQ_CURVE_SERVER)
    int rc = zmq_setsockopt (zocket, ZMQ_CURVE_SERVER, &curve_server, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_CURVE_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_curve_server (void *zocket)
{
#   if defined (ZMQ_CURVE_SERVER)
    int curve_server;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_CURVE_SERVER, &curve_server, &option_len);
    return curve_server;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_PUBLICKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_curve_publickey (void *zocket, const char * curve_publickey)
{
#   if defined (ZMQ_CURVE_PUBLICKEY)
    int rc = zmq_setsockopt (zocket, ZMQ_CURVE_PUBLICKEY, curve_publickey, strlen (curve_publickey));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_PUBLICKEY value from 32-octet binary
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_curve_publickey_bin (void *zocket, const byte *curve_publickey)
{
#   if defined (ZMQ_CURVE_PUBLICKEY)
    int rc = zmq_setsockopt (zocket, ZMQ_CURVE_PUBLICKEY, curve_publickey, 32);
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_CURVE_PUBLICKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_curve_publickey (void *zocket)
{
#   if defined (ZMQ_CURVE_PUBLICKEY)
    size_t option_len = 255;
    char *curve_publickey = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_CURVE_PUBLICKEY, curve_publickey, &option_len);
    return (char *) curve_publickey;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SECRETKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_curve_secretkey (void *zocket, const char * curve_secretkey)
{
#   if defined (ZMQ_CURVE_SECRETKEY)
    int rc = zmq_setsockopt (zocket, ZMQ_CURVE_SECRETKEY, curve_secretkey, strlen (curve_secretkey));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SECRETKEY value from 32-octet binary
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_curve_secretkey_bin (void *zocket, const byte *curve_secretkey)
{
#   if defined (ZMQ_CURVE_SECRETKEY)
    int rc = zmq_setsockopt (zocket, ZMQ_CURVE_SECRETKEY, curve_secretkey, 32);
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_CURVE_SECRETKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_curve_secretkey (void *zocket)
{
#   if defined (ZMQ_CURVE_SECRETKEY)
    size_t option_len = 255;
    char *curve_secretkey = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_CURVE_SECRETKEY, curve_secretkey, &option_len);
    return (char *) curve_secretkey;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SERVERKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_curve_serverkey (void *zocket, const char * curve_serverkey)
{
#   if defined (ZMQ_CURVE_SERVERKEY)
    int rc = zmq_setsockopt (zocket, ZMQ_CURVE_SERVERKEY, curve_serverkey, strlen (curve_serverkey));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SERVERKEY value from 32-octet binary
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_curve_serverkey_bin (void *zocket, const byte *curve_serverkey)
{
#   if defined (ZMQ_CURVE_SERVERKEY)
    int rc = zmq_setsockopt (zocket, ZMQ_CURVE_SERVERKEY, curve_serverkey, 32);
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_CURVE_SERVERKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_curve_serverkey (void *zocket)
{
#   if defined (ZMQ_CURVE_SERVERKEY)
    size_t option_len = 255;
    char *curve_serverkey = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_CURVE_SERVERKEY, curve_serverkey, &option_len);
    return (char *) curve_serverkey;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_ZAP_DOMAIN value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_zap_domain (void *zocket, const char * zap_domain)
{
#   if defined (ZMQ_ZAP_DOMAIN)
    int rc = zmq_setsockopt (zocket, ZMQ_ZAP_DOMAIN, zap_domain, strlen (zap_domain));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_ZAP_DOMAIN value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_zap_domain (void *zocket)
{
#   if defined (ZMQ_ZAP_DOMAIN)
    size_t option_len = 255;
    char *zap_domain = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_ZAP_DOMAIN, zap_domain, &option_len);
    return (char *) zap_domain;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rcvmore (void *zocket)
{
#   if defined (ZMQ_RCVMORE)
    int rcvmore;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVMORE, &rcvmore, &option_len);
    return rcvmore;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_fd (void *zocket)
{
#   if defined (ZMQ_FD)
    int fd;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_FD, &fd, &option_len);
    return fd;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_events (void *zocket)
{
#   if defined (ZMQ_EVENTS)
    int events;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_EVENTS, &events, &option_len);
    return events;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LAST_ENDPOINT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_last_endpoint (void *zocket)
{
#   if defined (ZMQ_LAST_ENDPOINT)
    size_t option_len = 255;
    char *last_endpoint = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_LAST_ENDPOINT, last_endpoint, &option_len);
    return (char *) last_endpoint;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_ROUTER_RAW value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_router_raw (void *zocket, int router_raw)
{
#   if defined (ZMQ_ROUTER_RAW)
    if (zsockopt_type (zocket) != ZMQ_ROUTER) {
        printf ("ZMQ_ROUTER_RAW is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_ROUTER_RAW, &router_raw, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IPV4ONLY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_ipv4only (void *zocket, int ipv4only)
{
#   if defined (ZMQ_IPV4ONLY)
    int rc = zmq_setsockopt (zocket, ZMQ_IPV4ONLY, &ipv4only, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IPV4ONLY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_ipv4only (void *zocket)
{
#   if defined (ZMQ_IPV4ONLY)
    int ipv4only;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_IPV4ONLY, &ipv4only, &option_len);
    return ipv4only;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_DELAY_ATTACH_ON_CONNECT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_delay_attach_on_connect (void *zocket, int delay_attach_on_connect)
{
#   if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    int rc = zmq_setsockopt (zocket, ZMQ_DELAY_ATTACH_ON_CONNECT, &delay_attach_on_connect, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket high-water mark, emulating 2.x API

void
zsocket_set_hwm (void *zocket, int hwm)
{
    zsocket_set_sndhwm (zocket, hwm);
    zsocket_set_rcvhwm (zocket, hwm);
}

#endif

#if (ZMQ_VERSION_MAJOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_HWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_hwm (void *zocket, int hwm)
{
#   if defined (ZMQ_HWM)
    uint64_t value = hwm;
    int rc = zmq_setsockopt (zocket, ZMQ_HWM, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_HWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_hwm (void *zocket)
{
#   if defined (ZMQ_HWM)
    uint64_t hwm;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_HWM, &hwm, &option_len);
    return (int) hwm;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SWAP value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_swap (void *zocket, int swap)
{
#   if defined (ZMQ_SWAP)
    int64_t value = swap;
    int rc = zmq_setsockopt (zocket, ZMQ_SWAP, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SWAP value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_swap (void *zocket)
{
#   if defined (ZMQ_SWAP)
    int64_t swap;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_SWAP, &swap, &option_len);
    return (int) swap;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_affinity (void *zocket, int affinity)
{
#   if defined (ZMQ_AFFINITY)
    uint64_t value = affinity;
    int rc = zmq_setsockopt (zocket, ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_affinity (void *zocket)
{
#   if defined (ZMQ_AFFINITY)
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_identity (void *zocket, const char * identity)
{
#   if defined (ZMQ_IDENTITY)
    int rc = zmq_setsockopt (zocket, ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char * 
zsocket_identity (void *zocket)
{
#   if defined (ZMQ_IDENTITY)
    size_t option_len = 255;
    char *identity = (char *) zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_IDENTITY, identity, &option_len);
    return (char *) identity;
#   else
    return NULL;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_rate (void *zocket, int rate)
{
#   if defined (ZMQ_RATE)
    int64_t value = rate;
    int rc = zmq_setsockopt (zocket, ZMQ_RATE, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rate (void *zocket)
{
#   if defined (ZMQ_RATE)
    int64_t rate;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_RATE, &rate, &option_len);
    return (int) rate;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_recovery_ivl (void *zocket, int recovery_ivl)
{
#   if defined (ZMQ_RECOVERY_IVL)
    int64_t value = recovery_ivl;
    int rc = zmq_setsockopt (zocket, ZMQ_RECOVERY_IVL, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_recovery_ivl (void *zocket)
{
#   if defined (ZMQ_RECOVERY_IVL)
    int64_t recovery_ivl;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return (int) recovery_ivl;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL_MSEC value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_recovery_ivl_msec (void *zocket, int recovery_ivl_msec)
{
#   if defined (ZMQ_RECOVERY_IVL_MSEC)
    int64_t value = recovery_ivl_msec;
    int rc = zmq_setsockopt (zocket, ZMQ_RECOVERY_IVL_MSEC, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL_MSEC value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_recovery_ivl_msec (void *zocket)
{
#   if defined (ZMQ_RECOVERY_IVL_MSEC)
    int64_t recovery_ivl_msec;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_RECOVERY_IVL_MSEC, &recovery_ivl_msec, &option_len);
    return (int) recovery_ivl_msec;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MCAST_LOOP value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_mcast_loop (void *zocket, int mcast_loop)
{
#   if defined (ZMQ_MCAST_LOOP)
    int64_t value = mcast_loop;
    int rc = zmq_setsockopt (zocket, ZMQ_MCAST_LOOP, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MCAST_LOOP value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_mcast_loop (void *zocket)
{
#   if defined (ZMQ_MCAST_LOOP)
    int64_t mcast_loop;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_MCAST_LOOP, &mcast_loop, &option_len);
    return (int) mcast_loop;
#   else
    return 0;
#   endif
}


#   if (ZMQ_VERSION_MINOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_rcvtimeo (void *zocket, int rcvtimeo)
{
#   if defined (ZMQ_RCVTIMEO)
    int rc = zmq_setsockopt (zocket, ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rcvtimeo (void *zocket)
{
#   if defined (ZMQ_RCVTIMEO)
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
#   else
    return 0;
#   endif
}


#   endif
#   if (ZMQ_VERSION_MINOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_sndtimeo (void *zocket, int sndtimeo)
{
#   if defined (ZMQ_SNDTIMEO)
    int rc = zmq_setsockopt (zocket, ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_sndtimeo (void *zocket)
{
#   if defined (ZMQ_SNDTIMEO)
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
#   else
    return 0;
#   endif
}


#   endif
//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_sndbuf (void *zocket, int sndbuf)
{
#   if defined (ZMQ_SNDBUF)
    uint64_t value = sndbuf;
    int rc = zmq_setsockopt (zocket, ZMQ_SNDBUF, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_sndbuf (void *zocket)
{
#   if defined (ZMQ_SNDBUF)
    uint64_t sndbuf;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_SNDBUF, &sndbuf, &option_len);
    return (int) sndbuf;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_rcvbuf (void *zocket, int rcvbuf)
{
#   if defined (ZMQ_RCVBUF)
    uint64_t value = rcvbuf;
    int rc = zmq_setsockopt (zocket, ZMQ_RCVBUF, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rcvbuf (void *zocket)
{
#   if defined (ZMQ_RCVBUF)
    uint64_t rcvbuf;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_RCVBUF, &rcvbuf, &option_len);
    return (int) rcvbuf;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_linger (void *zocket, int linger)
{
#   if defined (ZMQ_LINGER)
    int rc = zmq_setsockopt (zocket, ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_linger (void *zocket)
{
#   if defined (ZMQ_LINGER)
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_LINGER, &linger, &option_len);
    return linger;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl)
{
#   if defined (ZMQ_RECONNECT_IVL)
    int rc = zmq_setsockopt (zocket, ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_reconnect_ivl (void *zocket)
{
#   if defined (ZMQ_RECONNECT_IVL)
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max)
{
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int rc = zmq_setsockopt (zocket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_reconnect_ivl_max (void *zocket)
{
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_backlog (void *zocket, int backlog)
{
#   if defined (ZMQ_BACKLOG)
    int rc = zmq_setsockopt (zocket, ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_backlog (void *zocket)
{
#   if defined (ZMQ_BACKLOG)
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_subscribe (void *zocket, const char * subscribe)
{
#   if defined (ZMQ_SUBSCRIBE)
    if (zsockopt_type (zocket) != ZMQ_SUB) {
        printf ("ZMQ_SUBSCRIBE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsocket_set_unsubscribe (void *zocket, const char * unsubscribe)
{
#   if defined (ZMQ_UNSUBSCRIBE)
    if (zsockopt_type (zocket) != ZMQ_SUB) {
        printf ("ZMQ_UNSUBSCRIBE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_type (void *zocket)
{
#   if defined (ZMQ_TYPE)
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_TYPE, &type, &option_len);
    return type;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_rcvmore (void *zocket)
{
#   if defined (ZMQ_RCVMORE)
    int64_t rcvmore;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_RCVMORE, &rcvmore, &option_len);
    return (int) rcvmore;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_fd (void *zocket)
{
#   if defined (ZMQ_FD)
    int fd;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_FD, &fd, &option_len);
    return fd;
#   else
    return 0;
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int 
zsocket_events (void *zocket)
{
#   if defined (ZMQ_EVENTS)
    uint32_t events;
    size_t option_len = sizeof (uint32_t);
    zmq_getsockopt (zocket, ZMQ_EVENTS, &events, &option_len);
    return (int) events;
#   else
    return 0;
#   endif
}


#endif

//  --------------------------------------------------------------------------
//  Selftest
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsockopt_test (bool verbose)
{
    printf (" * zsockopt: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *zocket;
#if (ZMQ_VERSION_MAJOR == 3)
#       if defined (ZMQ_TYPE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_type (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_SNDHWM)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_sndhwm (zocket, 1);
    assert (zsocket_sndhwm (zocket) == 1);
    zsocket_sndhwm (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RCVHWM)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvhwm (zocket, 1);
    assert (zsocket_rcvhwm (zocket) == 1);
    zsocket_rcvhwm (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_AFFINITY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_affinity (zocket, 1);
    assert (zsocket_affinity (zocket) == 1);
    zsocket_affinity (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_SUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_subscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_UNSUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_unsubscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_IDENTITY)
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_set_identity (zocket, "test");
    char *identity = zsocket_identity (zocket);
    assert (identity);
    free (identity);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RATE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rate (zocket, 1);
    assert (zsocket_rate (zocket) == 1);
    zsocket_rate (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RECOVERY_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl (zocket, 1);
    assert (zsocket_recovery_ivl (zocket) == 1);
    zsocket_recovery_ivl (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_SNDBUF)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_sndbuf (zocket, 1);
    assert (zsocket_sndbuf (zocket) == 1);
    zsocket_sndbuf (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RCVBUF)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvbuf (zocket, 1);
    assert (zsocket_rcvbuf (zocket) == 1);
    zsocket_rcvbuf (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_LINGER)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_linger (zocket, 1);
    assert (zsocket_linger (zocket) == 1);
    zsocket_linger (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RECONNECT_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl (zocket, 1);
    assert (zsocket_reconnect_ivl (zocket) == 1);
    zsocket_reconnect_ivl (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RECONNECT_IVL_MAX)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl_max (zocket, 1);
    assert (zsocket_reconnect_ivl_max (zocket) == 1);
    zsocket_reconnect_ivl_max (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_BACKLOG)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_backlog (zocket, 1);
    assert (zsocket_backlog (zocket) == 1);
    zsocket_backlog (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_MAXMSGSIZE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_maxmsgsize (zocket, 1);
    assert (zsocket_maxmsgsize (zocket) == 1);
    zsocket_maxmsgsize (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_MULTICAST_HOPS)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_multicast_hops (zocket, 1);
    assert (zsocket_multicast_hops (zocket) == 1);
    zsocket_multicast_hops (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RCVTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvtimeo (zocket, 1);
    assert (zsocket_rcvtimeo (zocket) == 1);
    zsocket_rcvtimeo (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_SNDTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndtimeo (zocket, 1);
    assert (zsocket_sndtimeo (zocket) == 1);
    zsocket_sndtimeo (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_IPV6)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_ipv6 (zocket, 1);
    assert (zsocket_ipv6 (zocket) == 1);
    zsocket_ipv6 (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_IMMEDIATE)
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_set_immediate (zocket, 1);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_ROUTER_MANDATORY)
    zocket = zsocket_new (ctx, ZMQ_ROUTER);
    assert (zocket);
    zsocket_set_router_mandatory (zocket, 1);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_PROBE_ROUTER)
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_probe_router (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_XPUB_VERBOSE)
    zocket = zsocket_new (ctx, ZMQ_XPUB);
    assert (zocket);
    zsocket_set_xpub_verbose (zocket, 1);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_TCP_KEEPALIVE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive (zocket, 1);
    assert (zsocket_tcp_keepalive (zocket) == 1);
    zsocket_tcp_keepalive (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_idle (zocket, 1);
    assert (zsocket_tcp_keepalive_idle (zocket) == 1);
    zsocket_tcp_keepalive_idle (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_TCP_KEEPALIVE_CNT)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_cnt (zocket, 1);
    assert (zsocket_tcp_keepalive_cnt (zocket) == 1);
    zsocket_tcp_keepalive_cnt (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_intvl (zocket, 1);
    assert (zsocket_tcp_keepalive_intvl (zocket) == 1);
    zsocket_tcp_keepalive_intvl (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_TCP_ACCEPT_FILTER)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_accept_filter (zocket, "127.0.0.1");
    char *tcp_accept_filter = zsocket_tcp_accept_filter (zocket);
    assert (tcp_accept_filter);
    free (tcp_accept_filter);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_PLAIN_SERVER)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_plain_server (zocket, 1);
    assert (zsocket_plain_server (zocket) == 1);
    zsocket_plain_server (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_PLAIN_USERNAME)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_plain_username (zocket, "test");
    char *plain_username = zsocket_plain_username (zocket);
    assert (plain_username);
    free (plain_username);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_PLAIN_PASSWORD)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_plain_password (zocket, "test");
    char *plain_password = zsocket_plain_password (zocket);
    assert (plain_password);
    free (plain_password);
    zsocket_destroy (ctx, zocket);
#       endif
#     if defined (HAVE_LIBSODIUM)
#       if defined (ZMQ_CURVE_SERVER)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_curve_server (zocket, 1);
    assert (zsocket_curve_server (zocket) == 1);
    zsocket_curve_server (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#     endif
#     if defined (HAVE_LIBSODIUM)
#       if defined (ZMQ_CURVE_PUBLICKEY)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_curve_publickey (zocket, "Yne@$w-vo<fVvi]a<NY6T1ed:M$fCG*[IaLV{hID");
    char *curve_publickey = zsocket_curve_publickey (zocket);
    assert (curve_publickey);
    free (curve_publickey);
    zsocket_destroy (ctx, zocket);
#       endif
#     endif
#     if defined (HAVE_LIBSODIUM)
#       if defined (ZMQ_CURVE_SECRETKEY)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_curve_secretkey (zocket, "D:)Q[IlAW!ahhC2ac:9*A}h:p?([4%wOTJ%JR%cs");
    char *curve_secretkey = zsocket_curve_secretkey (zocket);
    assert (curve_secretkey);
    free (curve_secretkey);
    zsocket_destroy (ctx, zocket);
#       endif
#     endif
#     if defined (HAVE_LIBSODIUM)
#       if defined (ZMQ_CURVE_SERVERKEY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_curve_serverkey (zocket, "rq:rM>}U?@Lns47E1%kR.o@n%FcmmsL/@{H8]yf7");
    char *curve_serverkey = zsocket_curve_serverkey (zocket);
    assert (curve_serverkey);
    free (curve_serverkey);
    zsocket_destroy (ctx, zocket);
#       endif
#     endif
#       if defined (ZMQ_ZAP_DOMAIN)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_zap_domain (zocket, "test");
    char *zap_domain = zsocket_zap_domain (zocket);
    assert (zap_domain);
    free (zap_domain);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RCVMORE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_rcvmore (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_FD)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_fd (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_EVENTS)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_events (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_LAST_ENDPOINT)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    char *last_endpoint = zsocket_last_endpoint (zocket);
    assert (last_endpoint);
    free (last_endpoint);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_ROUTER_RAW)
    zocket = zsocket_new (ctx, ZMQ_ROUTER);
    assert (zocket);
    zsocket_set_router_raw (zocket, 1);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_IPV4ONLY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_ipv4only (zocket, 1);
    assert (zsocket_ipv4only (zocket) == 1);
    zsocket_ipv4only (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_delay_attach_on_connect (zocket, 1);
    zsocket_destroy (ctx, zocket);
#       endif

    zocket = zsocket_new (ctx, ZMQ_SUB);
    zsocket_set_hwm (zocket, 1);
#endif

#if (ZMQ_VERSION_MAJOR == 2)
#       if defined (ZMQ_HWM)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_hwm (zocket, 1);
    assert (zsocket_hwm (zocket) == 1);
    zsocket_hwm (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_SWAP)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_swap (zocket, 1);
    assert (zsocket_swap (zocket) == 1);
    zsocket_swap (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_AFFINITY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_affinity (zocket, 1);
    assert (zsocket_affinity (zocket) == 1);
    zsocket_affinity (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_IDENTITY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_identity (zocket, "test");
    char *identity = zsocket_identity (zocket);
    assert (identity);
    free (identity);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RATE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rate (zocket, 1);
    assert (zsocket_rate (zocket) == 1);
    zsocket_rate (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RECOVERY_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl (zocket, 1);
    assert (zsocket_recovery_ivl (zocket) == 1);
    zsocket_recovery_ivl (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RECOVERY_IVL_MSEC)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl_msec (zocket, 1);
    assert (zsocket_recovery_ivl_msec (zocket) == 1);
    zsocket_recovery_ivl_msec (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_MCAST_LOOP)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_mcast_loop (zocket, 1);
    assert (zsocket_mcast_loop (zocket) == 1);
    zsocket_mcast_loop (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#   if (ZMQ_VERSION_MINOR == 2)
#       if defined (ZMQ_RCVTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvtimeo (zocket, 1);
    assert (zsocket_rcvtimeo (zocket) == 1);
    zsocket_rcvtimeo (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
#       if defined (ZMQ_SNDTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndtimeo (zocket, 1);
    assert (zsocket_sndtimeo (zocket) == 1);
    zsocket_sndtimeo (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#   endif
#       if defined (ZMQ_SNDBUF)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndbuf (zocket, 1);
    assert (zsocket_sndbuf (zocket) == 1);
    zsocket_sndbuf (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RCVBUF)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvbuf (zocket, 1);
    assert (zsocket_rcvbuf (zocket) == 1);
    zsocket_rcvbuf (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_LINGER)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_linger (zocket, 1);
    assert (zsocket_linger (zocket) == 1);
    zsocket_linger (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RECONNECT_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl (zocket, 1);
    assert (zsocket_reconnect_ivl (zocket) == 1);
    zsocket_reconnect_ivl (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RECONNECT_IVL_MAX)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl_max (zocket, 1);
    assert (zsocket_reconnect_ivl_max (zocket) == 1);
    zsocket_reconnect_ivl_max (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_BACKLOG)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_backlog (zocket, 1);
    assert (zsocket_backlog (zocket) == 1);
    zsocket_backlog (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_SUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_subscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_UNSUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_unsubscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_TYPE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_type (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_RCVMORE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_rcvmore (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_FD)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_fd (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#       if defined (ZMQ_EVENTS)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_events (zocket);
    zsocket_destroy (ctx, zocket);
#       endif
#endif

    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
