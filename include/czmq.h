/*  =========================================================================
    czmq.h - CZMQ wrapper

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
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

#ifndef __CZMQ_H_INCLUDED__
#define __CZMQ_H_INCLUDED__

//  Set up environment for the application
//
#include <czmq_prelude.h>

//  CZMQ version macros for compile-time API detection

#define CZMQ_VERSION_MAJOR 1
#define CZMQ_VERSION_MINOR 3
#define CZMQ_VERSION_PATCH 0

#define CZMQ_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define CZMQ_VERSION \
    CZMQ_MAKE_VERSION(CZMQ_VERSION_MAJOR, CZMQ_VERSION_MINOR, CZMQ_VERSION_PATCH)

//  Classes listed in alphabetical order

#include <zclock.h>
#include <zctx.h>
#include <zfile.h>
#include <zframe.h>
#include <zhash.h>
#include <zlist.h>
#include <zloop.h>
#include <zmsg.h>
#include <zsocket.h>
#include <zsockopt.h>
#include <zstr.h>
#include <zthread.h>

#endif
