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
zsocket_set_hwm (void *zocket, int hwm)
{
#   if !defined (ZMQ_HWM)
#       define ZMQ_HWM 999          //  Using an out-of-date libzmq
#   endif
    uint64_t value = hwm;
    int rc = zmq_setsockopt (zocket, ZMQ_HWM, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_HWM value

int
zsocket_hwm (void *zocket)
{
    uint64_t hwm;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_HWM, &hwm, &option_len);
    return (int) hwm;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SWAP value

void
zsocket_set_swap (void *zocket, int swap)
{
#   if !defined (ZMQ_SWAP)
#       define ZMQ_SWAP 999         //  Using an out-of-date libzmq
#   endif
    int64_t value = swap;
    int rc = zmq_setsockopt (zocket, ZMQ_SWAP, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SWAP value

int
zsocket_swap (void *zocket)
{
    int64_t swap;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_SWAP, &swap, &option_len);
    return (int) swap;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value

void
zsocket_set_affinity (void *zocket, int affinity)
{
#   if !defined (ZMQ_AFFINITY)
#       define ZMQ_AFFINITY 999     //  Using an out-of-date libzmq
#   endif
    uint64_t value = affinity;
    int rc = zmq_setsockopt (zocket, ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value

int
zsocket_affinity (void *zocket)
{
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value

void
zsocket_set_identity (void *zocket, char * identity)
{
#   if !defined (ZMQ_IDENTITY)
#       define ZMQ_IDENTITY 999     //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value

char *
zsocket_identity (void *zocket)
{
    size_t option_len = 255;
    char *identity = zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_IDENTITY, identity, &option_len);
    return (char *) identity;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value

void
zsocket_set_rate (void *zocket, int rate)
{
#   if !defined (ZMQ_RATE)
#       define ZMQ_RATE 999         //  Using an out-of-date libzmq
#   endif
    int64_t value = rate;
    int rc = zmq_setsockopt (zocket, ZMQ_RATE, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value

int
zsocket_rate (void *zocket)
{
    int64_t rate;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_RATE, &rate, &option_len);
    return (int) rate;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value

void
zsocket_set_recovery_ivl (void *zocket, int recovery_ivl)
{
#   if !defined (ZMQ_RECOVERY_IVL)
#       define ZMQ_RECOVERY_IVL 999  //  Using an out-of-date libzmq
#   endif
    int64_t value = recovery_ivl;
    int rc = zmq_setsockopt (zocket, ZMQ_RECOVERY_IVL, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value

int
zsocket_recovery_ivl (void *zocket)
{
    int64_t recovery_ivl;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return (int) recovery_ivl;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL_MSEC value

void
zsocket_set_recovery_ivl_msec (void *zocket, int recovery_ivl_msec)
{
#   if !defined (ZMQ_RECOVERY_IVL_MSEC)
#       define ZMQ_RECOVERY_IVL_MSEC 999  //  Using an out-of-date libzmq
#   endif
    int64_t value = recovery_ivl_msec;
    int rc = zmq_setsockopt (zocket, ZMQ_RECOVERY_IVL_MSEC, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL_MSEC value

int
zsocket_recovery_ivl_msec (void *zocket)
{
    int64_t recovery_ivl_msec;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_RECOVERY_IVL_MSEC, &recovery_ivl_msec, &option_len);
    return (int) recovery_ivl_msec;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MCAST_LOOP value

void
zsocket_set_mcast_loop (void *zocket, int mcast_loop)
{
#   if !defined (ZMQ_MCAST_LOOP)
#       define ZMQ_MCAST_LOOP 999   //  Using an out-of-date libzmq
#   endif
    int64_t value = mcast_loop;
    int rc = zmq_setsockopt (zocket, ZMQ_MCAST_LOOP, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MCAST_LOOP value

int
zsocket_mcast_loop (void *zocket)
{
    int64_t mcast_loop;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_MCAST_LOOP, &mcast_loop, &option_len);
    return (int) mcast_loop;
}


#   if (ZMQ_VERSION_MINOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value

void
zsocket_set_rcvtimeo (void *zocket, int rcvtimeo)
{
#   if !defined (ZMQ_RCVTIMEO)
#       define ZMQ_RCVTIMEO 999     //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value

int
zsocket_rcvtimeo (void *zocket)
{
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
}


#   endif
#   if (ZMQ_VERSION_MINOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value

void
zsocket_set_sndtimeo (void *zocket, int sndtimeo)
{
#   if !defined (ZMQ_SNDTIMEO)
#       define ZMQ_SNDTIMEO 999     //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value

int
zsocket_sndtimeo (void *zocket)
{
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
}


#   endif
//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value

void
zsocket_set_sndbuf (void *zocket, int sndbuf)
{
#   if !defined (ZMQ_SNDBUF)
#       define ZMQ_SNDBUF 999       //  Using an out-of-date libzmq
#   endif
    uint64_t value = sndbuf;
    int rc = zmq_setsockopt (zocket, ZMQ_SNDBUF, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value

int
zsocket_sndbuf (void *zocket)
{
    uint64_t sndbuf;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_SNDBUF, &sndbuf, &option_len);
    return (int) sndbuf;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value

void
zsocket_set_rcvbuf (void *zocket, int rcvbuf)
{
#   if !defined (ZMQ_RCVBUF)
#       define ZMQ_RCVBUF 999       //  Using an out-of-date libzmq
#   endif
    uint64_t value = rcvbuf;
    int rc = zmq_setsockopt (zocket, ZMQ_RCVBUF, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value

int
zsocket_rcvbuf (void *zocket)
{
    uint64_t rcvbuf;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_RCVBUF, &rcvbuf, &option_len);
    return (int) rcvbuf;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value

void
zsocket_set_linger (void *zocket, int linger)
{
#   if !defined (ZMQ_LINGER)
#       define ZMQ_LINGER 999       //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value

int
zsocket_linger (void *zocket)
{
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_LINGER, &linger, &option_len);
    return linger;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value

void
zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl)
{
#   if !defined (ZMQ_RECONNECT_IVL)
#       define ZMQ_RECONNECT_IVL 999  //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value

int
zsocket_reconnect_ivl (void *zocket)
{
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value

void
zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max)
{
#   if !defined (ZMQ_RECONNECT_IVL_MAX)
#       define ZMQ_RECONNECT_IVL_MAX 999  //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value

int
zsocket_reconnect_ivl_max (void *zocket)
{
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value

void
zsocket_set_backlog (void *zocket, int backlog)
{
#   if !defined (ZMQ_BACKLOG)
#       define ZMQ_BACKLOG 999      //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value

int
zsocket_backlog (void *zocket)
{
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value

void
zsocket_set_subscribe (void *zocket, char * subscribe)
{
#   if !defined (ZMQ_SUBSCRIBE)
#       define ZMQ_SUBSCRIBE 999    //  Using an out-of-date libzmq
#   endif
    if (zsockopt_type (zocket) != ZMQ_SUB) {
        printf ("ZMQ_SUBSCRIBE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value

void
zsocket_set_unsubscribe (void *zocket, char * unsubscribe)
{
#   if !defined (ZMQ_UNSUBSCRIBE)
#       define ZMQ_UNSUBSCRIBE 999  //  Using an out-of-date libzmq
#   endif
    if (zsockopt_type (zocket) != ZMQ_SUB) {
        printf ("ZMQ_UNSUBSCRIBE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value

int
zsocket_type (void *zocket)
{
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_TYPE, &type, &option_len);
    return type;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value

int
zsocket_rcvmore (void *zocket)
{
    int64_t rcvmore;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_RCVMORE, &rcvmore, &option_len);
    return (int) rcvmore;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value

int
zsocket_fd (void *zocket)
{
    int fd;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_FD, &fd, &option_len);
    return fd;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value

int
zsocket_events (void *zocket)
{
    uint32_t events;
    size_t option_len = sizeof (uint32_t);
    zmq_getsockopt (zocket, ZMQ_EVENTS, &events, &option_len);
    return (int) events;
}


#endif

#if (ZMQ_VERSION_MAJOR == 3)
//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value

int
zsocket_type (void *zocket)
{
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_TYPE, &type, &option_len);
    return type;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDHWM value

void
zsocket_set_sndhwm (void *zocket, int sndhwm)
{
#   if !defined (ZMQ_SNDHWM)
#       define ZMQ_SNDHWM 999       //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_SNDHWM, &sndhwm, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDHWM value

int
zsocket_sndhwm (void *zocket)
{
    int sndhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_SNDHWM, &sndhwm, &option_len);
    return sndhwm;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVHWM value

void
zsocket_set_rcvhwm (void *zocket, int rcvhwm)
{
#   if !defined (ZMQ_RCVHWM)
#       define ZMQ_RCVHWM 999       //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RCVHWM, &rcvhwm, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVHWM value

int
zsocket_rcvhwm (void *zocket)
{
    int rcvhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVHWM, &rcvhwm, &option_len);
    return rcvhwm;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value

void
zsocket_set_affinity (void *zocket, int affinity)
{
#   if !defined (ZMQ_AFFINITY)
#       define ZMQ_AFFINITY 999     //  Using an out-of-date libzmq
#   endif
    uint64_t value = affinity;
    int rc = zmq_setsockopt (zocket, ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value

int
zsocket_affinity (void *zocket)
{
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zocket, ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value

void
zsocket_set_subscribe (void *zocket, char * subscribe)
{
#   if !defined (ZMQ_SUBSCRIBE)
#       define ZMQ_SUBSCRIBE 999    //  Using an out-of-date libzmq
#   endif
    if (zsockopt_type (zocket) != ZMQ_SUB) {
        printf ("ZMQ_SUBSCRIBE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value

void
zsocket_set_unsubscribe (void *zocket, char * unsubscribe)
{
#   if !defined (ZMQ_UNSUBSCRIBE)
#       define ZMQ_UNSUBSCRIBE 999  //  Using an out-of-date libzmq
#   endif
    if (zsockopt_type (zocket) != ZMQ_SUB) {
        printf ("ZMQ_UNSUBSCRIBE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value

void
zsocket_set_identity (void *zocket, char * identity)
{
#   if !defined (ZMQ_IDENTITY)
#       define ZMQ_IDENTITY 999     //  Using an out-of-date libzmq
#   endif
    if (zsockopt_type (zocket) != ZMQ_REQ
    &&  zsockopt_type (zocket) != ZMQ_REP
    &&  zsockopt_type (zocket) != ZMQ_DEALER
    &&  zsockopt_type (zocket) != ZMQ_ROUTER) {
        printf ("ZMQ_IDENTITY is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value

char *
zsocket_identity (void *zocket)
{
    size_t option_len = 255;
    char *identity = zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_IDENTITY, identity, &option_len);
    return (char *) identity;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value

void
zsocket_set_rate (void *zocket, int rate)
{
#   if !defined (ZMQ_RATE)
#       define ZMQ_RATE 999         //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RATE, &rate, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value

int
zsocket_rate (void *zocket)
{
    int rate;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RATE, &rate, &option_len);
    return rate;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value

void
zsocket_set_recovery_ivl (void *zocket, int recovery_ivl)
{
#   if !defined (ZMQ_RECOVERY_IVL)
#       define ZMQ_RECOVERY_IVL 999  //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RECOVERY_IVL, &recovery_ivl, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value

int
zsocket_recovery_ivl (void *zocket)
{
    int recovery_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return recovery_ivl;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value

void
zsocket_set_sndbuf (void *zocket, int sndbuf)
{
#   if !defined (ZMQ_SNDBUF)
#       define ZMQ_SNDBUF 999       //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_SNDBUF, &sndbuf, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value

int
zsocket_sndbuf (void *zocket)
{
    int sndbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_SNDBUF, &sndbuf, &option_len);
    return sndbuf;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value

void
zsocket_set_rcvbuf (void *zocket, int rcvbuf)
{
#   if !defined (ZMQ_RCVBUF)
#       define ZMQ_RCVBUF 999       //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RCVBUF, &rcvbuf, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value

int
zsocket_rcvbuf (void *zocket)
{
    int rcvbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVBUF, &rcvbuf, &option_len);
    return rcvbuf;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value

void
zsocket_set_linger (void *zocket, int linger)
{
#   if !defined (ZMQ_LINGER)
#       define ZMQ_LINGER 999       //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value

int
zsocket_linger (void *zocket)
{
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_LINGER, &linger, &option_len);
    return linger;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value

void
zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl)
{
#   if !defined (ZMQ_RECONNECT_IVL)
#       define ZMQ_RECONNECT_IVL 999  //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value

int
zsocket_reconnect_ivl (void *zocket)
{
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value

void
zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max)
{
#   if !defined (ZMQ_RECONNECT_IVL_MAX)
#       define ZMQ_RECONNECT_IVL_MAX 999  //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value

int
zsocket_reconnect_ivl_max (void *zocket)
{
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value

void
zsocket_set_backlog (void *zocket, int backlog)
{
#   if !defined (ZMQ_BACKLOG)
#       define ZMQ_BACKLOG 999      //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value

int
zsocket_backlog (void *zocket)
{
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MAXMSGSIZE value

void
zsocket_set_maxmsgsize (void *zocket, int maxmsgsize)
{
#   if !defined (ZMQ_MAXMSGSIZE)
#       define ZMQ_MAXMSGSIZE 999   //  Using an out-of-date libzmq
#   endif
    int64_t value = maxmsgsize;
    int rc = zmq_setsockopt (zocket, ZMQ_MAXMSGSIZE, &value, sizeof (int64_t));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MAXMSGSIZE value

int
zsocket_maxmsgsize (void *zocket)
{
    int64_t maxmsgsize;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zocket, ZMQ_MAXMSGSIZE, &maxmsgsize, &option_len);
    return (int) maxmsgsize;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_MULTICAST_HOPS value

void
zsocket_set_multicast_hops (void *zocket, int multicast_hops)
{
#   if !defined (ZMQ_MULTICAST_HOPS)
#       define ZMQ_MULTICAST_HOPS 999  //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_MULTICAST_HOPS, &multicast_hops, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MULTICAST_HOPS value

int
zsocket_multicast_hops (void *zocket)
{
    int multicast_hops;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_MULTICAST_HOPS, &multicast_hops, &option_len);
    return multicast_hops;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value

void
zsocket_set_rcvtimeo (void *zocket, int rcvtimeo)
{
#   if !defined (ZMQ_RCVTIMEO)
#       define ZMQ_RCVTIMEO 999     //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value

int
zsocket_rcvtimeo (void *zocket)
{
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value

void
zsocket_set_sndtimeo (void *zocket, int sndtimeo)
{
#   if !defined (ZMQ_SNDTIMEO)
#       define ZMQ_SNDTIMEO 999     //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value

int
zsocket_sndtimeo (void *zocket)
{
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IPV4ONLY value

void
zsocket_set_ipv4only (void *zocket, int ipv4only)
{
#   if !defined (ZMQ_IPV4ONLY)
#       define ZMQ_IPV4ONLY 999     //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_IPV4ONLY, &ipv4only, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IPV4ONLY value

int
zsocket_ipv4only (void *zocket)
{
    int ipv4only;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_IPV4ONLY, &ipv4only, &option_len);
    return ipv4only;
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_DELAY_ATTACH_ON_CONNECT value

void
zsocket_set_delay_attach_on_connect (void *zocket, int delay_attach_on_connect)
{
#   if !defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
#       define ZMQ_DELAY_ATTACH_ON_CONNECT 999  //  Using an out-of-date libzmq
#   endif
    int rc = zmq_setsockopt (zocket, ZMQ_DELAY_ATTACH_ON_CONNECT, &delay_attach_on_connect, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_ROUTER_MANDATORY value

void
zsocket_set_router_mandatory (void *zocket, int router_mandatory)
{
#   if !defined (ZMQ_ROUTER_MANDATORY)
#       define ZMQ_ROUTER_MANDATORY 999  //  Using an out-of-date libzmq
#   endif
    if (zsockopt_type (zocket) != ZMQ_ROUTER) {
        printf ("ZMQ_ROUTER_MANDATORY is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_ROUTER_MANDATORY, &router_mandatory, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_XPUB_VERBOSE value

void
zsocket_set_xpub_verbose (void *zocket, int xpub_verbose)
{
#   if !defined (ZMQ_XPUB_VERBOSE)
#       define ZMQ_XPUB_VERBOSE 999  //  Using an out-of-date libzmq
#   endif
    if (zsockopt_type (zocket) != ZMQ_XPUB) {
        printf ("ZMQ_XPUB_VERBOSE is not valid on %s sockets\n", zsocket_type_str (zocket));
        assert (false);
    }
    int rc = zmq_setsockopt (zocket, ZMQ_XPUB_VERBOSE, &xpub_verbose, sizeof (int));
    assert (rc == 0 || errno == ETERM);
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value

int
zsocket_rcvmore (void *zocket)
{
    int rcvmore;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_RCVMORE, &rcvmore, &option_len);
    return rcvmore;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value

int
zsocket_fd (void *zocket)
{
    int fd;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_FD, &fd, &option_len);
    return fd;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value

int
zsocket_events (void *zocket)
{
    int events;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zocket, ZMQ_EVENTS, &events, &option_len);
    return events;
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LAST_ENDPOINT value

char *
zsocket_last_endpoint (void *zocket)
{
    size_t option_len = 255;
    char *last_endpoint = zmalloc (option_len);
    zmq_getsockopt (zocket, ZMQ_LAST_ENDPOINT, last_endpoint, &option_len);
    return (char *) last_endpoint;
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

//  --------------------------------------------------------------------------
//  Selftest

int
zsockopt_test (Bool verbose)
{
    printf (" * zsockopt: ");

    //  @selftest
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *zocket;
#if (ZMQ_VERSION_MAJOR == 2)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_hwm (zocket, 1);
    assert (zsocket_hwm (zocket) == 1);
    zsocket_hwm (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_swap (zocket, 1);
    assert (zsocket_swap (zocket) == 1);
    zsocket_swap (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_affinity (zocket, 1);
    assert (zsocket_affinity (zocket) == 1);
    zsocket_affinity (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_identity (zocket, "test");
    char *identity = zsocket_identity (zocket);
    assert (identity);
    free (identity);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rate (zocket, 1);
    assert (zsocket_rate (zocket) == 1);
    zsocket_rate (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl (zocket, 1);
    assert (zsocket_recovery_ivl (zocket) == 1);
    zsocket_recovery_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl_msec (zocket, 1);
    assert (zsocket_recovery_ivl_msec (zocket) == 1);
    zsocket_recovery_ivl_msec (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_mcast_loop (zocket, 1);
    assert (zsocket_mcast_loop (zocket) == 1);
    zsocket_mcast_loop (zocket);
    zsocket_destroy (ctx, zocket);
#   if (ZMQ_VERSION_MINOR == 2)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvtimeo (zocket, 1);
    assert (zsocket_rcvtimeo (zocket) == 1);
    zsocket_rcvtimeo (zocket);
    zsocket_destroy (ctx, zocket);
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndtimeo (zocket, 1);
    assert (zsocket_sndtimeo (zocket) == 1);
    zsocket_sndtimeo (zocket);
    zsocket_destroy (ctx, zocket);
#   endif
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndbuf (zocket, 1);
    assert (zsocket_sndbuf (zocket) == 1);
    zsocket_sndbuf (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvbuf (zocket, 1);
    assert (zsocket_rcvbuf (zocket) == 1);
    zsocket_rcvbuf (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_linger (zocket, 1);
    assert (zsocket_linger (zocket) == 1);
    zsocket_linger (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl (zocket, 1);
    assert (zsocket_reconnect_ivl (zocket) == 1);
    zsocket_reconnect_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl_max (zocket, 1);
    assert (zsocket_reconnect_ivl_max (zocket) == 1);
    zsocket_reconnect_ivl_max (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_backlog (zocket, 1);
    assert (zsocket_backlog (zocket) == 1);
    zsocket_backlog (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_subscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_unsubscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_type (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_rcvmore (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_fd (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_events (zocket);
    zsocket_destroy (ctx, zocket);
#endif

#if (ZMQ_VERSION_MAJOR == 3)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_type (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_sndhwm (zocket, 1);
    assert (zsocket_sndhwm (zocket) == 1);
    zsocket_sndhwm (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvhwm (zocket, 1);
    assert (zsocket_rcvhwm (zocket) == 1);
    zsocket_rcvhwm (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_affinity (zocket, 1);
    assert (zsocket_affinity (zocket) == 1);
    zsocket_affinity (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_subscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_unsubscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_set_identity (zocket, "test");
    char *identity = zsocket_identity (zocket);
    assert (identity);
    free (identity);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rate (zocket, 1);
    assert (zsocket_rate (zocket) == 1);
    zsocket_rate (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl (zocket, 1);
    assert (zsocket_recovery_ivl (zocket) == 1);
    zsocket_recovery_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_sndbuf (zocket, 1);
    assert (zsocket_sndbuf (zocket) == 1);
    zsocket_sndbuf (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvbuf (zocket, 1);
    assert (zsocket_rcvbuf (zocket) == 1);
    zsocket_rcvbuf (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_linger (zocket, 1);
    assert (zsocket_linger (zocket) == 1);
    zsocket_linger (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl (zocket, 1);
    assert (zsocket_reconnect_ivl (zocket) == 1);
    zsocket_reconnect_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl_max (zocket, 1);
    assert (zsocket_reconnect_ivl_max (zocket) == 1);
    zsocket_reconnect_ivl_max (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_backlog (zocket, 1);
    assert (zsocket_backlog (zocket) == 1);
    zsocket_backlog (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_maxmsgsize (zocket, 1);
    assert (zsocket_maxmsgsize (zocket) == 1);
    zsocket_maxmsgsize (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_multicast_hops (zocket, 1);
    assert (zsocket_multicast_hops (zocket) == 1);
    zsocket_multicast_hops (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvtimeo (zocket, 1);
    assert (zsocket_rcvtimeo (zocket) == 1);
    zsocket_rcvtimeo (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndtimeo (zocket, 1);
    assert (zsocket_sndtimeo (zocket) == 1);
    zsocket_sndtimeo (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_ipv4only (zocket, 1);
    assert (zsocket_ipv4only (zocket) == 1);
    zsocket_ipv4only (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_delay_attach_on_connect (zocket, 1);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_ROUTER);
    assert (zocket);
    zsocket_set_router_mandatory (zocket, 1);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_XPUB);
    assert (zocket);
    zsocket_set_xpub_verbose (zocket, 1);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_rcvmore (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_fd (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_events (zocket);
    zsocket_destroy (ctx, zocket);
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    char *last_endpoint = zsocket_last_endpoint (zocket);
    assert (last_endpoint);
    free (last_endpoint);
    zsocket_destroy (ctx, zocket);

    zocket = zsocket_new (ctx, ZMQ_SUB);
    zsocket_set_hwm (zocket, 1);
#endif

    zctx_destroy (&ctx);
    //  @end

    printf ("OK\n");
    return 0;
}
