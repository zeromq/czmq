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

#define CZMQ_VERSION_MAJOR 2
#define CZMQ_VERSION_MINOR 2
#define CZMQ_VERSION_PATCH 0

#define CZMQ_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define CZMQ_VERSION \
    CZMQ_MAKE_VERSION(CZMQ_VERSION_MAJOR, CZMQ_VERSION_MINOR, CZMQ_VERSION_PATCH)

//  Public API classes

#include "zchunk.h"
#include "zclock.h"
#include "zframe.h"
#include "zlist.h"
#include "zhash.h"
#include "zconfig.h"
#include "zctx.h"
#include "zfile.h"
#include "zdir.h"
#include "zdir_patch.h"
#include "zdigest.h"
#include "zlog.h"
#include "zloop.h"
#include "zmsg.h"
#include "zmonitor.h"
#include "zmutex.h"
#include "zpoller.h"
#include "zsocket.h"
#include "zsockopt.h"
#include "zstr.h"
#include "zsys.h"
#include "zthread.h"
#include "zrex.h"
#include "zbeacon.h"
#include "zauth.h"
#include "zcert.h"
#include "zcertstore.h"
#include "zproxy.h"
#include "zuuid.h"

#endif
