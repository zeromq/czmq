/*  =========================================================================
    test_randof - development helper program to validate the range and
    dispersion of randof() implementation in current CZMQ codebase

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
    test_randof - development helper program to validate the range and
    dispersion of randof() implementation in current CZMQ codebase
@discuss
@end
*/

// Custom testing, needs recompiling
//#define ZSYS_RANDOF_MAX (INT32_MAX>>8)
//#define ZSYS_RANDOF_FLT double
//#define ZSYS_RANDOF_FUNC rand

#include "czmq_classes.h"

// Compiler can also tweak ZSYS_RANDOF_MAX to numeric or macroed value

#ifndef TESTMAX
//# define TESTMAX 999
# define TESTMAX (intmax_t)10000000
#endif

#ifndef ITERMAX
//# define ITERMAX 1000000000
# define ITERMAX (TESTMAX * 30)
#endif

#ifdef STRINGIZE_
#undef STRINGIZE_
#endif

#ifdef STRINGIZE
#undef STRINGIZE
#endif

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

int main (int argc, char *argv [])
{
    bool verbose = false;
    int argn;

    intmax_t testmax = TESTMAX,
        itermax = ITERMAX,
        iteration = 0,
        testnbr = 0,
        rndmisses = 0;

    for (argn = 1; argn < argc; argn++) {
        if (streq (argv [argn], "--help")
        ||  streq (argv [argn], "-h")) {
            printf ("test_randof [options]\n");
            printf ("  --verbose / -v         verbose test output\n");
            printf ("  --help / -h            this information\n");
            printf ("  --test-range / -r NUM  generate numbers from 0 to NUM-1 (default %jd)\n",
                (intmax_t)TESTMAX);
            printf ("  --iterations / -i ITR  run ITR takes at generating random values (default %jd); recommended at least 30*NUM\n",
                (intmax_t)ITERMAX);
            printf ("  ZSYS_RANDOF_MAX is defined during compilation, this binary was built with \"%s\" (%jd)\n",
                STRINGIZE(ZSYS_RANDOF_MAX), (intmax_t)ZSYS_RANDOF_MAX);
            printf ("  ZSYS_RANDOF_FLT is defined during compilation, this binary was built with \"%s\"\n",
                STRINGIZE(ZSYS_RANDOF_FLT));
            printf ("  ZSYS_RANDOF_FUNC is defined during compilation, this binary was built with \"%s\"\n",
                STRINGIZE(ZSYS_RANDOF_FUNC));
            // TODO: CSV output mode to a file for pretty graphing of the spectrum?
            return 0;
        }
        else
        if (streq (argv [argn], "--verbose")
        ||  streq (argv [argn], "-v"))
            verbose = true;
        else
        if (streq (argv [argn], "--test-range")
        ||  streq (argv [argn], "-r")) {
            if (argn + 1 >= argc) {
                printf ("Option %s requires a value\n", argv [argn]);
                return 1;
            }
            if (1 != sscanf (argv[argn+1], "%jd", &testmax) ) {
                printf ("Failed to parse option %s from value %s\n", argv [argn], argv [argn+1]);
                return 1;
            }
            argn++;
        }
        else
        if (streq (argv [argn], "--iterations")
        ||  streq (argv [argn], "-i")) {
            if (argn + 1 >= argc) {
                printf ("Option %s requires a value\n", argv [argn]);
                return 1;
            }
            if (1 != sscanf (argv[argn+1], "%jd", &itermax) ) {
                printf ("Failed to parse option %s from value %s\n", argv [argn], argv [argn+1]);
                return 1;
            }
            argn++;
        }
        else {
            printf ("Unknown option: %s\n", argv [argn]);
            return 1;
        }
    }

    if (testmax <= 0) {
        printf ("Variable testmax is too small: %jd", testmax);
        return 1;
    }

    if (itermax <= 0) {
        printf ("Variable itermax is too small: %jd", itermax);
        return 1;
    }

    // WARNING: Code below implies that "size_t" used by zmalloc() and memset()
    // would suffice to count the testmax-based byte size on your platform
#ifdef SIZE_MAX
    assert ( testmax < (SIZE_MAX / sizeof (size_t)) );
#endif

    //  Insert main code here
    zsys_init ();
    if (verbose)
        zsys_info ("test_randof - development helper for randof() testing");

    size_t *rndcnt = (size_t*)zmalloc (sizeof (size_t) * testmax);
    assert (rndcnt);
    memset (rndcnt, 0, sizeof (size_t) * testmax);

    unsigned int random_seed = (unsigned int) zclock_time ();
    srandom (random_seed);

    zsys_info ("test_randof : Running random loop for %jd iterations and %jd values (factor of %.6Lg) and random seed %u",
        itermax, testmax, (long double)itermax/(long double)testmax, random_seed);
    for (iteration = 0; iteration < itermax && !zsys_interrupted ; iteration++) {
        testnbr = randof (testmax);
        assert (testnbr != testmax);
        assert (testnbr < testmax);
        assert (testnbr >= 0);
        rndcnt[testnbr]++;
    }

    zsys_info ("test_randof : Testing hit-counts of %jd entries", testmax);
    for (iteration = 0; iteration < testmax && !zsys_interrupted; iteration++) {
        if (rndcnt[iteration] == 0) {
            if (verbose)
                zsys_warning("test_randof : random distribution fault : got 0 hits for %jd/%jd",
                    iteration, testmax);
            rndmisses++;
        }
    }

    //  Too many misses are suspicious... we can lose half the entries
    //  for each bit not used in the assumed ZSYS_RANDOF_MAX...
    //  We can also have a result-spectrum too discrete with smallish
    //  ZSYS_RANDOF_MAX and largish testmax...
    zsys_info ("test_randof : Got %jd zero-entries (misses) from %jd range (%.4Lg %%) with ZSYS_RANDOF_MAX=%jd and %s precision and %s()",
        rndmisses, testmax,
        rndmisses ? ((long double)(rndmisses)/(long double)testmax)*100.0 : (long double)0.0,
        (intmax_t)ZSYS_RANDOF_MAX,
        STRINGIZE(ZSYS_RANDOF_FLT),
        STRINGIZE(ZSYS_RANDOF_FUNC)
        );
    assert ( (rndmisses < (testmax / 3 )) );

    free(rndcnt);
    return 0;
}
