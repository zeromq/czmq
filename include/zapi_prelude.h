/*  =========================================================================
    zapi_prelude.h - zapi environment

    -------------------------------------------------------------------------
    Copyright (c) 1991-2011 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of zapi, the C binding for 0MQ: http://zapi.zeromq.org.

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

#ifndef __ZAPI_PRELUDE_H_INCLUDED__
#define __ZAPI_PRELUDE_H_INCLUDED__

//- Standard ANSI include files ---------------------------------------------

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <signal.h>
#include <setjmp.h>
#include <assert.h>

//- System-specific include files -------------------------------------------

#if (defined (_WIN32))
#   include <direct.h>
#   include <windows.h>
#   include <process.h>
#   include <malloc.h>
#   include <dos.h>
#   include <io.h>
#   include <fcntl.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <sys/utime.h>
#   include <share.h>
#else
    //  UNIX systems
#   include <fcntl.h>
#   include <netdb.h>
#   include <unistd.h>
#   include <pthread.h>
#   include <dirent.h>
#   include <pwd.h>
#   include <grp.h>
#   include <utime.h>
#   include <syslog.h>
#   include <sys/types.h>
#   include <sys/param.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <sys/stat.h>
#   include <sys/ioctl.h>
#   include <sys/file.h>
#   include <sys/wait.h>
#endif

//- Data types --------------------------------------------------------------

typedef          int    Bool;           //  Boolean TRUE/FALSE value
typedef unsigned char   byte;           //  Single unsigned byte = 8 bits
typedef unsigned short  dbyte;          //  Double byte = 16 bits
typedef unsigned int    qbyte;          //  Quad byte = 32 bits

//- Inevitable macros -------------------------------------------------------

#define streq(s1,s2)    (!strcmp ((s1), (s2)))
#define strneq(s1,s2)   (strcmp ((s1), (s2)))
#define randof(num)     (int) (((float) num) * rand () / (RAND_MAX + 1.0))
#if (!defined (TRUE))
#    define TRUE        1               //  ANSI standard
#    define FALSE       0
#endif

//- A number of POSIX and C99 keywords and data types -----------------------

#if (defined (_WIN32))
#   define inline __inline
#   define strtoull _strtoui64
#   define srandom      srand
#   define TIMEZONE     _timezone
#   define snprintf     _snprintf
#   define vsnprintf    _vsnprintf
    typedef unsigned long ulong;
    typedef unsigned int  uint;
    typedef __int64 int64_t;
    typedef unsigned __int64 uint64_t;
#endif

//- DLL exports -------------------------------------------------------------

#if (defined (_WIN32))
#   if defined DLL_EXPORT
#       define ZAPI_EXPORT __declspec(dllexport)
#   else
#       define ZAPI_EXPORT __declspec(dllimport)
#   endif
#else
#   define ZAPI_EXPORT
#endif

#endif
