/*  =========================================================================
    zlog - Syslog support class

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZLOG_H_INCLUDED__
#define __ZLOG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _zlog_t zlog_t;

//  @interface
//  Constructor; the sender name is prepended to every message, and may
//  not be null.
CZMQ_EXPORT zlog_t *
    zlog_new (const char *sender);

//  Destructor
CZMQ_EXPORT void
    zlog_destroy (zlog_t **self_p);

//  Set foreground logging on/off. By default, this is off on systems that
//  can do background logging using syslog, and on for systems without syslog
//  support.
CZMQ_EXPORT void
    zlog_set_foreground (zlog_t *self, bool foreground);
    
//  Log error condition - highest priority
CZMQ_EXPORT void
    zlog_error (zlog_t *self, const char *format, ...);

//  Log warning condition - high priority
CZMQ_EXPORT void
    zlog_warning (zlog_t *self, const char *format, ...);

//  Log normal, but significant, condition - normal priority
CZMQ_EXPORT void
    zlog_notice (zlog_t *self, const char *format, ...);

//  Log informational message - low priority
CZMQ_EXPORT void
    zlog_info (zlog_t *self, const char *format, ...);

//  Log debug-level message - lowest priority
CZMQ_EXPORT void
    zlog_debug (zlog_t *self, const char *format, ...);

//  Self test of this class
CZMQ_EXPORT int
    zlog_test (bool verbose);
//  @end

//  Compiler hints
CZMQ_EXPORT void zlog_error (zlog_t *self, const char *format, ...) CHECK_PRINTF (2);
CZMQ_EXPORT void zlog_warning (zlog_t *self, const char *format, ...) CHECK_PRINTF (2);
CZMQ_EXPORT void zlog_notice (zlog_t *self, const char *format, ...) CHECK_PRINTF (2);
CZMQ_EXPORT void zlog_info (zlog_t *self, const char *format, ...) CHECK_PRINTF (2);
CZMQ_EXPORT void zlog_debug (zlog_t *self, const char *format, ...) CHECK_PRINTF (2);

#ifdef __cplusplus
}
#endif

#endif
