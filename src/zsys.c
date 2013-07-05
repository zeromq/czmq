/*  =========================================================================
    zsys - system wrapper

    -------------------------------------------------------------------------
    Copyright (c) 1991-2013 iMatix Corporation <www.imatix.com>
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

/*
@header
    The zsys class provides a portable wrapper for miscellaneous functions
    that we want to wrap but which don't fit into any of the existing
    classes. Eventually all non-portable functionality might be moved here
    but for now it covers only file systems.
@discuss
@end
*/

#include "../include/czmq.h"

#if defined (__UNIX__)
static bool s_first_time = true;
static struct sigaction sigint_default;
static struct sigaction sigterm_default;
#endif

//  --------------------------------------------------------------------------
//  Set interrupt handler (NULL means external handler)
//  Idempotent; safe to call multiple times

void
zsys_handler_set (zsys_handler_fn *handler_fn)
{
#if defined (__UNIX__)
    //  Install signal handler for SIGINT and SIGTERM if not NULL
    //  and if this is the first time we've been called
    if (s_first_time) {
        s_first_time = false;
        if (handler_fn) {
            struct sigaction action;
            action.sa_handler = handler_fn;
            action.sa_flags = 0;
            sigemptyset (&action.sa_mask);
            sigaction (SIGINT, &action, &sigint_default);
            sigaction (SIGTERM, &action, &sigterm_default);
        }
        else {
            //  Save default handlers if not already done
            sigaction (SIGINT, NULL, &sigint_default);
            sigaction (SIGTERM, NULL, &sigterm_default);
        }
    }
#endif
}


//  --------------------------------------------------------------------------
//  Reset interrupt handler, call this at exit if needed
//  Idempotent; safe to call multiple times

void
zsys_handler_reset (void)
{
#if defined (__UNIX__)
    //  Restore default handlers if not already done
    if (sigint_default.sa_handler) {
        sigaction (SIGINT, &sigint_default, NULL);
        sigaction (SIGTERM, &sigterm_default, NULL);
        sigint_default.sa_handler = NULL;
        sigterm_default.sa_handler = NULL;
        s_first_time = true;
    }
#endif
}


//  --------------------------------------------------------------------------
//  Set network interface name to use for broadcasts
//  Use this to force the interface for beacons
//  This is experimental; may be merged into zbeacon class.

//  NOT thread safe, not a good design; this is to test the feasibility
//  of forcing a network interface name instead of writing code to find it.
static char *s_interface = NULL;

void
zsys_set_interface (char *interface_name)
{
    free (s_interface);
    s_interface = strdup (interface_name);
}


//  Return network interface name to use for broadcasts.
//  Returns "" if no interface was set.
//  This is experimental; may be merged into zbeacon class.

char *
zsys_interface (void)
{
    return (s_interface? s_interface: "");
}


//  --------------------------------------------------------------------------
//  Selftest

int
zsys_test (bool verbose)
{
    printf (" * zsys: ");
    zsys_handler_reset ();
    zsys_handler_set (NULL);
    zsys_handler_set (NULL);
    zsys_handler_reset ();
    zsys_handler_reset ();
    printf ("OK\n");
    return 0;
}
