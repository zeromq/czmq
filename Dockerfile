FROM ubuntu:trusty
MAINTAINER czmq Developers <zeromq-dev@lists.zeromq.org>

RUN DEBIAN_FRONTEND=noninteractive apt-get update -y -q
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y -q --force-yes build-essential git-core libtool autotools-dev autoconf automake pkg-config unzip libkrb5-dev cmake

RUN useradd -d /home/zmq -m -s /bin/bash zmq
RUN echo "zmq ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/zmq
RUN chmod 0440 /etc/sudoers.d/zmq

USER zmq

WORKDIR /home/zmq
RUN git clone --quiet https://github.com/zeromq/libzmq.git libzmq
WORKDIR /home/zmq/libzmq
RUN ./autogen.sh 2> /dev/null
RUN ./configure --quiet --without-docs
RUN make
RUN sudo make install
RUN sudo ldconfig

WORKDIR /home/zmq
RUN git clone --quiet git://github.com/zeromq/czmq.git czmq
WORKDIR /home/zmq/czmq czmq
RUN ./autogen.sh 2> /dev/null
RUN ./configure --quiet --without-docs
RUN make
RUN sudo make install
RUN sudo ldconfig
