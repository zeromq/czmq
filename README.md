
<A name="toc1-3" title="zapi - ØMQ Function Library" />
# zapi - ØMQ Function Library

<A name="toc2-6" title="Contents" />
## Contents


**<a href="#toc2-11">Overview</a>**
&emsp;<a href="#toc3-14">Scope and Goals</a>
&emsp;<a href="#toc3-23">Ownership and License</a>
&emsp;<a href="#toc3-30">Contributing</a>

**<a href="#toc2-41">Using zapi</a>**
&emsp;<a href="#toc3-44">Building and Installing</a>
&emsp;<a href="#toc3-65">Linking with an Application</a>
&emsp;<a href="#toc3-72">This Document</a>

<A name="toc2-11" title="Overview" />
## Overview

<A name="toc3-14" title="Scope and Goals" />
### Scope and Goals

zapi has these goals:

* To wrap the ØMQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of ØMQ.
* To provide a space for development of more sophisticated API semantics.

<A name="toc3-23" title="Ownership and License" />
### Ownership and License

zapi is maintained by Pieter Hintjens and Mikko Koppanen (build system). Its other authors and contributors are listed in the AUTHORS file. It is held by the ZeroMQ organization at github.com.

The authors of zapi grant you use of this software under the terms of the GNU Lesser General Public License (LGPL). For details see the files `COPYING` and `COPYING.LESSER` in this directory.

<A name="toc3-30" title="Contributing" />
### Contributing

To submit an issue use the [issue tracker](http://github.com/zeromq/zapi/issues). All discussion happens on the [zeromq-dev](zeromq-dev@lists.zeromq.org) list or #zeromq IRC channel at irc.freenode.net.

The proper way to submit patches is to clone this repository, make your changes, and use git to create a patch or a pull request. See http://www.zeromq.org/docs:contributing. All contributors are listed in AUTHORS.

All classes are maintained by a single person, who is the responsible editor for that class and who is named in the header as such. This is usually the originator of the class. When several people collaborate on a class, one single person is always the lead maintainer and the one to blame when it breaks.

The general rule is, if you contribute code to zapi you must be willing to maintain it as long as there are users of it. Code with no active maintainer will in general be deprecated and/or removed.

<A name="toc2-41" title="Using zapi" />
## Using zapi

<A name="toc3-44" title="Building and Installing" />
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

<A name="toc3-65" title="Linking with an Application" />
### Linking with an Application

Include `zapi.h` in your application and link with libzapi. Here is a typical gcc link command:

    gcc -lzapi -lzmq myapp.c -o myapp

<A name="toc3-72" title="This Document" />
### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
