/*  =========================================================================
    czmq.h - CZMQ wrapper

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __CZMQ_H_INCLUDED__
#define __CZMQ_H_INCLUDED__

//  Set up environment for the application

#include "czmq_prelude.h"

//  CZMQ version macros for compile-time API detection

#define CZMQ_VERSION_MAJOR 3
#define CZMQ_VERSION_MINOR 0
#define CZMQ_VERSION_PATCH 0

#define CZMQ_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define CZMQ_VERSION \
    CZMQ_MAKE_VERSION(CZMQ_VERSION_MAJOR, CZMQ_VERSION_MINOR, CZMQ_VERSION_PATCH)

//  Opaque class structures to allow forward references
typedef struct _zactor_t zactor_t;
typedef struct _zcert_t zcert_t;
typedef struct _zcertstore_t zcertstore_t;
typedef struct _zchunk_t zchunk_t;
typedef struct _zconfig_t zconfig_t;
typedef struct _zdigest_t zdigest_t;
typedef struct _zdir_t zdir_t;
typedef struct _zdir_patch_t zdir_patch_t;
typedef struct _zfile_t zfile_t;
typedef struct _zframe_t zframe_t;
typedef struct _zhash_t zhash_t;
typedef struct _zlist_t zlist_t;
typedef zlist_t ziflist_t;
typedef struct _zloop_t zloop_t;
typedef struct _zmsg_t zmsg_t;
typedef struct _zpoller_t zpoller_t;
typedef struct _zpubsub_t zpubsub_t;
typedef struct _zrex_t zrex_t;
typedef struct _zring_t zring_t;
typedef struct _zring_node_t zring_node_t;
typedef struct _zsock_t zsock_t;
typedef struct _ztimeout_t ztimeout_t;
typedef struct _ztimeout_node_t ztimeout_node_t;
typedef struct _zuuid_t zuuid_t;
//  Deprecated V2 classes, remove some time after 3.0 stability
typedef struct _zauth_t zauth_t;
typedef struct _zbeacon_t zbeacon_t;
typedef struct _zctx_t zctx_t;
typedef struct _zmonitor_t zmonitor_t;
typedef struct _zmutex_t zmutex_t;
typedef struct _zproxy_t zproxy_t;

//  These are signatures for handler functions that customize the
//  behavior of CZMQ containers
//  -- destroy an item
typedef void (czmq_destructor) (void **item);
//  -- duplicate an item
typedef void * (czmq_duplicator) (const void *item);
//  - compare two items, for sorting
typedef int (czmq_comparator) (const void *item1, const void *item2);


//  Public API classes
#include "zactor.h"
#include "zauth.h"
#include "zbeacon.h"
#include "zcert.h"
#include "zcertstore.h"
#include "zchunk.h"
#include "zclock.h"
#include "zconfig.h"
#include "zdigest.h"
#include "zdir.h"
#include "zdir_patch.h"
#include "zfile.h"
#include "zframe.h"
#include "zgossip.h"
#include "zhash.h"
#include "ziflist.h"
#include "zlist.h"
#include "zloop.h"
#include "zmonitor.h"
#include "zmsg.h"
#include "zpoller.h"
#include "zproxy.h"
#include "zpubsub.h"
#include "zpubsub_option.h"
#include "zrex.h"
#include "zring.h"
#include "zsock.h"
#include "zsock_option.h"
#include "zstr.h"
#include "zsys.h"
#include "ztimeout.h"
#include "zuuid.h"

//  Deprecated V2 classes, remove some time after 3.0 stability
#include "zauth_v2.h"
#include "zbeacon_v2.h"
#include "zctx.h"
#include "zmonitor_v2.h"
#include "zmutex.h"
#include "zproxy_v2.h"
#include "zsocket.h"
#include "zsockopt.h"
#include "zthread.h"

#endif
