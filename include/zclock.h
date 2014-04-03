/*  =========================================================================
    zclock - millisecond clocks and delays

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZCLOCK_H_INCLUDED__
#define __ZCLOCK_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Sleep for a number of milliseconds
CZMQ_EXPORT void
    zclock_sleep (int msecs);

//  Return current system clock as milliseconds
CZMQ_EXPORT int64_t
    zclock_time (void);

//  Print formatted string to stdout, prefixed by date/time and
//  terminated with a newline.
CZMQ_EXPORT void
    zclock_log (const char *format, ...);

//  Return formatted date/time as fresh string. Free using zstr_free().
CZMQ_EXPORT char *
    zclock_timestr (void);

//  Self test of this class
CZMQ_EXPORT int
    zclock_test (bool verbose);
//  @end

//  Compiler hints
CZMQ_EXPORT void zclock_log (const char *format, ...) CHECK_PRINTF (1);

#ifdef __cplusplus
}
#endif

#endif
