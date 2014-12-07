/*  =========================================================================
    czmq_tests.c - run selftests

    Runs all selftests.

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of czmq, the high-level C binding for 0MQ:
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

#include "../include/czmq.h"

int
main (int argc, char *argv [])
{
    bool verbose;
    if (argc == 2 && streq (argv [1], "-v"))
        verbose = true;
    else
        verbose = false;

    printf ("Running CZMQ selftests...\n");

    //  These are ordered from lowest level to highest level
    zarmour_test (verbose);
    zrex_test (verbose);
    zsys_test (verbose);
    zchunk_test (verbose);
    zconfig_test (verbose);
    zclock_test (verbose);
    zdir_patch_test (verbose);
    zdir_test (verbose);
    zdigest_test (verbose);
    zframe_test (verbose);
    zstr_test (verbose);
    zlist_test (verbose);
    zlistx_test (verbose);
    zhash_test (verbose);
    zhashx_test (verbose);
    zmsg_test (verbose);
    zfile_test (verbose);
    ziflist_test (verbose);
    zuuid_test (verbose);
    zsock_test (verbose);
    zsock_option_test (verbose);
    zactor_test (verbose);
    zpoller_test (verbose);
    zloop_test (verbose);
    zproxy_test (verbose);
    zmonitor_test (verbose);
    zbeacon_test (verbose);
    zgossip_test (verbose);
    zcert_test (verbose);
    zcertstore_test (verbose);
    zauth_test (verbose);

    //  Deprecated V2 classes*/
    zauth_v2_test (verbose);
    zbeacon_v2_test (verbose);
    zctx_test (verbose);
    zmonitor_v2_test (verbose);
    zmutex_test (verbose);
    zproxy_v2_test (verbose);
    zsocket_test (verbose);
    zsockopt_test (verbose);
    zthread_test (verbose);

    zsys_shutdown ();

    printf ("Number of memory allocations=%" PRId64 "\n", zsys_allocs);
    printf ("Tests passed OK\n");
    return 0;
}
