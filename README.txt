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

You will need the libtool and autotools packages. After building, run the CZMQ selftests:

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

### API Summary

#### zauth - authentication for ZeroMQ security mechanisms

.pull src/zauth.c@header,left

This is the class interface:

.pull include/zauth.h@interface,code

.pull src/zauth.c@discuss,left

#### zbeacon - LAN service announcement and discovery

.pull src/zbeacon.c@header,left

This is the class interface:

.pull include/zbeacon.h@interface,code

.pull src/zbeacon.c@discuss,left

#### zcert - work with CURVE security certificates

.pull src/zcert.c@header,left

This is the class interface:

.pull include/zcert.h@interface,code

.pull src/zcert.c@discuss,left

#### zcertstore - work with CURVE security certificate stores

.pull src/zcertstore.c@header,left

This is the class interface:

.pull include/zcertstore.h@interface,code

.pull src/zcertstore.c@discuss,left

#### zchunk - work with memory chunks

.pull src/zchunk.c@header,left

This is the class interface:

.pull include/zchunk.h@interface,code

.pull src/zchunk.c@discuss,left

#### zclock - millisecond clocks and delays

.pull src/zclock.c@header,left

This is the class interface:

.pull include/zclock.h@interface,code

.pull src/zclock.c@discuss,left

#### zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.

.pull src/zconfig.c@header,left

This is the class interface:

.pull include/zconfig.h@interface,code

.pull src/zconfig.c@discuss,left

#### zctx - working with 0MQ contexts

.pull src/zctx.c@header,left

This is the class interface:

.pull include/zctx.h@interface,code

.pull src/zctx.c@discuss,left

#### zdir - work with file-system directories

.pull src/zdir.c@header,left

This is the class interface:

.pull include/zdir.h@interface,code

.pull src/zdir.c@discuss,left

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

#### zhash - generic hash table container

.pull src/zhash.c@header,left

This is the class interface:

.pull include/zhash.h@interface,code

.pull src/zhash.c@discuss,left

#### zlist - generic list container

.pull src/zlist.c@header,left

This is the class interface:

.pull include/zlist.h@interface,code

.pull src/zlist.c@discuss,left

#### zloop - event-driven reactor

.pull src/zloop.c@header,left

This is the class interface:

.pull include/zloop.h@interface,code

.pull src/zloop.c@discuss,left

#### zmonitor - socket event monitor

.pull src/zmonitor.c@header,left

This is the class interface:

.pull include/zmonitor.h@interface,code

.pull src/zmonitor.c@discuss,left

#### zmsg - working with multipart messages

.pull src/zmsg.c@header,left

This is the class interface:

.pull include/zmsg.h@interface,code

.pull src/zmsg.c@discuss,left

#### zmutex - wrap lightweight mutexes

.pull src/zmutex.c@header,left

This is the class interface:

.pull include/zmutex.h@interface,code

.pull src/zmutex.c@discuss,left

#### zpoller - trivial socket poller class

.pull src/zpoller.c@header,left

This is the class interface:

.pull include/zpoller.h@interface,code

.pull src/zpoller.c@discuss,left

#### zproxy - convenient zmq_proxy API

.pull src/zproxy.c@header,left

This is the class interface:

.pull include/zproxy.h@interface,code

.pull src/zproxy.c@discuss,left

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

#### zsys - system-level methods

.pull src/zsys.c@header,left

This is the class interface:

.pull include/zsys.h@interface,code

.pull src/zsys.c@discuss,left

#### zrex - working with regular expressions

.pull src/zrex.c@header,left

This is the class interface:

.pull include/zrex.h@interface,code

.pull src/zrex.c@discuss,left

#### zthread - working with system threads

.pull src/zthread.c@header,left

This is the class interface:

.pull include/zthread.h@interface,code

.pull src/zthread.c@discuss,left

#### zuuid - UUID management

.pull src/zuuid.c@header,left

This is the class interface:

.pull include/zuuid.h@interface,code

.pull src/zuuid.c@discuss,left

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

### Code Generation

We generate the zsockopt class using [https://github.com/imatix/gsl GSL], using a code generator script in scripts/sockopts.gsl.

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
