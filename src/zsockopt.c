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

/*
@header
    The zsockopt class provides access to the 0MQ getsockopt/setsockopt API.
@discuss
    This class is generated, using the GSL code generator. See the sockopts
    XML file, which provides the metadata, and the sockopts.gsl template,
    which does the work.
@end
*/

#include "../include/czmq_prelude.h"
#include "../include/zctx.h"
#include "../include/zsocket.h"
#include "../include/zsockopt.h"


#if (ZMQ_VERSION_MAJOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_HWM value

void
zsocket_set_hwm (void *socket, int hwm)
{
    uint64_t value = hwm;
    int rc = zmq_setsockopt (socket, ZMQ_HWM, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_HWM value

int
zsocket_hwm (void *socket)
{
    uint64_t hwm;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (socket, ZMQ_HWM, &hwm, &option_len);
    return (int) hwm;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SWAP value

void
zsocket_set_swap (void *socket, int swap)
{
    int64_t value = swap;
    int rc = zmq_setsockopt (socket, ZMQ_SWAP, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SWAP value

int
zsocket_swap (void *socket)
{
    int64_t swap;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (socket, ZMQ_SWAP, &swap, &option_len);
    return (int) swap;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value

void
zsocket_set_affinity (void *socket, int affinity)
{
    uint64_t value = affinity;
    int rc = zmq_setsockopt (socket, ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value

int
zsocket_affinity (void *socket)
{
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (socket, ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value

void
zsocket_set_identity (void *socket, char * identity)
{
    int rc = zmq_setsockopt (socket, ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value

char *
zsocket_identity (void *socket)
{
    size_t option_len = 255;
    char *identity = zmalloc (option_len);
    zmq_getsockopt (socket, ZMQ_IDENTITY, &identity, &option_len);
    return (char *) identity;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value

void
zsocket_set_rate (void *socket, int rate)
{
    int64_t value = rate;
    int rc = zmq_setsockopt (socket, ZMQ_RATE, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value

int
zsocket_rate (void *socket)
{
    int64_t rate;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (socket, ZMQ_RATE, &rate, &option_len);
    return (int) rate;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value

void
zsocket_set_recovery_ivl (void *socket, int recovery_ivl)
{
    int64_t value = recovery_ivl;
    int rc = zmq_setsockopt (socket, ZMQ_RECOVERY_IVL, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value

int
zsocket_recovery_ivl (void *socket)
{
    int64_t recovery_ivl;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (socket, ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return (int) recovery_ivl;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL_MSEC value

void
zsocket_set_recovery_ivl_msec (void *socket, int recovery_ivl_msec)
{
    int64_t value = recovery_ivl_msec;
    int rc = zmq_setsockopt (socket, ZMQ_RECOVERY_IVL_MSEC, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL_MSEC value

int
zsocket_recovery_ivl_msec (void *socket)
{
    int64_t recovery_ivl_msec;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (socket, ZMQ_RECOVERY_IVL_MSEC, &recovery_ivl_msec, &option_len);
    return (int) recovery_ivl_msec;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MCAST_LOOP value

void
zsocket_set_mcast_loop (void *socket, int mcast_loop)
{
    int64_t value = mcast_loop;
    int rc = zmq_setsockopt (socket, ZMQ_MCAST_LOOP, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MCAST_LOOP value

int
zsocket_mcast_loop (void *socket)
{
    int64_t mcast_loop;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (socket, ZMQ_MCAST_LOOP, &mcast_loop, &option_len);
    return (int) mcast_loop;
}


#   if (ZMQ_VERSION_MINOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value

void
zsocket_set_rcvtimeo (void *socket, int rcvtimeo)
{
    int rc = zmq_setsockopt (socket, ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value

int
zsocket_rcvtimeo (void *socket)
{
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
}


#   endif
#   if (ZMQ_VERSION_MINOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value

void
zsocket_set_sndtimeo (void *socket, int sndtimeo)
{
    int rc = zmq_setsockopt (socket, ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value

int
zsocket_sndtimeo (void *socket)
{
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
}


#   endif
//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value

void
zsocket_set_sndbuf (void *socket, int sndbuf)
{
    uint64_t value = sndbuf;
    int rc = zmq_setsockopt (socket, ZMQ_SNDBUF, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value

int
zsocket_sndbuf (void *socket)
{
    uint64_t sndbuf;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (socket, ZMQ_SNDBUF, &sndbuf, &option_len);
    return (int) sndbuf;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value

void
zsocket_set_rcvbuf (void *socket, int rcvbuf)
{
    uint64_t value = rcvbuf;
    int rc = zmq_setsockopt (socket, ZMQ_RCVBUF, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value

int
zsocket_rcvbuf (void *socket)
{
    uint64_t rcvbuf;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (socket, ZMQ_RCVBUF, &rcvbuf, &option_len);
    return (int) rcvbuf;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value

void
zsocket_set_linger (void *socket, int linger)
{
    int rc = zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value

int
zsocket_linger (void *socket)
{
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_LINGER, &linger, &option_len);
    return linger;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value

void
zsocket_set_reconnect_ivl (void *socket, int reconnect_ivl)
{
    int rc = zmq_setsockopt (socket, ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value

int
zsocket_reconnect_ivl (void *socket)
{
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value

void
zsocket_set_reconnect_ivl_max (void *socket, int reconnect_ivl_max)
{
    int rc = zmq_setsockopt (socket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value

int
zsocket_reconnect_ivl_max (void *socket)
{
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value

void
zsocket_set_backlog (void *socket, int backlog)
{
    int rc = zmq_setsockopt (socket, ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value

int
zsocket_backlog (void *socket)
{
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value

void
zsocket_set_subscribe (void *socket, char * subscribe)
{
    int rc = zmq_setsockopt (socket, ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value

void
zsocket_set_unsubscribe (void *socket, char * unsubscribe)
{
    int rc = zmq_setsockopt (socket, ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value

int
zsocket_type (void *socket)
{
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_TYPE, &type, &option_len);
    return type;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value

int
zsocket_rcvmore (void *socket)
{
    int64_t rcvmore;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (socket, ZMQ_RCVMORE, &rcvmore, &option_len);
    return (int) rcvmore;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value

int
zsocket_fd (void *socket)
{
    int fd;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_FD, &fd, &option_len);
    return fd;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value

int
zsocket_events (void *socket)
{
    uint32_t events;
    size_t option_len = sizeof (uint32_t);
    zmq_getsockopt (socket, ZMQ_EVENTS, &events, &option_len);
    return (int) events;
}


#endif

#if (ZMQ_VERSION_MAJOR == 3)
//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value

int
zsocket_type (void *socket)
{
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_TYPE, &type, &option_len);
    return type;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDHWM value

void
zsocket_set_sndhwm (void *socket, int sndhwm)
{
    int rc = zmq_setsockopt (socket, ZMQ_SNDHWM, &sndhwm, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDHWM value

int
zsocket_sndhwm (void *socket)
{
    int sndhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_SNDHWM, &sndhwm, &option_len);
    return sndhwm;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVHWM value

void
zsocket_set_rcvhwm (void *socket, int rcvhwm)
{
    int rc = zmq_setsockopt (socket, ZMQ_RCVHWM, &rcvhwm, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVHWM value

int
zsocket_rcvhwm (void *socket)
{
    int rcvhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RCVHWM, &rcvhwm, &option_len);
    return rcvhwm;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value

void
zsocket_set_affinity (void *socket, int affinity)
{
    uint64_t value = affinity;
    int rc = zmq_setsockopt (socket, ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value

int
zsocket_affinity (void *socket)
{
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (socket, ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value

void
zsocket_set_subscribe (void *socket, char * subscribe)
{
    int rc = zmq_setsockopt (socket, ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value

void
zsocket_set_unsubscribe (void *socket, char * unsubscribe)
{
    int rc = zmq_setsockopt (socket, ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value

void
zsocket_set_identity (void *socket, char * identity)
{
    int rc = zmq_setsockopt (socket, ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value

char *
zsocket_identity (void *socket)
{
    size_t option_len = 255;
    char *identity = zmalloc (option_len);
    zmq_getsockopt (socket, ZMQ_IDENTITY, &identity, &option_len);
    return (char *) identity;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value

void
zsocket_set_rate (void *socket, int rate)
{
    int rc = zmq_setsockopt (socket, ZMQ_RATE, &rate, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value

int
zsocket_rate (void *socket)
{
    int rate;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RATE, &rate, &option_len);
    return rate;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value

void
zsocket_set_recovery_ivl (void *socket, int recovery_ivl)
{
    int rc = zmq_setsockopt (socket, ZMQ_RECOVERY_IVL, &recovery_ivl, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value

int
zsocket_recovery_ivl (void *socket)
{
    int recovery_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return recovery_ivl;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value

void
zsocket_set_sndbuf (void *socket, int sndbuf)
{
    int rc = zmq_setsockopt (socket, ZMQ_SNDBUF, &sndbuf, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value

int
zsocket_sndbuf (void *socket)
{
    int sndbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_SNDBUF, &sndbuf, &option_len);
    return sndbuf;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value

void
zsocket_set_rcvbuf (void *socket, int rcvbuf)
{
    int rc = zmq_setsockopt (socket, ZMQ_RCVBUF, &rcvbuf, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value

int
zsocket_rcvbuf (void *socket)
{
    int rcvbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RCVBUF, &rcvbuf, &option_len);
    return rcvbuf;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value

void
zsocket_set_linger (void *socket, int linger)
{
    int rc = zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value

int
zsocket_linger (void *socket)
{
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_LINGER, &linger, &option_len);
    return linger;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value

void
zsocket_set_reconnect_ivl (void *socket, int reconnect_ivl)
{
    int rc = zmq_setsockopt (socket, ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value

int
zsocket_reconnect_ivl (void *socket)
{
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value

void
zsocket_set_reconnect_ivl_max (void *socket, int reconnect_ivl_max)
{
    int rc = zmq_setsockopt (socket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value

int
zsocket_reconnect_ivl_max (void *socket)
{
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value

void
zsocket_set_backlog (void *socket, int backlog)
{
    int rc = zmq_setsockopt (socket, ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value

int
zsocket_backlog (void *socket)
{
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MAXMSGSIZE value

void
zsocket_set_maxmsgsize (void *socket, int maxmsgsize)
{
    int64_t value = maxmsgsize;
    int rc = zmq_setsockopt (socket, ZMQ_MAXMSGSIZE, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MAXMSGSIZE value

int
zsocket_maxmsgsize (void *socket)
{
    int64_t maxmsgsize;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (socket, ZMQ_MAXMSGSIZE, &maxmsgsize, &option_len);
    return (int) maxmsgsize;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MULTICAST_HOPS value

void
zsocket_set_multicast_hops (void *socket, int multicast_hops)
{
    int rc = zmq_setsockopt (socket, ZMQ_MULTICAST_HOPS, &multicast_hops, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MULTICAST_HOPS value

int
zsocket_multicast_hops (void *socket)
{
    int multicast_hops;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_MULTICAST_HOPS, &multicast_hops, &option_len);
    return multicast_hops;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value

void
zsocket_set_rcvtimeo (void *socket, int rcvtimeo)
{
    int rc = zmq_setsockopt (socket, ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value

int
zsocket_rcvtimeo (void *socket)
{
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value

void
zsocket_set_sndtimeo (void *socket, int sndtimeo)
{
    int rc = zmq_setsockopt (socket, ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value

int
zsocket_sndtimeo (void *socket)
{
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IPV4ONLY value

void
zsocket_set_ipv4only (void *socket, int ipv4only)
{
    int rc = zmq_setsockopt (socket, ZMQ_IPV4ONLY, &ipv4only, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IPV4ONLY value

int
zsocket_ipv4only (void *socket)
{
    int ipv4only;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_IPV4ONLY, &ipv4only, &option_len);
    return ipv4only;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_FAIL_UNROUTABLE value

void
zsocket_set_fail_unroutable (void *socket, int fail_unroutable)
{
    int rc = zmq_setsockopt (socket, ZMQ_FAIL_UNROUTABLE, &fail_unroutable, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value

int
zsocket_rcvmore (void *socket)
{
    int rcvmore;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_RCVMORE, &rcvmore, &option_len);
    return rcvmore;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value

int
zsocket_fd (void *socket)
{
    int fd;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_FD, &fd, &option_len);
    return fd;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value

int
zsocket_events (void *socket)
{
    int events;
    size_t option_len = sizeof (int);
    zmq_getsockopt (socket, ZMQ_EVENTS, &events, &option_len);
    return events;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LAST_ENDPOINT value

char *
zsocket_last_endpoint (void *socket)
{
    size_t option_len = 255;
    char *last_endpoint = zmalloc (option_len);
    zmq_getsockopt (socket, ZMQ_LAST_ENDPOINT, &last_endpoint, &option_len);
    return (char *) last_endpoint;
}


//  --------------------------------------------------------------------------
//  Set socket high-water mark, emulating 2.x API

void
zsocket_set_hwm (void *socket, int hwm)
{
    zsocket_set_sndhwm (socket, hwm);
    zsocket_set_rcvhwm (socket, hwm);
}

#endif

//  --------------------------------------------------------------------------
//  Selftest

int
zsockopt_test (Bool verbose)
{
    printf (" * zsockopt: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *socket;
#if (ZMQ_VERSION_MAJOR == 2)
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_hwm (socket, 1);
    assert (zsocket_hwm (socket) == 1);
    zsocket_hwm (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_swap (socket, 1);
    assert (zsocket_swap (socket) == 1);
    zsocket_swap (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_affinity (socket, 1);
    assert (zsocket_affinity (socket) == 1);
    zsocket_affinity (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_identity (socket, "test"); //a
    zsocket_identity (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_rate (socket, 1);
    assert (zsocket_rate (socket) == 1);
    zsocket_rate (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_recovery_ivl (socket, 1);
    assert (zsocket_recovery_ivl (socket) == 1);
    zsocket_recovery_ivl (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_recovery_ivl_msec (socket, 1);
    assert (zsocket_recovery_ivl_msec (socket) == 1);
    zsocket_recovery_ivl_msec (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_mcast_loop (socket, 1);
    assert (zsocket_mcast_loop (socket) == 1);
    zsocket_mcast_loop (socket);
    zsocket_destroy (ctx, socket);
#   if (ZMQ_VERSION_MINOR == 2)
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_rcvtimeo (socket, 1);
    assert (zsocket_rcvtimeo (socket) == 1);
    zsocket_rcvtimeo (socket);
    zsocket_destroy (ctx, socket);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_sndtimeo (socket, 1);
    assert (zsocket_sndtimeo (socket) == 1);
    zsocket_sndtimeo (socket);
    zsocket_destroy (ctx, socket);
#   endif
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_sndbuf (socket, 1);
    assert (zsocket_sndbuf (socket) == 1);
    zsocket_sndbuf (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_rcvbuf (socket, 1);
    assert (zsocket_rcvbuf (socket) == 1);
    zsocket_rcvbuf (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_linger (socket, 1);
    assert (zsocket_linger (socket) == 1);
    zsocket_linger (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_reconnect_ivl (socket, 1);
    assert (zsocket_reconnect_ivl (socket) == 1);
    zsocket_reconnect_ivl (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_reconnect_ivl_max (socket, 1);
    assert (zsocket_reconnect_ivl_max (socket) == 1);
    zsocket_reconnect_ivl_max (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_backlog (socket, 1);
    assert (zsocket_backlog (socket) == 1);
    zsocket_backlog (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_subscribe (socket, "test"); //a
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_unsubscribe (socket, "test"); //a
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_type (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_rcvmore (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_fd (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_events (socket);
    zsocket_destroy (ctx, socket);
#endif

#if (ZMQ_VERSION_MAJOR == 3)
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_type (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_PUB);
    assert (socket);
    zsocket_set_sndhwm (socket, 1);
    assert (zsocket_sndhwm (socket) == 1);
    zsocket_sndhwm (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_rcvhwm (socket, 1);
    assert (zsocket_rcvhwm (socket) == 1);
    zsocket_rcvhwm (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_affinity (socket, 1);
    assert (zsocket_affinity (socket) == 1);
    zsocket_affinity (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_subscribe (socket, "test"); //a
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_unsubscribe (socket, "test"); //a
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_identity (socket, "test"); //a
    zsocket_identity (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_rate (socket, 1);
    assert (zsocket_rate (socket) == 1);
    zsocket_rate (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_recovery_ivl (socket, 1);
    assert (zsocket_recovery_ivl (socket) == 1);
    zsocket_recovery_ivl (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_PUB);
    assert (socket);
    zsocket_set_sndbuf (socket, 1);
    assert (zsocket_sndbuf (socket) == 1);
    zsocket_sndbuf (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_rcvbuf (socket, 1);
    assert (zsocket_rcvbuf (socket) == 1);
    zsocket_rcvbuf (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_linger (socket, 1);
    assert (zsocket_linger (socket) == 1);
    zsocket_linger (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_reconnect_ivl (socket, 1);
    assert (zsocket_reconnect_ivl (socket) == 1);
    zsocket_reconnect_ivl (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_reconnect_ivl_max (socket, 1);
    assert (zsocket_reconnect_ivl_max (socket) == 1);
    zsocket_reconnect_ivl_max (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_backlog (socket, 1);
    assert (zsocket_backlog (socket) == 1);
    zsocket_backlog (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_maxmsgsize (socket, 1);
    assert (zsocket_maxmsgsize (socket) == 1);
    zsocket_maxmsgsize (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_multicast_hops (socket, 1);
    assert (zsocket_multicast_hops (socket) == 1);
    zsocket_multicast_hops (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_rcvtimeo (socket, 1);
    assert (zsocket_rcvtimeo (socket) == 1);
    zsocket_rcvtimeo (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_sndtimeo (socket, 1);
    assert (zsocket_sndtimeo (socket) == 1);
    zsocket_sndtimeo (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_set_ipv4only (socket, 1);
    assert (zsocket_ipv4only (socket) == 1);
    zsocket_ipv4only (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_ROUTER);
    assert (socket);
    zsocket_set_fail_unroutable (socket, 1);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_rcvmore (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_fd (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_events (socket);
    zsocket_destroy (ctx, socket);
    socket = zsocket_new (ctx, ZMQ_SUB);
    assert (socket);
    zsocket_last_endpoint (socket);
    zsocket_destroy (ctx, socket);

    zsocket_set_hwm (socket, 1);
#endif

    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
