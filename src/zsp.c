/*  =========================================================================
    zsp - description

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
    zsp - ZSubProc testing tool
@discuss
    Command line tool to support tests for zsubproc class
@end
*/

#include "czmq_classes.h"

int main (int argc, char *argv [])
{
    int argn;

    bool verbose = false;
    bool use_stdin  = false;
    bool use_stderr = false;
    bool use_stdout = false;

    char *message = NULL;

    for (argn = 1; argn < argc; argn++) {
        if (streq (argv [argn], "--help")
        ||  streq (argv [argn], "-h")) {
            puts ("zsp [options] ...");
            puts ("  --stdin / -i           read input from stdin");
            puts ("  --stderr / -e          output on stderr");
            puts ("  --stdout / -o          output on stdout");
            puts ("  --verbose / -v         verbose mode");
            puts ("  --help / -h            this information");
            return 0;
        }
        else
        if (streq (argv [argn], "--stderr")
        ||  streq (argv [argn], "-e"))
            use_stderr = true;
        else
        if (streq (argv [argn], "--stdout")
        ||  streq (argv [argn], "-o"))
            use_stdout = true;
        else
        if (streq (argv [argn], "--stdin")
        ||  streq (argv [argn], "-i"))
            use_stdin = true;
        else
        if (streq (argv [argn], "--verbose")
        ||  streq (argv [argn], "-v"))
            verbose = true;
        else
        if (argv [argn][0] == '-') {
            printf ("Unknown option: %s\n", argv [argn]);
            return 1;
        }
        else {
            message = argv[argn];
        }
    }

    zsys_init ();
    if (verbose)
        zsys_debug ("argn=%d, argc=%d", argn, argc);

    message = "Lorem ipsum\n";

    //  Insert main code here
    while (!zsys_interrupted) {
        if (use_stdin) {
            size_t size = 0;
            ssize_t r = getline (&message, &size, stdin);
            if (r == -1) {
                if (errno == 0)
                    break;
                else {
                    zsys_error ("Error reading stdin: %s", strerror (errno));
                    exit (EXIT_FAILURE);
                }
            }
        }
        if (use_stderr)
            fputs (message, stderr);
        if (use_stdout)
            fputs (message, stdout);

        if (use_stdin)
            zstr_free (&message);
        zclock_sleep (50);
    }

    return 0;
}
