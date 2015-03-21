#!/usr/bin/env bash

# Assumes Travis CI os:linux (x86_64)

git clone git://github.com/jedisct1/libsodium.git &&
( cd libsodium; ./autogen.sh && ./configure && make check && sudo make install && sudo ldconfig ) || exit 1

# Build, check, and install ZeroMQ
git clone git://github.com/zeromq/libzmq.git &&
( cd libzmq; ./autogen.sh && ./configure && make check && sudo make install && sudo ldconfig ) || exit 1

# Build, check, and install CZMQ from local source
( cd ../..; ./autogen.sh && ./configure && make check-py )
