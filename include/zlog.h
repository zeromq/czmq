/*  =========================================================================
    zlog - Syslog support class

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
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

#ifndef __ZLOG_H_INCLUDED__
#define __ZLOG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zlog_t zlog_t;

//  Constructor; the sender name is prepended to every message, and may
//  not be null.
CZMQ_EXPORT zlog_t *
    zlog_new (const char *sender);

//  Destructor
CZMQ_EXPORT void
    zlog_destroy (zlog_t **self_p);

//  Print informational message
CZMQ_EXPORT void
    zlog_info (zlog_t *self, const char *format, ...);

//  Self test of this class
CZMQ_EXPORT int
    zlog_test (bool verbose);

#ifdef __cplusplus
}
#endif

#endif
