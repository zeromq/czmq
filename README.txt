.set GIT=https://github.com/zeromq/czmq
.sub 0MQ=ØMQ

# CZMQ - High-level C binding for 0MQ

[![Build Status](https://travis-ci.org/zeromq/czmq.png?branch=master)](https://travis-ci.org/zeromq/czmq)

## Contents

.toc

## Overview

### Scope and Goals

CZMQ has these goals:

* To wrap the 0MQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of 0MQ, particularly 2.x and 3.x.
* To provide a space for development of more sophisticated API semantics.

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

### Highlights

* Single API hides differences between 0MQ/2.x, and 0MQ/3.x.
* Work with messages as strings, individual frames, or multipart messages.
* Automatic closure of any open sockets at context termination.
* Automatic LINGER configuration of all sockets for context termination.
* Portable API for creating child threads and 0MQ pipes to talk to them.
* Simple reactor with one-off and repeated timers, and socket readers.
* System clock functions for sleeping and calculating timers.
* Easy API to get/set all socket options.
* Portable to Linux, UNIX, OS X, Windows (porting is not yet complete).
* Includes generic hash and list containers.
* Full self tests on all classes.

### Ownership and License

CZMQ's contributors are listed in the AUTHORS file. It is held by the ZeroMQ organization at github.com. The authors of CZMQ grant you use of this software under the terms of the GNU Lesser General Public License (LGPL). For details see the files `COPYING` and `COPYING.LESSER` in this directory.

### Contributing

CZMQ uses the [C4.1 (Collective Code Construction Contract)](http://rfc.zeromq.org/spec:22) process for contributions.

CZMQ uses the [CLASS (C Language Style for Scalabilty)](http://rfc.zeromq.org/spec:21) guide for code style.

To report an issue, use the [CZMQ issue tracker](https://github.com/zeromq/czmq/issues) at github.com.

## Using CZMQ

### Building and Installing

CZMQ uses autotools for packaging. To build from git (all example commands are for Linux):

    git clone git://github.com/zeromq/czmq.git
    cd czmq
    sh autogen.sh
    ./configure
    make all
    sudo make install
    sudo ldconfig

You will need the libtool and autotools packages. On FreeBSD, you may need to specify the default directories for configure:

    ./configure --with-libzmq=/usr/local

After building, you can run the CZMQ selftests:

    make check

### Linking with an Application

Include `czmq.h` in your application and link with libczmq. Here is a typical gcc link command:

    gcc -lczmq -lzmq myapp.c -o myapp

### API Summary

#### zctx - working with 0MQ contexts

.pull src/zctx.c@header,left

This is the class interface:

.pull include/zctx.h@interface,code

.pull src/zctx.c@discuss,left

#### zsocket - working with 0MQ sockets

.pull src/zsocket.c@header,left

This is the class interface:

.pull include/zsocket.h@interface,code

.pull src/zsocket.c@discuss,left

#### zsockopt - working with 0MQ socket options

.pull src/zsockopt.c@header,left

This is the class interface:

.pull include/zsockopt.h@interface,code

.pull src/zsockopt.c@discuss,left

#### zstr - sending and receiving strings

.pull src/zstr.c@header,left

[diagram]

           Memory                       Wire
           +-------------+---+          +---+-------------+
    Send   | S t r i n g | 0 |  ---->   | 6 | S t r i n g |
           +-------------+---+          +---+-------------+

           Wire                         Heap
           +---+-------------+          +-------------+---+
    Recv   | 6 | S t r i n g |  ---->   | S t r i n g | 0 |
           +---+-------------+          +-------------+---+

[/diagram]

This is the class interface:

.pull include/zstr.h@interface,code

.pull src/zstr.c@discuss,left

#### zfile - work with files

.pull src/zfile.c@header,left

This is the class interface:

.pull include/zfile.h@interface,code

.pull src/zfile.c@discuss,left

#### zframe - working with single message frames

.pull src/zframe.c@header,left

This is the class interface:

.pull include/zframe.h@interface,code

.pull src/zframe.c@discuss,left

#### zmsg - working with multipart messages

.pull src/zmsg.c@header,left

This is the class interface:

.pull include/zmsg.h@interface,code

.pull src/zmsg.c@discuss,left

#### zloop - event-driven reactor

.pull src/zloop.c@header,left

This is the class interface:

.pull include/zloop.h@interface,code

.pull src/zloop.c@discuss,left

#### zthread - working with system threads

.pull src/zthread.c@header,left

This is the class interface:

.pull include/zthread.h@interface,code

.pull src/zthread.c@discuss,left

#### zhash - expandable hash table container

.pull src/zhash.c@header,left

This is the class interface:

.pull include/zhash.h@interface,code

.pull src/zhash.c@discuss,left

#### zlist - singly-linked list container

.pull src/zlist.c@header,left

This is the class interface:

.pull include/zlist.h@interface,code

.pull src/zlist.c@discuss,left

#### zclock - millisecond clocks and delays

.pull src/zclock.c@header,left

This is the class interface:

.pull include/zclock.h@interface,code

.pull src/zclock.c@discuss,left

#### zmutex - wrap lightweight mutexes

.pull src/zmutex.c@header,left

This is the class interface:

.pull include/zmutex.h@interface,code

.pull src/zmutex.c@discuss,left

## Under the Hood

### Adding a New Class

If you define a new CZMQ class `myclass` you need to:

* Write the `zmyclass.c` and `zmyclass.h` source files, in `src` and `include` respectively.
* Add`#include <zmyclass.h>` to `include/czmq.h`.
* Add the myclass header and test call to `src/czmq_selftest.c`.
* Add a reference documentation to 'doc/zmyclass.txt'.
* Add myclass to 'src/Makefile.am` and `doc/Makefile.am`.

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

### Code Generation

We generate the zsockopt class using [https://github.com/imatix/gsl GSL].

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
