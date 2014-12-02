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
/*
@header
The zclock class provides essential sleep and system time functions, used
to slow down threads for testing, and calculate timers for polling. Wraps
the non-portable system calls in a simple portable API.
@discuss
The Win32 Sleep() call defaults to 16ms resolution unless the system timer
resolution is increased with a call to timeBeginPeriod() permitting 1ms
granularity.
@end
*/

#include "../include/czmq.h"

#if defined (__WINDOWS__)
//  --------------------------------------------------------------------------
//  Convert FILETIME "Contains a 64-bit value representing the number of
//  100-nanosecond intervals since January 1, 1601 (UTC)."

static int64_t
s_filetime_to_msec (const FILETIME *ft)
{
    //  As per Windows documentation for FILETIME, copy the resulting FILETIME
    //  structure to a ULARGE_INTEGER structure using memcpy (using memcpy
    //  instead of direct assignment can prevent alignment faults on 64-bit
    //  Windows).
    ULARGE_INTEGER dateTime;
    memcpy (&dateTime, ft, sizeof (dateTime));

    return (int64_t) (dateTime.QuadPart / 10000);
}

#endif


//  --------------------------------------------------------------------------
//  Sleep for a number of milliseconds

void
zclock_sleep (int msecs)
{
#if defined (__UNIX__)
    struct timespec t;
    t.tv_sec = msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep (&t, NULL);
#elif (defined (__WINDOWS__))
    //  Windows XP/2000:  A value of zero causes the thread to relinquish the
    //  remainder of its time slice to any other thread of equal priority that
    //  is ready to run. If there are no other threads of equal priority ready
    //  to run, the function returns immediately, and the thread continues
    //  execution. This behavior changed starting with Windows Server 2003.

#   if defined (NTDDI_VERSION) && defined (NTDDI_WS03) && (NTDDI_VERSION >= NTDDI_WS03)
    Sleep (msecs);
#   else
    if (msecs > 0)
        Sleep (msecs);
#   endif
#endif
}


//  --------------------------------------------------------------------------
//  Return current system clock as milliseconds. Note that this clock can
//  jump backwards (if the system clock is changed) so is unsafe to use for
//  timers and time offsets. Use zclock_mono for that instead.

int64_t
zclock_time (void)
{
#if defined (__UNIX__)
    struct timeval tv;
    gettimeofday (&tv, NULL);

    return (int64_t) ((int64_t) tv.tv_sec * 1000 + (int64_t) tv.tv_usec / 1000);
#elif (defined (__WINDOWS__))
    FILETIME ft;
    GetSystemTimeAsFileTime (&ft);
    return s_filetime_to_msec (&ft);
#endif
}


//  --------------------------------------------------------------------------
//  Return current monotonic clock in milliseconds. Use this when you compute
//  time offsets. The monotonic clock is not affected by system changes and
//  so will never be reset backwards, unlike a system clock.

int64_t
zclock_mono (void)
{
#if defined (__UTYPE_OSX)
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service (mach_host_self (), SYSTEM_CLOCK, &cclock);
    clock_get_time (cclock, &mts);
    mach_port_deallocate (mach_task_self (), cclock);
    return (int64_t) ((int64_t) mts.tv_sec * 1000 + (int64_t) mts.tv_nsec / 1000000);
    
#elif defined (__UNIX__)
    struct timespec ts;
    clock_gettime (CLOCK_MONOTONIC, &ts);
    return (int64_t) ((int64_t) ts.tv_sec * 1000 + (int64_t) ts.tv_nsec / 1000000);
    
#elif (defined (__WINDOWS__))
    //  System frequency does not change at run-time, cache it
    static int64_t frequency = 0;
    if (frequency == 0) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency (&freq);
        // Windows documentation says that XP and later will always return non-zero
        assert (freq.QuadPart != 0);
        frequency = freq.QuadPart;
    }
    LARGE_INTEGER count;
    QueryPerformanceCounter (&count);
    return (int64_t) (count.QuadPart * 1000) / frequency;
#endif
}


//  --------------------------------------------------------------------------
//  Return current monotonic clock in microseconds. Use this when you compute
//  time offsets. The monotonic clock is not affected by system changes and
//  so will never be reset backwards, unlike a system clock.

int64_t
zclock_usecs (void)
{
#if defined (__UTYPE_OSX)
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service (mach_host_self (), SYSTEM_CLOCK, &cclock);
    clock_get_time (cclock, &mts);
    mach_port_deallocate (mach_task_self (), cclock);
    return (int64_t) ((int64_t) mts.tv_sec * 1000000 + (int64_t) mts.tv_nsec / 1000);

#elif defined (__UNIX__)
    struct timespec ts;
    clock_gettime (CLOCK_MONOTONIC, &ts);
    return (int64_t) ((int64_t) ts.tv_sec * 1000000 + (int64_t) ts.tv_nsec / 1000);

#elif (defined (__WINDOWS__))
    //  System frequency does not change at run-time, cache it
    static int64_t frequency = 0;
    if (frequency == 0) {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency (&freq);
        // Windows documentation says that XP and later will always return non-zero
        assert (freq.QuadPart != 0);
        frequency = freq.QuadPart;
    }
    LARGE_INTEGER count;
    QueryPerformanceCounter (&count);
    return (int64_t) (count.QuadPart * 1000000) / frequency;
#endif
}


//  --------------------------------------------------------------------------
//  Return formatted date/time as fresh string. Free using zstr_free().

char *
zclock_timestr (void)
{
    time_t curtime = time (NULL);
    struct tm *loctime = localtime (&curtime);
    char formatted [20];
    strftime (formatted, 20, "%Y-%m-%d %H:%M:%S", loctime);
    return strdup (formatted);
}


//  --------------------------------------------------------------------------
//  DEPRECATED in favor of zsys logging, see issue #519
//  Print formatted string to stdout, prefixed by date/time and
//  terminated with a newline.

void
zclock_log (const char *format, ...)
{
    time_t curtime = time (NULL);
    struct tm *loctime = localtime (&curtime);
    char formatted [20];
    strftime (formatted, 20, "%y-%m-%d %H:%M:%S ", loctime);
    printf ("%s", formatted);

    va_list argptr;
    va_start (argptr, format);
    vprintf (format, argptr);
    va_end (argptr);
    printf ("\n");
    fflush (stdout);
}


//  --------------------------------------------------------------------------
//  Self test of this class
void
zclock_test (bool verbose)
{
    printf (" * zclock: ");

    //  @selftest
    int64_t start = zclock_time ();
    zclock_sleep (10);
    assert ((zclock_time () - start) >= 10);
    start = zclock_mono ();
    int64_t usecs = zclock_usecs ();
    zclock_sleep (10);
    assert ((zclock_mono () - start) >= 10);
    assert ((zclock_usecs () - usecs) >= 10000);
    char *timestr = zclock_timestr ();
    if (verbose)
        puts (timestr);
    free (timestr);
    //  @end

    printf ("OK\n");
}
