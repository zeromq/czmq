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
    bool abrt = false;
    int quit = 0;

    char *message = NULL;

    for (argn = 1; argn < argc; argn++) {
        if (streq (argv [argn], "--help")
        ||  streq (argv [argn], "-h")) {
            puts ("zsp [options] ...");
#if ! defined (__WINDOWS__)
            puts ("  --stdin / -i           read input from stdin");
#endif
            puts ("  --stderr / -e          output on stderr");
            puts ("  --stdout / -o          output on stdout");
            puts ("  --abrt / -a            crash with SIGABRT on start");
            puts ("  --verbose / -v         verbose mode");
            puts ("  --quit / -q X          quit after X seconds");
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
        if (streq (argv [argn], "--abrt")
        ||  streq (argv [argn], "-a"))
            abrt = true;
        else
        if (streq (argv [argn], "--quit")
        ||  streq (argv [argn], "-q")) {
            quit = atoi (argv [argn + 1]) * 1000;
            ++argn;
        }
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

    message = "Lorem ipsum";
    if (getenv ("ZSP_MESSAGE"))
        message = getenv ("ZSP_MESSAGE");

    zfile_t *stdinf = NULL;

    if (use_stdin) {
        stdinf = zfile_new ("/dev", "stdin");
        int r = zfile_input (stdinf);
        assert (r == 0);
    }

    if (abrt) {
        if (verbose)
            zsys_info ("Going to abort myself");
#if defined (__WINDOWS__)
        assert (false); // TODO: how to do kill myelf on Windows?
#else
        kill (getpid (), SIGABRT);
#endif
    }

    //  Insert main code here
    int64_t start = zclock_mono ();
    while (!zsys_interrupted) {
#if ! defined (__WINDOWS__)
        if (use_stdin) {
            const char *line = zfile_readln (stdinf);

            if (!line && errno != 0) {
                    zsys_error ("Error reading stdin: %s", strerror (errno));
                    exit (EXIT_FAILURE);
            }

            if (zfile_eof (stdinf) || !line)
                break;

            message = (char*) line;
        }
#endif
        if (use_stderr)
            fprintf (stderr, "%s\n", message);
        if (use_stdout)
            fprintf (stdout, "%s\n", message);

        zclock_sleep (50);
        if (quit && zclock_mono () - start > quit) break;
    }

    zfile_destroy (&stdinf);

    return 0;
}
