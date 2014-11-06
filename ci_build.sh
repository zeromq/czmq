#!/usr/bin/env bash

if [ $BUILD_TYPE == "default" ]; then
    ./autogen.sh

    # Perform regression test build against ZeroMQ v2.x
    git clone git://github.com/zeromq/zeromq2-x.git &&
    ( cd zeromq2-x; ./autogen.sh && ./configure && make check && sudo make install && sudo ldconfig ) &&
    ./configure && make check &&
    echo "*** CZMQ OK against ZeroMQ v2.x"

    # Perform regression test build against ZeroMQ v3.x
    git clone git://github.com/zeromq/zeromq3-x.git &&
    ( cd zeromq3-x; ./autogen.sh && ./configure && make check && sudo make install && sudo ldconfig ) &&
    ./configure && make check &&
    echo "*** CZMQ OK against ZeroMQ v3.x"

    # Perform regression test build against ZeroMQ v4.x + libsodium
    git clone git://github.com/jedisct1/libsodium.git &&
    ( cd libsodium; ./autogen.sh && ./configure && make check && sudo make install && sudo ldconfig ) &&
    git clone git://github.com/zeromq/zeromq4-x.git &&
    ( cd zeromq4-x; ./autogen.sh && ./configure && make check && sudo make install && sudo ldconfig ) &&
    ./configure && make check &&
    echo "*** CZMQ OK against ZeroMQ v4.x"

    # Perform regression test build against ZeroMQ master + libsodium
    git clone git://github.com/zeromq/libzmq.git &&
    ( cd libzmq; ./autogen.sh && ./configure && make check && sudo make install && sudo ldconfig ) &&
    ./configure && make check &&
    echo "*** CZMQ OK against libzmq master"
else
    cd ./builds/${BUILD_TYPE} && ./ci_build.sh
fi

