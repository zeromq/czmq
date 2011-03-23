/*  =========================================================================
    zapi_tests.c - run selftests

    Runs all selftests.

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

#include "../include/zapi_prelude.h"
#include "../include/zctx.h"
#include "../include/zframe.h"
#include "../include/zhash.h"
#include "../include/zlist.h"
#include "../include/zloop.h"
#include "../include/zmsg.h"
#include "../include/zstr.h"

int main (int argc, char *argv [])
{
    Bool verbose;
    if (argc == 2 && streq (argv [1], "-v"))
        verbose = TRUE;
    else
        verbose = FALSE;

    printf ("Running zapi self tests...\n");

    zctx_test (verbose);
//     zframe_test (verbose);
//     zhash_test (verbose);
//     zlist_test (verbose);
//     zloop_test (verbose);
//     zmsg_test (verbose);
//     zstr_test (verbose);

    printf ("Tests passed OK\n");
    return 0;
}
