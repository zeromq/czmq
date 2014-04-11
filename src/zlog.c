/*  =========================================================================
    zlog - logging class

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    Wraps the syslog API in a portable framework. On platforms without
    syslog, sends output to the console. 
@discuss
@end
*/

#include "../include/czmq.h"

//  Structure of our class

struct _zlog_t {
    char *sender;
    bool foreground;            //  Do we want logging to console?
};


//  --------------------------------------------------------------------------
//  Constructor; the sender name is prepended to every message, and may
//  not be null.

zlog_t *
zlog_new (const char *sender)
{
    assert (sender);
    zlog_t *self = (zlog_t *) zmalloc (sizeof (zlog_t));
    //  We take a copy of sender since syslog assumes it's always accessible
    self->sender = strdup (sender);
#if defined (__UNIX__)
    openlog (self->sender, 0, LOG_DAEMON);
#else
    self->foreground = true;
#endif
    return self;
}


//  -----------------------------------------------------------------
//  Destructor

void
zlog_destroy (zlog_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zlog_t *self = *self_p;
        free (self->sender);
        free (self);
        *self_p = NULL;
#if defined (__UNIX__)
        closelog ();            //  Just to be pedantic
#endif
    }
}


//  --------------------------------------------------------------------------
//  Set foreground logging on/off. By default, this is off on systems that
//  can do background logging using syslog, and on for systems without syslog
//  support.

void
zlog_set_foreground (zlog_t *self, bool foreground)
{
    assert (self);
    self->foreground = foreground;
}


//  --------------------------------------------------------------------------
//  Log error condition - highest priority

void
zlog_error (zlog_t *self, const char *format, ...)
{
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
#if defined (__UNIX__)
    syslog (LOG_ERR, "%s", string);
#endif
    if (self->foreground)
        zclock_log ("E: %s", string);
    free (string);
}


//  --------------------------------------------------------------------------
//  Log warning condition - high priority

void
zlog_warning (zlog_t *self, const char *format, ...)
{
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
#if defined (__UNIX__)
    syslog (LOG_WARNING, "%s", string);
#endif
    if (self->foreground)
        zclock_log ("W: %s", string);
    free (string);
}


//  --------------------------------------------------------------------------
//  Log normal, but significant, condition - normal priority

void
zlog_notice (zlog_t *self, const char *format, ...)
{
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
#if defined (__UNIX__)
    syslog (LOG_NOTICE, "%s", string);
#endif
    if (self->foreground)
        zclock_log ("N: %s", string);
    free (string);
}


//  --------------------------------------------------------------------------
//  Log informational message - low priority

void
zlog_info (zlog_t *self, const char *format, ...)
{
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
#if defined (__UNIX__)
    syslog (LOG_INFO, "%s", string);
#endif
    if (self->foreground)
        zclock_log ("I: %s", string);
    free (string);
}


//  --------------------------------------------------------------------------
//  Log debug-level message - lowest priority

void
zlog_debug (zlog_t *self, const char *format, ...)
{
    assert (self);
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
#if defined (__UNIX__)
    syslog (LOG_DEBUG, "%s", string);
#endif
    if (self->foreground)
        zclock_log ("D: %s", string);
    free (string);
}


//  --------------------------------------------------------------------------
//  Selftest

int
zlog_test (bool verbose)
{
    printf (" * zlog: ");

    //  @selftest
    zlog_t *log = zlog_new ("zlog_test");
    assert (log);
    zlog_error   (log, "%s", "My pizza was stolen!");
    zlog_warning (log, "%s", "My pizza is late :(");
    zlog_notice  (log, "%s", "My pizza arrived on time");
    zlog_info    (log, "%s", "My pizza smells great!");
    zlog_debug   (log, "%s", "My pizza is round and flat");
    if (verbose) {
        zlog_set_foreground (log, true);
        zlog_notice (log, "%s", "My pizza arrived on time");
    }
    zlog_destroy (&log);
    //  @end

    printf ("OK\n");
    return 0;
}
