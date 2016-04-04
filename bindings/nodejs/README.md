# Node.js Binding for CZMQ

This is a development kit.

## Prerequisites

### Node.js

* You need Python (v2.7 recommended, v3.x not supported)
* You need (I recommend) nvm and Node.js.
* If your Linux has an existing 'node' command, `sudo apt-get remove node`.
* In every terminal, or .bashrc: `nvm use v5.5.0`

To install the necessary Node tools:

```
sudo apt-get update
sudo apt-get install build-essential libssl-dev
curl https://raw.githubusercontent.com/creationix/nvm/v0.11.1/install.sh | bash
# close terminal, re-open
nvm ls-remote
nvm install v5.5.0
npm install -g node-ninja
npm install -g prebuild
npm install -g bindings
```

To build:

```
mkdir -p $HOME/temp
cd $HOME/temp
git clone https://github.com/zeromq/czmq
cd czmq/bindings/nodejs
#   Clones dependencies, builds everything
./build.sh
```

## API

This is a wrapping of the native C libczmq library. See binding.cc for the code.

We get these classes:

* Zarmour - armoured text encoding and decoding
* Zcert - work with CURVE security certificates
* Zcertstore - work with CURVE security certificate stores
* Zchunk - work with memory chunks
* Zclock - millisecond clocks and delays
* Zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.
* Zdigest - provides hashing functions (SHA-1 at present)
* Zdir - work with file-system directories
* ZdirPatch - work with directory patches
* Zfile - helper functions for working with files.
* Zframe - working with single message frames
* Zhash - generic type-free hash container (simple)
* Zhashx - extended generic type-free hash container
* Ziflist - List of network interfaces available on system
* Zlist - simple generic list container
* Zlistx - extended generic list container
* Zloop - event-driven reactor
* Zmsg - working with multipart messages
* Zproc - process configuration and status
* Zsock - high-level socket API that hides libzmq contexts and sockets
* Zstr - sending and receiving strings
* Ztrie - simple trie for tokenizable strings
* Zuuid - UUID support class
