/*  =========================================================================
    zpubsub_option - options for the zpubsub class

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
    The zpubsub_option class provides global options for the zpubsub class.
    Active options at the time of creation of a zpubsub instance, will
    be used for that instance.
@discuss
@end
*/

#include "../include/czmq.h"


//  --------------------------------------------------------------------------
//  Globals to use for a new pubsub instance.

static zpubsub_port_fn *s_port_function = NULL;        //  Function pointer for calculating port from domain id.
static dbyte s_beacon_port = 55666;                    //  Port for beacon.
static int s_beacon_interval = 5000;                   //  Beacon interval in milliseconds.


//  --------------------------------------------------------------------------
//  Get the currently installed function for calculating port number from
//  domain id. If NULL, the default implementation in the zpubsub class
//  will be used.

zpubsub_port_fn *
zpubsub_port_function (void)
{
	return s_port_function;
}


//  --------------------------------------------------------------------------
//  Set the function for calculating port number from domain id. If NULL,
//  the default implementation in the zpubsub class will be used.

void
zpubsub_set_port_function (zpubsub_port_fn *port_fn)
{
	s_port_function = port_fn;
}


//  --------------------------------------------------------------------------
//  Get the beacon port number.

dbyte
zpubsub_beacon_port (void)
{
	return s_beacon_port;
}


//  --------------------------------------------------------------------------
//  Set the beacon port number (1025-65535).

void
zpubsub_set_beacon_port (dbyte port)
{
	assert (port >= 1025 && port <= 65535);
	
	s_beacon_port = port;
}


//  --------------------------------------------------------------------------
//  Get the beacon interval.

int
zpubsub_beacon_interval (void)
{
	return s_beacon_interval;
}


//  --------------------------------------------------------------------------
//  Set the beacon interval (1000-60000).

void
zpubsub_set_beacon_interval (int interval)
{
	assert (interval >= 1000 && interval <= 60000);
	
	s_beacon_interval = interval;
}


//  --------------------------------------------------------------------------
//  Test port function

static dbyte
s_test_port_fn (int domain)
{
	return (dbyte) ((domain % 64536) + 1025);
}


//  --------------------------------------------------------------------------
//  Selftest

void
zpubsub_option_test (bool verbose)
{
    printf (" * zpubsub_option: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    zpubsub_port_fn *port_fn = zpubsub_port_function ();
    assert (port_fn == NULL);
    zpubsub_set_port_function (s_test_port_fn);
    port_fn = zpubsub_port_function ();
    assert (port_fn != NULL);
    dbyte port = (port_fn) (0);
    assert (port == 1025);
    port = (port_fn) (99);
    assert (port == 1124);
    zpubsub_set_port_function (NULL);
    port_fn = zpubsub_port_function ();
    assert (port_fn == NULL);
    
    dbyte beacon_port = zpubsub_beacon_port ();
    assert (beacon_port == 55666);
    zpubsub_set_beacon_port (46798);
    beacon_port = zpubsub_beacon_port ();
    assert (beacon_port == 46798);
    zpubsub_set_beacon_port (55666);
    beacon_port = zpubsub_beacon_port ();
    assert (beacon_port == 55666);
    
    int beacon_interval = zpubsub_beacon_interval ();
    assert (beacon_interval == 5000);
    zpubsub_set_beacon_interval (30000);
    beacon_interval = zpubsub_beacon_interval ();
    assert (beacon_interval == 30000);
    zpubsub_set_beacon_interval (5000);
    beacon_interval = zpubsub_beacon_interval ();
    assert (beacon_interval == 5000);
    //  @end

    printf ("OK\n");
}
