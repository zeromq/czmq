/*  =========================================================================
    zclock - millisecond clocks and delays

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the C binding for 0MQ: http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under the
    terms of the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your option)
    any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
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
    t.tv_sec  =  msecs / 1000;
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
//  Return current system clock as milliseconds

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
//  Return formatted date/time as fresh string.

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
//  Self test of this class

int
zclock_test (bool verbose)
{
    printf (" * zclock: ");

    //  @selftest
    int64_t start = zclock_time ();
    zclock_sleep (10);
    assert ((zclock_time () - start) >= 10);
    char *timestr = zclock_timestr ();
    if (verbose)
        puts (timestr);
    free (timestr);
    //  @end

    printf ("OK\n");
    return 0;
}
