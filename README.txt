.set GIT=https://github.com/zeromq/zapi
.sub 0MQ=ØMQ

# zapi - High-level C binding for 0MQ

## Contents

.toc

## Overview

### Scope and Goals

zapi has these goals:

* To wrap the 0MQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of 0MQ, particularly 2.0, 2.1, and 3.0.
* To provide a space for development of more sophisticated API semantics.

zapi grew out of concepts developed in [ØMQ - The Guide](http://zguide.zeromq.org) and [ZFL](http://zfl.zeromq.org).

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
  Close sockets   | libzapi |           |    get/set socket options
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

* Single API hides differences between 0MQ/2.1, and 0MQ/3.0.
* Broken into easy-to-use classes: zctx, zstr, zframe, zmsg, zloop, zhash, and zlist.
* Work with messages as strings, individual frames, or multipart messages.
* Automatic closure of any open sockets at context termination.
* Automatic LINGER configuration of all sockets for context termination.
* Portable API for creating child threads and 0MQ pipes to talk to them.
* Simple reactor with one-off and repeated timers, and socket readers.
* System clock functions for sleeping and calculating timers.
* Portable to Linux, UNIX, OS X, Windows (porting is not yet complete).
* Includes generic hash and list containers.
* Full selftests on all classes.

### Ownership and License

zapi is maintained by Pieter Hintjens and Mikko Koppanen (build system). Its other authors and contributors are listed in the AUTHORS file. It is held by the ZeroMQ organization at github.com.

The authors of zapi grant you use of this software under the terms of the GNU Lesser General Public License (LGPL). For details see the files `COPYING` and `COPYING.LESSER` in this directory.

### Contributing

To submit an issue use the [issue tracker](http://github.com/zeromq/zapi/issues). All discussion happens on the [zeromq-dev](zeromq-dev@lists.zeromq.org) list or #zeromq IRC channel at irc.freenode.net.

The proper way to submit patches is to clone this repository, make your changes, and use git to create a patch or a pull request. See http://www.zeromq.org/docs:contributing. All contributors are listed in AUTHORS.

The general rule is, if you contribute code to zapi you must be willing to maintain it as long as there are users of it. Code with no active maintainer will in general be deprecated and/or removed.

## Using zapi

### Building and Installing

zapi uses autotools for packaging. To build from git (all example commands are for Linux):

    git clone git://github.com/zeromq/zapi.git
    cd zapi
    sh autogen.sh
    ./configure
    make all
    sudo make install
    sudo ldconfig

You will need the libtool and autotools packages. On FreeBSD, you may need to specify the default directories for configure:

    ./configure --with-zeromq=/usr/local

After building, you can run the zapi selftests:

    make check

### Linking with an Application

Include `zapi.h` in your application and link with libzapi. Here is a typical gcc link command:

    gcc -lzapi -lzmq myapp.c -o myapp

### API Summary

#### zctx - working with 0MQ contexts

.pull src/zctx.c@header,left

This is the class interface:

.pull include/zctx.h@interface,code

.pull src/zctx.c@discuss,left

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

### Development

zapi is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
