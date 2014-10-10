.set GIT=https://github.com/zeromq/czmq
.sub 0MQ=ØMQ

# CZMQ - High-level C binding for 0MQ

[![Build Status](https://travis-ci.org/zeromq/czmq.png?branch=master)](https://travis-ci.org/zeromq/czmq)

## Contents

.toc 3

## Overview

### Scope and Goals

CZMQ has these goals:

* To wrap the 0MQ core API in semantics that lead to shorter, more readable applications.
* To hide as far as possible the differences between different versions of 0MQ (2.x, 3.x, 4.x).
* To provide a space for development of more sophisticated API semantics.
* To wrap the 0MQ security features with high-level tools and APIs.
* To become the basis for other language bindings built on top of CZMQ.

CZMQ grew out of concepts developed in [ØMQ - The Guide](http://zguide.zeromq.org).

[diagram]
                              +---------------+
                              |               |
                              | C application |
                              |               |
                              +-----+---+-----+
                                    |   |
                                    |   |
                       +------------+   |
                       |                |
                       v                |
  Open context    +---------+           |
  Create sockets  |         |           |    Connect, bind sockets
  Close sockets   |  CZMQ   |           |    get/set socket options
  Send/receive    |    cYEL |           |
  Multithreading  +----+----+           |
  Reactor pattern      |                |
  Hash container       +------------+   |
  List container                    |   |
  System clock                      v   v
  Close context                  +---------+
                                 |         |
                                 | libzmq  |
                                 |         |
                                 +---------+

[/diagram]

### Ownership and License

The contributors are listed in AUTHORS. This project uses the MPL v2 license, see LICENSE.

CZMQ uses the [C4.1 (Collective Code Construction Contract)](http://rfc.zeromq.org/spec:22) process for contributions.

CZMQ uses the [CLASS (C Language Style for Scalabilty)](http://rfc.zeromq.org/spec:21) guide for code style.

To report an issue, use the [CZMQ issue tracker](https://github.com/zeromq/czmq/issues) at github.com.

## Using CZMQ

### Building and Installing

Here's how to build CZMQ from GitHub (building from packages is very similar, you don't clone a repo but unpack a tarball), including the libsodium (for security) and libzmq (ZeroMQ core) libraries:

    git clone git://github.com/jedisct1/libsodium.git
    cd libsodium
    ./autogen.sh
    ./configure && make check
    sudo make install
    sudo ldconfig
    cd ..

    git clone git://github.com/zeromq/libzmq.git
    cd libzmq
    ./autogen.sh
    ./configure && make check
    sudo make install
    sudo ldconfig
    cd ..

    git clone git://github.com/zeromq/czmq.git
    cd czmq
    ./autogen.sh
    ./configure && make check
    sudo make install
    sudo ldconfig
    cd ..

In general CZMQ works best with the latest libzmq master. If you already have an older version of libzmq installed on your system, e.g. in /usr/, then you can install libzmq master to your home directory ($HOME/local):

    #   Building libzmq in our home directory
    ./configure --prefix=$HOME/local

And then to build CZMQ against this installation of libzmq:

    export CFLAGS=-I$HOME/local/include
    export LDFLAGS=-L$HOME/local/lib64
    ./configure

You will need the pkg-config, libtool, and autoreconf packages. After building, run the CZMQ selftests:

    make check

### Linking with an Application

Include `czmq.h` in your application and link with libczmq. Here is a typical gcc link command:

    gcc -lczmq -lzmq myapp.c -o myapp

### Use from Other Languages

This is a list of known higher-level wrappers around CZMQ:

* https://github.com/1100110/CZMQ - D bindings
* https://github.com/methodmissing/rbczmq - Ruby
* https://github.com/zeromq/pyczmq - Python
* https://github.com/lhope/cl-czmq - Common Lisp
* https://github.com/fmp88/ocaml-czmq - Ocaml
* https://github.com/gar1t/erlang-czmq - Erlang
* https://github.com/mtortonesi/ruby-czmq-ffi - Ruby FFI

### API v3 Summary

This is the API provided by CZMQ v3.x, in alphabetical order.

.pull doc/zactor.doc
.pull doc/zauth.doc
.pull doc/zbeacon.doc
.pull doc/zcert.doc
.pull doc/zcertstore.doc
.pull doc/zchunk.doc
.pull doc/zclock.doc
.pull doc/zconfig.doc
.pull doc/zdigest.doc
.pull doc/zdir.doc
.pull doc/zdir_patch.doc
.pull doc/zfile.doc
.pull doc/zframe.doc
.pull doc/zgossip.doc
.pull doc/zhash.doc
.pull doc/ziflist.doc
.pull doc/zlist.doc
.pull doc/zloop.doc
.pull doc/zmonitor.doc
.pull doc/zmsg.doc
.pull doc/zpoller.doc
.pull doc/zproxy.doc
.pull doc/zrex.doc
.pull doc/zring.doc
.pull doc/zsock.doc
.pull doc/zsock_option.doc
.pull doc/zstr.doc
.pull doc/zsys.doc
.pull doc/zuuid.doc

### API v2 Summary

This is the deprecated API provided by CZMQ v2.x, in alphabetical order.

.pull doc/zauth_v2.doc
.pull doc/zctx.doc
.pull doc/zmonitor_v2.doc
.pull doc/zmutex.doc
.pull doc/zproxy_v2.doc
.pull doc/zsocket.doc
.pull doc/zsockopt.doc
.pull doc/zthread.doc

## Error Handling

The CZMQ policy is to reduce the error flow to 0/-1 where possible. libzmq still does a lot of errno setting. CZMQ does not do that, as it creates a fuzzy API. Things either work as expected, or they fail, and the application's best strategy is usually to assert on non-zero return codes.

Some older libraries still return plethora of error codes, to indicate different types of failure. This ironically makes both library and application more likely to be buggy. The reason is simply that it needs more code on both sides of the API, and the more code, the more bugs.

The use of black/white error handling fits the CLASS style for APIs where each call is explicit and without side effects of any kind, and where damage is either impossible, or fatal.

The one exception is running out of resources (memory, sockets). In that case, there are two strategies that work, for different types of app. One is to assert, to force better sizing of the machine and/or limits such as max connections. Two is to degrade carefully, e.g. refuse new connections, however that is considerably harder to do correctly and probably unrealistic for most developers.

Some CZMQ methods used to actually assert, e.g. in zsocket_bind, if the action failed, instead of returning -1. That was just closer to the majority case where the action MUST work, or nothing can continue. However there's a small slice of cases where failure means something positive, and for these cases, such calls return -1 on failure. 99% of calling code simply asserts the return value is not -1.

There are a few cases where the return value is overloaded to return -1, 0, or other values. These are somewhat confusing special cases and we aim to eliminate them over time.

The overall goal with this strategy is robustness, and absolute minimal and predictable expression in the code. You can see that it works: the CZMQ code is generally very simple and clear, with a few exceptions of places where people have used their old C style (we fix these over time).

## CZMQ Actors

The v2 API had a zthread class that let you create "attached threads" connected to their parent by an inproc:// PIPE socket. In v3 this has been simplified and better wrapped as the zactor class. CZMQ actors are in effect threads with a socket interface. A zactor_t instance works like a socket, and the CZMQ classes that deal with sockets (like zmsg and zpoller) all accept zactor_t references as well as zsock_t and libzmq void * socket handles.

To write an actor, use this template. Note that your actor is a single function "void myname (zsock_t *pipe, void *args)" function:

/*  =========================================================================
        someclass - some description

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
        Please take e.g. include/zmonitor.h as basis for your public API.
        And delete this text, and write your own, when you create an actor :-)
    @discuss

    @end
    */

    #include "../include/czmq.h"

    //  --------------------------------------------------------------------------
    //  The self_t structure holds the state for one actor instance

    typedef struct {
        zsock_t *pipe;              //  Actor command pipe
        zpoller_t *poller;          //  Socket poller
        //  ... you'll be adding other stuff here
        bool terminated;            //  Did caller ask us to quit?
        bool verbose;               //  Verbose logging enabled?
    } self_t;

    static self_t *
    s_self_new (zsock_t *pipe)
    {
        self_t *self = (self_t *) zmalloc (sizeof (self_t));
        self->pipe = pipe;
        //  ... initialize your own state including any other
        //  sockets, which you can add to the poller:
        self->poller = zpoller_new (self->pipe, NULL);
        return self;
    }

    static void
    s_self_destroy (self_t **self_p)
    {
        assert (self_p);
        if (*self_p) {
            self_t *self = *self_p;
            zpoller_destroy (&self->poller);
            //  ... destroy your own state here
            free (self);
            *self_p = NULL;
        }
    }


    //  --------------------------------------------------------------------------
    //  Handle a command from calling application

    static int
    s_self_handle_pipe (self_t *self)
    {
        //  Get the whole message off the pipe in one go
        zmsg_t *request = zmsg_recv (self->pipe);
        if (!request)
            return -1;                  //  Interrupted

        char *command = zmsg_popstr (request);
        if (self->verbose)
            zsys_info ("zxxx: API command=%s", command);
        if (streq (command, "VERBOSE"))
            self->verbose = true;
        else
        //  An example of a command that the caller would wait for
        //  via a signal, so that the two threads synchronize
        if (streq (command, "WAIT"))
            zsock_signal (self->pipe, 0);
        else
        if (streq (command, "$TERM"))
            self->terminated = true;
        else {
            zsys_error ("zxxx: - invalid command: %s", command);
            assert (false);
        }
        zstr_free (&command);
        zmsg_destroy (&request);
        return 0;
    }


    //  --------------------------------------------------------------------------
    //  zxxx() implements the zxxx actor interface

    void
    zxxx (zsock_t *pipe, void *args)
    {
        self_t *self = s_self_new (pipe);
        //  Signal successful initialization
        zsock_signal (pipe, 0);

        while (!self->terminated) {
            zsock_t *which = zpoller_wait (self->poller, -1);
            if (which == self->pipe)
                s_self_handle_pipe (self);
            else
            if (zpoller_terminated (self->poller))
                break;          //  Interrupted
        }
        s_self_destroy (&self);
    }


    //  --------------------------------------------------------------------------
    //  Selftest

    void
    zxxx_test (bool verbose)
    {
        printf (" * zxxx: ");
        if (verbose)
            printf ("\n");

        //  @selftest
        zactor_t *xxx = zactor_new (zxxx, NULL);
        assert (xxx);
        if (verbose)
            zstr_sendx (xxx, "VERBOSE", NULL);

        zactor_destroy (&xxx);
        //  @end
        printf ("OK\n");
    }

The selftest code shows how to create, talk to, and destroy an actor.

## Under the Hood

### Adding a New Class

If you define a new CZMQ class `myclass` you need to:

* Write the `zmyclass.c` and `zmyclass.h` source files, in `src` and `include` respectively.
* Add`#include <zmyclass.h>` to `include/czmq.h`.
* Add the myclass header and test call to `src/czmq_selftest.c`.
* Add a reference documentation to 'doc/zmyclass.txt'.
* Add myclass to 'model/projects.xml` and read model/README.txt.
* Add a section to README.txt.

### Documentation

Man pages are generated from the class header and source files via the doc/mkman tool, and similar functionality in the gitdown tool (http://github.com/imatix/gitdown). The header file for a class must wrap its interface as follows (example is from include/zclock.h):

    //  @interface
    //  Sleep for a number of milliseconds
    void
        zclock_sleep (int msecs);

    //  Return current system clock as milliseconds
    int64_t
        zclock_time (void);

    //  Self test of this class
    int
        zclock_test (Bool verbose);
    //  @end

The source file for a class must provide documentation as follows:

    /*
    @header
    ...short explanation of class...
    @discuss
    ...longer discussion of how it works...
    @end
    */

The source file for a class then provides the self test example as follows:

    //  @selftest
    int64_t start = zclock_time ();
    zclock_sleep (10);
    assert ((zclock_time () - start) >= 10);
    //  @end

The template for man pages is in doc/mkman.

### Development

CZMQ is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

### Porting CZMQ

When you try CZMQ on an OS that it's not been used on (ever, or for a while), you will hit code that does not compile. In some cases the patches are trivial, in other cases (usually when porting to Windows), the work needed to build equivalent functionality may be non-trivial. In any case, the benefit is that once ported, the functionality is available to all applications.

Before attempting to patch code for portability, please read the `czmq_prelude.h` header file. There are several typical types of changes you may need to make to get functionality working on a specific operating system:

* Defining typedefs which are missing on that specific compiler: do this in czmq_prelude.h.
* Defining macros that rename exotic library functions to more conventional names: do this in czmq_prelude.h.
* Reimplementing specific methods to use a non-standard API: this is typically needed on Windows. Do this in the relevant class, using #ifdefs to properly differentiate code for different platforms.

### Hints to Contributors

CZMQ is a nice, neat library, and you may not immediately appreciate why. Read the CLASS style guide please, and write your code to make it indistinguishable from the rest of the code in the library. That is the only real criteria for good style: it's invisible.

Don't include system headers in source files. The right place for these is czmq_prelude.h. If you need to check against configured libraries and/or headers, include platform.h in the source before including czmq.h.

Do read your code after you write it and ask, "Can I make this simpler?" We do use a nice minimalist and yet readable style. Learn it, adopt it, use it.

Before opening a pull request read our [contribution guidelines](https://github.com/zeromq/czmq/blob/master/CONTRIBUTING.md). Thanks!


### Code Generation

We generate the zsockopt class using [GSL](https://github.com/imatix/gsl), using a code generator script in scripts/sockopts.gsl. We also generate the project files.

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
