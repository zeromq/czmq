
[![GitHub release](https://img.shields.io/github/release/zeromq/czmq.svg)](https://github.com/zeromq/czmq/releases)
[![OBS draft](https://img.shields.io/badge/OBS%20master-draft-yellow.svg)](http://software.opensuse.org/download.html?project=home%3Azeromq%3Agit-draft&package=czmq)
[![OBS stable](https://img.shields.io/badge/OBS%20master-stable-yellow.svg)](http://software.opensuse.org/download.html?project=home%3Azeromq%3Agit-stable&package=czmq)
<a target="_blank" href="http://webchat.freenode.net?channels=%23zeromq&uio=d4"><img src="https://cloud.githubusercontent.com/assets/493242/14886493/5c660ea2-0d51-11e6-8249-502e6c71e9f2.png" height = "20" /></a>
[![license](https://img.shields.io/github/license/zeromq/czmq.svg)](https://github.com/zeromq/czmq/blob/master/LICENSE)

<A name="toc1-9" title="CZMQ - High-level C binding for ØMQ" />
# CZMQ - High-level C binding for ØMQ

| Linux & MacOSX | Windows  |
|:--------------:|:--------:|
|[![Build Status](https://travis-ci.org/zeromq/czmq.png?branch=master)](https://travis-ci.org/zeromq/czmq)|[![Build status](https://ci.appveyor.com/api/projects/status/q7y22juu3pnl5wq6?svg=true)](https://ci.appveyor.com/project/zeromq/czmq)|

<A name="toc2-16" title="Contents" />
## Contents


**<a href="#toc2-21">Overview</a>**

**<a href="#toc3-24">Scope and Goals</a>**

**<a href="#toc3-65">Ownership and License</a>**

**<a href="#toc2-76">Using CZMQ</a>**

**<a href="#toc3-79">Building and Installing</a>**

**<a href="#toc3-149">Building on Windows</a>**

**<a href="#toc3-205">Linking with an Application</a>**

**<a href="#toc3-212">Use from Other Languages</a>**

**<a href="#toc3-237">API v3 Summary</a>**
*  <a href="#toc4-242">zactor - simple actor framework</a>
*  <a href="#toc4-324">zauth - authentication for ZeroMQ security mechanisms</a>
*  <a href="#toc4-582">zbeacon - LAN discovery and presence</a>
*  <a href="#toc4-770">zcert - work with CURVE security certificates</a>
*  <a href="#toc4-944">zcertstore - work with CURVE security certificate stores</a>
*  <a href="#toc4-1079">zchunk - work with memory chunks</a>
*  <a href="#toc4-1293">zclock - millisecond clocks and delays</a>
*  <a href="#toc4-1359">zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.</a>
*  <a href="#toc4-1633">zdigest - provides hashing functions (SHA-1 at present)</a>
*  <a href="#toc4-1703">zdir - work with file-system directories</a>
*  <a href="#toc4-1939">zdir_patch - work with directory patches</a>
*  <a href="#toc4-2015">zfile - provides methods to work with files in a portable fashion.</a>
*  <a href="#toc4-2311">zframe - working with single message frames</a>
*  <a href="#toc4-2621">zgossip - decentralized configuration management</a>
*  <a href="#toc4-2820">zhash - simple generic hash container</a>
*  <a href="#toc4-3149">zhashx - extended generic hash container</a>
*  <a href="#toc4-3598">ziflist - list of network interfaces available on system</a>
*  <a href="#toc4-3682">zlist - simple generic list container</a>
*  <a href="#toc4-3948">zlistx - extended generic list container</a>
*  <a href="#toc4-4245">zloop - event-driven reactor</a>
*  <a href="#toc4-4460">zmonitor - socket event monitor</a>
*  <a href="#toc4-4568">zmsg - working with multipart messages</a>
*  <a href="#toc4-5062">zpoller - trivial socket poller class</a>
*  <a href="#toc4-5233">zproc - process configuration and status</a>
*  <a href="#toc4-5385">zproxy - run a steerable proxy in the background</a>
*  <a href="#toc4-5736">zrex - work with regular expressions</a>
*  <a href="#toc4-5875">zsock - high-level socket API that hides libzmq contexts and sockets</a>
*  <a href="#toc4-7161">zstr - sending and receiving strings</a>
*  <a href="#toc4-7336">zsys - system-level methods</a>
*  <a href="#toc4-7823">ztimerset - timer set</a>
*  <a href="#toc4-7949">ztrie - simple trie for tokenizable strings</a>
*  <a href="#toc4-8192">zuuid - UUID support class</a>

**<a href="#toc2-8313">Error Handling</a>**

**<a href="#toc2-8330">CZMQ Actors</a>**

**<a href="#toc2-8476">Under the Hood</a>**

**<a href="#toc3-8479">Adding a New Class</a>**

**<a href="#toc3-8491">Documentation</a>**

**<a href="#toc3-8530">Development</a>**

**<a href="#toc3-8540">Porting CZMQ</a>**

**<a href="#toc3-8551">Hints to Contributors</a>**

**<a href="#toc3-8562">Code Generation</a>**

**<a href="#toc3-8567">This Document</a>**

<A name="toc2-21" title="Overview" />
## Overview

<A name="toc3-24" title="Scope and Goals" />
### Scope and Goals

CZMQ has these goals:

* To wrap the ØMQ core API in semantics that lead to shorter, more readable applications.
* To hide as far as possible the differences between different versions of ØMQ (2.x, 3.x, 4.x).
* To provide a space for development of more sophisticated API semantics.
* To wrap the ØMQ security features with high-level tools and APIs.
* To become the basis for other language bindings built on top of CZMQ.

CZMQ grew out of concepts developed in [ØMQ - The Guide](http://zguide.zeromq.org).

<center>
<img src="https://github.com/zeromq/czmq/raw/master/images/README_1.png" alt="1">
</center>

<A name="toc3-65" title="Ownership and License" />
### Ownership and License

The contributors are listed in AUTHORS. This project uses the MPL v2 license, see LICENSE.

CZMQ uses the [C4.1 (Collective Code Construction Contract)](http://rfc.zeromq.org/spec:22) process for contributions.

CZMQ uses the [CLASS (C Language Style for Scalabilty)](http://rfc.zeromq.org/spec:21) guide for code style.

To report an issue, use the [CZMQ issue tracker](https://github.com/zeromq/czmq/issues) at github.com.

<A name="toc2-76" title="Using CZMQ" />
## Using CZMQ

<A name="toc3-79" title="Building and Installing" />
### Building and Installing

To start with, you need at least these packages:

* {{git-all}} -- git is how we share code with other people.

* {{build-essential}}, {{libtool}}, {{pkg-config}} - the C compiler and related tools.

* {{autotools-dev}}, {{autoconf}}, {{automake}} - the GNU autoconf makefile generators.

* {{cmake}} - the CMake makefile generators (an alternative to autoconf).

Plus some others:

* {{uuid-dev}}, {{libpcre3-dev}} - utility libraries.

* {{valgrind}} - a useful tool for checking your code.

* {{pkg-config}} - an optional useful tool to make building with dependencies easier.

Which we install like this (using the Debian-style apt-get package manager):

    sudo apt-get update
    sudo apt-get install -y \
        git-all build-essential libtool \
        pkg-config autotools-dev autoconf automake cmake \
        uuid-dev libpcre3-dev valgrind

    # only execute this next line if interested in updating the man pages as well (adds to build time):
    sudo apt-get install -y asciidoc

Here's how to build CZMQ from GitHub (building from packages is very similar, you don't clone a repo but unpack a tarball), including the libzmq (ZeroMQ core) library:

    git clone git://github.com/zeromq/libzmq.git
    cd libzmq
    ./autogen.sh
    # do not specify "--with-libsodium" if you prefer to use internal tweetnacl security implementation (recommended for development)
    ./configure --with-libsodium
    make check
    sudo make install
    sudo ldconfig
    cd ..

    git clone git://github.com/zeromq/czmq.git
    cd czmq
    ./autogen.sh && ./configure && make check
    sudo make install
    sudo ldconfig
    cd ..

In general CZMQ works best with the latest libzmq master. If you already have an older version of libzmq installed on your system, e.g. in /usr/, then you can install libzmq master to your home directory ($HOME/local):

    #   Building libzmq in our home directory
    ./configure --prefix=$HOME/local

And then to build CZMQ against this installation of libzmq:

    export CFLAGS=-I$HOME/local/include
    export LDFLAGS=-L$HOME/local/lib64
    export PKG_CONFIG_PATH=$HOME/local/lib64/pkgconfig
    ./configure

NOTE: the PKG_CONFIG_PATH is not mandatory, and the actual directory might be different. If you cannot or do not want to use pkg-config, please make sure to MANUALLY add all the necessary CFLAGS and LDFLAGS from all dependencies (for example -DZMQ_BUILD_DRAFT_API=1 if you want the DRAFT APIs).

You will need the pkg-config, libtool, and autoreconf packages. After building, run the CZMQ selftests:

    make check


<A name="toc3-149" title="Building on Windows" />
### Building on Windows

To start with, you need MS Visual Studio (C/C++). The free community edition works well.

Then, install git, and make sure it works from a DevStudio command prompt:

```
git
```

Now let's build CZMQ from GitHub:

```
    git clone --depth 1 -b stable https://github.com/jedisct1/libsodium.git
    cd libsodium\builds\msvc\build
    buildall.bat
    cd ..\..\..\..

    :: if libsodium is on disk, the Windows build of libzmq will automatically use it
    git clone git://github.com/zeromq/libzmq.git
    cd libzmq\builds\msvc
    configure.bat
    cd build
    buildall.bat
    cd ..\..\..\..

    git clone git://github.com/zeromq/czmq.git
    cd czmq\builds\msvc
    configure.bat
    cd build
    buildall.bat
    cd ..\..\..\..
```

Let's test by running `czmq_selftest`:

```
   czmq>dir/s/b czmq_selftest.exe
   czmq\builds\msvc\vs2013\DebugDEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\DebugLEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\DebugSEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\ReleaseDEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\ReleaseLEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\ReleaseSEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\x64\DebugDEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\x64\DebugLEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\x64\DebugSEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\x64\ReleaseDEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\x64\ReleaseLEXE\czmq_selftest.exe
   czmq\builds\msvc\vs2013\x64\ReleaseSEXE\czmq_selftest.exe

    :: select your choice and run it
    czmq\builds\msvc\vs2013\x64\ReleaseDEXE\czmq_selftest.exe
```

<A name="toc3-205" title="Linking with an Application" />
### Linking with an Application

Include `czmq.h` in your application and link with libczmq. Here is a typical gcc link command:

    gcc myapp.c -o myapp -lczmq -lzmq

<A name="toc3-212" title="Use from Other Languages" />
### Use from Other Languages

This is a list of auto-generated bindings:

* https://github.com/zeromq/czmq/tree/master/bindings/jni - Java
* https://github.com/zeromq/czmq/tree/master/bindings/nodejs - NodeJS
* https://github.com/zeromq/czmq/tree/master/bindings/python - Python
* https://github.com/zeromq/czmq/tree/master/bindings/python_cffi - Python (cffi)
* https://github.com/zeromq/czmq/tree/master/bindings/qml - QML
* https://github.com/zeromq/czmq/tree/master/bindings/qt - Qt
* https://github.com/zeromq/czmq/tree/master/bindings/ruby - Ruby (FFI)

This is a list of known higher-level wrappers around CZMQ:

* https://github.com/1100110/CZMQ - D bindings
* https://github.com/methodmissing/rbczmq - Ruby
* https://github.com/paddor/cztop - Ruby, based on generated FFI binding
* https://github.com/zeromq/pyczmq - Python
* https://github.com/lhope/cl-czmq - Common Lisp
* https://github.com/fmp88/ocaml-czmq - Ocaml
* https://github.com/gar1t/erlang-czmq - Erlang
* https://github.com/mtortonesi/ruby-czmq-ffi - Ruby FFI
* https://github.com/zeromq/goczmq - Go

<A name="toc3-237" title="API v3 Summary" />
### API v3 Summary

This is the API provided by CZMQ v3.x, in alphabetical order.

<A name="toc4-242" title="zactor - simple actor framework" />
#### zactor - simple actor framework

The zactor class provides a simple actor framework. It replaces the
CZMQ zthread class, which had a complex API that did not fit the CLASS
standard. A CZMQ actor is implemented as a thread plus a PAIR-PAIR
pipe. The constructor and destructor are always synchronized, so the
caller can be sure all resources are created, and destroyed, when these
calls complete. (This solves a major problem with zthread, that a caller
could not be sure when a child thread had finished.)

A zactor_t instance acts like a zsock_t and you can pass it to any CZMQ
method that would take a zsock_t argument, including methods in zframe,
zmsg, zstr, and zpoller. (zloop somehow escaped and needs catching.)

An actor function MUST call zsock_signal (pipe) when initialized
and MUST listen to pipe and exit on $TERM command.

Please add @discuss section in ../src/zactor.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    // Actors get a pipe and arguments from caller
    typedef void (zactor_fn) (
        zsock_t *pipe, void *args);
    
    //  Create a new actor passing arbitrary arguments reference.
    CZMQ_EXPORT zactor_t *
        zactor_new (zactor_fn task, void *args);
    
    //  Destroy an actor.
    CZMQ_EXPORT void
        zactor_destroy (zactor_t **self_p);
    
    //  Send a zmsg message to the actor, take ownership of the message
    //  and destroy when it has been sent.                             
    CZMQ_EXPORT int
        zactor_send (zactor_t *self, zmsg_t **msg_p);
    
    //  Receive a zmsg message from the actor. Returns NULL if the actor 
    //  was interrupted before the message could be received, or if there
    //  was a timeout on the actor.                                      
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zmsg_t *
        zactor_recv (zactor_t *self);
    
    //  Probe the supplied object, and report if it looks like a zactor_t.
    CZMQ_EXPORT bool
        zactor_is (void *self);
    
    //  Probe the supplied reference. If it looks like a zactor_t instance,
    //  return the underlying libzmq actor handle; else if it looks like   
    //  a libzmq actor handle, return the supplied value.                  
    CZMQ_EXPORT void *
        zactor_resolve (void *self);
    
    //  Return the actor's zsock handle. Use this when you absolutely need
    //  to work with the zsock instance rather than the actor.            
    CZMQ_EXPORT zsock_t *
        zactor_sock (zactor_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zactor_test (bool verbose);
    
```

This is the class self test code:

```c
    zactor_t *actor = zactor_new (echo_actor, "Hello, World");
    assert (actor);
    zstr_sendx (actor, "ECHO", "This is a string", NULL);
    char *string = zstr_recv (actor);
    assert (streq (string, "This is a string"));
    free (string);
    zactor_destroy (&actor);
```

<A name="toc4-324" title="zauth - authentication for ZeroMQ security mechanisms" />
#### zauth - authentication for ZeroMQ security mechanisms

A zauth actor takes over authentication for all incoming connections in
its context. You can whitelist or blacklist peers based on IP address,
and define policies for securing PLAIN, CURVE, and GSSAPI connections.

This class replaces zauth_v2, and is meant for applications that use the
CZMQ v3 API (meaning, zsock).

This is the class interface:

```h
    #define CURVE_ALLOW_ANY "*"
    
    //  CZMQ v3 API (for use with zsock, not zsocket, which is deprecated).
    //
    //  Create new zauth actor instance. This installs authentication on all 
    //  zsock sockets. Until you add policies, all incoming NULL connections are
    //  allowed (classic ZeroMQ behaviour), and all PLAIN and CURVE connections
    //  are denied:
    //  
    //      zactor_t *auth = zactor_new (zauth, NULL);
    //
    //  Destroy zauth instance. This removes authentication and allows all
    //  connections to pass, without authentication:
    //  
    //      zactor_destroy (&auth);
    //
    //  Note that all zauth commands are synchronous, so your application always
    //  waits for a signal from the actor after each command.
    //
    //  Enable verbose logging of commands and activity. Verbose logging can help
    //  debug non-trivial authentication policies:
    //
    //      zstr_send (auth, "VERBOSE");
    //      zsock_wait (auth);
    //
    //  Allow (whitelist) a list of IP addresses. For NULL, all clients from
    //  these addresses will be accepted. For PLAIN and CURVE, they will be
    //  allowed to continue with authentication. You can call this method
    //  multiple times to whitelist more IP addresses. If you whitelist one
    //  or more addresses, any non-whitelisted addresses are treated as
    //  blacklisted:
    //  
    //      zstr_sendx (auth, "ALLOW", "127.0.0.1", "127.0.0.2", NULL);
    //      zsock_wait (auth);
    //  
    //  Deny (blacklist) a list of IP addresses. For all security mechanisms,
    //  this rejects the connection without any further authentication. Use
    //  either a whitelist, or a blacklist, not not both. If you define both
    //  a whitelist and a blacklist, only the whitelist takes effect:
    //  
    //      zstr_sendx (auth, "DENY", "192.168.0.1", "192.168.0.2", NULL);
    //      zsock_wait (auth);
    //
    //  Configure PLAIN authentication using a plain-text password file. You can
    //  modify the password file at any time; zauth will reload it automatically
    //  if modified externally:
    //  
    //      zstr_sendx (auth, "PLAIN", filename, NULL);
    //      zsock_wait (auth);
    //
    //  Configure CURVE authentication, using a directory that holds all public
    //  client certificates, i.e. their public keys. The certificates must be in
    //  zcert_save format. You can add and remove certificates in that directory
    //  at any time. To allow all client keys without checking, specify
    //  CURVE_ALLOW_ANY for the directory name:
    //
    //      zstr_sendx (auth, "CURVE", directory, NULL);
    //      zsock_wait (auth);
    //
    //  Configure GSSAPI authentication, using an underlying mechanism (usually
    //  Kerberos) to establish a secure context and perform mutual authentication:
    //
    //      zstr_sendx (auth, "GSSAPI", NULL);
    //      zsock_wait (auth);
    //
    //  This is the zauth constructor as a zactor_fn:
    CZMQ_EXPORT void
        zauth (zsock_t *pipe, void *certstore);
    
    //  Selftest
    CZMQ_EXPORT void
        zauth_test (bool verbose);
```

This is the class self test code:

```c
    //  Create temporary directory for test files
    #   define TESTDIR ".test_zauth"
    zsys_dir_create (TESTDIR);
    
    //  Check there's no authentication
    zsock_t *server = zsock_new (ZMQ_PULL);
    assert (server);
    zsock_t *client = zsock_new (ZMQ_PUSH);
    assert (client);
    bool success = s_can_connect (&server, &client, true);
    assert (success);
    
    //  Install the authenticator
    zactor_t *auth = zactor_new (zauth, NULL);
    assert (auth);
    if (verbose) {
        zstr_sendx (auth, "VERBOSE", NULL);
        zsock_wait (auth);
    }
    //  Check there's no authentication on a default NULL server
    success = s_can_connect (&server, &client, true);
    assert (success);
    
    //  When we set a domain on the server, we switch on authentication
    //  for NULL sockets, but with no policies, the client connection
    //  will be allowed.
    zsock_set_zap_domain (server, "global");
    success = s_can_connect (&server, &client, true);
    assert (success);
    
    //  Blacklist 127.0.0.1, connection should fail
    zsock_set_zap_domain (server, "global");
    zstr_sendx (auth, "DENY", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&server, &client, true);
    assert (!success);
    
    //  Whitelist our address, which overrides the blacklist
    zsock_set_zap_domain (server, "global");
    zstr_sendx (auth, "ALLOW", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&server, &client, true);
    assert (success);
    
    //  Try PLAIN authentication
    zsock_set_plain_server (server, 1);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "Password");
    success = s_can_connect (&server, &client, true);
    assert (!success);
    
    FILE *password = fopen (TESTDIR "/password-file", "w");
    assert (password);
    fprintf (password, "admin=Password\n");
    fclose (password);
    zsock_set_plain_server (server, 1);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "Password");
    zstr_sendx (auth, "PLAIN", TESTDIR "/password-file", NULL);
    zsock_wait (auth);
    success = s_can_connect (&server, &client, true);
    assert (success);
    
    zsock_set_plain_server (server, 1);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "Bogus");
    success = s_can_connect (&server, &client, true);
    assert (!success);
    
    if (zsys_has_curve ()) {
        //  Try CURVE authentication
        //  We'll create two new certificates and save the client public
        //  certificate on disk; in a real case we'd transfer this securely
        //  from the client machine to the server machine.
        zcert_t *server_cert = zcert_new ();
        assert (server_cert);
        zcert_t *client_cert = zcert_new ();
        assert (client_cert);
        const char *server_key = zcert_public_txt (server_cert);
    
        //  Test without setting-up any authentication
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        success = s_can_connect (&server, &client, true);
        assert (!success);
    
        //  Test CURVE_ALLOW_ANY
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        zstr_sendx (auth, "CURVE", CURVE_ALLOW_ANY, NULL);
        zsock_wait (auth);
        success = s_can_connect (&server, &client, true);
        assert (success);
    
        //  Test full client authentication using certificates
        zcert_set_meta (client_cert, "Hello", "%s", "World!");
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        zcert_save_public (client_cert, TESTDIR "/mycert.txt");
        zstr_sendx (auth, "CURVE", TESTDIR, NULL);
        zsock_wait (auth);
        success = s_can_connect (&server, &client, false);
        assert (success);
    
    #if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (4, 1, 0))
        // Test send/recv certificate metadata
        zframe_t *frame = zframe_recv (server);
        assert (frame != NULL);
        const char *meta = zframe_meta (frame, "Hello");
        assert (meta != NULL);
        assert (streq (meta, "World!"));
        zframe_destroy (&frame);
        s_renew_sockets(&server, &client);
    #endif
    
        zcert_destroy (&server_cert);
        zcert_destroy (&client_cert);
    
        // Test custom zcertstore
        zcertstore_t *certstore = zcertstore_new (NULL);
        zcertstore_set_loader (certstore, s_test_loader, NULL, NULL);
        zactor_destroy(&auth);
        auth = zactor_new (zauth, certstore);
        assert (auth);
        if (verbose) {
            zstr_sendx (auth, "VERBOSE", NULL);
            zsock_wait (auth);
        }
    
        byte public_key [32] = { 105, 76, 150, 58, 214, 191, 218, 65, 50, 172,
                                 131, 188, 247, 211, 136, 170, 227, 26, 57, 170,
                                 185, 63, 246, 225, 177, 230, 12, 8, 134, 136,
                                 105, 106 };
        byte secret_key [32] = { 245, 217, 172, 73, 106, 28, 195, 17, 218, 132,
                                 135, 209, 99, 240, 98, 232, 7, 137, 244, 100,
                                 242, 23, 29, 114, 70, 223, 83, 1, 113, 207,
                                 132, 149 };
        zcert_t *shared_cert = zcert_new_from (public_key, secret_key);
        assert (shared_cert);
        zcert_apply (shared_cert, server);
        zcert_apply (shared_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, "x?T*N/1Y{8goubv{Ts}#&#f}TXJ//DVe#D2HkoLU");
        success = s_can_connect (&server, &client, true);
        assert (success);
        zcert_destroy (&shared_cert);
    }
    //  Remove the authenticator and check a normal connection works
    zactor_destroy (&auth);
    success = s_can_connect (&server, &client, true);
    assert (success);
    
    zsock_destroy (&client);
    zsock_destroy (&server);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
```

<A name="toc4-582" title="zbeacon - LAN discovery and presence" />
#### zbeacon - LAN discovery and presence

The zbeacon class implements a peer-to-peer discovery service for local
networks. A beacon can broadcast and/or capture service announcements
using UDP messages on the local area network. This implementation uses
IPv4 UDP broadcasts. You can define the format of your outgoing beacons,
and set a filter that validates incoming beacons. Beacons are sent and
received asynchronously in the background.

This class replaces zbeacon_v2, and is meant for applications that use
the CZMQ v3 API (meaning, zsock).

This is the class interface:

```h
    //  Create new zbeacon actor instance:
    //
    //      zactor_t *beacon = zactor_new (zbeacon, NULL);
    //
    //  Destroy zbeacon instance:
    //
    //      zactor_destroy (&beacon);
    //
    //  Enable verbose logging of commands and activity:
    //
    //      zstr_send (beacon, "VERBOSE");
    //
    //  Configure beacon to run on specified UDP port, and return the name of
    //  the host, which can be used as endpoint for incoming connections. To
    //  force the beacon to operate on a given interface, set the environment
    //  variable ZSYS_INTERFACE, or call zsys_set_interface() before creating
    //  the beacon. If the system does not support UDP broadcasts (lacking a
    //  workable interface), returns an empty hostname:
    //
    //      //  Pictures: 's' = C string, 'i' = int
    //      zsock_send (beacon, "si", "CONFIGURE", port_number);
    //      char *hostname = zstr_recv (beacon);
    //
    //  Start broadcasting a beacon at a specified interval in msec. The beacon
    //  data can be at most UDP_FRAME_MAX bytes; this constant is defined in
    //  zsys.h to be 255:
    //
    //      //  Pictures: 'b' = byte * data + size_t size
    //      zsock_send (beacon, "sbi", "PUBLISH", data, size, interval);
    //
    //  Stop broadcasting the beacon:
    //
    //      zstr_sendx (beacon, "SILENCE", NULL);
    //
    //  Start listening to beacons from peers. The filter is used to do a prefix
    //  match on received beacons, to remove junk. Note that any received data
    //  that is identical to our broadcast beacon_data is discarded in any case.
    //  If the filter size is zero, we get all peer beacons:
    //  
    //      zsock_send (beacon, "sb", "SUBSCRIBE", filter_data, filter_size);
    //
    //  Stop listening to other peers
    //
    //      zstr_sendx (beacon, "UNSUBSCRIBE", NULL);
    //
    //  Receive next beacon from a peer. Received beacons are always a 2-frame
    //  message containing the ipaddress of the sender, and then the binary
    //  beacon data as published by the sender:
    //
    //      zmsg_t *msg = zmsg_recv (beacon);
    //
    //  This is the zbeacon constructor as a zactor_fn:
    CZMQ_EXPORT void
        zbeacon (zsock_t *pipe, void *unused);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zbeacon_test (bool verbose);
```

This is the class self test code:

```c
    //  Test 1 - two beacons, one speaking, one listening
    //  Create speaker beacon to broadcast our service
    zactor_t *speaker = zactor_new (zbeacon, NULL);
    assert (speaker);
    if (verbose)
        zstr_sendx (speaker, "VERBOSE", NULL);
    
    zsock_send (speaker, "si", "CONFIGURE", 9999);
    char *hostname = zstr_recv (speaker);
    if (!*hostname) {
        printf ("OK (skipping test, no UDP broadcasting)\n");
        zactor_destroy (&speaker);
        free (hostname);
        return;
    }
    free (hostname);
    
    //  Create listener beacon on port 9999 to lookup service
    zactor_t *listener = zactor_new (zbeacon, NULL);
    assert (listener);
    if (verbose)
        zstr_sendx (listener, "VERBOSE", NULL);
    zsock_send (listener, "si", "CONFIGURE", 9999);
    hostname = zstr_recv (listener);
    assert (*hostname);
    free (hostname);
    
    //  We will broadcast the magic value 0xCAFE
    byte announcement [2] = { 0xCA, 0xFE };
    zsock_send (speaker, "sbi", "PUBLISH", announcement, 2, 100);
    //  We will listen to anything (empty subscription)
    zsock_send (listener, "sb", "SUBSCRIBE", "", 0);
    
    //  Wait for at most 1/2 second if there's no broadcasting
    zsock_set_rcvtimeo (listener, 500);
    char *ipaddress = zstr_recv (listener);
    if (ipaddress) {
        zframe_t *content = zframe_recv (listener);
        assert (zframe_size (content) == 2);
        assert (zframe_data (content) [0] == 0xCA);
        assert (zframe_data (content) [1] == 0xFE);
        zframe_destroy (&content);
        zstr_free (&ipaddress);
        zstr_sendx (speaker, "SILENCE", NULL);
    }
    zactor_destroy (&listener);
    zactor_destroy (&speaker);
    
    //  Test subscription filter using a 3-node setup
    zactor_t *node1 = zactor_new (zbeacon, NULL);
    assert (node1);
    zsock_send (node1, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node1);
    assert (*hostname);
    free (hostname);
    
    zactor_t *node2 = zactor_new (zbeacon, NULL);
    assert (node2);
    zsock_send (node2, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node2);
    assert (*hostname);
    free (hostname);
    
    zactor_t *node3 = zactor_new (zbeacon, NULL);
    assert (node3);
    zsock_send (node3, "si", "CONFIGURE", 5670);
    hostname = zstr_recv (node3);
    assert (*hostname);
    free (hostname);
    
    zsock_send (node1, "sbi", "PUBLISH", "NODE/1", 6, 250);
    zsock_send (node2, "sbi", "PUBLISH", "NODE/2", 6, 250);
    zsock_send (node3, "sbi", "PUBLISH", "RANDOM", 6, 250);
    zsock_send (node1, "sb", "SUBSCRIBE", "NODE", 4);
    
    //  Poll on three API sockets at once
    zpoller_t *poller = zpoller_new (node1, node2, node3, NULL);
    assert (poller);
    int64_t stop_at = zclock_mono () + 1000;
    while (zclock_mono () < stop_at) {
        long timeout = (long) (stop_at - zclock_mono ());
        if (timeout < 0)
            timeout = 0;
        void *which = zpoller_wait (poller, timeout * ZMQ_POLL_MSEC);
        if (which) {
            assert (which == node1);
            char *ipaddress, *received;
            zstr_recvx (node1, &ipaddress, &received, NULL);
            assert (streq (received, "NODE/2"));
            zstr_free (&ipaddress);
            zstr_free (&received);
        }
    }
    zpoller_destroy (&poller);
    
    //  Stop listening
    zstr_sendx (node1, "UNSUBSCRIBE", NULL);
    
    //  Stop all node broadcasts
    zstr_sendx (node1, "SILENCE", NULL);
    zstr_sendx (node2, "SILENCE", NULL);
    zstr_sendx (node3, "SILENCE", NULL);
    
    //  Destroy the test nodes
    zactor_destroy (&node1);
    zactor_destroy (&node2);
    zactor_destroy (&node3);
```

<A name="toc4-770" title="zcert - work with CURVE security certificates" />
#### zcert - work with CURVE security certificates

The zcert class provides a way to create and work with security
certificates for the ZMQ CURVE mechanism. A certificate contains a
public + secret key pair, plus metadata. It can be used as a
temporary object in memory, or persisted to disk. On disk, a
certificate is stored as two files. One is public and contains only
the public key. The second is secret and contains both keys. The
two have the same filename, with the secret file adding "_secret".
To exchange certificates, send the public file via some secure route.
Certificates are not signed but are text files that can be verified by
eye.

Certificates are stored in the ZPL (ZMQ RFC 4) format. They have two
sections, "metadata" and "curve". The first contains a list of 'name =
value' pairs, one per line. Values may be enclosed in quotes. The curve
section has a 'public-key = keyvalue' and, for secret certificates, a
'secret-key = keyvalue' line. The keyvalue is a Z85-encoded CURVE key.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    //  Create and initialize a new certificate in memory
    CZMQ_EXPORT zcert_t *
        zcert_new (void);
    
    //  Accepts public/secret key pair from caller
    CZMQ_EXPORT zcert_t *
        zcert_new_from (const byte *public_key, const byte *secret_key);
    
    //  Load certificate from file
    CZMQ_EXPORT zcert_t *
        zcert_load (const char *filename);
    
    //  Destroy a certificate in memory
    CZMQ_EXPORT void
        zcert_destroy (zcert_t **self_p);
    
    //  Return public part of key pair as 32-byte binary string
    CZMQ_EXPORT const byte *
        zcert_public_key (zcert_t *self);
    
    //  Return secret part of key pair as 32-byte binary string
    CZMQ_EXPORT const byte *
        zcert_secret_key (zcert_t *self);
    
    //  Return public part of key pair as Z85 armored string
    CZMQ_EXPORT const char *
        zcert_public_txt (zcert_t *self);
    
    //  Return secret part of key pair as Z85 armored string
    CZMQ_EXPORT const char *
        zcert_secret_txt (zcert_t *self);
    
    //  Set certificate metadata from formatted string.
    CZMQ_EXPORT void
        zcert_set_meta (zcert_t *self, const char *name, const char *format, ...) CHECK_PRINTF (3);
    
    //  Get metadata value from certificate; if the metadata value doesn't
    //  exist, returns NULL.                                              
    CZMQ_EXPORT const char *
        zcert_meta (zcert_t *self, const char *name);
    
    //  Get list of metadata fields from certificate. Caller is responsible for
    //  destroying list. Caller should not modify the values of list items.    
    CZMQ_EXPORT zlist_t *
        zcert_meta_keys (zcert_t *self);
    
    //  Save full certificate (public + secret) to file for persistent storage  
    //  This creates one public file and one secret file (filename + "_secret").
    CZMQ_EXPORT int
        zcert_save (zcert_t *self, const char *filename);
    
    //  Save public certificate only to file for persistent storage
    CZMQ_EXPORT int
        zcert_save_public (zcert_t *self, const char *filename);
    
    //  Save secret certificate only to file for persistent storage
    CZMQ_EXPORT int
        zcert_save_secret (zcert_t *self, const char *filename);
    
    //  Apply certificate to socket, i.e. use for CURVE security on socket.
    //  If certificate was loaded from public file, the secret key will be 
    //  undefined, and this certificate will not work successfully.        
    CZMQ_EXPORT void
        zcert_apply (zcert_t *self, void *socket);
    
    //  Return copy of certificate; if certificate is NULL or we exhausted
    //  heap memory, returns NULL.                                        
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zcert_t *
        zcert_dup (zcert_t *self);
    
    //  Return true if two certificates have the same keys
    CZMQ_EXPORT bool
        zcert_eq (zcert_t *self, zcert_t *compare);
    
    //  Print certificate contents to stdout
    CZMQ_EXPORT void
        zcert_print (zcert_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zcert_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  Unset certificate metadata.
    CZMQ_EXPORT void
        zcert_unset_meta (zcert_t *self, const char *name);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Create temporary directory for test files
    #   define TESTDIR ".test_zcert"
    zsys_dir_create (TESTDIR);
    
    //  Create a simple certificate with metadata
    zcert_t *cert = zcert_new ();
    assert (cert);
    zcert_set_meta (cert, "email", "ph@imatix.com");
    zcert_set_meta (cert, "name", "Pieter Hintjens");
    zcert_set_meta (cert, "organization", "iMatix Corporation");
    zcert_set_meta (cert, "version", "%d", 1);
    zcert_set_meta (cert, "delete_me", "now");
    zcert_unset_meta (cert, "delete_me");
    assert (streq (zcert_meta (cert, "email"), "ph@imatix.com"));
    zlist_t *keys = zcert_meta_keys (cert);
    assert (zlist_size (keys) == 4);
    zlist_destroy (&keys);
    
    //  Check the dup and eq methods
    zcert_t *shadow = zcert_dup (cert);
    assert (zcert_eq (cert, shadow));
    zcert_destroy (&shadow);
    
    //  Check we can save and load certificate
    zcert_save (cert, TESTDIR "/mycert.txt");
    assert (zsys_file_exists (TESTDIR "/mycert.txt"));
    assert (zsys_file_exists (TESTDIR "/mycert.txt_secret"));
    
    //  Load certificate, will in fact load secret one
    shadow = zcert_load (TESTDIR "/mycert.txt");
    assert (shadow);
    assert (zcert_eq (cert, shadow));
    zcert_destroy (&shadow);
    
    //  Delete secret certificate, load public one
    int rc = zsys_file_delete (TESTDIR "/mycert.txt_secret");
    assert (rc == 0);
    shadow = zcert_load (TESTDIR "/mycert.txt");
    
    //  32-byte null key encodes as 40 '0' characters
    assert (streq (zcert_secret_txt (shadow), FORTY_ZEROES));
    
    zcert_destroy (&shadow);
    zcert_destroy (&cert);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
```

<A name="toc4-944" title="zcertstore - work with CURVE security certificate stores" />
#### zcertstore - work with CURVE security certificate stores

To authenticate new clients using the ZeroMQ CURVE security mechanism,
we have to check that the client's public key matches a key we know and
accept. There are numerous ways to store accepted client public keys.
The mechanism CZMQ implements is "certificates" (plain text files) held
in a "certificate store" (a disk directory). This class works with such
certificate stores, and lets you easily load them from disk, and check
if a given client public key is known or not. The zcert class does the
work of managing a single certificate.

The certificate store can be memory-only, in which case you can load it
yourself by inserting certificate objects one by one, or it can be loaded
from disk, in which case you can add, modify, or remove certificates on
disk at any time, and the store will detect such changes and refresh
itself automatically. In most applications you won't use this class
directly but through the zauth class, which provides a high-level API for
authentication (and manages certificate stores for you). To actually
create certificates on disk, use the zcert class in code, or the
tools/zmakecert.c command line tool, or any text editor. The format of a
certificate file is defined in the zcert man page.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    //  Create a new certificate store from a disk directory, loading and        
    //  indexing all certificates in that location. The directory itself may be  
    //  absent, and created later, or modified at any time. The certificate store
    //  is automatically refreshed on any zcertstore_lookup() call. If the       
    //  location is specified as NULL, creates a pure-memory store, which you    
    //  can work with by inserting certificates at runtime.                      
    CZMQ_EXPORT zcertstore_t *
        zcertstore_new (const char *location);
    
    //  Destroy a certificate store object in memory. Does not affect anything
    //  stored on disk.                                                       
    CZMQ_EXPORT void
        zcertstore_destroy (zcertstore_t **self_p);
    
    //  Look up certificate by public key, returns zcert_t object if found,
    //  else returns NULL. The public key is provided in Z85 text format.  
    CZMQ_EXPORT zcert_t *
        zcertstore_lookup (zcertstore_t *self, const char *public_key);
    
    //  Insert certificate into certificate store in memory. Note that this
    //  does not save the certificate to disk. To do that, use zcert_save()
    //  directly on the certificate. Takes ownership of zcert_t object.    
    CZMQ_EXPORT void
        zcertstore_insert (zcertstore_t *self, zcert_t **cert_p);
    
    //  Print list of certificates in store to logging facility
    CZMQ_EXPORT void
        zcertstore_print (zcertstore_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zcertstore_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    // Loaders retrieve certificates from an arbitrary source.
    typedef void (zcertstore_loader) (
        zcertstore_t *self);
    
    // Destructor for loader state.
    typedef void (zcertstore_destructor) (
        void **self_p);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Override the default disk loader with a custom loader fn.
    CZMQ_EXPORT void
        zcertstore_set_loader (zcertstore_t *self, zcertstore_loader loader, zcertstore_destructor destructor, void *state);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Empty certificate hashtable. This wrapper exists to be friendly to bindings,
    //  which don't usually have access to struct internals.                        
    CZMQ_EXPORT void
        zcertstore_empty (zcertstore_t *self);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Create temporary directory for test files
    #   define TESTDIR ".test_zcertstore"
    zsys_dir_create (TESTDIR);
    
    //  Load certificate store from disk; it will be empty
    zcertstore_t *certstore = zcertstore_new (TESTDIR);
    assert (certstore);
    
    //  Create a single new certificate and save to disk
    zcert_t *cert = zcert_new ();
    assert (cert);
    char *client_key = strdup (zcert_public_txt (cert));
    assert (client_key);
    zcert_set_meta (cert, "name", "John Doe");
    zcert_save (cert, TESTDIR "/mycert.txt");
    zcert_destroy (&cert);
    
    //  Check that certificate store refreshes as expected
    cert = zcertstore_lookup (certstore, client_key);
    assert (cert);
    assert (streq (zcert_meta (cert, "name"), "John Doe"));
    
    //  Test custom loader
    test_loader_state *state = (test_loader_state *) zmalloc (sizeof (test_loader_state));
    state->index = 0;
    zcertstore_set_loader (certstore, s_test_loader, s_test_destructor, (void *)state);
    #if (ZMQ_VERSION_MAJOR >= 4)
    cert = zcertstore_lookup (certstore, client_key);
    assert (cert == NULL);
    cert = zcertstore_lookup (certstore, "abcdefghijklmnopqrstuvwxyzabcdefghijklmn");
    assert (cert);
    #endif
    
    free (client_key);
    
    if (verbose)
        zcertstore_print (certstore);
    zcertstore_destroy (&certstore);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
```

<A name="toc4-1079" title="zchunk - work with memory chunks" />
#### zchunk - work with memory chunks

The zchunk class works with variable sized blobs. Not as efficient as
ZeroMQ's messages but they do less weirdness and so are easier to understand.
The chunk class has methods to read and write chunks from disk.

Please add @discuss section in ../src/zchunk.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  Create a new chunk of the specified size. If you specify the data, it   
    //  is copied into the chunk. If you do not specify the data, the chunk is  
    //  allocated and left empty, and you can then add data using zchunk_append.
    CZMQ_EXPORT zchunk_t *
        zchunk_new (const void *data, size_t size);
    
    //  Destroy a chunk
    CZMQ_EXPORT void
        zchunk_destroy (zchunk_t **self_p);
    
    //  Resizes chunk max_size as requested; chunk_cur size is set to zero
    CZMQ_EXPORT void
        zchunk_resize (zchunk_t *self, size_t size);
    
    //  Return chunk cur size
    CZMQ_EXPORT size_t
        zchunk_size (zchunk_t *self);
    
    //  Return chunk max size
    CZMQ_EXPORT size_t
        zchunk_max_size (zchunk_t *self);
    
    //  Return chunk data
    CZMQ_EXPORT byte *
        zchunk_data (zchunk_t *self);
    
    //  Set chunk data from user-supplied data; truncate if too large. Data may
    //  be null. Returns actual size of chunk                                  
    CZMQ_EXPORT size_t
        zchunk_set (zchunk_t *self, const void *data, size_t size);
    
    //  Fill chunk data from user-supplied octet
    CZMQ_EXPORT size_t
        zchunk_fill (zchunk_t *self, byte filler, size_t size);
    
    //  Append user-supplied data to chunk, return resulting chunk size. If the 
    //  data would exceeded the available space, it is truncated. If you want to
    //  grow the chunk to accommodate new data, use the zchunk_extend method.   
    CZMQ_EXPORT size_t
        zchunk_append (zchunk_t *self, const void *data, size_t size);
    
    //  Append user-supplied data to chunk, return resulting chunk size. If the
    //  data would exceeded the available space, the chunk grows in size.      
    CZMQ_EXPORT size_t
        zchunk_extend (zchunk_t *self, const void *data, size_t size);
    
    //  Copy as much data from 'source' into the chunk as possible; returns the  
    //  new size of chunk. If all data from 'source' is used, returns exhausted  
    //  on the source chunk. Source can be consumed as many times as needed until
    //  it is exhausted. If source was already exhausted, does not change chunk. 
    CZMQ_EXPORT size_t
        zchunk_consume (zchunk_t *self, zchunk_t *source);
    
    //  Returns true if the chunk was exhausted by consume methods, or if the
    //  chunk has a size of zero.                                            
    CZMQ_EXPORT bool
        zchunk_exhausted (zchunk_t *self);
    
    //  Read chunk from an open file descriptor
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zchunk_t *
        zchunk_read (FILE *handle, size_t bytes);
    
    //  Write chunk to an open file descriptor
    CZMQ_EXPORT int
        zchunk_write (zchunk_t *self, FILE *handle);
    
    //  Try to slurp an entire file into a chunk. Will read up to maxsize of  
    //  the file. If maxsize is 0, will attempt to read the entire file and   
    //  fail with an assertion if that cannot fit into memory. Returns a new  
    //  chunk containing the file data, or NULL if the file could not be read.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zchunk_t *
        zchunk_slurp (const char *filename, size_t maxsize);
    
    //  Create copy of chunk, as new chunk object. Returns a fresh zchunk_t   
    //  object, or null if there was not enough heap memory. If chunk is null,
    //  returns null.                                                         
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zchunk_t *
        zchunk_dup (zchunk_t *self);
    
    //  Return chunk data encoded as printable hex string. Caller must free
    //  string when finished with it.                                      
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zchunk_strhex (zchunk_t *self);
    
    //  Return chunk data copied into freshly allocated string
    //  Caller must free string when finished with it.        
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zchunk_strdup (zchunk_t *self);
    
    //  Return TRUE if chunk body is equal to string, excluding terminator
    CZMQ_EXPORT bool
        zchunk_streq (zchunk_t *self, const char *string);
    
    //  Transform zchunk into a zframe that can be sent in a message.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zframe_t *
        zchunk_pack (zchunk_t *self);
    
    //  Transform a zframe into a zchunk.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zchunk_t *
        zchunk_unpack (zframe_t *frame);
    
    //  Calculate SHA1 digest for chunk, using zdigest class.
    CZMQ_EXPORT const char *
        zchunk_digest (zchunk_t *self);
    
    //  Dump chunk to FILE stream, for debugging and tracing.
    CZMQ_EXPORT void
        zchunk_fprint (zchunk_t *self, FILE *file);
    
    //  Dump message to stderr, for debugging and tracing.
    //  See zchunk_fprint for details                     
    CZMQ_EXPORT void
        zchunk_print (zchunk_t *self);
    
    //  Probe the supplied object, and report if it looks like a zchunk_t.
    CZMQ_EXPORT bool
        zchunk_is (void *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zchunk_test (bool verbose);
    
```

This is the class self test code:

```c
    zchunk_t *chunk = zchunk_new ("1234567890", 10);
    assert (chunk);
    assert (zchunk_size (chunk) == 10);
    assert (memcmp (zchunk_data (chunk), "1234567890", 10) == 0);
    zchunk_destroy (&chunk);
    
    chunk = zchunk_new (NULL, 10);
    assert (chunk);
    zchunk_append (chunk, "12345678", 8);
    zchunk_append (chunk, "90ABCDEF", 8);
    zchunk_append (chunk, "GHIJKLMN", 8);
    assert (memcmp (zchunk_data (chunk), "1234567890", 10) == 0);
    assert (zchunk_size (chunk) == 10);
    assert (zchunk_streq (chunk, "1234567890"));
    assert (streq (zchunk_digest (chunk), "01B307ACBA4F54F55AAFC33BB06BBBF6CA803E9A"));
    char *string = zchunk_strdup (chunk);
    assert (streq (string, "1234567890"));
    free (string);
    string = zchunk_strhex (chunk);
    assert (streq (string, "31323334353637383930"));
    free (string);
    
    zframe_t *frame = zchunk_pack (chunk);
    assert (frame);
    
    zchunk_t *chunk2 = zchunk_unpack (frame);
    assert (chunk2);
    assert (memcmp (zchunk_data (chunk2), "1234567890", 10) == 0);
    zframe_destroy (&frame);
    zchunk_destroy (&chunk2);
    
    zchunk_t *copy = zchunk_dup (chunk);
    assert (copy);
    assert (memcmp (zchunk_data (copy), "1234567890", 10) == 0);
    assert (zchunk_size (copy) == 10);
    zchunk_destroy (&copy);
    zchunk_destroy (&chunk);
    
    chunk = zchunk_new (NULL, 0);
    zchunk_extend (chunk, "12345678", 8);
    zchunk_extend (chunk, "90ABCDEF", 8);
    zchunk_extend (chunk, "GHIJKLMN", 8);
    assert (zchunk_size (chunk) == 24);
    assert (zchunk_streq (chunk, "1234567890ABCDEFGHIJKLMN"));
    zchunk_destroy (&chunk);
    
    copy = zchunk_new ("1234567890abcdefghij", 20);
    assert (copy);
    chunk = zchunk_new (NULL, 8);
    assert (chunk);
    zchunk_consume (chunk, copy);
    assert (!zchunk_exhausted (copy));
    assert (memcmp (zchunk_data (chunk), "12345678", 8) == 0);
    zchunk_set (chunk, NULL, 0);
    zchunk_consume (chunk, copy);
    assert (!zchunk_exhausted (copy));
    assert (memcmp (zchunk_data (chunk), "90abcdef", 8) == 0);
    zchunk_set (chunk, NULL, 0);
    zchunk_consume (chunk, copy);
    assert (zchunk_exhausted (copy));
    assert (zchunk_size (chunk) == 4);
    assert (memcmp (zchunk_data (chunk), "ghij", 4) == 0);
    zchunk_destroy (&copy);
    zchunk_destroy (&chunk);
```

<A name="toc4-1293" title="zclock - millisecond clocks and delays" />
#### zclock - millisecond clocks and delays

The zclock class provides essential sleep and system time functions,
used to slow down threads for testing, and calculate timers for polling.
Wraps the non-portable system calls in a simple portable API.

The Win32 Sleep() call defaults to 16ms resolution unless the system
timer resolution is increased with a call to timeBeginPeriod() permitting
1ms granularity.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  Sleep for a number of milliseconds
    CZMQ_EXPORT void
        zclock_sleep (int msecs);
    
    //  Return current system clock as milliseconds. Note that this clock can  
    //  jump backwards (if the system clock is changed) so is unsafe to use for
    //  timers and time offsets. Use zclock_mono for that instead.             
    CZMQ_EXPORT int64_t
        zclock_time (void);
    
    //  Return current monotonic clock in milliseconds. Use this when you compute
    //  time offsets. The monotonic clock is not affected by system changes and  
    //  so will never be reset backwards, unlike a system clock.                 
    CZMQ_EXPORT int64_t
        zclock_mono (void);
    
    //  Return current monotonic clock in microseconds. Use this when you compute
    //  time offsets. The monotonic clock is not affected by system changes and  
    //  so will never be reset backwards, unlike a system clock.                 
    CZMQ_EXPORT int64_t
        zclock_usecs (void);
    
    //  Return formatted date/time as fresh string. Free using zstr_free().
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zclock_timestr (void);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zclock_test (bool verbose);
    
```

This is the class self test code:

```c
    int64_t start = zclock_time ();
    zclock_sleep (10);
    assert ((zclock_time () - start) >= 10);
    start = zclock_mono ();
    int64_t usecs = zclock_usecs ();
    zclock_sleep (10);
    assert ((zclock_mono () - start) >= 10);
    assert ((zclock_usecs () - usecs) >= 10000);
    char *timestr = zclock_timestr ();
    if (verbose)
        puts (timestr);
    free (timestr);
```

<A name="toc4-1359" title="zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL." />
#### zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.

Lets applications load, work with, and save configuration files.
This implements rfc.zeromq.org/spec:4/ZPL, which is a simple structured
text format for configuration files.

Here is an example ZPL stream and corresponding config structure:

    context
        iothreads = 1
        verbose = 1      #   Ask for a trace
    main
        type = zqueue    #  ZMQ_DEVICE type
        frontend
            option
                hwm = 1000
                swap = 25000000     #  25MB
            bind = 'inproc://addr1'
            bind = 'ipc://addr2'
        backend
            bind = inproc://addr3

    root                    Down = child
    |                     Across = next
    v
    context-->main
    |         |
    |         v
    |       type=queue-->frontend-->backend
    |                      |          |
    |                      |          v
    |                      |        bind=inproc://addr3
    |                      v
    |                    option-->bind=inproc://addr1-->bind=ipc://addr2
    |                      |
    |                      v
    |                    hwm=1000-->swap=25000000
    v
    iothreads=1-->verbose=false

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    // 
    typedef int (zconfig_fct) (
        zconfig_t *self, void *arg, int level);
    
    //  Create new config item
    CZMQ_EXPORT zconfig_t *
        zconfig_new (const char *name, zconfig_t *parent);
    
    //  Load a config tree from a specified ZPL text file; returns a zconfig_t  
    //  reference for the root, if the file exists and is readable. Returns NULL
    //  if the file does not exist.                                             
    CZMQ_EXPORT zconfig_t *
        zconfig_load (const char *filename);
    
    //  Equivalent to zconfig_load, taking a format string instead of a fixed
    //  filename.                                                            
    CZMQ_EXPORT zconfig_t *
        zconfig_loadf (const char *format, ...) CHECK_PRINTF (1);
    
    //  Destroy a config item and all its children
    CZMQ_EXPORT void
        zconfig_destroy (zconfig_t **self_p);
    
    //  Return name of config item
    CZMQ_EXPORT char *
        zconfig_name (zconfig_t *self);
    
    //  Return value of config item
    CZMQ_EXPORT char *
        zconfig_value (zconfig_t *self);
    
    //  Insert or update configuration key with value
    CZMQ_EXPORT void
        zconfig_put (zconfig_t *self, const char *path, const char *value);
    
    //  Equivalent to zconfig_put, accepting a format specifier and variable
    //  argument list, instead of a single string value.                    
    CZMQ_EXPORT void
        zconfig_putf (zconfig_t *self, const char *path, const char *format, ...) CHECK_PRINTF (3);
    
    //  Get value for config item into a string value; leading slash is optional
    //  and ignored.                                                            
    CZMQ_EXPORT char *
        zconfig_get (zconfig_t *self, const char *path, const char *default_value);
    
    //  Set config item name, name may be NULL
    CZMQ_EXPORT void
        zconfig_set_name (zconfig_t *self, const char *name);
    
    //  Set new value for config item. The new value may be a string, a printf  
    //  format, or NULL. Note that if string may possibly contain '%', or if it 
    //  comes from an insecure source, you must use '%s' as the format, followed
    //  by the string.                                                          
    CZMQ_EXPORT void
        zconfig_set_value (zconfig_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Find our first child, if any
    CZMQ_EXPORT zconfig_t *
        zconfig_child (zconfig_t *self);
    
    //  Find our first sibling, if any
    CZMQ_EXPORT zconfig_t *
        zconfig_next (zconfig_t *self);
    
    //  Find a config item along a path; leading slash is optional and ignored.
    CZMQ_EXPORT zconfig_t *
        zconfig_locate (zconfig_t *self, const char *path);
    
    //  Locate the last config item at a specified depth
    CZMQ_EXPORT zconfig_t *
        zconfig_at_depth (zconfig_t *self, int level);
    
    //  Execute a callback for each config item in the tree; returns zero if
    //  successful, else -1.                                                
    CZMQ_EXPORT int
        zconfig_execute (zconfig_t *self, zconfig_fct handler, void *arg);
    
    //  Add comment to config item before saving to disk. You can add as many
    //  comment lines as you like. If you use a null format, all comments are
    //  deleted.                                                             
    CZMQ_EXPORT void
        zconfig_set_comment (zconfig_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Return comments of config item, as zlist.
    CZMQ_EXPORT zlist_t *
        zconfig_comments (zconfig_t *self);
    
    //  Save a config tree to a specified ZPL text file, where a filename
    //  "-" means dump to standard output.                               
    CZMQ_EXPORT int
        zconfig_save (zconfig_t *self, const char *filename);
    
    //  Equivalent to zconfig_save, taking a format string instead of a fixed
    //  filename.                                                            
    CZMQ_EXPORT int
        zconfig_savef (zconfig_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Report filename used during zconfig_load, or NULL if none
    CZMQ_EXPORT const char *
        zconfig_filename (zconfig_t *self);
    
    //  Reload config tree from same file that it was previously loaded from.
    //  Returns 0 if OK, -1 if there was an error (and then does not change  
    //  existing data).                                                      
    CZMQ_EXPORT int
        zconfig_reload (zconfig_t **self_p);
    
    //  Load a config tree from a memory chunk
    CZMQ_EXPORT zconfig_t *
        zconfig_chunk_load (zchunk_t *chunk);
    
    //  Save a config tree to a new memory chunk
    CZMQ_EXPORT zchunk_t *
        zconfig_chunk_save (zconfig_t *self);
    
    //  Load a config tree from a null-terminated string
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zconfig_t *
        zconfig_str_load (const char *string);
    
    //  Save a config tree to a new null terminated string
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zconfig_str_save (zconfig_t *self);
    
    //  Return true if a configuration tree was loaded from a file and that
    //  file has changed in since the tree was loaded.                     
    CZMQ_EXPORT bool
        zconfig_has_changed (zconfig_t *self);
    
    //  Print the config file to open stream
    CZMQ_EXPORT void
        zconfig_fprint (zconfig_t *self, FILE *file);
    
    //  Print properties of object
    CZMQ_EXPORT void
        zconfig_print (zconfig_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zconfig_test (bool verbose);
    
```

This is the class self test code:

```c
    //  Create temporary directory for test files
    #   define TESTDIR ".test_zconfig"
    zsys_dir_create (TESTDIR);
    
    zconfig_t *root = zconfig_new ("root", NULL);
    assert (root);
    zconfig_t *section, *item;
    
    section = zconfig_new ("headers", root);
    assert (section);
    item = zconfig_new ("email", section);
    assert (item);
    zconfig_set_value (item, "some@random.com");
    item = zconfig_new ("name", section);
    assert (item);
    zconfig_set_value (item, "Justin Kayce");
    zconfig_putf (root, "/curve/secret-key", "%s", "Top Secret");
    zconfig_set_comment (root, "   CURVE certificate");
    zconfig_set_comment (root, "   -----------------");
    assert (zconfig_comments (root));
    zconfig_save (root, TESTDIR "/test.cfg");
    zconfig_destroy (&root);
    root = zconfig_load (TESTDIR "/test.cfg");
    if (verbose)
        zconfig_save (root, "-");
    assert (streq (zconfig_filename (root), TESTDIR "/test.cfg"));
    
    char *email = zconfig_get (root, "/headers/email", NULL);
    assert (email);
    assert (streq (email, "some@random.com"));
    char *passwd = zconfig_get (root, "/curve/secret-key", NULL);
    assert (passwd);
    assert (streq (passwd, "Top Secret"));
    
    zconfig_savef (root, "%s/%s", TESTDIR, "test.cfg");
    assert (!zconfig_has_changed (root));
    int rc = zconfig_reload (&root);
    assert (rc == 0);
    assert (!zconfig_has_changed (root));
    zconfig_destroy (&root);
    
    //  Test chunk load/save
    root = zconfig_new ("root", NULL);
    assert (root);
    section = zconfig_new ("section", root);
    assert (section);
    item = zconfig_new ("value", section);
    assert (item);
    zconfig_set_value (item, "somevalue");
    zconfig_t *search = zconfig_locate (root, "section/value");
    assert (search == item);
    zchunk_t *chunk = zconfig_chunk_save (root);
    assert (strlen ((char *) zchunk_data (chunk)) == 32);
    char *string = zconfig_str_save (root);
    assert (string);
    assert (streq (string, (char *) zchunk_data (chunk)));
    free (string);
    assert (chunk);
    zconfig_destroy (&root);
    
    root = zconfig_chunk_load (chunk);
    assert (root);
    char *value = zconfig_get (root, "/section/value", NULL);
    assert (value);
    assert (streq (value, "somevalue"));
    
    //  Test config can't be saved to a file in a path that doesn't
    //  exist or isn't writable
    rc = zconfig_savef (root, "%s/path/that/doesnt/exist/%s", TESTDIR, "test.cfg");
    assert (rc == -1);
    
    zconfig_destroy (&root);
    zchunk_destroy (&chunk);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);
```

<A name="toc4-1633" title="zdigest - provides hashing functions (SHA-1 at present)" />
#### zdigest - provides hashing functions (SHA-1 at present)

The zdigest class generates a hash from zchunks of data. The current
algorithm is SHA-1, chosen for speed. We are aiming to generate a
unique digest for a file, and there are no security issues in this
use case.

The current code depends on OpenSSL, which might be replaced by hard
coded SHA-1 implementation to reduce build dependencies.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  Constructor - creates new digest object, which you use to build up a
    //  digest by repeatedly calling zdigest_update() on chunks of data.    
    CZMQ_EXPORT zdigest_t *
        zdigest_new (void);
    
    //  Destroy a digest object
    CZMQ_EXPORT void
        zdigest_destroy (zdigest_t **self_p);
    
    //  Add buffer into digest calculation
    CZMQ_EXPORT void
        zdigest_update (zdigest_t *self, const byte *buffer, size_t length);
    
    //  Return final digest hash data. If built without crypto support,
    //  returns NULL.                                                  
    CZMQ_EXPORT const byte *
        zdigest_data (zdigest_t *self);
    
    //  Return final digest hash size
    CZMQ_EXPORT size_t
        zdigest_size (zdigest_t *self);
    
    //  Return digest as printable hex string; caller should not modify nor   
    //  free this string. After calling this, you may not use zdigest_update()
    //  on the same digest. If built without crypto support, returns NULL.    
    CZMQ_EXPORT char *
        zdigest_string (zdigest_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zdigest_test (bool verbose);
    
```

This is the class self test code:

```c
    byte *buffer = (byte *) zmalloc (1024);
    memset (buffer, 0xAA, 1024);
    
    zdigest_t *digest = zdigest_new ();
    assert (digest);
    zdigest_update (digest, buffer, 1024);
    const byte *data = zdigest_data (digest);
    assert (data [0] == 0xDE);
    assert (data [1] == 0xB2);
    assert (data [2] == 0x38);
    assert (data [3] == 0x07);
    assert (streq (zdigest_string (digest),
                   "DEB23807D4FE025E900FE9A9C7D8410C3DDE9671"));
    zdigest_destroy (&digest);
    free (buffer);
```

<A name="toc4-1703" title="zdir - work with file-system directories" />
#### zdir - work with file-system directories

The zdir class gives access to the file system index. It will load
a directory tree (a directory plus all child directories) into a
zdir structure and then let you navigate that structure. It exists
mainly to wrap non-portable OS functions to do this.

Please add @discuss section in ../src/zdir.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  Create a new directory item that loads in the full tree of the specified
    //  path, optionally located under some parent path. If parent is "-", then 
    //  loads only the top-level directory, and does not use parent as a path.  
    CZMQ_EXPORT zdir_t *
        zdir_new (const char *path, const char *parent);
    
    //  Destroy a directory tree and all children it contains.
    CZMQ_EXPORT void
        zdir_destroy (zdir_t **self_p);
    
    //  Return directory path
    CZMQ_EXPORT const char *
        zdir_path (zdir_t *self);
    
    //  Return last modification time for directory.
    CZMQ_EXPORT time_t
        zdir_modified (zdir_t *self);
    
    //  Return total hierarchy size, in bytes of data contained in all files
    //  in the directory tree.                                              
    CZMQ_EXPORT off_t
        zdir_cursize (zdir_t *self);
    
    //  Return directory count
    CZMQ_EXPORT size_t
        zdir_count (zdir_t *self);
    
    //  Returns a sorted list of zfile objects; Each entry in the list is a pointer
    //  to a zfile_t item already allocated in the zdir tree. Do not destroy the   
    //  original zdir tree until you are done with this list.                      
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zlist_t *
        zdir_list (zdir_t *self);
    
    //  Remove directory, optionally including all files that it contains, at  
    //  all levels. If force is false, will only remove the directory if empty.
    //  If force is true, will remove all files and all subdirectories.        
    CZMQ_EXPORT void
        zdir_remove (zdir_t *self, bool force);
    
    //  Calculate differences between two versions of a directory tree.    
    //  Returns a list of zdir_patch_t patches. Either older or newer may  
    //  be null, indicating the directory is empty/absent. If alias is set,
    //  generates virtual filename (minus path, plus alias).               
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zlist_t *
        zdir_diff (zdir_t *older, zdir_t *newer, const char *alias);
    
    //  Return full contents of directory as a zdir_patch list.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zlist_t *
        zdir_resync (zdir_t *self, const char *alias);
    
    //  Load directory cache; returns a hash table containing the SHA-1 digests
    //  of every file in the tree. The cache is saved between runs in .cache.  
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zhash_t *
        zdir_cache (zdir_t *self);
    
    //  Print contents of directory to open stream
    CZMQ_EXPORT void
        zdir_fprint (zdir_t *self, FILE *file, int indent);
    
    //  Print contents of directory to stdout
    CZMQ_EXPORT void
        zdir_print (zdir_t *self, int indent);
    
    //  Create a new zdir_watch actor instance:                       
    //                                                                
    //      zactor_t *watch = zactor_new (zdir_watch, NULL);          
    //                                                                
    //  Destroy zdir_watch instance:                                  
    //                                                                
    //      zactor_destroy (&watch);                                  
    //                                                                
    //  Enable verbose logging of commands and activity:              
    //                                                                
    //      zstr_send (watch, "VERBOSE");                             
    //                                                                
    //  Subscribe to changes to a directory path:                     
    //                                                                
    //      zsock_send (watch, "ss", "SUBSCRIBE", "directory_path");  
    //                                                                
    //  Unsubscribe from changes to a directory path:                 
    //                                                                
    //      zsock_send (watch, "ss", "UNSUBSCRIBE", "directory_path");
    //                                                                
    //  Receive directory changes:                                    
    //      zsock_recv (watch, "sp", &path, &patches);                
    //                                                                
    //      // Delete the received data.                              
    //      free (path);                                              
    //      zlist_destroy (&patches);                                 
    CZMQ_EXPORT void
        zdir_watch (zsock_t *pipe, void *unused);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zdir_test (bool verbose);
    
```

This is the class self test code:

```c
    // need to create a file in the test directory we're watching
    // in order to ensure the directory exists
    zfile_t *initfile = zfile_new ("./zdir-test-dir", "initial_file");
    assert (initfile);
    zfile_output (initfile);
    fprintf (zfile_handle (initfile), "initial file\n");
    zfile_close (initfile);
    zfile_destroy (&initfile);
    
    zdir_t *older = zdir_new ("zdir-test-dir", NULL);
    assert (older);
    if (verbose) {
        printf ("\n");
        zdir_dump (older, 0);
    }
    zdir_t *newer = zdir_new (".", NULL);
    assert (newer);
    zlist_t *patches = zdir_diff (older, newer, "/");
    assert (patches);
    while (zlist_size (patches)) {
        zdir_patch_t *patch = (zdir_patch_t *) zlist_pop (patches);
        zdir_patch_destroy (&patch);
    }
    zlist_destroy (&patches);
    zdir_destroy (&older);
    zdir_destroy (&newer);
    
    zdir_t *nosuch = zdir_new ("does-not-exist", NULL);
    assert (nosuch == NULL);
    
    // zdir_watch test:
    zactor_t *watch = zactor_new (zdir_watch, NULL);
    assert (watch);
    
    if (verbose) {
        zsock_send (watch, "s", "VERBOSE");
        assert (zsock_wait (watch) == 0);
    }
    
    zclock_sleep (1001); // wait for initial file to become 'stable'
    
    zsock_send (watch, "si", "TIMEOUT", 100);
    assert (zsock_wait (watch) == 0);
    
    zsock_send (watch, "ss", "SUBSCRIBE", "zdir-test-dir");
    assert (zsock_wait (watch) == 0);
    
    zsock_send (watch, "ss", "UNSUBSCRIBE", "zdir-test-dir");
    assert (zsock_wait (watch) == 0);
    
    zsock_send (watch, "ss", "SUBSCRIBE", "zdir-test-dir");
    assert (zsock_wait (watch) == 0);
    
    zfile_t *newfile = zfile_new ("zdir-test-dir", "test_abc");
    zfile_output (newfile);
    fprintf (zfile_handle (newfile), "test file\n");
    zfile_close (newfile);
    
    zpoller_t *watch_poll = zpoller_new (watch, NULL);
    
    // poll for a certain timeout before giving up and failing the test.
    assert (zpoller_wait (watch_poll, 1001) == watch);
    
    // wait for notification of the file being added
    char *path;
    int rc = zsock_recv (watch, "sp", &path, &patches);
    assert (rc == 0);
    
    assert (streq (path, "zdir-test-dir"));
    free (path);
    
    assert (zlist_size (patches) == 1);
    
    zdir_patch_t *patch = (zdir_patch_t *) zlist_pop (patches);
    assert (streq (zdir_patch_path (patch), "zdir-test-dir"));
    
    zfile_t *patch_file = zdir_patch_file (patch);
    assert (streq (zfile_filename (patch_file, ""), "zdir-test-dir/test_abc"));
    
    zdir_patch_destroy (&patch);
    zlist_destroy (&patches);
    
    // remove the file
    zfile_remove (newfile);
    zfile_destroy (&newfile);
    
    // poll for a certain timeout before giving up and failing the test.
    assert (zpoller_wait (watch_poll, 1001) == watch);
    
    // wait for notification of the file being removed
    rc = zsock_recv (watch, "sp", &path, &patches);
    assert (rc == 0);
    
    assert (streq (path, "zdir-test-dir"));
    free (path);
    
    assert (zlist_size (patches) == 1);
    
    patch = (zdir_patch_t *) zlist_pop (patches);
    assert (streq (zdir_patch_path (patch), "zdir-test-dir"));
    
    patch_file = zdir_patch_file (patch);
    assert (streq (zfile_filename (patch_file, ""), "zdir-test-dir/test_abc"));
    
    zdir_patch_destroy (&patch);
    zlist_destroy (&patches);
    
    zpoller_destroy (&watch_poll);
    zactor_destroy (&watch);
    
    // clean up by removing the test directory.
    zdir_t *testdir = zdir_new ("zdir-test-dir", NULL);
    zdir_remove (testdir, true);
    zdir_destroy (&testdir);
```

<A name="toc4-1939" title="zdir_patch - work with directory patches" />
#### zdir_patch - work with directory patches

The zdir_patch class works with one patch, which says "create this
file" or "delete this file" (referring to a zfile item each time).

Please add @discuss section in ../src/zdir_patch.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    #define ZDIR_PATCH_CREATE 1                 // Creates a new file
    #define ZDIR_PATCH_DELETE 2                 // Delete a file
    
    //  Create new patch
    CZMQ_EXPORT zdir_patch_t *
        zdir_patch_new (const char *path, zfile_t *file, int op, const char *alias);
    
    //  Destroy a patch
    CZMQ_EXPORT void
        zdir_patch_destroy (zdir_patch_t **self_p);
    
    //  Create copy of a patch. If the patch is null, or memory was exhausted,
    //  returns null.                                                         
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zdir_patch_t *
        zdir_patch_dup (zdir_patch_t *self);
    
    //  Return patch file directory path
    CZMQ_EXPORT const char *
        zdir_patch_path (zdir_patch_t *self);
    
    //  Return patch file item
    CZMQ_EXPORT zfile_t *
        zdir_patch_file (zdir_patch_t *self);
    
    //  Return operation
    CZMQ_EXPORT int
        zdir_patch_op (zdir_patch_t *self);
    
    //  Return patch virtual file path
    CZMQ_EXPORT const char *
        zdir_patch_vpath (zdir_patch_t *self);
    
    //  Calculate hash digest for file (create only)
    CZMQ_EXPORT void
        zdir_patch_digest_set (zdir_patch_t *self);
    
    //  Return hash digest for patch file
    CZMQ_EXPORT const char *
        zdir_patch_digest (zdir_patch_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zdir_patch_test (bool verbose);
    
```

This is the class self test code:

```c
    zfile_t *file = zfile_new (".", "bilbo");
    assert (file);
    zdir_patch_t *patch = zdir_patch_new (".", file, patch_create, "/");
    assert (patch);
    zfile_destroy (&file);
    
    file = zdir_patch_file (patch);
    assert (file);
    assert (streq (zfile_filename (file, "."), "bilbo"));
    assert (streq (zdir_patch_vpath (patch), "/bilbo"));
    zdir_patch_destroy (&patch);
```

<A name="toc4-2015" title="zfile - provides methods to work with files in a portable fashion." />
#### zfile - provides methods to work with files in a portable fashion.

The zfile class provides methods to work with disk files. A file object
provides the modified date, current size, and type of the file. You can
create a file object for a filename that does not yet exist. To read or
write data from the file, use the input and output methods, and then
read and write chunks. The output method lets you both read and write
chunks, at any offset. Finally, this class provides portable symbolic
links. If a filename ends in ".ln", the first line of text in the file
is read, and used as the underlying file for read/write operations.
This lets you manipulate (e.g.) copy symbolic links without copying
the perhaps very large files they point to.

This class is a new API, deprecating the old zfile class (which still
exists but is implemented in zsys now).

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  If file exists, populates properties. CZMQ supports portable symbolic
    //  links, which are files with the extension ".ln". A symbolic link is a
    //  text file containing one line, the filename of a target file. Reading
    //  data from the symbolic link actually reads from the target file. Path
    //  may be NULL, in which case it is not used.                           
    CZMQ_EXPORT zfile_t *
        zfile_new (const char *path, const char *name);
    
    //  Destroy a file item
    CZMQ_EXPORT void
        zfile_destroy (zfile_t **self_p);
    
    //  Duplicate a file item, returns a newly constructed item. If the file
    //  is null, or memory was exhausted, returns null.                     
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zfile_t *
        zfile_dup (zfile_t *self);
    
    //  Return file name, remove path if provided
    CZMQ_EXPORT const char *
        zfile_filename (zfile_t *self, const char *path);
    
    //  Refresh file properties from disk; this is not done automatically   
    //  on access methods, otherwise it is not possible to compare directory
    //  snapshots.                                                          
    CZMQ_EXPORT void
        zfile_restat (zfile_t *self);
    
    //  Return when the file was last modified. If you want this to reflect the
    //  current situation, call zfile_restat before checking this property.    
    CZMQ_EXPORT time_t
        zfile_modified (zfile_t *self);
    
    //  Return the last-known size of the file. If you want this to reflect the
    //  current situation, call zfile_restat before checking this property.    
    CZMQ_EXPORT off_t
        zfile_cursize (zfile_t *self);
    
    //  Return true if the file is a directory. If you want this to reflect   
    //  any external changes, call zfile_restat before checking this property.
    CZMQ_EXPORT bool
        zfile_is_directory (zfile_t *self);
    
    //  Return true if the file is a regular file. If you want this to reflect
    //  any external changes, call zfile_restat before checking this property.
    CZMQ_EXPORT bool
        zfile_is_regular (zfile_t *self);
    
    //  Return true if the file is readable by this process. If you want this to
    //  reflect any external changes, call zfile_restat before checking this    
    //  property.                                                               
    CZMQ_EXPORT bool
        zfile_is_readable (zfile_t *self);
    
    //  Return true if the file is writeable by this process. If you want this 
    //  to reflect any external changes, call zfile_restat before checking this
    //  property.                                                              
    CZMQ_EXPORT bool
        zfile_is_writeable (zfile_t *self);
    
    //  Check if file has stopped changing and can be safely processed.
    //  Updates the file statistics from disk at every call.           
    CZMQ_EXPORT bool
        zfile_is_stable (zfile_t *self);
    
    //  Return true if the file was changed on disk since the zfile_t object
    //  was created, or the last zfile_restat() call made on it.            
    CZMQ_EXPORT bool
        zfile_has_changed (zfile_t *self);
    
    //  Remove the file from disk
    CZMQ_EXPORT void
        zfile_remove (zfile_t *self);
    
    //  Open file for reading                             
    //  Returns 0 if OK, -1 if not found or not accessible
    CZMQ_EXPORT int
        zfile_input (zfile_t *self);
    
    //  Open file for writing, creating directory if needed               
    //  File is created if necessary; chunks can be written to file at any
    //  location. Returns 0 if OK, -1 if error.                           
    CZMQ_EXPORT int
        zfile_output (zfile_t *self);
    
    //  Read chunk from file at specified position. If this was the last chunk,
    //  sets the eof property. Returns a null chunk in case of error.          
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zchunk_t *
        zfile_read (zfile_t *self, size_t bytes, off_t offset);
    
    //  Returns true if zfile_read() just read the last chunk in the file.
    CZMQ_EXPORT bool
        zfile_eof (zfile_t *self);
    
    //  Write chunk to file at specified position
    //  Return 0 if OK, else -1                  
    CZMQ_EXPORT int
        zfile_write (zfile_t *self, zchunk_t *chunk, off_t offset);
    
    //  Read next line of text from file. Returns a pointer to the text line,
    //  or NULL if there was nothing more to read from the file.             
    CZMQ_EXPORT const char *
        zfile_readln (zfile_t *self);
    
    //  Close file, if open
    CZMQ_EXPORT void
        zfile_close (zfile_t *self);
    
    //  Return file handle, if opened
    CZMQ_EXPORT FILE *
        zfile_handle (zfile_t *self);
    
    //  Calculate SHA1 digest for file, using zdigest class.
    CZMQ_EXPORT const char *
        zfile_digest (zfile_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zfile_test (bool verbose);
    
    //  These methods are deprecated, and now moved to zsys class.
    CZMQ_EXPORT bool
        zfile_exists (const char *filename);
    CZMQ_EXPORT ssize_t
        zfile_size   (const char *filename);
    CZMQ_EXPORT mode_t
        zfile_mode   (const char *filename);
    CZMQ_EXPORT int
        zfile_delete (const char *filename);
    CZMQ_EXPORT bool
        zfile_stable (const char *filename);
    CZMQ_EXPORT int
        zfile_mkdir  (const char *pathname);
    CZMQ_EXPORT int
        zfile_rmdir  (const char *pathname);
    CZMQ_EXPORT void
        zfile_mode_private (void);
    CZMQ_EXPORT void
        zfile_mode_default (void);
```

This is the class self test code:

```c
    zfile_t *file = zfile_new (NULL, "bilbo");
    assert (file);
    assert (streq (zfile_filename (file, "."), "bilbo"));
    assert (zfile_is_readable (file) == false);
    zfile_destroy (&file);
    
    //  Create a test file in some random subdirectory
    file = zfile_new ("./this/is/a/test", "bilbo");
    assert (file);
    int rc = zfile_output (file);
    assert (rc == 0);
    zchunk_t *chunk = zchunk_new (NULL, 100);
    assert (chunk);
    zchunk_fill (chunk, 0, 100);
    
    //  Write 100 bytes at position 1,000,000 in the file
    rc = zfile_write (file, chunk, 1000000);
    assert (rc == 0);
    zchunk_destroy (&chunk);
    zfile_close (file);
    assert (zfile_is_readable (file));
    assert (zfile_cursize (file) == 1000100);
    assert (!zfile_is_stable (file));
    assert (zfile_digest (file));
    
    //  Now truncate file from outside
    int handle = open ("./this/is/a/test/bilbo", O_WRONLY | O_TRUNC | O_BINARY, 0);
    assert (handle >= 0);
    rc = write (handle, "Hello, World\n", 13);
    assert (rc == 13);
    close (handle);
    assert (zfile_has_changed (file));
    zclock_sleep (1001);
    assert (zfile_has_changed (file));
    
    assert (!zfile_is_stable (file));
    zfile_restat (file);
    assert (zfile_is_stable (file));
    assert (streq (zfile_digest (file), "4AB299C8AD6ED14F31923DD94F8B5F5CB89DFB54"));
    
    //  Check we can read from file
    rc = zfile_input (file);
    assert (rc == 0);
    chunk = zfile_read (file, 1000100, 0);
    assert (chunk);
    assert (zchunk_size (chunk) == 13);
    zchunk_destroy (&chunk);
    zfile_close (file);
    
    //  Check we can read lines from file
    rc = zfile_input (file);
    assert (rc == 0);
    const char *line = zfile_readln (file);
    assert (streq (line, "Hello, World"));
    line = zfile_readln (file);
    assert (line == NULL);
    zfile_close (file);
    
    //  Try some fun with symbolic links
    zfile_t *link = zfile_new ("./this/is/a/test", "bilbo.ln");
    assert (link);
    rc = zfile_output (link);
    assert (rc == 0);
    fprintf (zfile_handle (link), "./this/is/a/test/bilbo\n");
    zfile_destroy (&link);
    
    link = zfile_new ("./this/is/a/test", "bilbo.ln");
    assert (link);
    rc = zfile_input (link);
    assert (rc == 0);
    chunk = zfile_read (link, 1000100, 0);
    assert (chunk);
    assert (zchunk_size (chunk) == 13);
    zchunk_destroy (&chunk);
    zfile_destroy (&link);
    
    //  Remove file and directory
    zdir_t *dir = zdir_new ("./this", NULL);
    assert (dir);
    assert (zdir_cursize (dir) == 26);
    zdir_remove (dir, true);
    assert (zdir_cursize (dir) == 0);
    zdir_destroy (&dir);
    
    //  Check we can no longer read from file
    assert (zfile_is_readable (file));
    zfile_restat (file);
    assert (!zfile_is_readable (file));
    rc = zfile_input (file);
    assert (rc == -1);
    zfile_destroy (&file);
    
    file = zfile_new ("./", "eof_checkfile");
    assert (file);
    //  1. Write something first
    rc = zfile_output (file);
    assert (rc == 0);
    chunk = zchunk_new ("123456789", 9);
    assert (chunk);
    
    rc = zfile_write (file, chunk, 0);
    assert (rc == 0);
    zchunk_destroy (&chunk);
    zfile_close (file);
    assert (zfile_cursize (file) == 9);
    
    // 2. Read the written something
    rc = zfile_input (file);
    assert (rc != -1);
    // try to read more bytes than there is in the file
    chunk = zfile_read (file, 1000, 0);
    assert (zfile_eof(file));
    assert (zchunk_streq (chunk, "123456789"));
    zchunk_destroy (&chunk);
    
    // reading is ok
    chunk = zfile_read (file, 5, 0);
    assert (!zfile_eof(file));
    assert (zchunk_streq (chunk, "12345"));
    zchunk_destroy (&chunk);
    
    // read from non zero offset until the end
    chunk = zfile_read (file, 5, 5);
    assert (zfile_eof(file));
    assert (zchunk_streq (chunk, "6789"));
    zchunk_destroy (&chunk);
    zfile_remove (file);
```

<A name="toc4-2311" title="zframe - working with single message frames" />
#### zframe - working with single message frames

The zframe class provides methods to send and receive single message
frames across ØMQ sockets. A 'frame' corresponds to one zmq_msg_t. When
you read a frame from a socket, the zframe_more() method indicates if the
frame is part of an unfinished multipart message. The zframe_send method
normally destroys the frame, but with the ZFRAME_REUSE flag, you can send
the same frame many times. Frames are binary, and this class has no
special support for text data.

Please add @discuss section in ../src/zframe.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    #define ZFRAME_MORE 1                       // 
    #define ZFRAME_REUSE 2                      // 
    #define ZFRAME_DONTWAIT 4                   // 
    
    //  Create a new frame. If size is not null, allocates the frame data
    //  to the specified size. If additionally, data is not null, copies 
    //  size octets from the specified data into the frame body.         
    CZMQ_EXPORT zframe_t *
        zframe_new (const void *data, size_t size);
    
    //  Create an empty (zero-sized) frame
    CZMQ_EXPORT zframe_t *
        zframe_new_empty (void);
    
    //  Create a frame with a specified string content.
    CZMQ_EXPORT zframe_t *
        zframe_from (const char *string);
    
    //  Receive frame from socket, returns zframe_t object or NULL if the recv  
    //  was interrupted. Does a blocking recv, if you want to not block then use
    //  zpoller or zloop.                                                       
    CZMQ_EXPORT zframe_t *
        zframe_recv (void *source);
    
    //  Destroy a frame
    CZMQ_EXPORT void
        zframe_destroy (zframe_t **self_p);
    
    //  Send a frame to a socket, destroy frame after sending.
    //  Return -1 on error, 0 on success.                     
    CZMQ_EXPORT int
        zframe_send (zframe_t **self_p, void *dest, int flags);
    
    //  Return number of bytes in frame data
    CZMQ_EXPORT size_t
        zframe_size (zframe_t *self);
    
    //  Return address of frame data
    CZMQ_EXPORT byte *
        zframe_data (zframe_t *self);
    
    //  Return meta data property for frame           
    //  Caller must free string when finished with it.
    CZMQ_EXPORT const char *
        zframe_meta (zframe_t *self, const char *property);
    
    //  Create a new frame that duplicates an existing frame. If frame is null,
    //  or memory was exhausted, returns null.                                 
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zframe_t *
        zframe_dup (zframe_t *self);
    
    //  Return frame data encoded as printable hex string, useful for ØMQ UUIDs.
    //  Caller must free string when finished with it.                          
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zframe_strhex (zframe_t *self);
    
    //  Return frame data copied into freshly allocated string
    //  Caller must free string when finished with it.        
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zframe_strdup (zframe_t *self);
    
    //  Return TRUE if frame body is equal to string, excluding terminator
    CZMQ_EXPORT bool
        zframe_streq (zframe_t *self, const char *string);
    
    //  Return frame MORE indicator (1 or 0), set when reading frame from socket
    //  or by the zframe_set_more() method                                      
    CZMQ_EXPORT int
        zframe_more (zframe_t *self);
    
    //  Set frame MORE indicator (1 or 0). Note this is NOT used when sending
    //  frame to socket, you have to specify flag explicitly.                
    CZMQ_EXPORT void
        zframe_set_more (zframe_t *self, int more);
    
    //  Return TRUE if two frames have identical size and data
    //  If either frame is NULL, equality is always false.    
    CZMQ_EXPORT bool
        zframe_eq (zframe_t *self, zframe_t *other);
    
    //  Set new contents for frame
    CZMQ_EXPORT void
        zframe_reset (zframe_t *self, const void *data, size_t size);
    
    //  Send message to zsys log sink (may be stdout, or system facility as       
    //  configured by zsys_set_logstream). Prefix shows before frame, if not null.
    CZMQ_EXPORT void
        zframe_print (zframe_t *self, const char *prefix);
    
    //  Probe the supplied object, and report if it looks like a zframe_t.
    CZMQ_EXPORT bool
        zframe_is (void *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zframe_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  Return frame routing ID, if the frame came from a ZMQ_SERVER socket.
    //  Else returns zero.                                                  
    CZMQ_EXPORT uint32_t
        zframe_routing_id (zframe_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Set routing ID on frame. This is used if/when the frame is sent to a
    //  ZMQ_SERVER socket.                                                  
    CZMQ_EXPORT void
        zframe_set_routing_id (zframe_t *self, uint32_t routing_id);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Return frame group of radio-dish pattern.
    CZMQ_EXPORT const char *
        zframe_group (zframe_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Set group on frame. This is used if/when the frame is sent to a
    //  ZMQ_RADIO socket.                                              
    //  Return -1 on error, 0 on success.                              
    CZMQ_EXPORT int
        zframe_set_group (zframe_t *self, const char *group);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new_pair ("@tcp://127.0.0.1:9001");
    assert (output);
    zsock_t *input = zsock_new_pair (">tcp://127.0.0.1:9001");
    assert (input);
    
    //  Send five different frames, test ZFRAME_MORE
    int frame_nbr;
    for (frame_nbr = 0; frame_nbr < 5; frame_nbr++) {
        frame = zframe_new ("Hello", 5);
        assert (frame);
        rc = zframe_send (&frame, output, ZFRAME_MORE);
        assert (rc == 0);
    }
    //  Send same frame five times, test ZFRAME_REUSE
    frame = zframe_new ("Hello", 5);
    assert (frame);
    for (frame_nbr = 0; frame_nbr < 5; frame_nbr++) {
        rc = zframe_send (&frame, output, ZFRAME_MORE + ZFRAME_REUSE);
        assert (rc == 0);
    }
    assert (frame);
    zframe_t *copy = zframe_dup (frame);
    assert (zframe_eq (frame, copy));
    zframe_destroy (&frame);
    assert (!zframe_eq (frame, copy));
    assert (zframe_size (copy) == 5);
    zframe_destroy (&copy);
    assert (!zframe_eq (frame, copy));
    
    //  Test zframe_new_empty
    frame = zframe_new_empty ();
    assert (frame);
    assert (zframe_size (frame) == 0);
    zframe_destroy (&frame);
    
    //  Send END frame
    frame = zframe_new ("NOT", 3);
    assert (frame);
    zframe_reset (frame, "END", 3);
    char *string = zframe_strhex (frame);
    assert (streq (string, "454E44"));
    free (string);
    string = zframe_strdup (frame);
    assert (streq (string, "END"));
    free (string);
    rc = zframe_send (&frame, output, 0);
    assert (rc == 0);
    
    //  Read and count until we receive END
    frame_nbr = 0;
    for (frame_nbr = 0;; frame_nbr++) {
        zframe_t *frame = zframe_recv (input);
        if (zframe_streq (frame, "END")) {
            zframe_destroy (&frame);
            break;
        }
        assert (zframe_more (frame));
        zframe_set_more (frame, 0);
        assert (zframe_more (frame) == 0);
        zframe_destroy (&frame);
    }
    assert (frame_nbr == 10);
    
    #if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (4, 1, 0))
    // Test zframe_meta
    frame = zframe_new ("Hello", 5);
    assert (frame);
    rc = zframe_send (&frame, output, 0);
    assert (rc == 0);
    frame = zframe_recv (input);
    const char *meta = zframe_meta (frame, "Socket-Type");
    assert (meta != NULL);
    assert (streq (meta, "PAIR"));
    assert (zframe_meta (frame, "nonexistent") == NULL);
    zframe_destroy (&frame);
    #endif
    
    zsock_destroy (&input);
    zsock_destroy (&output);
    
    #if defined (ZMQ_SERVER)
    //  Create server and client sockets and connect over inproc
    zsock_t *server = zsock_new_server ("inproc://zframe-test-routing");
    assert (server);
    zsock_t *client = zsock_new_client ("inproc://zframe-test-routing");
    assert (client);
    
    //  Send request from client to server
    zframe_t *request = zframe_new ("Hello", 5);
    assert (request);
    rc = zframe_send (&request, client, 0);
    assert (rc == 0);
    assert (!request);
    
    //  Read request and send reply
    request = zframe_recv (server);
    assert (request);
    assert (zframe_streq (request, "Hello"));
    assert (zframe_routing_id (request));
    
    zframe_t *reply = zframe_new ("World", 5);
    assert (reply);
    zframe_set_routing_id (reply, zframe_routing_id (request));
    rc = zframe_send (&reply, server, 0);
    assert (rc == 0);
    zframe_destroy (&request);
    
    //  Read reply
    reply = zframe_recv (client);
    assert (zframe_streq (reply, "World"));
    assert (zframe_routing_id (reply) == 0);
    zframe_destroy (&reply);
    
    //  Client and server disallow multipart
    frame = zframe_new ("Hello", 5);
    rc = zframe_send (&frame, client, ZFRAME_MORE);
    assert (rc == -1);
    rc = zframe_send (&frame, server, ZFRAME_MORE);
    assert (rc == -1);
    zframe_destroy (&frame);
    
    zsock_destroy (&client);
    zsock_destroy (&server);
    #endif
    
    #ifdef ZMQ_RADIO
    //  Create radio and dish sockets and connect over inproc
    zsock_t *radio = zsock_new_radio ("inproc://zframe-test-radio");
    assert (radio);
    zsock_t *dish = zsock_new_dish ("inproc://zframe-test-radio");
    assert (dish);
    
    //  Join the group
    rc = zsock_join (dish, "World");
    assert (rc == 0);
    
    //  Publish message from radio
    zframe_t *message = zframe_new ("Hello", 5);
    assert (message);
    rc = zframe_set_group (message, "World");
    assert (rc == 0);
    rc = zframe_send (&message, radio, 0);
    assert (rc == 0);
    assert (!message);
    
    //  Receive the message from dish
    message = zframe_recv (dish);
    assert (message);
    assert (zframe_streq (message, "Hello"));
    assert (strcmp("World", zframe_group (message)) == 0);
    zframe_destroy (&message);
    
    zsock_destroy (&dish);
    zsock_destroy (&radio);
    #endif
    
```

<A name="toc4-2621" title="zgossip - decentralized configuration management" />
#### zgossip - decentralized configuration management

Implements a gossip protocol for decentralized configuration management.
Your applications nodes form a loosely connected network (which can have
cycles), and publish name/value tuples. Each node re-distributes the new
tuples it receives, so that the entire network eventually achieves a
consistent state. The current design does not expire tuples.

Provides these commands (sent as multipart strings to the actor):

* BIND endpoint -- binds the gossip service to specified endpoint
* PORT -- returns the last TCP port, if any, used for binding
* LOAD configfile -- load configuration from specified file
* SET configpath value -- set configuration path = value
* SAVE configfile -- save configuration to specified file
* CONNECT endpoint -- connect the gossip service to the specified peer
* PUBLISH key value -- publish a key/value pair to the gossip cluster
* STATUS -- return number of key/value pairs held by gossip service

Returns these messages:

* PORT number -- reply to PORT command
* STATUS number -- reply to STATUS command
* DELIVER key value -- new tuple delivered from network

The gossip protocol distributes information around a loosely-connected
network of gossip services. The information consists of name/value pairs
published by applications at any point in the network. The goal of the
gossip protocol is to create eventual consistency between all the using
applications.

The name/value pairs (tuples) can be used for configuration data, for
status updates, for presence, or for discovery. When used for discovery,
the gossip protocol works as an alternative to e.g. UDP beaconing.

The gossip network consists of a set of loosely-coupled nodes that
exchange tuples. Nodes can be connected across arbitrary transports,
so the gossip network can have nodes that communicate over inproc,
over IPC, and/or over TCP, at the same time.

Each node runs the same stack, which is a server-client hybrid using
a modified Harmony pattern (from Chapter 8 of the Guide):
http://zguide.zeromq.org/page:all#True-Peer-Connectivity-Harmony-Pattern

Each node provides a ROUTER socket that accepts client connections on an
key defined by the application via a BIND command. The state machine
for these connections is in zgossip.xml, and the generated code is in
zgossip_engine.inc.

Each node additionally creates outbound connections via DEALER sockets
to a set of servers ("remotes"), and under control of the calling app,
which sends CONNECT commands for each configured remote.

The messages between client and server are defined in zgossip_msg.xml.
We built this stack using the zeromq/zproto toolkit.

To join the gossip network, a node connects to one or more peers. Each
peer acts as a forwarder. This loosely-coupled network can scale to
thousands of nodes. However the gossip protocol is NOT designed to be
efficient, and should not be used for application data, as the same
tuples may be sent many times across the network.

The basic logic of the gossip service is to accept PUBLISH messages
from its owning application, and to forward these to every remote, and
every client it talks to. When a node gets a duplicate tuple, it throws
it away. When a node gets a new tuple, it stores it, and fowards it as
just described. At any point the application can access the node's set
of tuples.

At present there is no way to expire tuples from the network.

The assumptions in this design are:

* The data set is slow-changing. Thus, the cost of the gossip protocol
  is irrelevant with respect to other traffic.

This is the class interface:

```h
    //  To work with zgossip, use the CZMQ zactor API:
    //
    //  Create new zgossip instance, passing logging prefix:
    //
    //      zactor_t *zgossip = zactor_new (zgossip, "myname");
    //
    //  Destroy zgossip instance
    //
    //      zactor_destroy (&zgossip);
    //
    //  Enable verbose logging of commands and activity:
    //
    //      zstr_send (zgossip, "VERBOSE");
    //
    //  Bind zgossip to specified endpoint. TCP endpoints may specify
    //  the port number as "*" to aquire an ephemeral port:
    //
    //      zstr_sendx (zgossip, "BIND", endpoint, NULL);
    //
    //  Return assigned port number, specifically when BIND was done using an
    //  an ephemeral port:
    //
    //      zstr_sendx (zgossip, "PORT", NULL);
    //      char *command, *port_str;
    //      zstr_recvx (zgossip, &command, &port_str, NULL);
    //      assert (streq (command, "PORT"));
    //
    //  Specify configuration file to load, overwriting any previous loaded
    //  configuration file or options:
    //
    //      zstr_sendx (zgossip, "LOAD", filename, NULL);
    //
    //  Set configuration path value:
    //
    //      zstr_sendx (zgossip, "SET", path, value, NULL);
    //
    //  Save configuration data to config file on disk:
    //
    //      zstr_sendx (zgossip, "SAVE", filename, NULL);
    //
    //  Send zmsg_t instance to zgossip:
    //
    //      zactor_send (zgossip, &msg);
    //
    //  Receive zmsg_t instance from zgossip:
    //
    //      zmsg_t *msg = zactor_recv (zgossip);
    //
    //  This is the zgossip constructor as a zactor_fn:
    //
    CZMQ_EXPORT void
        zgossip (zsock_t *pipe, void *args);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zgossip_test (bool verbose);
```

This is the class self test code:

```c
    //  Test basic client-to-server operation of the protocol
    zactor_t *server = zactor_new (zgossip, "server");
    assert (server);
    if (verbose)
        zstr_send (server, "VERBOSE");
    zstr_sendx (server, "BIND", "inproc://zgossip", NULL);
    
    zsock_t *client = zsock_new (ZMQ_DEALER);
    assert (client);
    zsock_set_rcvtimeo (client, 2000);
    int rc = zsock_connect (client, "inproc://zgossip");
    assert (rc == 0);
    
    //  Send HELLO, which gets no message
    zgossip_msg_t *message = zgossip_msg_new ();
    zgossip_msg_set_id (message, ZGOSSIP_MSG_HELLO);
    zgossip_msg_send (message, client);
    
    //  Send PING, expect PONG back
    zgossip_msg_set_id (message, ZGOSSIP_MSG_PING);
    zgossip_msg_send (message, client);
    zgossip_msg_recv (message, client);
    assert (zgossip_msg_id (message) == ZGOSSIP_MSG_PONG);
    zgossip_msg_destroy (&message);
    
    zactor_destroy (&server);
    zsock_destroy (&client);
    
    //  Test peer-to-peer operations
    zactor_t *base = zactor_new (zgossip, "base");
    assert (base);
    if (verbose)
        zstr_send (base, "VERBOSE");
    //  Set a 100msec timeout on clients so we can test expiry
    zstr_sendx (base, "SET", "server/timeout", "100", NULL);
    zstr_sendx (base, "BIND", "inproc://base", NULL);
    
    zactor_t *alpha = zactor_new (zgossip, "alpha");
    assert (alpha);
    zstr_sendx (alpha, "CONNECT", "inproc://base", NULL);
    zstr_sendx (alpha, "PUBLISH", "inproc://alpha-1", "service1", NULL);
    zstr_sendx (alpha, "PUBLISH", "inproc://alpha-2", "service2", NULL);
    
    zactor_t *beta = zactor_new (zgossip, "beta");
    assert (beta);
    zstr_sendx (beta, "CONNECT", "inproc://base", NULL);
    zstr_sendx (beta, "PUBLISH", "inproc://beta-1", "service1", NULL);
    zstr_sendx (beta, "PUBLISH", "inproc://beta-2", "service2", NULL);
    
    //  got nothing
    zclock_sleep (200);
    
    zactor_destroy (&base);
    zactor_destroy (&alpha);
    zactor_destroy (&beta);
    
```

<A name="toc4-2820" title="zhash - simple generic hash container" />
#### zhash - simple generic hash container

zhash is an expandable hash table container. This is a simple container.
For heavy-duty applications we recommend using zhashx.

Note that it's relatively slow (~50K insertions/deletes per second), so
don't do inserts/updates on the critical path for message I/O. It can
do ~2.5M lookups per second for 16-char keys. Timed on a 1.6GHz CPU.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    // Callback function for zhash_freefn method
    typedef void (zhash_free_fn) (
        void *data);
    
    //  Create a new, empty hash container
    CZMQ_EXPORT zhash_t *
        zhash_new (void);
    
    //  Unpack binary frame into a new hash table. Packed data must follow format
    //  defined by zhash_pack. Hash table is set to autofree. An empty frame     
    //  unpacks to an empty hash table.                                          
    CZMQ_EXPORT zhash_t *
        zhash_unpack (zframe_t *frame);
    
    //  Destroy a hash container and all items in it
    CZMQ_EXPORT void
        zhash_destroy (zhash_t **self_p);
    
    //  Insert item into hash table with specified key and item.               
    //  If key is already present returns -1 and leaves existing item unchanged
    //  Returns 0 on success.                                                  
    CZMQ_EXPORT int
        zhash_insert (zhash_t *self, const char *key, void *item);
    
    //  Update item into hash table with specified key and item.            
    //  If key is already present, destroys old item and inserts new one.   
    //  Use free_fn method to ensure deallocator is properly called on item.
    CZMQ_EXPORT void
        zhash_update (zhash_t *self, const char *key, void *item);
    
    //  Remove an item specified by key from the hash table. If there was no such
    //  item, this function does nothing.                                        
    CZMQ_EXPORT void
        zhash_delete (zhash_t *self, const char *key);
    
    //  Return the item at the specified key, or null
    CZMQ_EXPORT void *
        zhash_lookup (zhash_t *self, const char *key);
    
    //  Reindexes an item from an old key to a new key. If there was no such
    //  item, does nothing. Returns 0 if successful, else -1.               
    CZMQ_EXPORT int
        zhash_rename (zhash_t *self, const char *old_key, const char *new_key);
    
    //  Set a free function for the specified hash table item. When the item is
    //  destroyed, the free function, if any, is called on that item.          
    //  Use this when hash items are dynamically allocated, to ensure that     
    //  you don't have memory leaks. You can pass 'free' or NULL as a free_fn. 
    //  Returns the item, or NULL if there is no such item.                    
    CZMQ_EXPORT void *
        zhash_freefn (zhash_t *self, const char *key, zhash_free_fn free_fn);
    
    //  Return the number of keys/items in the hash table
    CZMQ_EXPORT size_t
        zhash_size (zhash_t *self);
    
    //  Make copy of hash table; if supplied table is null, returns null.    
    //  Does not copy items themselves. Rebuilds new table so may be slow on 
    //  very large tables. NOTE: only works with item values that are strings
    //  since there's no other way to know how to duplicate the item value.  
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zhash_t *
        zhash_dup (zhash_t *self);
    
    //  Return keys for items in table
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zlist_t *
        zhash_keys (zhash_t *self);
    
    //  Simple iterator; returns first item in hash table, in no given order, 
    //  or NULL if the table is empty. This method is simpler to use than the 
    //  foreach() method, which is deprecated. To access the key for this item
    //  use zhash_cursor(). NOTE: do NOT modify the table while iterating.    
    CZMQ_EXPORT void *
        zhash_first (zhash_t *self);
    
    //  Simple iterator; returns next item in hash table, in no given order, 
    //  or NULL if the last item was already returned. Use this together with
    //  zhash_first() to process all items in a hash table. If you need the  
    //  items in sorted order, use zhash_keys() and then zlist_sort(). To    
    //  access the key for this item use zhash_cursor(). NOTE: do NOT modify 
    //  the table while iterating.                                           
    CZMQ_EXPORT void *
        zhash_next (zhash_t *self);
    
    //  After a successful first/next method, returns the key for the item that
    //  was returned. This is a constant string that you may not modify or     
    //  deallocate, and which lasts as long as the item in the hash. After an  
    //  unsuccessful first/next, returns NULL.                                 
    CZMQ_EXPORT const char *
        zhash_cursor (zhash_t *self);
    
    //  Add a comment to hash table before saving to disk. You can add as many   
    //  comment lines as you like. These comment lines are discarded when loading
    //  the file. If you use a null format, all comments are deleted.            
    CZMQ_EXPORT void
        zhash_comment (zhash_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Serialize hash table to a binary frame that can be sent in a message.
    //  The packed format is compatible with the 'dictionary' type defined in
    //  http://rfc.zeromq.org/spec:35/FILEMQ, and implemented by zproto:     
    //                                                                       
    //     ; A list of name/value pairs                                      
    //     dictionary      = dict-count *( dict-name dict-value )            
    //     dict-count      = number-4                                        
    //     dict-value      = longstr                                         
    //     dict-name       = string                                          
    //                                                                       
    //     ; Strings are always length + text contents                       
    //     longstr         = number-4 *VCHAR                                 
    //     string          = number-1 *VCHAR                                 
    //                                                                       
    //     ; Numbers are unsigned integers in network byte order             
    //     number-1        = 1OCTET                                          
    //     number-4        = 4OCTET                                          
    //                                                                       
    //  Comments are not included in the packed data. Item values MUST be    
    //  strings.                                                             
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zframe_t *
        zhash_pack (zhash_t *self);
    
    //  Save hash table to a text file in name=value format. Hash values must be
    //  printable strings; keys may not contain '=' character. Returns 0 if OK, 
    //  else -1 if a file error occurred.                                       
    CZMQ_EXPORT int
        zhash_save (zhash_t *self, const char *filename);
    
    //  Load hash table from a text file in name=value format; hash table must 
    //  already exist. Hash values must printable strings; keys may not contain
    //  '=' character. Returns 0 if OK, else -1 if a file was not readable.    
    CZMQ_EXPORT int
        zhash_load (zhash_t *self, const char *filename);
    
    //  When a hash table was loaded from a file by zhash_load, this method will
    //  reload the file if it has been modified since, and is "stable", i.e. not
    //  still changing. Returns 0 if OK, -1 if there was an error reloading the 
    //  file.                                                                   
    CZMQ_EXPORT int
        zhash_refresh (zhash_t *self);
    
    //  Set hash for automatic value destruction
    CZMQ_EXPORT void
        zhash_autofree (zhash_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zhash_test (bool verbose);
    
```

This is the class self test code:

```c
    zhash_t *hash = zhash_new ();
    assert (hash);
    assert (zhash_size (hash) == 0);
    assert (zhash_first (hash) == NULL);
    assert (zhash_cursor (hash) == NULL);
    
    //  Insert some items
    int rc;
    rc = zhash_insert (hash, "DEADBEEF", "dead beef");
    char *item = (char *) zhash_first (hash);
    assert (streq (zhash_cursor (hash), "DEADBEEF"));
    assert (streq (item, "dead beef"));
    assert (rc == 0);
    rc = zhash_insert (hash, "ABADCAFE", "a bad cafe");
    assert (rc == 0);
    rc = zhash_insert (hash, "C0DEDBAD", "coded bad");
    assert (rc == 0);
    rc = zhash_insert (hash, "DEADF00D", "dead food");
    assert (rc == 0);
    assert (zhash_size (hash) == 4);
    
    //  Look for existing items
    item = (char *) zhash_lookup (hash, "DEADBEEF");
    assert (streq (item, "dead beef"));
    item = (char *) zhash_lookup (hash, "ABADCAFE");
    assert (streq (item, "a bad cafe"));
    item = (char *) zhash_lookup (hash, "C0DEDBAD");
    assert (streq (item, "coded bad"));
    item = (char *) zhash_lookup (hash, "DEADF00D");
    assert (streq (item, "dead food"));
    
    //  Look for non-existent items
    item = (char *) zhash_lookup (hash, "foo");
    assert (item == NULL);
    
    //  Try to insert duplicate items
    rc = zhash_insert (hash, "DEADBEEF", "foo");
    assert (rc == -1);
    item = (char *) zhash_lookup (hash, "DEADBEEF");
    assert (streq (item, "dead beef"));
    
    //  Some rename tests
    
    //  Valid rename, key is now LIVEBEEF
    rc = zhash_rename (hash, "DEADBEEF", "LIVEBEEF");
    assert (rc == 0);
    item = (char *) zhash_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));
    
    //  Trying to rename an unknown item to a non-existent key
    rc = zhash_rename (hash, "WHATBEEF", "NONESUCH");
    assert (rc == -1);
    
    //  Trying to rename an unknown item to an existing key
    rc = zhash_rename (hash, "WHATBEEF", "LIVEBEEF");
    assert (rc == -1);
    item = (char *) zhash_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));
    
    //  Trying to rename an existing item to another existing item
    rc = zhash_rename (hash, "LIVEBEEF", "ABADCAFE");
    assert (rc == -1);
    item = (char *) zhash_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));
    item = (char *) zhash_lookup (hash, "ABADCAFE");
    assert (streq (item, "a bad cafe"));
    
    //  Test keys method
    zlist_t *keys = zhash_keys (hash);
    assert (zlist_size (keys) == 4);
    zlist_destroy (&keys);
    
    //  Test dup method
    zhash_t *copy = zhash_dup (hash);
    assert (zhash_size (copy) == 4);
    item = (char *) zhash_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhash_destroy (&copy);
    
    //  Test pack/unpack methods
    zframe_t *frame = zhash_pack (hash);
    copy = zhash_unpack (frame);
    zframe_destroy (&frame);
    assert (zhash_size (copy) == 4);
    item = (char *) zhash_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhash_destroy (&copy);
    
    //  Test save and load
    zhash_comment (hash, "This is a test file");
    zhash_comment (hash, "Created by %s", "czmq_selftest");
    zhash_save (hash, ".cache");
    copy = zhash_new ();
    assert (copy);
    zhash_load (copy, ".cache");
    item = (char *) zhash_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhash_destroy (&copy);
    zsys_file_delete (".cache");
    
    //  Delete a item
    zhash_delete (hash, "LIVEBEEF");
    item = (char *) zhash_lookup (hash, "LIVEBEEF");
    assert (item == NULL);
    assert (zhash_size (hash) == 3);
    
    //  Check that the queue is robust against random usage
    struct {
        char name [100];
        bool exists;
    } testset [200];
    memset (testset, 0, sizeof (testset));
    int testmax = 200, testnbr, iteration;
    
    srandom ((unsigned) time (NULL));
    for (iteration = 0; iteration < 25000; iteration++) {
        testnbr = randof (testmax);
        if (testset [testnbr].exists) {
            item = (char *) zhash_lookup (hash, testset [testnbr].name);
            assert (item);
            zhash_delete (hash, testset [testnbr].name);
            testset [testnbr].exists = false;
        }
        else {
            sprintf (testset [testnbr].name, "%x-%x", rand (), rand ());
            if (zhash_insert (hash, testset [testnbr].name, "") == 0)
                testset [testnbr].exists = true;
        }
    }
    //  Test 10K lookups
    for (iteration = 0; iteration < 10000; iteration++)
        item = (char *) zhash_lookup (hash, "DEADBEEFABADCAFE");
    
    //  Destructor should be safe to call twice
    zhash_destroy (&hash);
    zhash_destroy (&hash);
    assert (hash == NULL);
    
    // Test autofree; automatically copies and frees string values
    hash = zhash_new ();
    assert (hash);
    zhash_autofree (hash);
    char value [255];
    strcpy (value, "This is a string");
    rc = zhash_insert (hash, "key1", value);
    assert (rc == 0);
    strcpy (value, "Inserting with the same key will fail");
    rc = zhash_insert (hash, "key1", value);
    assert (rc == -1);
    strcpy (value, "Ring a ding ding");
    rc = zhash_insert (hash, "key2", value);
    assert (rc == 0);
    assert (streq ((char *) zhash_lookup (hash, "key1"), "This is a string"));
    assert (streq ((char *) zhash_lookup (hash, "key2"), "Ring a ding ding"));
    zhash_destroy (&hash);
```

<A name="toc4-3149" title="zhashx - extended generic hash container" />
#### zhashx - extended generic hash container

zhashx is an extended hash table container with more functionality than
zhash, its simpler cousin.

The hash table always has a size that is prime and roughly doubles its
size when 75% full. In case of hash collisions items are chained in a
linked list. The hash table size is increased slightly (up to 5 times
before roughly doubling the size) when an overly long chain (between 1
and 63 items depending on table size) is detected.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    // Destroy an item
    typedef void (zhashx_destructor_fn) (
        void **item);
    
    // Duplicate an item
    typedef void * (zhashx_duplicator_fn) (
        const void *item);
    
    // Compare two items, for sorting
    typedef int (zhashx_comparator_fn) (
        const void *item1, const void *item2);
    
    // compare two items, for sorting
    typedef void (zhashx_free_fn) (
        void *data);
    
    // compare two items, for sorting
    typedef size_t (zhashx_hash_fn) (
        const void *key);
    
    // Serializes an item to a longstr.                       
    // The caller takes ownership of the newly created object.
    typedef char * (zhashx_serializer_fn) (
        const void *item);
    
    // Deserializes a longstr into an item.                   
    // The caller takes ownership of the newly created object.
    typedef void * (zhashx_deserializer_fn) (
        const char *item_str);
    
    //  Create a new, empty hash container
    CZMQ_EXPORT zhashx_t *
        zhashx_new (void);
    
    //  Unpack binary frame into a new hash table. Packed data must follow format
    //  defined by zhashx_pack. Hash table is set to autofree. An empty frame    
    //  unpacks to an empty hash table.                                          
    CZMQ_EXPORT zhashx_t *
        zhashx_unpack (zframe_t *frame);
    
    //  Destroy a hash container and all items in it
    CZMQ_EXPORT void
        zhashx_destroy (zhashx_t **self_p);
    
    //  Insert item into hash table with specified key and item.               
    //  If key is already present returns -1 and leaves existing item unchanged
    //  Returns 0 on success.                                                  
    CZMQ_EXPORT int
        zhashx_insert (zhashx_t *self, const void *key, void *item);
    
    //  Update or insert item into hash table with specified key and item. If the
    //  key is already present, destroys old item and inserts new one. If you set
    //  a container item destructor, this is called on the old value. If the key 
    //  was not already present, inserts a new item. Sets the hash cursor to the 
    //  new item.                                                                
    CZMQ_EXPORT void
        zhashx_update (zhashx_t *self, const void *key, void *item);
    
    //  Remove an item specified by key from the hash table. If there was no such
    //  item, this function does nothing.                                        
    CZMQ_EXPORT void
        zhashx_delete (zhashx_t *self, const void *key);
    
    //  Delete all items from the hash table. If the key destructor is  
    //  set, calls it on every key. If the item destructor is set, calls
    //  it on every item.                                               
    CZMQ_EXPORT void
        zhashx_purge (zhashx_t *self);
    
    //  Return the item at the specified key, or null
    CZMQ_EXPORT void *
        zhashx_lookup (zhashx_t *self, const void *key);
    
    //  Reindexes an item from an old key to a new key. If there was no such
    //  item, does nothing. Returns 0 if successful, else -1.               
    CZMQ_EXPORT int
        zhashx_rename (zhashx_t *self, const void *old_key, const void *new_key);
    
    //  Set a free function for the specified hash table item. When the item is
    //  destroyed, the free function, if any, is called on that item.          
    //  Use this when hash items are dynamically allocated, to ensure that     
    //  you don't have memory leaks. You can pass 'free' or NULL as a free_fn. 
    //  Returns the item, or NULL if there is no such item.                    
    CZMQ_EXPORT void *
        zhashx_freefn (zhashx_t *self, const void *key, zhashx_free_fn free_fn);
    
    //  Return the number of keys/items in the hash table
    CZMQ_EXPORT size_t
        zhashx_size (zhashx_t *self);
    
    //  Return a zlistx_t containing the keys for the items in the       
    //  table. Uses the key_duplicator to duplicate all keys and sets the
    //  key_destructor as destructor for the list.                       
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zlistx_t *
        zhashx_keys (zhashx_t *self);
    
    //  Return a zlistx_t containing the values for the items in the  
    //  table. Uses the duplicator to duplicate all items and sets the
    //  destructor as destructor for the list.                        
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zlistx_t *
        zhashx_values (zhashx_t *self);
    
    //  Simple iterator; returns first item in hash table, in no given order, 
    //  or NULL if the table is empty. This method is simpler to use than the 
    //  foreach() method, which is deprecated. To access the key for this item
    //  use zhashx_cursor(). NOTE: do NOT modify the table while iterating.   
    CZMQ_EXPORT void *
        zhashx_first (zhashx_t *self);
    
    //  Simple iterator; returns next item in hash table, in no given order, 
    //  or NULL if the last item was already returned. Use this together with
    //  zhashx_first() to process all items in a hash table. If you need the 
    //  items in sorted order, use zhashx_keys() and then zlistx_sort(). To  
    //  access the key for this item use zhashx_cursor(). NOTE: do NOT modify
    //  the table while iterating.                                           
    CZMQ_EXPORT void *
        zhashx_next (zhashx_t *self);
    
    //  After a successful first/next method, returns the key for the item that
    //  was returned. This is a constant string that you may not modify or     
    //  deallocate, and which lasts as long as the item in the hash. After an  
    //  unsuccessful first/next, returns NULL.                                 
    CZMQ_EXPORT const void *
        zhashx_cursor (zhashx_t *self);
    
    //  Add a comment to hash table before saving to disk. You can add as many   
    //  comment lines as you like. These comment lines are discarded when loading
    //  the file. If you use a null format, all comments are deleted.            
    CZMQ_EXPORT void
        zhashx_comment (zhashx_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Save hash table to a text file in name=value format. Hash values must be
    //  printable strings; keys may not contain '=' character. Returns 0 if OK, 
    //  else -1 if a file error occurred.                                       
    CZMQ_EXPORT int
        zhashx_save (zhashx_t *self, const char *filename);
    
    //  Load hash table from a text file in name=value format; hash table must 
    //  already exist. Hash values must printable strings; keys may not contain
    //  '=' character. Returns 0 if OK, else -1 if a file was not readable.    
    CZMQ_EXPORT int
        zhashx_load (zhashx_t *self, const char *filename);
    
    //  When a hash table was loaded from a file by zhashx_load, this method will
    //  reload the file if it has been modified since, and is "stable", i.e. not 
    //  still changing. Returns 0 if OK, -1 if there was an error reloading the  
    //  file.                                                                    
    CZMQ_EXPORT int
        zhashx_refresh (zhashx_t *self);
    
    //  Serialize hash table to a binary frame that can be sent in a message.
    //  The packed format is compatible with the 'dictionary' type defined in
    //  http://rfc.zeromq.org/spec:35/FILEMQ, and implemented by zproto:     
    //                                                                       
    //     ; A list of name/value pairs                                      
    //     dictionary      = dict-count *( dict-name dict-value )            
    //     dict-count      = number-4                                        
    //     dict-value      = longstr                                         
    //     dict-name       = string                                          
    //                                                                       
    //     ; Strings are always length + text contents                       
    //     longstr         = number-4 *VCHAR                                 
    //     string          = number-1 *VCHAR                                 
    //                                                                       
    //     ; Numbers are unsigned integers in network byte order             
    //     number-1        = 1OCTET                                          
    //     number-4        = 4OCTET                                          
    //                                                                       
    //  Comments are not included in the packed data. Item values MUST be    
    //  strings.                                                             
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zframe_t *
        zhashx_pack (zhashx_t *self);
    
    //  Make a copy of the list; items are duplicated if you set a duplicator 
    //  for the list, otherwise not. Copying a null reference returns a null  
    //  reference. Note that this method's behavior changed slightly for CZMQ 
    //  v3.x, as it does not set nor respect autofree. It does however let you
    //  duplicate any hash table safely. The old behavior is in zhashx_dup_v2.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zhashx_t *
        zhashx_dup (zhashx_t *self);
    
    //  Set a user-defined deallocator for hash items; by default items are not
    //  freed when the hash is destroyed.                                      
    CZMQ_EXPORT void
        zhashx_set_destructor (zhashx_t *self, zhashx_destructor_fn destructor);
    
    //  Set a user-defined duplicator for hash items; by default items are not
    //  copied when the hash is duplicated.                                   
    CZMQ_EXPORT void
        zhashx_set_duplicator (zhashx_t *self, zhashx_duplicator_fn duplicator);
    
    //  Set a user-defined deallocator for keys; by default keys are freed
    //  when the hash is destroyed using free().                          
    CZMQ_EXPORT void
        zhashx_set_key_destructor (zhashx_t *self, zhashx_destructor_fn destructor);
    
    //  Set a user-defined duplicator for keys; by default keys are duplicated
    //  using strdup.                                                         
    CZMQ_EXPORT void
        zhashx_set_key_duplicator (zhashx_t *self, zhashx_duplicator_fn duplicator);
    
    //  Set a user-defined comparator for keys; by default keys are
    //  compared using strcmp.                                     
    CZMQ_EXPORT void
        zhashx_set_key_comparator (zhashx_t *self, zhashx_comparator_fn comparator);
    
    //  Set a user-defined comparator for keys; by default keys are
    //  compared using strcmp.                                     
    CZMQ_EXPORT void
        zhashx_set_key_hasher (zhashx_t *self, zhashx_hash_fn hasher);
    
    //  Make copy of hash table; if supplied table is null, returns null.    
    //  Does not copy items themselves. Rebuilds new table so may be slow on 
    //  very large tables. NOTE: only works with item values that are strings
    //  since there's no other way to know how to duplicate the item value.  
    CZMQ_EXPORT zhashx_t *
        zhashx_dup_v2 (zhashx_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zhashx_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  Same as unpack but uses a user-defined deserializer function to convert
    //  a longstr back into item format.                                       
    CZMQ_EXPORT zhashx_t *
        zhashx_unpack_own (zframe_t *frame, zhashx_deserializer_fn deserializer);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Same as pack but uses a user-defined serializer function to convert items
    //  into longstr.                                                            
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zframe_t *
        zhashx_pack_own (zhashx_t *self, zhashx_serializer_fn serializer);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    zhashx_t *hash = zhashx_new ();
    assert (hash);
    assert (zhashx_size (hash) == 0);
    assert (zhashx_first (hash) == NULL);
    assert (zhashx_cursor (hash) == NULL);
    
    //  Insert some items
    int rc;
    rc = zhashx_insert (hash, "DEADBEEF", "dead beef");
    char *item = (char *) zhashx_first (hash);
    assert (streq ((char *) zhashx_cursor (hash), "DEADBEEF"));
    assert (streq (item, "dead beef"));
    assert (rc == 0);
    rc = zhashx_insert (hash, "ABADCAFE", "a bad cafe");
    assert (rc == 0);
    rc = zhashx_insert (hash, "C0DEDBAD", "coded bad");
    assert (rc == 0);
    rc = zhashx_insert (hash, "DEADF00D", "dead food");
    assert (rc == 0);
    assert (zhashx_size (hash) == 4);
    
    //  Look for existing items
    item = (char *) zhashx_lookup (hash, "DEADBEEF");
    assert (streq (item, "dead beef"));
    item = (char *) zhashx_lookup (hash, "ABADCAFE");
    assert (streq (item, "a bad cafe"));
    item = (char *) zhashx_lookup (hash, "C0DEDBAD");
    assert (streq (item, "coded bad"));
    item = (char *) zhashx_lookup (hash, "DEADF00D");
    assert (streq (item, "dead food"));
    
    //  Look for non-existent items
    item = (char *) zhashx_lookup (hash, "foo");
    assert (item == NULL);
    
    //  Try to insert duplicate items
    rc = zhashx_insert (hash, "DEADBEEF", "foo");
    assert (rc == -1);
    item = (char *) zhashx_lookup (hash, "DEADBEEF");
    assert (streq (item, "dead beef"));
    
    //  Some rename tests
    
    //  Valid rename, key is now LIVEBEEF
    rc = zhashx_rename (hash, "DEADBEEF", "LIVEBEEF");
    assert (rc == 0);
    item = (char *) zhashx_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));
    
    //  Trying to rename an unknown item to a non-existent key
    rc = zhashx_rename (hash, "WHATBEEF", "NONESUCH");
    assert (rc == -1);
    
    //  Trying to rename an unknown item to an existing key
    rc = zhashx_rename (hash, "WHATBEEF", "LIVEBEEF");
    assert (rc == -1);
    item = (char *) zhashx_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));
    
    //  Trying to rename an existing item to another existing item
    rc = zhashx_rename (hash, "LIVEBEEF", "ABADCAFE");
    assert (rc == -1);
    item = (char *) zhashx_lookup (hash, "LIVEBEEF");
    assert (streq (item, "dead beef"));
    item = (char *) zhashx_lookup (hash, "ABADCAFE");
    assert (streq (item, "a bad cafe"));
    
    //  Test keys method
    zlistx_t *keys = zhashx_keys (hash);
    assert (zlistx_size (keys) == 4);
    zlistx_destroy (&keys);
    
    zlistx_t *values = zhashx_values(hash);
    assert (zlistx_size (values) == 4);
    zlistx_destroy (&values);
    
    //  Test dup method
    zhashx_t *copy = zhashx_dup (hash);
    assert (zhashx_size (copy) == 4);
    item = (char *) zhashx_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhashx_destroy (&copy);
    
    //  Test pack/unpack methods
    zframe_t *frame = zhashx_pack (hash);
    copy = zhashx_unpack (frame);
    zframe_destroy (&frame);
    assert (zhashx_size (copy) == 4);
    item = (char *) zhashx_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhashx_destroy (&copy);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  Test own pack/unpack methods
    zhashx_t *own_hash = zhashx_new ();
    zhashx_set_destructor (own_hash, s_test_destroy_int);
    assert (own_hash);
    int *val1 = (int *) zmalloc (sizeof (int));
    int *val2 = (int *) zmalloc (sizeof (int));
    *val1 = 25;
    *val2 = 100;
    zhashx_insert (own_hash, "val1", val1);
    zhashx_insert (own_hash, "val2", val2);
    frame = zhashx_pack_own (own_hash, s_test_serialize_int);
    copy = zhashx_unpack_own (frame, s_test_deserialze_int);
    zhashx_set_destructor (copy, s_test_destroy_int);
    zframe_destroy (&frame);
    assert (zhashx_size (copy) == 2);
    assert (*((int *) zhashx_lookup (copy, "val1")) == 25);
    assert (*((int *) zhashx_lookup (copy, "val2")) == 100);
    zhashx_destroy (&copy);
    zhashx_destroy (&own_hash);
    #endif // CZMQ_BUILD_DRAFT_API
    
    //  Test save and load
    zhashx_comment (hash, "This is a test file");
    zhashx_comment (hash, "Created by %s", "czmq_selftest");
    zhashx_save (hash, ".cache");
    copy = zhashx_new ();
    assert (copy);
    zhashx_load (copy, ".cache");
    item = (char *) zhashx_lookup (copy, "LIVEBEEF");
    assert (item);
    assert (streq (item, "dead beef"));
    zhashx_destroy (&copy);
    zsys_file_delete (".cache");
    
    //  Delete a item
    zhashx_delete (hash, "LIVEBEEF");
    item = (char *) zhashx_lookup (hash, "LIVEBEEF");
    assert (item == NULL);
    assert (zhashx_size (hash) == 3);
    
    //  Check that the queue is robust against random usage
    struct {
        char name [100];
        bool exists;
    } testset [200];
    memset (testset, 0, sizeof (testset));
    int testmax = 200, testnbr, iteration;
    
    srandom ((unsigned) time (NULL));
    for (iteration = 0; iteration < 25000; iteration++) {
        testnbr = randof (testmax);
        if (testset [testnbr].exists) {
            item = (char *) zhashx_lookup (hash, testset [testnbr].name);
            assert (item);
            zhashx_delete (hash, testset [testnbr].name);
            testset [testnbr].exists = false;
        }
        else {
            sprintf (testset [testnbr].name, "%x-%x", rand (), rand ());
            if (zhashx_insert (hash, testset [testnbr].name, "") == 0)
                testset [testnbr].exists = true;
        }
    }
    //  Test 10K lookups
    for (iteration = 0; iteration < 10000; iteration++)
        item = (char *) zhashx_lookup (hash, "DEADBEEFABADCAFE");
    
    //  Destructor should be safe to call twice
    zhashx_destroy (&hash);
    zhashx_destroy (&hash);
    assert (hash == NULL);
    
    //  Test destructor; automatically copies and frees string values
    hash = zhashx_new ();
    assert (hash);
    zhashx_set_destructor (hash, (zhashx_destructor_fn *) zstr_free);
    zhashx_set_duplicator (hash, (zhashx_duplicator_fn *) strdup);
    char value [255];
    strcpy (value, "This is a string");
    rc = zhashx_insert (hash, "key1", value);
    assert (rc == 0);
    strcpy (value, "Ring a ding ding");
    rc = zhashx_insert (hash, "key2", value);
    assert (rc == 0);
    assert (streq ((char *) zhashx_lookup (hash, "key1"), "This is a string"));
    assert (streq ((char *) zhashx_lookup (hash, "key2"), "Ring a ding ding"));
    zhashx_destroy (&hash);
```

<A name="toc4-3598" title="ziflist - list of network interfaces available on system" />
#### ziflist - list of network interfaces available on system

The ziflist class takes a snapshot of the network interfaces that the
system currently supports (this can change arbitrarily, especially on
mobile devices). The caller can then access the network interface
information using an iterator that works like zlistx. Only stores those
interfaces with broadcast capability, and ignores the loopback interface.

Please add @discuss section in ../src/ziflist.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  Get a list of network interfaces currently defined on the system
    CZMQ_EXPORT ziflist_t *
        ziflist_new (void);
    
    //  Destroy a ziflist instance
    CZMQ_EXPORT void
        ziflist_destroy (ziflist_t **self_p);
    
    //  Reload network interfaces from system
    CZMQ_EXPORT void
        ziflist_reload (ziflist_t *self);
    
    //  Return the number of network interfaces on system
    CZMQ_EXPORT size_t
        ziflist_size (ziflist_t *self);
    
    //  Get first network interface, return NULL if there are none
    CZMQ_EXPORT const char *
        ziflist_first (ziflist_t *self);
    
    //  Get next network interface, return NULL if we hit the last one
    CZMQ_EXPORT const char *
        ziflist_next (ziflist_t *self);
    
    //  Return the current interface IP address as a printable string
    CZMQ_EXPORT const char *
        ziflist_address (ziflist_t *self);
    
    //  Return the current interface broadcast address as a printable string
    CZMQ_EXPORT const char *
        ziflist_broadcast (ziflist_t *self);
    
    //  Return the current interface network mask as a printable string
    CZMQ_EXPORT const char *
        ziflist_netmask (ziflist_t *self);
    
    //  Return the list of interfaces.
    CZMQ_EXPORT void
        ziflist_print (ziflist_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        ziflist_test (bool verbose);
    
```

This is the class self test code:

```c
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    
    size_t items = ziflist_size (iflist);
    
    if (verbose) {
        printf ("ziflist: interfaces=%zu\n", ziflist_size (iflist));
        const char *name = ziflist_first (iflist);
        while (name) {
            printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
                    name, ziflist_address (iflist), ziflist_netmask (iflist), ziflist_broadcast (iflist));
            name = ziflist_next (iflist);
        }
    }
    ziflist_reload (iflist);
    assert (items == ziflist_size (iflist));
    ziflist_destroy (&iflist);
```

<A name="toc4-3682" title="zlist - simple generic list container" />
#### zlist - simple generic list container

Provides a generic container implementing a fast singly-linked list. You
can use this to construct multi-dimensional lists, and other structures
together with other generic containers like zhash. This is a simple
class. For demanding applications we recommend using zlistx.

To iterate through a list, use zlist_first to get the first item, then
loop while not null, and do zlist_next at the end of each iteration.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    // Comparison function e.g. for sorting and removing.
    typedef int (zlist_compare_fn) (
        void *item1, void *item2);
    
    // Callback function for zlist_freefn method
    typedef void (zlist_free_fn) (
        void *data);
    
    //  Create a new list container
    CZMQ_EXPORT zlist_t *
        zlist_new (void);
    
    //  Destroy a list container
    CZMQ_EXPORT void
        zlist_destroy (zlist_t **self_p);
    
    //  Return the item at the head of list. If the list is empty, returns NULL.
    //  Leaves cursor pointing at the head item, or NULL if the list is empty.  
    CZMQ_EXPORT void *
        zlist_first (zlist_t *self);
    
    //  Return the next item. If the list is empty, returns NULL. To move to
    //  the start of the list call zlist_first (). Advances the cursor.     
    CZMQ_EXPORT void *
        zlist_next (zlist_t *self);
    
    //  Return the item at the tail of list. If the list is empty, returns NULL.
    //  Leaves cursor pointing at the tail item, or NULL if the list is empty.  
    CZMQ_EXPORT void *
        zlist_last (zlist_t *self);
    
    //  Return first item in the list, or null, leaves the cursor
    CZMQ_EXPORT void *
        zlist_head (zlist_t *self);
    
    //  Return last item in the list, or null, leaves the cursor
    CZMQ_EXPORT void *
        zlist_tail (zlist_t *self);
    
    //  Return the current item of list. If the list is empty, returns NULL.     
    //  Leaves cursor pointing at the current item, or NULL if the list is empty.
    CZMQ_EXPORT void *
        zlist_item (zlist_t *self);
    
    //  Append an item to the end of the list, return 0 if OK or -1 if this  
    //  failed for some reason (out of memory). Note that if a duplicator has
    //  been set, this method will also duplicate the item.                  
    CZMQ_EXPORT int
        zlist_append (zlist_t *self, void *item);
    
    //  Push an item to the start of the list, return 0 if OK or -1 if this  
    //  failed for some reason (out of memory). Note that if a duplicator has
    //  been set, this method will also duplicate the item.                  
    CZMQ_EXPORT int
        zlist_push (zlist_t *self, void *item);
    
    //  Pop the item off the start of the list, if any
    CZMQ_EXPORT void *
        zlist_pop (zlist_t *self);
    
    //  Checks if an item already is present. Uses compare method to determine if 
    //  items are equal. If the compare method is NULL the check will only compare
    //  pointers. Returns true if item is present else false.                     
    CZMQ_EXPORT bool
        zlist_exists (zlist_t *self, void *item);
    
    //  Remove the specified item from the list if present
    CZMQ_EXPORT void
        zlist_remove (zlist_t *self, void *item);
    
    //  Make a copy of list. If the list has autofree set, the copied list will  
    //  duplicate all items, which must be strings. Otherwise, the list will hold
    //  pointers back to the items in the original list. If list is null, returns
    //  NULL.                                                                    
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zlist_t *
        zlist_dup (zlist_t *self);
    
    //  Purge all items from list
    CZMQ_EXPORT void
        zlist_purge (zlist_t *self);
    
    //  Return number of items in the list
    CZMQ_EXPORT size_t
        zlist_size (zlist_t *self);
    
    //  Sort the list. If the compare function is null, sorts the list by     
    //  ascending key value using a straight ASCII comparison. If you specify 
    //  a compare function, this decides how items are sorted. The sort is not
    //  stable, so may reorder items with the same keys. The algorithm used is
    //  combsort, a compromise between performance and simplicity.            
    CZMQ_EXPORT void
        zlist_sort (zlist_t *self, zlist_compare_fn compare);
    
    //  Set list for automatic item destruction; item values MUST be strings. 
    //  By default a list item refers to a value held elsewhere. When you set 
    //  this, each time you append or push a list item, zlist will take a copy
    //  of the string value. Then, when you destroy the list, it will free all
    //  item values automatically. If you use any other technique to allocate 
    //  list values, you must free them explicitly before destroying the list.
    //  The usual technique is to pop list items and destroy them, until the  
    //  list is empty.                                                        
    CZMQ_EXPORT void
        zlist_autofree (zlist_t *self);
    
    //  Sets a compare function for this list. The function compares two items.
    //  It returns an integer less than, equal to, or greater than zero if the 
    //  first item is found, respectively, to be less than, to match, or be    
    //  greater than the second item.                                          
    //  This function is used for sorting, removal and exists checking.        
    CZMQ_EXPORT void
        zlist_comparefn (zlist_t *self, zlist_compare_fn fn);
    
    //  Set a free function for the specified list item. When the item is     
    //  destroyed, the free function, if any, is called on that item.         
    //  Use this when list items are dynamically allocated, to ensure that    
    //  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
    //  Returns the item, or NULL if there is no such item.                   
    CZMQ_EXPORT void *
        zlist_freefn (zlist_t *self, void *item, zlist_free_fn fn, bool at_tail);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zlist_test (bool verbose);
    
```

This is the class self test code:

```c
    zlist_t *list = zlist_new ();
    assert (list);
    assert (zlist_size (list) == 0);
    
    //  Three items we'll use as test data
    //  List items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";
    
    zlist_append (list, cheese);
    assert (zlist_size (list) == 1);
    assert ( zlist_exists (list, cheese));
    assert (!zlist_exists (list, bread));
    assert (!zlist_exists (list, wine));
    zlist_append (list, bread);
    assert (zlist_size (list) == 2);
    assert ( zlist_exists (list, cheese));
    assert ( zlist_exists (list, bread));
    assert (!zlist_exists (list, wine));
    zlist_append (list, wine);
    assert (zlist_size (list) == 3);
    assert ( zlist_exists (list, cheese));
    assert ( zlist_exists (list, bread));
    assert ( zlist_exists (list, wine));
    
    assert (zlist_head (list) == cheese);
    assert (zlist_next (list) == cheese);
    
    assert (zlist_first (list) == cheese);
    assert (zlist_tail (list) == wine);
    assert (zlist_next (list) == bread);
    
    assert (zlist_first (list) == cheese);
    assert (zlist_next (list) == bread);
    assert (zlist_next (list) == wine);
    assert (zlist_next (list) == NULL);
    //  After we reach end of list, next wraps around
    assert (zlist_next (list) == cheese);
    assert (zlist_size (list) == 3);
    
    zlist_remove (list, wine);
    assert (zlist_size (list) == 2);
    
    assert (zlist_first (list) == cheese);
    zlist_remove (list, cheese);
    assert (zlist_size (list) == 1);
    assert (zlist_first (list) == bread);
    
    zlist_remove (list, bread);
    assert (zlist_size (list) == 0);
    
    zlist_append (list, cheese);
    zlist_append (list, bread);
    assert (zlist_last (list) == bread);
    zlist_remove (list, bread);
    assert (zlist_last (list) == cheese);
    zlist_remove (list, cheese);
    assert (zlist_last (list) == NULL);
    
    zlist_push (list, cheese);
    assert (zlist_size (list) == 1);
    assert (zlist_first (list) == cheese);
    
    zlist_push (list, bread);
    assert (zlist_size (list) == 2);
    assert (zlist_first (list) == bread);
    assert (zlist_item (list) == bread);
    
    zlist_append (list, wine);
    assert (zlist_size (list) == 3);
    assert (zlist_first (list) == bread);
    
    zlist_t *sub_list = zlist_dup (list);
    assert (sub_list);
    assert (zlist_size (sub_list) == 3);
    
    zlist_sort (list, NULL);
    char *item;
    item = (char *) zlist_pop (list);
    assert (item == bread);
    item = (char *) zlist_pop (list);
    assert (item == wine);
    item = (char *) zlist_pop (list);
    assert (item == cheese);
    assert (zlist_size (list) == 0);
    
    assert (zlist_size (sub_list) == 3);
    zlist_push (list, sub_list);
    zlist_t *sub_list_2 = zlist_dup (sub_list);
    zlist_append (list, sub_list_2);
    assert (zlist_freefn (list, sub_list, &s_zlist_free, false) == sub_list);
    assert (zlist_freefn (list, sub_list_2, &s_zlist_free, true) == sub_list_2);
    zlist_destroy (&list);
    
    //  Test autofree functionality
    list = zlist_new ();
    assert (list);
    zlist_autofree (list);
    //  Set equals function otherwise equals will not work as autofree copies strings
    zlist_comparefn (list, (zlist_compare_fn *) strcmp);
    zlist_push (list, bread);
    zlist_append (list, cheese);
    assert (zlist_size (list) == 2);
    zlist_append (list, wine);
    assert (zlist_exists (list, wine));
    zlist_remove (list, wine);
    assert (!zlist_exists (list, wine));
    assert (streq ((const char *) zlist_first (list), bread));
    item = (char *) zlist_pop (list);
    assert (streq (item, bread));
    free (item);
    item = (char *) zlist_pop (list);
    assert (streq (item, cheese));
    free (item);
    
    zlist_destroy (&list);
    assert (list == NULL);
```

<A name="toc4-3948" title="zlistx - extended generic list container" />
#### zlistx - extended generic list container

Provides a generic doubly-linked list container. This container provides
hooks for duplicator, comparator, and destructor functions. These tie
into CZMQ and standard C semantics, so e.g. for string items you can
use strdup, strcmp, and zstr_free. To store custom objects, define your
own duplicator and comparator, and use the standard object destructor.

This is a reworking of the simpler zlist container. It is faster to
insert and delete items anywhere in the list, and to keep ordered lists.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    // Destroy an item
    typedef void (zlistx_destructor_fn) (
        void **item);
    
    // Duplicate an item
    typedef void * (zlistx_duplicator_fn) (
        const void *item);
    
    // Compare two items, for sorting
    typedef int (zlistx_comparator_fn) (
        const void *item1, const void *item2);
    
    //  Create a new, empty list.
    CZMQ_EXPORT zlistx_t *
        zlistx_new (void);
    
    //  Destroy a list. If an item destructor was specified, all items in the
    //  list are automatically destroyed as well.                            
    CZMQ_EXPORT void
        zlistx_destroy (zlistx_t **self_p);
    
    //  Add an item to the head of the list. Calls the item duplicator, if any,
    //  on the item. Resets cursor to list head. Returns an item handle on     
    //  success, NULL if memory was exhausted.                                 
    CZMQ_EXPORT void *
        zlistx_add_start (zlistx_t *self, void *item);
    
    //  Add an item to the tail of the list. Calls the item duplicator, if any,
    //  on the item. Resets cursor to list head. Returns an item handle on     
    //  success, NULL if memory was exhausted.                                 
    CZMQ_EXPORT void *
        zlistx_add_end (zlistx_t *self, void *item);
    
    //  Return the number of items in the list
    CZMQ_EXPORT size_t
        zlistx_size (zlistx_t *self);
    
    //  Return first item in the list, or null, leaves the cursor
    CZMQ_EXPORT void *
        zlistx_head (zlistx_t *self);
    
    //  Return last item in the list, or null, leaves the cursor
    CZMQ_EXPORT void *
        zlistx_tail (zlistx_t *self);
    
    //  Return the item at the head of list. If the list is empty, returns NULL.
    //  Leaves cursor pointing at the head item, or NULL if the list is empty.  
    CZMQ_EXPORT void *
        zlistx_first (zlistx_t *self);
    
    //  Return the next item. At the end of the list (or in an empty list),     
    //  returns NULL. Use repeated zlistx_next () calls to work through the list
    //  from zlistx_first (). First time, acts as zlistx_first().               
    CZMQ_EXPORT void *
        zlistx_next (zlistx_t *self);
    
    //  Return the previous item. At the start of the list (or in an empty list),
    //  returns NULL. Use repeated zlistx_prev () calls to work through the list 
    //  backwards from zlistx_last (). First time, acts as zlistx_last().        
    CZMQ_EXPORT void *
        zlistx_prev (zlistx_t *self);
    
    //  Return the item at the tail of list. If the list is empty, returns NULL.
    //  Leaves cursor pointing at the tail item, or NULL if the list is empty.  
    CZMQ_EXPORT void *
        zlistx_last (zlistx_t *self);
    
    //  Returns the value of the item at the cursor, or NULL if the cursor is
    //  not pointing to an item.                                             
    CZMQ_EXPORT void *
        zlistx_item (zlistx_t *self);
    
    //  Returns the handle of the item at the cursor, or NULL if the cursor is
    //  not pointing to an item.                                              
    CZMQ_EXPORT void *
        zlistx_cursor (zlistx_t *self);
    
    //  Returns the item associated with the given list handle, or NULL if passed     
    //  in handle is NULL. Asserts that the passed in handle points to a list element.
    CZMQ_EXPORT void *
        zlistx_handle_item (void *handle);
    
    //  Find an item in the list, searching from the start. Uses the item     
    //  comparator, if any, else compares item values directly. Returns the   
    //  item handle found, or NULL. Sets the cursor to the found item, if any.
    CZMQ_EXPORT void *
        zlistx_find (zlistx_t *self, void *item);
    
    //  Detach an item from the list, using its handle. The item is not modified, 
    //  and the caller is responsible for destroying it if necessary. If handle is
    //  null, detaches the first item on the list. Returns item that was detached,
    //  or null if none was. If cursor was at item, moves cursor to previous item,
    //  so you can detach items while iterating forwards through a list.          
    CZMQ_EXPORT void *
        zlistx_detach (zlistx_t *self, void *handle);
    
    //  Detach item at the cursor, if any, from the list. The item is not modified,
    //  and the caller is responsible for destroying it as necessary. Returns item 
    //  that was detached, or null if none was. Moves cursor to previous item, so  
    //  you can detach items while iterating forwards through a list.              
    CZMQ_EXPORT void *
        zlistx_detach_cur (zlistx_t *self);
    
    //  Delete an item, using its handle. Calls the item destructor is any is 
    //  set. If handle is null, deletes the first item on the list. Returns 0 
    //  if an item was deleted, -1 if not. If cursor was at item, moves cursor
    //  to previous item, so you can delete items while iterating forwards    
    //  through a list.                                                       
    CZMQ_EXPORT int
        zlistx_delete (zlistx_t *self, void *handle);
    
    //  Move an item to the start of the list, via its handle.
    CZMQ_EXPORT void
        zlistx_move_start (zlistx_t *self, void *handle);
    
    //  Move an item to the end of the list, via its handle.
    CZMQ_EXPORT void
        zlistx_move_end (zlistx_t *self, void *handle);
    
    //  Remove all items from the list, and destroy them if the item destructor
    //  is set.                                                                
    CZMQ_EXPORT void
        zlistx_purge (zlistx_t *self);
    
    //  Sort the list. If an item comparator was set, calls that to compare    
    //  items, otherwise compares on item value. The sort is not stable, so may
    //  reorder equal items.                                                   
    CZMQ_EXPORT void
        zlistx_sort (zlistx_t *self);
    
    //  Create a new node and insert it into a sorted list. Calls the item        
    //  duplicator, if any, on the item. If low_value is true, starts searching   
    //  from the start of the list, otherwise searches from the end. Use the item 
    //  comparator, if any, to find where to place the new node. Returns a handle 
    //  to the new node, or NULL if memory was exhausted. Resets the cursor to the
    //  list head.                                                                
    CZMQ_EXPORT void *
        zlistx_insert (zlistx_t *self, void *item, bool low_value);
    
    //  Move an item, specified by handle, into position in a sorted list. Uses 
    //  the item comparator, if any, to determine the new location. If low_value
    //  is true, starts searching from the start of the list, otherwise searches
    //  from the end.                                                           
    CZMQ_EXPORT void
        zlistx_reorder (zlistx_t *self, void *handle, bool low_value);
    
    //  Make a copy of the list; items are duplicated if you set a duplicator
    //  for the list, otherwise not. Copying a null reference returns a null 
    //  reference.                                                           
    CZMQ_EXPORT zlistx_t *
        zlistx_dup (zlistx_t *self);
    
    //  Set a user-defined deallocator for list items; by default items are not
    //  freed when the list is destroyed.                                      
    CZMQ_EXPORT void
        zlistx_set_destructor (zlistx_t *self, zlistx_destructor_fn destructor);
    
    //  Set a user-defined duplicator for list items; by default items are not
    //  copied when the list is duplicated.                                   
    CZMQ_EXPORT void
        zlistx_set_duplicator (zlistx_t *self, zlistx_duplicator_fn duplicator);
    
    //  Set a user-defined comparator for zlistx_find and zlistx_sort; the method 
    //  must return -1, 0, or 1 depending on whether item1 is less than, equal to,
    //  or greater than, item2.                                                   
    CZMQ_EXPORT void
        zlistx_set_comparator (zlistx_t *self, zlistx_comparator_fn comparator);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zlistx_test (bool verbose);
    
```

This is the class self test code:

```c
    zlistx_t *list = zlistx_new ();
    assert (list);
    assert (zlistx_size (list) == 0);
    
    //  Test operations on an empty list
    assert (zlistx_first (list) == NULL);
    assert (zlistx_last (list) == NULL);
    assert (zlistx_next (list) == NULL);
    assert (zlistx_prev (list) == NULL);
    assert (zlistx_find (list, "hello") == NULL);
    assert (zlistx_delete (list, NULL) == -1);
    assert (zlistx_detach (list, NULL) == NULL);
    assert (zlistx_delete (list, NULL) == -1);
    assert (zlistx_detach (list, NULL) == NULL);
    zlistx_purge (list);
    zlistx_sort (list);
    
    //  Use item handlers
    zlistx_set_destructor (list, (zlistx_destructor_fn *) zstr_free);
    zlistx_set_duplicator (list, (zlistx_duplicator_fn *) strdup);
    zlistx_set_comparator (list, (zlistx_comparator_fn *) strcmp);
    
    //  Try simple insert/sort/delete/next
    assert (zlistx_next (list) == NULL);
    zlistx_add_end (list, "world");
    assert (streq ((char *) zlistx_next (list), "world"));
    zlistx_add_end (list, "hello");
    assert (streq ((char *) zlistx_prev (list), "hello"));
    zlistx_sort (list);
    assert (zlistx_size (list) == 2);
    void *handle = zlistx_find (list, "hello");
    char *item1 = (char *) zlistx_item (list);
    char *item2 = (char *) zlistx_handle_item (handle);
    assert (item1 == item2);
    assert (streq (item1, "hello"));
    zlistx_delete (list, handle);
    assert (zlistx_size (list) == 1);
    char *string = (char *) zlistx_detach (list, NULL);
    assert (streq (string, "world"));
    free (string);
    assert (zlistx_size (list) == 0);
    
    //  Check next/back work
    //  Now populate the list with items
    zlistx_add_start (list, "five");
    zlistx_add_end   (list, "six");
    zlistx_add_start (list, "four");
    zlistx_add_end   (list, "seven");
    zlistx_add_start (list, "three");
    zlistx_add_end   (list, "eight");
    zlistx_add_start (list, "two");
    zlistx_add_end   (list, "nine");
    zlistx_add_start (list, "one");
    zlistx_add_end   (list, "ten");
    
    //  Test our navigation skills
    assert (zlistx_size (list) == 10);
    assert (streq ((char *) zlistx_last (list), "ten"));
    assert (streq ((char *) zlistx_prev (list), "nine"));
    assert (streq ((char *) zlistx_prev (list), "eight"));
    assert (streq ((char *) zlistx_prev (list), "seven"));
    assert (streq ((char *) zlistx_prev (list), "six"));
    assert (streq ((char *) zlistx_prev (list), "five"));
    assert (streq ((char *) zlistx_first (list), "one"));
    assert (streq ((char *) zlistx_next (list), "two"));
    assert (streq ((char *) zlistx_next (list), "three"));
    assert (streq ((char *) zlistx_next (list), "four"));
    
    //  Sort by alphabetical order
    zlistx_sort (list);
    assert (streq ((char *) zlistx_first (list), "eight"));
    assert (streq ((char *) zlistx_last (list), "two"));
    
    //  Moving items around
    handle = zlistx_find (list, "six");
    zlistx_move_start (list, handle);
    assert (streq ((char *) zlistx_first (list), "six"));
    zlistx_move_end (list, handle);
    assert (streq ((char *) zlistx_last (list), "six"));
    zlistx_sort (list);
    assert (streq ((char *) zlistx_last (list), "two"));
    
    //  Copying a list
    zlistx_t *copy = zlistx_dup (list);
    assert (copy);
    assert (zlistx_size (copy) == 10);
    assert (streq ((char *) zlistx_first (copy), "eight"));
    assert (streq ((char *) zlistx_last (copy), "two"));
    zlistx_destroy (&copy);
    
    //  Delete items while iterating
    string = (char *) zlistx_first (list);
    assert (streq (string, "eight"));
    string = (char *) zlistx_next (list);
    assert (streq (string, "five"));
    zlistx_delete (list, zlistx_cursor (list));
    string = (char *) zlistx_next (list);
    assert (streq (string, "four"));
    
    zlistx_purge (list);
    zlistx_destroy (&list);
```

<A name="toc4-4245" title="zloop - event-driven reactor" />
#### zloop - event-driven reactor

The zloop class provides an event-driven reactor pattern. The reactor
handles zmq_pollitem_t items (pollers or writers, sockets or fds), and
once-off or repeated timers. Its resolution is 1 msec. It uses a tickless
timer to reduce CPU interrupts in inactive processes.

Please add @discuss section in ../src/zloop.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    // Callback function for reactor socket activity
    typedef int (zloop_reader_fn) (
        zloop_t *loop, zsock_t *reader, void *arg);
    
    // Callback function for reactor events (low-level)
    typedef int (zloop_fn) (
        zloop_t *loop, zmq_pollitem_t *item, void *arg);
    
    // Callback for reactor timer events
    typedef int (zloop_timer_fn) (
        zloop_t *loop, int timer_id, void *arg);
    
    //  Create a new zloop reactor
    CZMQ_EXPORT zloop_t *
        zloop_new (void);
    
    //  Destroy a reactor
    CZMQ_EXPORT void
        zloop_destroy (zloop_t **self_p);
    
    //  Register socket reader with the reactor. When the reader has messages, 
    //  the reactor will call the handler, passing the arg. Returns 0 if OK, -1
    //  if there was an error. If you register the same socket more than once, 
    //  each instance will invoke its corresponding handler.                   
    CZMQ_EXPORT int
        zloop_reader (zloop_t *self, zsock_t *sock, zloop_reader_fn handler, void *arg);
    
    //  Cancel a socket reader from the reactor. If multiple readers exist for
    //  same socket, cancels ALL of them.                                     
    CZMQ_EXPORT void
        zloop_reader_end (zloop_t *self, zsock_t *sock);
    
    //  Configure a registered reader to ignore errors. If you do not set this,
    //  then readers that have errors are removed from the reactor silently.   
    CZMQ_EXPORT void
        zloop_reader_set_tolerant (zloop_t *self, zsock_t *sock);
    
    //  Register low-level libzmq pollitem with the reactor. When the pollitem  
    //  is ready, will call the handler, passing the arg. Returns 0 if OK, -1   
    //  if there was an error. If you register the pollitem more than once, each
    //  instance will invoke its corresponding handler. A pollitem with         
    //  socket=NULL and fd=0 means 'poll on FD zero'.                           
    CZMQ_EXPORT int
        zloop_poller (zloop_t *self, zmq_pollitem_t *item, zloop_fn handler, void *arg);
    
    //  Cancel a pollitem from the reactor, specified by socket or FD. If both
    //  are specified, uses only socket. If multiple poll items exist for same
    //  socket/FD, cancels ALL of them.                                       
    CZMQ_EXPORT void
        zloop_poller_end (zloop_t *self, zmq_pollitem_t *item);
    
    //  Configure a registered poller to ignore errors. If you do not set this,
    //  then poller that have errors are removed from the reactor silently.    
    CZMQ_EXPORT void
        zloop_poller_set_tolerant (zloop_t *self, zmq_pollitem_t *item);
    
    //  Register a timer that expires after some delay and repeats some number of
    //  times. At each expiry, will call the handler, passing the arg. To run a  
    //  timer forever, use 0 times. Returns a timer_id that is used to cancel the
    //  timer in the future. Returns -1 if there was an error.                   
    CZMQ_EXPORT int
        zloop_timer (zloop_t *self, size_t delay, size_t times, zloop_timer_fn handler, void *arg);
    
    //  Cancel a specific timer identified by a specific timer_id (as returned by
    //  zloop_timer).                                                            
    CZMQ_EXPORT int
        zloop_timer_end (zloop_t *self, int timer_id);
    
    //  Register a ticket timer. Ticket timers are very fast in the case where   
    //  you use a lot of timers (thousands), and frequently remove and add them. 
    //  The main use case is expiry timers for servers that handle many clients, 
    //  and which reset the expiry timer for each message received from a client.
    //  Whereas normal timers perform poorly as the number of clients grows, the 
    //  cost of ticket timers is constant, no matter the number of clients. You  
    //  must set the ticket delay using zloop_set_ticket_delay before creating a 
    //  ticket. Returns a handle to the timer that you should use in             
    //  zloop_ticket_reset and zloop_ticket_delete.                              
    CZMQ_EXPORT void *
        zloop_ticket (zloop_t *self, zloop_timer_fn handler, void *arg);
    
    //  Reset a ticket timer, which moves it to the end of the ticket list and
    //  resets its execution time. This is a very fast operation.             
    CZMQ_EXPORT void
        zloop_ticket_reset (zloop_t *self, void *handle);
    
    //  Delete a ticket timer. We do not actually delete the ticket here, as    
    //  other code may still refer to the ticket. We mark as deleted, and remove
    //  later and safely.                                                       
    CZMQ_EXPORT void
        zloop_ticket_delete (zloop_t *self, void *handle);
    
    //  Set the ticket delay, which applies to all tickets. If you lower the   
    //  delay and there are already tickets created, the results are undefined.
    CZMQ_EXPORT void
        zloop_set_ticket_delay (zloop_t *self, size_t ticket_delay);
    
    //  Set hard limit on number of timers allowed. Setting more than a small  
    //  number of timers (10-100) can have a dramatic impact on the performance
    //  of the reactor. For high-volume cases, use ticket timers. If the hard  
    //  limit is reached, the reactor stops creating new timers and logs an    
    //  error.                                                                 
    CZMQ_EXPORT void
        zloop_set_max_timers (zloop_t *self, size_t max_timers);
    
    //  Set verbose tracing of reactor on/off. The default verbose setting is
    //  off (false).                                                         
    CZMQ_EXPORT void
        zloop_set_verbose (zloop_t *self, bool verbose);
    
    //  Start the reactor. Takes control of the thread and returns when the ØMQ  
    //  context is terminated or the process is interrupted, or any event handler
    //  returns -1. Event handlers may register new sockets and timers, and      
    //  cancel sockets. Returns 0 if interrupted, -1 if canceled by a handler.   
    CZMQ_EXPORT int
        zloop_start (zloop_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zloop_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  By default the reactor stops if the process receives a SIGINT or SIGTERM 
    //  signal. This makes it impossible to shut-down message based architectures
    //  like zactors. This method lets you switch off break handling. The default
    //  nonstop setting is off (false).                                          
    CZMQ_EXPORT void
        zloop_set_nonstop (zloop_t *self, bool nonstop);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new (ZMQ_PAIR);
    assert (output);
    zsock_bind (output, "inproc://zloop.test");
    
    zsock_t *input = zsock_new (ZMQ_PAIR);
    assert (input);
    zsock_connect (input, "inproc://zloop.test");
    
    zloop_t *loop = zloop_new ();
    assert (loop);
    zloop_set_verbose (loop, verbose);
    
    //  Create a timer that will be cancelled
    int timer_id = zloop_timer (loop, 1000, 1, s_timer_event, NULL);
    zloop_timer (loop, 5, 1, s_cancel_timer_event, &timer_id);
    
    //  After 20 msecs, send a ping message to output3
    zloop_timer (loop, 20, 1, s_timer_event, output);
    
    //  Set up some tickets that will never expire
    zloop_set_ticket_delay (loop, 10000);
    void *ticket1 = zloop_ticket (loop, s_timer_event, NULL);
    void *ticket2 = zloop_ticket (loop, s_timer_event, NULL);
    void *ticket3 = zloop_ticket (loop, s_timer_event, NULL);
    
    //  When we get the ping message, end the reactor
    rc = zloop_reader (loop, input, s_socket_event, NULL);
    assert (rc == 0);
    zloop_reader_set_tolerant (loop, input);
    zloop_start (loop);
    
    zloop_ticket_delete (loop, ticket1);
    zloop_ticket_delete (loop, ticket2);
    zloop_ticket_delete (loop, ticket3);
    
    //  Check whether loop properly ignores zsys_interrupted flag
    //  when asked to
    zloop_destroy (&loop);
    loop = zloop_new ();
    
    bool timer_event_called = false;
    zloop_timer (loop, 1, 1, s_timer_event3, &timer_event_called);
    
    zsys_interrupted = 1;
    zloop_start (loop);
    //  zloop returns immediately without giving any handler a chance to run
    assert (!timer_event_called);
    
    zloop_set_nonstop (loop, true);
    zloop_start (loop);
    //  zloop runs the handler which will terminate the loop
    assert (timer_event_called);
    zsys_interrupted = 0;
    
    //  cleanup
    zloop_destroy (&loop);
    assert (loop == NULL);
    
    zsock_destroy (&input);
    zsock_destroy (&output);
```

<A name="toc4-4460" title="zmonitor - socket event monitor" />
#### zmonitor - socket event monitor

The zmonitor actor provides an API for obtaining socket events such as
connected, listen, disconnected, etc. Socket events are only available
for sockets connecting or bound to ipc:// and tcp:// endpoints.

This class wraps the ZMQ socket monitor API, see zmq_socket_monitor for
details. Works on all versions of libzmq from 3.2 onwards. This class
replaces zproxy_v2, and is meant for applications that use the CZMQ v3
API (meaning, zsock).

This is the class interface:

```h
    //  Create new zmonitor actor instance to monitor a zsock_t socket:
    //
    //      zactor_t *monitor = zactor_new (zmonitor, mysocket);
    //
    //  Destroy zmonitor instance.
    //
    //      zactor_destroy (&monitor);
    //
    //  Enable verbose logging of commands and activity.
    //
    //      zstr_send (monitor, "VERBOSE");
    //
    //  Listen to monitor event type (zero or types, ending in NULL):
    //      zstr_sendx (monitor, "LISTEN", type, ..., NULL);
    //  
    //      Events:
    //      CONNECTED
    //      CONNECT_DELAYED
    //      CONNECT_RETRIED
    //      LISTENING
    //      BIND_FAILED
    //      ACCEPTED
    //      ACCEPT_FAILED
    //      CLOSED
    //      CLOSE_FAILED
    //      DISCONNECTED
    //      MONITOR_STOPPED
    //      ALL
    //
    //  Start monitor; after this, any further LISTEN commands are ignored.
    //
    //      zstr_send (monitor, "START");
    //      zsock_wait (monitor);
    //
    //  Receive next monitor event:
    //
    //      zmsg_t *msg = zmsg_recv (monitor);
    //
    //  This is the zmonitor constructor as a zactor_fn; the argument can be
    //  a zactor_t, zsock_t, or libzmq void * socket:
    CZMQ_EXPORT void
        zmonitor (zsock_t *pipe, void *sock);
    
    //  Selftest
    CZMQ_EXPORT void
        zmonitor_test (bool verbose);
```

This is the class self test code:

```c
    zsock_t *client = zsock_new (ZMQ_DEALER);
    assert (client);
    zactor_t *clientmon = zactor_new (zmonitor, client);
    assert (clientmon);
    if (verbose)
        zstr_sendx (clientmon, "VERBOSE", NULL);
    zstr_sendx (clientmon, "LISTEN", "LISTENING", "ACCEPTED", NULL);
    zstr_sendx (clientmon, "START", NULL);
    zsock_wait (clientmon);
    
    zsock_t *server = zsock_new (ZMQ_DEALER);
    assert (server);
    zactor_t *servermon = zactor_new (zmonitor, server);
    assert (servermon);
    if (verbose)
        zstr_sendx (servermon, "VERBOSE", NULL);
    zstr_sendx (servermon, "LISTEN", "CONNECTED", "DISCONNECTED", NULL);
    zstr_sendx (servermon, "START", NULL);
    zsock_wait (servermon);
    
    //  Allow a brief time for the message to get there...
    zmq_poll (NULL, 0, 200);
    
    //  Check client is now listening
    int port_nbr = zsock_bind (client, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    s_assert_event (clientmon, "LISTENING");
    
    //  Check server connected to client
    zsock_connect (server, "tcp://127.0.0.1:%d", port_nbr);
    s_assert_event (servermon, "CONNECTED");
    
    //  Check client accepted connection
    s_assert_event (clientmon, "ACCEPTED");
    
    zactor_destroy (&clientmon);
    zactor_destroy (&servermon);
    zsock_destroy (&client);
    zsock_destroy (&server);
    #endif
```

<A name="toc4-4568" title="zmsg - working with multipart messages" />
#### zmsg - working with multipart messages

The zmsg class provides methods to send and receive multipart messages
across ØMQ sockets. This class provides a list-like container interface,
with methods to work with the overall container. zmsg_t messages are
composed of zero or more zframe_t frames.

Please add @discuss section in ../src/zmsg.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    //  Create a new empty message object
    CZMQ_EXPORT zmsg_t *
        zmsg_new (void);
    
    //  Receive message from socket, returns zmsg_t object or NULL if the recv   
    //  was interrupted. Does a blocking recv. If you want to not block then use 
    //  the zloop class or zmsg_recv_nowait or zmq_poll to check for socket input
    //  before receiving.                                                        
    CZMQ_EXPORT zmsg_t *
        zmsg_recv (void *source);
    
    //  Load/append an open file into new message, return the message.
    //  Returns NULL if the message could not be loaded.              
    CZMQ_EXPORT zmsg_t *
        zmsg_load (FILE *file);
    
    //  Decodes a serialized message frame created by zmsg_encode () and returns
    //  a new zmsg_t object. Returns NULL if the frame was badly formatted or   
    //  there was insufficient memory to work.                                  
    CZMQ_EXPORT zmsg_t *
        zmsg_decode (zframe_t *frame);
    
    //  Generate a signal message encoding the given status. A signal is a short
    //  message carrying a 1-byte success/failure code (by convention, 0 means  
    //  OK). Signals are encoded to be distinguishable from "normal" messages.  
    CZMQ_EXPORT zmsg_t *
        zmsg_new_signal (byte status);
    
    //  Destroy a message object and all frames it contains
    CZMQ_EXPORT void
        zmsg_destroy (zmsg_t **self_p);
    
    //  Send message to destination socket, and destroy the message after sending
    //  it successfully. If the message has no frames, sends nothing but destroys
    //  the message anyhow. Nullifies the caller's reference to the message (as  
    //  it is a destructor).                                                     
    CZMQ_EXPORT int
        zmsg_send (zmsg_t **self_p, void *dest);
    
    //  Send message to destination socket as part of a multipart sequence, and 
    //  destroy the message after sending it successfully. Note that after a    
    //  zmsg_sendm, you must call zmsg_send or another method that sends a final
    //  message part. If the message has no frames, sends nothing but destroys  
    //  the message anyhow. Nullifies the caller's reference to the message (as 
    //  it is a destructor).                                                    
    CZMQ_EXPORT int
        zmsg_sendm (zmsg_t **self_p, void *dest);
    
    //  Return size of message, i.e. number of frames (0 or more).
    CZMQ_EXPORT size_t
        zmsg_size (zmsg_t *self);
    
    //  Return total size of all frames in message.
    CZMQ_EXPORT size_t
        zmsg_content_size (zmsg_t *self);
    
    //  Push frame to the front of the message, i.e. before all other frames.  
    //  Message takes ownership of frame, will destroy it when message is sent.
    //  Returns 0 on success, -1 on error. Deprecates zmsg_push, which did not 
    //  nullify the caller's frame reference.                                  
    CZMQ_EXPORT int
        zmsg_prepend (zmsg_t *self, zframe_t **frame_p);
    
    //  Add frame to the end of the message, i.e. after all other frames.      
    //  Message takes ownership of frame, will destroy it when message is sent.
    //  Returns 0 on success. Deprecates zmsg_add, which did not nullify the   
    //  caller's frame reference.                                              
    CZMQ_EXPORT int
        zmsg_append (zmsg_t *self, zframe_t **frame_p);
    
    //  Remove first frame from message, if any. Returns frame, or NULL.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zframe_t *
        zmsg_pop (zmsg_t *self);
    
    //  Push block of memory to front of message, as a new frame.
    //  Returns 0 on success, -1 on error.                       
    CZMQ_EXPORT int
        zmsg_pushmem (zmsg_t *self, const void *data, size_t size);
    
    //  Add block of memory to the end of the message, as a new frame.
    //  Returns 0 on success, -1 on error.                            
    CZMQ_EXPORT int
        zmsg_addmem (zmsg_t *self, const void *data, size_t size);
    
    //  Push string as new frame to front of message.
    //  Returns 0 on success, -1 on error.           
    CZMQ_EXPORT int
        zmsg_pushstr (zmsg_t *self, const char *string);
    
    //  Push string as new frame to end of message.
    //  Returns 0 on success, -1 on error.         
    CZMQ_EXPORT int
        zmsg_addstr (zmsg_t *self, const char *string);
    
    //  Push formatted string as new frame to front of message.
    //  Returns 0 on success, -1 on error.                     
    CZMQ_EXPORT int
        zmsg_pushstrf (zmsg_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Push formatted string as new frame to end of message.
    //  Returns 0 on success, -1 on error.                   
    CZMQ_EXPORT int
        zmsg_addstrf (zmsg_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Pop frame off front of message, return as fresh string. If there were
    //  no more frames in the message, returns NULL.                         
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zmsg_popstr (zmsg_t *self);
    
    //  Push encoded message as a new frame. Message takes ownership of    
    //  submessage, so the original is destroyed in this call. Returns 0 on
    //  success, -1 on error.                                              
    CZMQ_EXPORT int
        zmsg_addmsg (zmsg_t *self, zmsg_t **msg_p);
    
    //  Remove first submessage from message, if any. Returns zmsg_t, or NULL if
    //  decoding was not successful.                                            
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zmsg_t *
        zmsg_popmsg (zmsg_t *self);
    
    //  Remove specified frame from list, if present. Does not destroy frame.
    CZMQ_EXPORT void
        zmsg_remove (zmsg_t *self, zframe_t *frame);
    
    //  Set cursor to first frame in message. Returns frame, or NULL, if the
    //  message is empty. Use this to navigate the frames as a list.        
    CZMQ_EXPORT zframe_t *
        zmsg_first (zmsg_t *self);
    
    //  Return the next frame. If there are no more frames, returns NULL. To move
    //  to the first frame call zmsg_first(). Advances the cursor.               
    CZMQ_EXPORT zframe_t *
        zmsg_next (zmsg_t *self);
    
    //  Return the last frame. If there are no frames, returns NULL.
    CZMQ_EXPORT zframe_t *
        zmsg_last (zmsg_t *self);
    
    //  Save message to an open file, return 0 if OK, else -1. The message is  
    //  saved as a series of frames, each with length and data. Note that the  
    //  file is NOT guaranteed to be portable between operating systems, not   
    //  versions of CZMQ. The file format is at present undocumented and liable
    //  to arbitrary change.                                                   
    CZMQ_EXPORT int
        zmsg_save (zmsg_t *self, FILE *file);
    
    //  Serialize multipart message to a single message frame. Use this method
    //  to send structured messages across transports that do not support     
    //  multipart data. Allocates and returns a new frame containing the      
    //  serialized message. To decode a serialized message frame, use         
    //  zmsg_decode ().                                                       
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zframe_t *
        zmsg_encode (zmsg_t *self);
    
    //  Create copy of message, as new message object. Returns a fresh zmsg_t
    //  object. If message is null, or memory was exhausted, returns null.   
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT zmsg_t *
        zmsg_dup (zmsg_t *self);
    
    //  Send message to zsys log sink (may be stdout, or system facility as
    //  configured by zsys_set_logstream).                                 
    CZMQ_EXPORT void
        zmsg_print (zmsg_t *self);
    
    //  Return true if the two messages have the same number of frames and each  
    //  frame in the first message is identical to the corresponding frame in the
    //  other message. As with zframe_eq, return false if either message is NULL.
    CZMQ_EXPORT bool
        zmsg_eq (zmsg_t *self, zmsg_t *other);
    
    //  Return signal value, 0 or greater, if message is a signal, -1 if not.
    CZMQ_EXPORT int
        zmsg_signal (zmsg_t *self);
    
    //  Probe the supplied object, and report if it looks like a zmsg_t.
    CZMQ_EXPORT bool
        zmsg_is (void *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zmsg_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  Return message routing ID, if the message came from a ZMQ_SERVER socket.
    //  Else returns zero.                                                      
    CZMQ_EXPORT uint32_t
        zmsg_routing_id (zmsg_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Set routing ID on message. This is used if/when the message is sent to a
    //  ZMQ_SERVER socket.                                                      
    CZMQ_EXPORT void
        zmsg_set_routing_id (zmsg_t *self, uint32_t routing_id);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new_pair ("@inproc://zmsg.test");
    assert (output);
    zsock_t *input = zsock_new_pair (">inproc://zmsg.test");
    assert (input);
    
    //  Test send and receive of single-frame message
    zmsg_t *msg = zmsg_new ();
    assert (msg);
    zframe_t *frame = zframe_new ("Hello", 5);
    assert (frame);
    zmsg_prepend (msg, &frame);
    assert (zmsg_size (msg) == 1);
    assert (zmsg_content_size (msg) == 5);
    rc = zmsg_send (&msg, output);
    assert (msg == NULL);
    assert (rc == 0);
    
    msg = zmsg_recv (input);
    assert (msg);
    assert (zmsg_size (msg) == 1);
    assert (zmsg_content_size (msg) == 5);
    zmsg_destroy (&msg);
    
    //  Test send and receive of multi-frame message
    msg = zmsg_new ();
    assert (msg);
    rc = zmsg_addmem (msg, "Frame0", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame1", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame2", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame3", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame4", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame5", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame6", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame7", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame8", 6);
    assert (rc == 0);
    rc = zmsg_addmem (msg, "Frame9", 6);
    assert (rc == 0);
    zmsg_t *copy = zmsg_dup (msg);
    assert (copy);
    rc = zmsg_send (&copy, output);
    assert (rc == 0);
    rc = zmsg_send (&msg, output);
    assert (rc == 0);
    
    copy = zmsg_recv (input);
    assert (copy);
    assert (zmsg_size (copy) == 10);
    assert (zmsg_content_size (copy) == 60);
    zmsg_destroy (&copy);
    
    msg = zmsg_recv (input);
    assert (msg);
    assert (zmsg_size (msg) == 10);
    assert (zmsg_content_size (msg) == 60);
    
    //  Save to a file, read back
    FILE *file = fopen ("zmsg.test", "w");
    assert (file);
    rc = zmsg_save (msg, file);
    assert (rc == 0);
    fclose (file);
    
    file = fopen ("zmsg.test", "r");
    rc = zmsg_save (msg, file);
    assert (rc == -1);
    fclose (file);
    zmsg_destroy (&msg);
    
    file = fopen ("zmsg.test", "r");
    msg = zmsg_load (file);
    assert (msg);
    fclose (file);
    remove ("zmsg.test");
    assert (zmsg_size (msg) == 10);
    assert (zmsg_content_size (msg) == 60);
    
    //  Remove all frames except first and last
    int frame_nbr;
    for (frame_nbr = 0; frame_nbr < 8; frame_nbr++) {
        zmsg_first (msg);
        frame = zmsg_next (msg);
        zmsg_remove (msg, frame);
        zframe_destroy (&frame);
    }
    //  Test message frame manipulation
    assert (zmsg_size (msg) == 2);
    frame = zmsg_last (msg);
    assert (zframe_streq (frame, "Frame9"));
    assert (zmsg_content_size (msg) == 12);
    frame = zframe_new ("Address", 7);
    assert (frame);
    zmsg_prepend (msg, &frame);
    assert (zmsg_size (msg) == 3);
    rc = zmsg_addstr (msg, "Body");
    assert (rc == 0);
    assert (zmsg_size (msg) == 4);
    frame = zmsg_pop (msg);
    zframe_destroy (&frame);
    assert (zmsg_size (msg) == 3);
    char *body = zmsg_popstr (msg);
    assert (streq (body, "Frame0"));
    free (body);
    zmsg_destroy (&msg);
    
    //  Test encoding/decoding
    msg = zmsg_new ();
    assert (msg);
    byte *blank = (byte *) zmalloc (100000);
    assert (blank);
    rc = zmsg_addmem (msg, blank, 0);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 1);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 253);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 254);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 255);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 256);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 65535);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 65536);
    assert (rc == 0);
    rc = zmsg_addmem (msg, blank, 65537);
    assert (rc == 0);
    free (blank);
    assert (zmsg_size (msg) == 9);
    frame = zmsg_encode (msg);
    zmsg_destroy (&msg);
    msg = zmsg_decode (frame);
    assert (msg);
    zmsg_destroy (&msg);
    zframe_destroy (&frame);
    
    //  Test submessages
    msg = zmsg_new ();
    assert (msg);
    zmsg_t *submsg = zmsg_new ();
    zmsg_pushstr (msg, "matr");
    zmsg_pushstr (submsg, "joska");
    rc = zmsg_addmsg (msg, &submsg);
    assert (rc == 0);
    assert (submsg == NULL);
    submsg = zmsg_popmsg (msg);
    assert (submsg == NULL);   // string "matr" is not encoded zmsg_t, so was discarded
    submsg = zmsg_popmsg (msg);
    assert (submsg);
    body = zmsg_popstr (submsg);
    assert (streq (body, "joska"));
    free (body);
    zmsg_destroy (&submsg);
    frame = zmsg_pop (msg);
    assert (frame == NULL);
    zmsg_destroy (&msg);
    
    //  Test comparison of two messages
    msg = zmsg_new ();
    zmsg_addstr (msg, "One");
    zmsg_addstr (msg, "Two");
    zmsg_addstr (msg, "Three");
    zmsg_t *msg_other = zmsg_new ();
    zmsg_addstr (msg_other, "One");
    zmsg_addstr (msg_other, "Two");
    zmsg_addstr (msg_other, "One-Hundred");
    zmsg_t *msg_dup = zmsg_dup (msg);
    zmsg_t *empty_msg = zmsg_new ();
    zmsg_t *empty_msg_2 = zmsg_new ();
    assert (zmsg_eq (msg, msg_dup));
    assert (!zmsg_eq (msg, msg_other));
    assert (zmsg_eq (empty_msg, empty_msg_2));
    assert (!zmsg_eq (msg, NULL));
    assert (!zmsg_eq (NULL, empty_msg));
    assert (!zmsg_eq (NULL, NULL));
    zmsg_destroy (&msg);
    zmsg_destroy (&msg_other);
    zmsg_destroy (&msg_dup);
    zmsg_destroy (&empty_msg);
    zmsg_destroy (&empty_msg_2);
    
    //  Test signal messages
    msg = zmsg_new_signal (0);
    assert (zmsg_signal (msg) == 0);
    zmsg_destroy (&msg);
    msg = zmsg_new_signal (-1);
    assert (zmsg_signal (msg) == 255);
    zmsg_destroy (&msg);
    
    //  Now try methods on an empty message
    msg = zmsg_new ();
    assert (msg);
    assert (zmsg_size (msg) == 0);
    assert (zmsg_unwrap (msg) == NULL);
    assert (zmsg_first (msg) == NULL);
    assert (zmsg_last (msg) == NULL);
    assert (zmsg_next (msg) == NULL);
    assert (zmsg_pop (msg) == NULL);
    //  Sending an empty message is valid and destroys the message
    assert (zmsg_send (&msg, output) == 0);
    assert (!msg);
    
    zsock_destroy (&input);
    zsock_destroy (&output);
    
    #if defined (ZMQ_SERVER)
    //  Create server and client sockets and connect over inproc
    zsock_t *server = zsock_new_server ("inproc://zmsg-test-routing");
    assert (server);
    zsock_t *client = zsock_new_client ("inproc://zmsg-test-routing");
    assert (client);
    
    //  Send request from client to server
    zmsg_t *request = zmsg_new ();
    assert (request);
    zmsg_addstr (request, "Hello");
    rc = zmsg_send (&request, client);
    assert (rc == 0);
    assert (!request);
    
    //  Read request and send reply
    request = zmsg_recv (server);
    assert (request);
    char *string = zmsg_popstr (request);
    assert (streq (string, "Hello"));
    assert (zmsg_routing_id (request));
    zstr_free (&string);
    
    zmsg_t *reply = zmsg_new ();
    assert (reply);
    zmsg_addstr (reply, "World");
    zmsg_set_routing_id (reply, zmsg_routing_id (request));
    rc = zmsg_send (&reply, server);
    assert (rc == 0);
    zmsg_destroy (&request);
    
    //  Read reply
    reply = zmsg_recv (client);
    string = zmsg_popstr (reply);
    assert (streq (string, "World"));
    assert (zmsg_routing_id (reply) == 0);
    zmsg_destroy (&reply);
    zstr_free (&string);
    
    //  Client and server disallow multipart
    msg = zmsg_new ();
    zmsg_addstr (msg, "One");
    zmsg_addstr (msg, "Two");
    rc = zmsg_send (&msg, client);
    assert (rc == -1);
    assert (zmsg_size (msg) == 2);
    rc = zmsg_send (&msg, server);
    assert (rc == -1);
    assert (zmsg_size (msg) == 2);
    zmsg_destroy (&msg);
    
    zsock_destroy (&client);
    zsock_destroy (&server);
    #endif
```

<A name="toc4-5062" title="zpoller - trivial socket poller class" />
#### zpoller - trivial socket poller class

The zpoller class provides a minimalist interface to ZeroMQ's zmq_poll
API, for the very common case of reading from a number of sockets.
It does not provide polling for output, nor polling on file handles.
If you need either of these, use the zmq_poll API directly.

The class implements the poller using the zmq_poller API if that exists,
else does the work itself.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    //  Create new poller, specifying zero or more readers. The list of 
    //  readers ends in a NULL. Each reader can be a zsock_t instance, a
    //  zactor_t instance, a libzmq socket (void *), or a file handle.  
    CZMQ_EXPORT zpoller_t *
        zpoller_new (void *reader, ...);
    
    //  Destroy a poller
    CZMQ_EXPORT void
        zpoller_destroy (zpoller_t **self_p);
    
    //  Add a reader to be polled. Returns 0 if OK, -1 on failure. The reader may
    //  be a libzmq void * socket, a zsock_t instance, or a zactor_t instance.   
    CZMQ_EXPORT int
        zpoller_add (zpoller_t *self, void *reader);
    
    //  Remove a reader from the poller; returns 0 if OK, -1 on failure. The reader
    //  must have been passed during construction, or in an zpoller_add () call.   
    CZMQ_EXPORT int
        zpoller_remove (zpoller_t *self, void *reader);
    
    //  Poll the registered readers for I/O, return first reader that has input.  
    //  The reader will be a libzmq void * socket, or a zsock_t or zactor_t       
    //  instance as specified in zpoller_new/zpoller_add. The timeout should be   
    //  zero or greater, or -1 to wait indefinitely. Socket priority is defined   
    //  by their order in the poll list. If you need a balanced poll, use the low 
    //  level zmq_poll method directly. If the poll call was interrupted (SIGINT),
    //  or the ZMQ context was destroyed, or the timeout expired, returns NULL.   
    //  You can test the actual exit condition by calling zpoller_expired () and  
    //  zpoller_terminated (). The timeout is in msec.                            
    CZMQ_EXPORT void *
        zpoller_wait (zpoller_t *self, int timeout);
    
    //  Return true if the last zpoller_wait () call ended because the timeout
    //  expired, without any error.                                           
    CZMQ_EXPORT bool
        zpoller_expired (zpoller_t *self);
    
    //  Return true if the last zpoller_wait () call ended because the process
    //  was interrupted, or the parent context was destroyed.                 
    CZMQ_EXPORT bool
        zpoller_terminated (zpoller_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zpoller_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  By default the poller stops if the process receives a SIGINT or SIGTERM  
    //  signal. This makes it impossible to shut-down message based architectures
    //  like zactors. This method lets you switch off break handling. The default
    //  nonstop setting is off (false).                                          
    CZMQ_EXPORT void
        zpoller_set_nonstop (zpoller_t *self, bool nonstop);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Create a few sockets
    zsock_t *vent = zsock_new (ZMQ_PUSH);
    assert (vent);
    int port_nbr = zsock_bind (vent, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    zsock_t *sink = zsock_new (ZMQ_PULL);
    assert (sink);
    int rc = zsock_connect (sink, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc != -1);
    zsock_t *bowl = zsock_new (ZMQ_PULL);
    assert (bowl);
    zsock_t *dish = zsock_new (ZMQ_PULL);
    assert (dish);
    
    //  Set up poller
    zpoller_t *poller = zpoller_new (bowl, dish, NULL);
    assert (poller);
    
    // Add a reader to the existing poller
    rc = zpoller_add (poller, sink);
    assert (rc == 0);
    
    zstr_send (vent, "Hello, World");
    
    //  We expect a message only on the sink
    zsock_t *which = (zsock_t *) zpoller_wait (poller, -1);
    assert (which == sink);
    assert (zpoller_expired (poller) == false);
    assert (zpoller_terminated (poller) == false);
    char *message = zstr_recv (which);
    assert (streq (message, "Hello, World"));
    zstr_free (&message);
    
    //  Stop polling reader
    rc = zpoller_remove (poller, sink);
    assert (rc == 0);
    
    //  Check we can poll an FD
    rc = zsock_connect (bowl, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc != -1);
    SOCKET fd = zsock_fd (bowl);
    rc = zpoller_add (poller, (void *) &fd);
    assert (rc != -1);
    zstr_send (vent, "Hello again, world");
    assert (zpoller_wait (poller, 500) == &fd);
    
    // Check zpoller_set_nonstop ()
    zsys_interrupted = 1;
    zpoller_wait (poller, 0);
    assert (zpoller_terminated (poller));
    zpoller_set_nonstop (poller, true);
    zpoller_wait (poller, 0);
    assert (!zpoller_terminated (poller));
    zsys_interrupted = 0;
    
    zpoller_destroy (&poller);
    zsock_destroy (&vent);
    zsock_destroy (&sink);
    zsock_destroy (&bowl);
    zsock_destroy (&dish);
    
    #ifdef ZMQ_SERVER
    //  Check thread safe sockets
    zpoller_destroy (&poller);
    zsock_t *client = zsock_new (ZMQ_CLIENT);
    assert (client);
    zsock_t *server = zsock_new (ZMQ_SERVER);
    assert (server);
    poller = zpoller_new (client, server, NULL);
    assert (poller);
    port_nbr = zsock_bind (server, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    rc = zsock_connect (client, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc != -1);
    
    zstr_send (client, "Hello, World");
    
    //  We expect a message only on the server
    which = (zsock_t *) zpoller_wait (poller, -1);
    assert (which == server);
    assert (zpoller_expired (poller) == false);
    assert (zpoller_terminated (poller) == false);
    message = zstr_recv (which);
    assert (streq (message, "Hello, World"));
    zstr_free (&message);
    
    zpoller_destroy (&poller);
    zsock_destroy (&client);
    zsock_destroy (&server);
    #endif
```

<A name="toc4-5233" title="zproc - process configuration and status" />
#### zproc - process configuration and status

zproc - process configuration and status

Please add @discuss section in ../src/zproc.c.

This is the class interface:

```h
    //  This is a draft class, and may change without notice. It is disabled in
    //  stable builds by default. If you use this in applications, please ask
    //  for it to be pushed to stable state. Use --enable-drafts to enable.
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  Returns CZMQ version as a single 6-digit integer encoding the major
    //  version (x 10000), the minor version (x 100) and the patch.        
    CZMQ_EXPORT int
        zproc_czmq_version (void);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Returns true if the process received a SIGINT or SIGTERM signal.
    //  It is good practice to use this method to exit any infinite loop
    //  processing messages.                                            
    CZMQ_EXPORT bool
        zproc_interrupted (void);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Returns true if the underlying libzmq supports CURVE security.
    CZMQ_EXPORT bool
        zproc_has_curve (void);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Return current host name, for use in public tcp:// endpoints.
    //  If the host name is not resolvable, returns NULL.            
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zproc_hostname (void);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Move the current process into the background. The precise effect     
    //  depends on the operating system. On POSIX boxes, moves to a specified
    //  working directory (if specified), closes all file handles, reopens   
    //  stdin, stdout, and stderr to the null device, and sets the process to
    //  ignore SIGHUP. On Windows, does nothing. Returns 0 if OK, -1 if there
    //  was an error.                                                        
    CZMQ_EXPORT void
        zproc_daemonize (const char *workdir);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Drop the process ID into the lockfile, with exclusive lock, and   
    //  switch the process to the specified group and/or user. Any of the 
    //  arguments may be null, indicating a no-op. Returns 0 on success,  
    //  -1 on failure. Note if you combine this with zsys_daemonize, run  
    //  after, not before that method, or the lockfile will hold the wrong
    //  process ID.                                                       
    CZMQ_EXPORT void
        zproc_run_as (const char *lockfile, const char *group, const char *user);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Configure the number of I/O threads that ZeroMQ will use. A good  
    //  rule of thumb is one thread per gigabit of traffic in or out. The 
    //  default is 1, sufficient for most applications. If the environment
    //  variable ZSYS_IO_THREADS is defined, that provides the default.   
    //  Note that this method is valid only before any socket is created. 
    CZMQ_EXPORT void
        zproc_set_io_threads (size_t io_threads);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Configure the number of sockets that ZeroMQ will allow. The default  
    //  is 1024. The actual limit depends on the system, and you can query it
    //  by using zsys_socket_limit (). A value of zero means "maximum".      
    //  Note that this method is valid only before any socket is created.    
    CZMQ_EXPORT void
        zproc_set_max_sockets (size_t max_sockets);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Set network interface name to use for broadcasts, particularly zbeacon.    
    //  This lets the interface be configured for test environments where required.
    //  For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is  
    //  the default when there is no specified interface. If the environment       
    //  variable ZSYS_INTERFACE is set, use that as the default interface name.    
    //  Setting the interface to "*" means "use all available interfaces".         
    CZMQ_EXPORT void
        zproc_set_biface (const char *value);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Return network interface to use for broadcasts, or "" if none was set.
    CZMQ_EXPORT const char *
        zproc_biface (void);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Set log identity, which is a string that prefixes all log messages sent
    //  by this process. The log identity defaults to the environment variable 
    //  ZSYS_LOGIDENT, if that is set.                                         
    CZMQ_EXPORT void
        zproc_set_log_ident (const char *value);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Sends log output to a PUB socket bound to the specified endpoint. To   
    //  collect such log output, create a SUB socket, subscribe to the traffic 
    //  you care about, and connect to the endpoint. Log traffic is sent as a  
    //  single string frame, in the same format as when sent to stdout. The    
    //  log system supports a single sender; multiple calls to this method will
    //  bind the same sender to multiple endpoints. To disable the sender, call
    //  this method with a null argument.                                      
    CZMQ_EXPORT void
        zproc_set_log_sender (const char *endpoint);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Enable or disable logging to the system facility (syslog on POSIX boxes,
    //  event log on Windows). By default this is disabled.                     
    CZMQ_EXPORT void
        zproc_set_log_system (bool logsystem);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Log error condition - highest priority
    CZMQ_EXPORT void
        zproc_log_error (const char *format, ...) CHECK_PRINTF (1);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Log warning condition - high priority
    CZMQ_EXPORT void
        zproc_log_warning (const char *format, ...) CHECK_PRINTF (1);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Log normal, but significant, condition - normal priority
    CZMQ_EXPORT void
        zproc_log_notice (const char *format, ...) CHECK_PRINTF (1);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Log informational message - low priority
    CZMQ_EXPORT void
        zproc_log_info (const char *format, ...) CHECK_PRINTF (1);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Log debug-level message - lowest priority
    CZMQ_EXPORT void
        zproc_log_debug (const char *format, ...) CHECK_PRINTF (1);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Self test of this class.
    CZMQ_EXPORT void
        zproc_test (bool verbose);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

Please add @selftest section in ../src/zproc.c.

<A name="toc4-5385" title="zproxy - run a steerable proxy in the background" />
#### zproxy - run a steerable proxy in the background

A zproxy actor switches messages between a frontend and a backend socket.
It acts much like the zmq_proxy_steerable method, though it makes benefit
of CZMQ's facilities, to be somewhat simpler to set-up.

This class replaces zproxy_v2, and is meant for applications that use the
CZMQ v3 API (meaning, zsock).

This is the class interface:

```h
    //  Create new zproxy actor instance. The proxy switches messages between
    //  a frontend socket and a backend socket; use the FRONTEND and BACKEND
    //  commands to configure these:
    //
    //      zactor_t *proxy = zactor_new (zproxy, NULL);
    //
    //  Destroy zproxy instance. This destroys the two sockets and stops any
    //  message flow between them:
    //
    //      zactor_destroy (&proxy);
    //
    //  Note that all zproxy commands are synchronous, so your application always
    //  waits for a signal from the actor after each command.
    //
    //  Enable verbose logging of commands and activity:
    //
    //      zstr_send (proxy, "VERBOSE");
    //      zsock_wait (proxy);
    //
    //  Specify frontend socket type -- see zsock_type_str () -- and attach to
    //  endpoints, see zsock_attach (). Note that a proxy socket is always
    //  serverish:
    //
    //      zstr_sendx (proxy, "FRONTEND", "XSUB", endpoints, NULL);
    //      zsock_wait (proxy);
    //
    //  Specify backend socket type -- see zsock_type_str () -- and attach to
    //  endpoints, see zsock_attach (). Note that a proxy socket is always
    //  serverish:
    //
    //      zstr_sendx (proxy, "BACKEND", "XPUB", endpoints, NULL);
    //      zsock_wait (proxy);
    //
    //  Capture all proxied messages; these are delivered to the application
    //  via an inproc PULL socket that you have already bound to the specified
    //  endpoint:
    //
    //      zstr_sendx (proxy, "CAPTURE", endpoint, NULL);
    //      zsock_wait (proxy);
    //
    //  Pause the proxy. A paused proxy will cease processing messages, causing
    //  them to be queued up and potentially hit the high-water mark on the
    //  frontend or backend socket, causing messages to be dropped, or writing
    //  applications to block:
    //
    //      zstr_sendx (proxy, "PAUSE", NULL);
    //      zsock_wait (proxy);
    //
    //  Resume the proxy. Note that the proxy starts automatically as soon as it
    //  has a properly attached frontend and backend socket:
    //
    //      zstr_sendx (proxy, "RESUME", NULL);
    //      zsock_wait (proxy);
    //
    //  Configure an authentication domain for the "FRONTEND" or "BACKEND" proxy
    //  socket -- see zsock_set_zap_domain (). Call before binding socket:
    //
    //      zstr_sendx (proxy, "DOMAIN", "FRONTEND", "global", NULL);
    //      zsock_wait (proxy);
    //
    //  Configure PLAIN authentication for the "FRONTEND" or "BACKEND" proxy
    //  socket -- see zsock_set_plain_server (). Call before binding socket:
    //
    //      zstr_sendx (proxy, "PLAIN", "BACKEND", NULL);
    //      zsock_wait (proxy);
    //
    //  Configure CURVE authentication for the "FRONTEND" or "BACKEND" proxy
    //  socket -- see zsock_set_curve_server () -- specifying both the public and
    //  secret keys of a certificate as Z85 armored strings -- see
    //  zcert_public_txt () and zcert_secret_txt (). Call before binding socket:
    //
    //      zstr_sendx (proxy, "CURVE", "FRONTEND", public_txt, secret_txt, NULL);
    //      zsock_wait (proxy);
    //
    //  This is the zproxy constructor as a zactor_fn; the argument is a
    //  character string specifying frontend and backend socket types as two
    //  uppercase strings separated by a hyphen:
    CZMQ_EXPORT void
        zproxy (zsock_t *pipe, void *unused);
    
    //  Selftest
    CZMQ_EXPORT void
        zproxy_test (bool verbose);
```

This is the class self test code:

```c
    //  Create and configure our proxy
    zactor_t *proxy = zactor_new (zproxy, NULL);
    assert (proxy);
    if (verbose) {
        zstr_sendx (proxy, "VERBOSE", NULL);
        zsock_wait (proxy);
    }
    zstr_sendx (proxy, "FRONTEND", "PULL", "inproc://frontend", NULL);
    zsock_wait (proxy);
    zstr_sendx (proxy, "BACKEND", "PUSH", "inproc://backend", NULL);
    zsock_wait (proxy);
    
    //  Connect application sockets to proxy
    zsock_t *faucet = zsock_new_push (">inproc://frontend");
    assert (faucet);
    zsock_t *sink = zsock_new_pull (">inproc://backend");
    assert (sink);
    
    //  Send some messages and check they arrived
    char *hello, *world;
    zstr_sendx (faucet, "Hello", "World", NULL);
    zstr_recvx (sink, &hello, &world, NULL);
    assert (streq (hello, "Hello"));
    assert (streq (world, "World"));
    zstr_free (&hello);
    zstr_free (&world);
    
    //  Test pause/resume functionality
    zstr_sendx (proxy, "PAUSE", NULL);
    zsock_wait (proxy);
    zstr_sendx (faucet, "Hello", "World", NULL);
    zsock_set_rcvtimeo (sink, 100);
    zstr_recvx (sink, &hello, &world, NULL);
    assert (!hello && !world);
    
    zstr_sendx (proxy, "RESUME", NULL);
    zsock_wait (proxy);
    zstr_recvx (sink, &hello, &world, NULL);
    assert (streq (hello, "Hello"));
    assert (streq (world, "World"));
    zstr_free (&hello);
    zstr_free (&world);
    
    //  Test capture functionality
    zsock_t *capture = zsock_new_pull ("inproc://capture");
    assert (capture);
    
    //  Switch on capturing, check that it works
    zstr_sendx (proxy, "CAPTURE", "inproc://capture", NULL);
    zsock_wait (proxy);
    zstr_sendx (faucet, "Hello", "World", NULL);
    zstr_recvx (sink, &hello, &world, NULL);
    assert (streq (hello, "Hello"));
    assert (streq (world, "World"));
    zstr_free (&hello);
    zstr_free (&world);
    
    zstr_recvx (capture, &hello, &world, NULL);
    assert (streq (hello, "Hello"));
    assert (streq (world, "World"));
    zstr_free (&hello);
    zstr_free (&world);
    
    zsock_destroy (&faucet);
    zsock_destroy (&sink);
    zsock_destroy (&capture);
    zactor_destroy (&proxy);
    
    //  Test socket creation dependency
    proxy = zactor_new (zproxy, NULL);
    assert (proxy);
    
    sink = zsock_new_sub (">ipc://backend", "whatever");
    assert (sink);
    
    zstr_sendx (proxy, "BACKEND", "XPUB", "ipc://backend", NULL);
    zsock_wait (proxy);
    
    zsock_destroy(&sink);
    zactor_destroy(&proxy);
    
    #if (ZMQ_VERSION_MAJOR == 4)
    // Test authentication functionality
    #   define TESTDIR ".test_zproxy"
    
    //  Create temporary directory for test files
    zsys_dir_create (TESTDIR);
    
    char *frontend = NULL;
    char *backend = NULL;
    
    //  Check there's no authentication
    s_create_test_sockets (&proxy, &faucet, &sink, verbose);
    s_bind_test_sockets (proxy, &frontend, &backend);
    bool success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);
    
    //  Install the authenticator
    zactor_t *auth = zactor_new (zauth, NULL);
    assert (auth);
    if (verbose) {
        zstr_sendx (auth, "VERBOSE", NULL);
        zsock_wait (auth);
    }
    
    //  Check there's no authentication on a default NULL server
    s_bind_test_sockets (proxy, &frontend, &backend);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);
    
    //  When we set a domain on the server, we switch on authentication
    //  for NULL sockets, but with no policies, the client connection
    //  will be allowed.
    zstr_sendx (proxy, "DOMAIN", "FRONTEND", "global", NULL);
    zsock_wait (proxy);
    s_bind_test_sockets (proxy, &frontend, &backend);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);
    
    //  Blacklist 127.0.0.1, connection should fail
    zstr_sendx (proxy, "DOMAIN", "FRONTEND", "global", NULL);
    zsock_wait (proxy);
    s_bind_test_sockets (proxy, &frontend, &backend);
    zstr_sendx (auth, "DENY", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (!success);
    
    //  Whitelist our address, which overrides the blacklist
    zstr_sendx (proxy, "DOMAIN", "FRONTEND", "global", NULL);
    zsock_wait (proxy);
    zstr_sendx (proxy, "DOMAIN", "BACKEND", "global", NULL);
    zsock_wait (proxy);
    s_bind_test_sockets (proxy, &frontend, &backend);
    zstr_sendx (auth, "ALLOW", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);
    
    //  Try PLAIN authentication
    
    //  Test negative case (no server-side passwords defined)
    zstr_sendx (proxy, "PLAIN", "FRONTEND", NULL);
    zsock_wait (proxy);
    s_bind_test_sockets (proxy, &frontend, &backend);
    zsock_set_plain_username (faucet, "admin");
    zsock_set_plain_password (faucet, "Password");
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (!success);
    
    //  Test positive case (server-side passwords defined)
    FILE *password = fopen (TESTDIR "/password-file", "w");
    assert (password);
    fprintf (password, "admin=Password\n");
    fclose (password);
    zstr_sendx (proxy, "PLAIN", "FRONTEND", NULL);
    zsock_wait (proxy);
    zstr_sendx (proxy, "PLAIN", "BACKEND", NULL);
    zsock_wait (proxy);
    s_bind_test_sockets (proxy, &frontend, &backend);
    zsock_set_plain_username (faucet, "admin");
    zsock_set_plain_password (faucet, "Password");
    zsock_set_plain_username (sink, "admin");
    zsock_set_plain_password (sink, "Password");
    zstr_sendx (auth, "PLAIN", TESTDIR "/password-file", NULL);
    zsock_wait (auth);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);
    
    //  Test negative case (bad client password)
    zstr_sendx (proxy, "PLAIN", "FRONTEND", NULL);
    zsock_wait (proxy);
    s_bind_test_sockets (proxy, &frontend, &backend);
    zsock_set_plain_username (faucet, "admin");
    zsock_set_plain_password (faucet, "Bogus");
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (!success);
    
    if (zsys_has_curve ()) {
        //  We'll create two new certificates and save the client public
        //  certificate on disk
        zcert_t *server_cert = zcert_new ();
        assert (server_cert);
        zcert_t *client_cert = zcert_new ();
        assert (client_cert);
        const char *public_key = zcert_public_txt (server_cert);
        const char *secret_key = zcert_secret_txt (server_cert);
    
        //  Try CURVE authentication
    
        //  Test without setting-up any authentication
        zstr_sendx (proxy, "CURVE", "FRONTEND", public_key, secret_key, NULL);
        zsock_wait (proxy);
        s_bind_test_sockets (proxy, &frontend, &backend);
        zcert_apply (client_cert, faucet);
        zsock_set_curve_serverkey (faucet, public_key);
        success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
        assert (!success);
    
        //  Test CURVE_ALLOW_ANY
        zstr_sendx (proxy, "CURVE", "FRONTEND", public_key, secret_key, NULL);
        zsock_wait (proxy);
        s_bind_test_sockets (proxy, &frontend, &backend);
        zcert_apply (client_cert, faucet);
        zsock_set_curve_serverkey (faucet, public_key);
        zstr_sendx (auth, "CURVE", CURVE_ALLOW_ANY, NULL);
        zsock_wait (auth);
        success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
        assert (success);
    
        //  Test with client certificate file in authentication folder
        zstr_sendx (proxy, "CURVE", "FRONTEND", public_key, secret_key, NULL);
        zsock_wait (proxy);
        zstr_sendx (proxy, "CURVE", "BACKEND", public_key, secret_key, NULL);
        zsock_wait (proxy);
        s_bind_test_sockets (proxy, &frontend, &backend);
        zcert_apply (client_cert, faucet);
        zsock_set_curve_serverkey (faucet, public_key);
        zcert_apply (client_cert, sink);
        zsock_set_curve_serverkey (sink, public_key);
        zcert_save_public (client_cert, TESTDIR "/mycert.txt");
        zstr_sendx (auth, "CURVE", TESTDIR, NULL);
        zsock_wait (auth);
        success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
        assert (success);
    
        zcert_destroy (&server_cert);
        zcert_destroy (&client_cert);
    }
    
    //  Remove the authenticator and check a normal connection works
    zactor_destroy (&auth);
    s_bind_test_sockets (proxy, &frontend, &backend);
    success = s_can_connect (&proxy, &faucet, &sink, frontend, backend, verbose);
    assert (success);
    
    //  Cleanup
    zsock_destroy (&faucet);
    zsock_destroy (&sink);
    zactor_destroy (&proxy);
    zstr_free (&frontend);
    zstr_free (&backend);
    
    //  Delete temporary directory and test files
    zsys_file_delete (TESTDIR "/password-file");
    zsys_file_delete (TESTDIR "/mycert.txt");
    zsys_dir_delete (TESTDIR);
    #endif
```

<A name="toc4-5736" title="zrex - work with regular expressions" />
#### zrex - work with regular expressions

Wraps a very simple regular expression library (SLRE) as a CZMQ class.
Supports this syntax:

    ^               Match beginning of a buffer
    $               Match end of a buffer
    ()              Grouping and substring capturing
    [...]           Match any character from set
    [^...]          Match any character but ones from set
    .               Match any character
    \s              Match whitespace
    \S              Match non-whitespace
    \d              Match decimal digit
    \D              Match non decimal digit
    \a              Match alphabetic character
    \A              Match non-alphabetic character
    \w              Match alphanumeric character
    \W              Match non-alphanumeric character
    \r              Match carriage return
    \n              Match newline
    +               Match one or more times (greedy)
    +?              Match one or more times (non-greedy)
    *               Match zero or more times (greedy)
    *?              Match zero or more times (non-greedy)
    ?               Match zero or once
    \xDD            Match byte with hex value 0xDD
    \meta           Match one of the meta character: ^$().[*+?\

Please add @discuss section in ../src/zrex.c.

This is the class interface:

```h
    //  Constructor. Optionally, sets an expression against which we can match
    //  text and capture hits. If there is an error in the expression, reports
    //  zrex_valid() as false and provides the error in zrex_strerror(). If you
    //  set a pattern, you can call zrex_matches() to test it against text.
    CZMQ_EXPORT zrex_t *
        zrex_new (const char *expression);
    
    //  Destructor
    CZMQ_EXPORT void
        zrex_destroy (zrex_t **self_p);
    
    //  Return true if the expression was valid and compiled without errors.
    CZMQ_EXPORT bool
        zrex_valid (zrex_t *self);
    
    //  Return the error message generated during compilation of the expression.
    CZMQ_EXPORT const char *
        zrex_strerror (zrex_t *self);
    
    //  Returns true if the text matches the previously compiled expression.
    //  Use this method to compare one expression against many strings.
    CZMQ_EXPORT bool
        zrex_matches (zrex_t *self, const char *text);
    
    //  Returns true if the text matches the supplied expression. Use this
    //  method to compare one string against several expressions.
    CZMQ_EXPORT bool
        zrex_eq (zrex_t *self, const char *text, const char *expression);
    
    //  Returns number of hits from last zrex_matches or zrex_eq. If the text
    //  matched, returns 1 plus the number of capture groups. If the text did
    //  not match, returns zero. To retrieve individual capture groups, call
    //  zrex_hit ().
    CZMQ_EXPORT int
        zrex_hits (zrex_t *self);
    
    //  Returns the Nth capture group from the last expression match, where
    //  N is 0 to the value returned by zrex_hits(). Capture group 0 is the
    //  whole matching string. Sequence 1 is the first capture group, if any,
    //  and so on.
    CZMQ_EXPORT const char *
        zrex_hit (zrex_t *self, uint index);
    
    //  Fetches hits into string variables provided by caller; this makes for
    //  nicer code than accessing hits by index. Caller should not modify nor
    //  free the returned values. Returns number of strings returned. This
    //  method starts at hit 1, i.e. first capture group, as hit 0 is always
    //  the original matched string.
    CZMQ_EXPORT int
        zrex_fetch (zrex_t *self, const char **string_p, ...);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zrex_test (bool verbose);
```

This is the class self test code:

```c
    //  This shows the pattern of matching many lines to a single pattern
    zrex_t *rex = zrex_new ("\\d+-\\d+-\\d+");
    assert (rex);
    assert (zrex_valid (rex));
    bool matches = zrex_matches (rex, "123-456-789");
    assert (matches);
    assert (zrex_hits (rex) == 1);
    assert (streq (zrex_hit (rex, 0), "123-456-789"));
    assert (zrex_hit (rex, 1) == NULL);
    zrex_destroy (&rex);
    
    //  Here we pick out hits using capture groups
    rex = zrex_new ("(\\d+)-(\\d+)-(\\d+)");
    assert (rex);
    assert (zrex_valid (rex));
    matches = zrex_matches (rex, "123-456-ABC");
    assert (!matches);
    matches = zrex_matches (rex, "123-456-789");
    assert (matches);
    assert (zrex_hits (rex) == 4);
    assert (streq (zrex_hit (rex, 0), "123-456-789"));
    assert (streq (zrex_hit (rex, 1), "123"));
    assert (streq (zrex_hit (rex, 2), "456"));
    assert (streq (zrex_hit (rex, 3), "789"));
    zrex_destroy (&rex);
    
    //  This shows the pattern of matching one line against many
    //  patterns and then handling the case when it hits
    rex = zrex_new (NULL);      //  No initial pattern
    assert (rex);
    char *input = "Mechanism: CURVE";
    matches = zrex_eq (rex, input, "Version: (.+)");
    assert (!matches);
    assert (zrex_hits (rex) == 0);
    matches = zrex_eq (rex, input, "Mechanism: (.+)");
    assert (matches);
    assert (zrex_hits (rex) == 2);
    const char *mechanism;
    zrex_fetch (rex, &mechanism, NULL);
    assert (streq (zrex_hit (rex, 1), "CURVE"));
    assert (streq (mechanism, "CURVE"));
    zrex_destroy (&rex);
    
```

<A name="toc4-5875" title="zsock - high-level socket API that hides libzmq contexts and sockets" />
#### zsock - high-level socket API that hides libzmq contexts and sockets

The zsock class wraps the libzmq socket handle (a void *) with a proper
structure that follows the CLASS rules for construction and destruction.
Some zsock methods take a void * "polymorphic" reference, which can be
either a zsock_t or a zactor_t reference, or a libzmq void *.

Please add @discuss section in ../src/zsock.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    //  Create a new socket. Returns the new socket, or NULL if the new socket
    //  could not be created. Note that the symbol zsock_new (and other       
    //  constructors/destructors for zsock) are redirected to the *_checked   
    //  variant, enabling intelligent socket leak detection. This can have    
    //  performance implications if you use a LOT of sockets. To turn off this
    //  redirection behaviour, define ZSOCK_NOCHECK.                          
    CZMQ_EXPORT zsock_t *
        zsock_new (int type);
    
    //  Create a PUB socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_pub (const char *endpoint);
    
    //  Create a SUB socket, and optionally subscribe to some prefix string. Default
    //  action is connect.                                                          
    CZMQ_EXPORT zsock_t *
        zsock_new_sub (const char *endpoint, const char *subscribe);
    
    //  Create a REQ socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_req (const char *endpoint);
    
    //  Create a REP socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_rep (const char *endpoint);
    
    //  Create a DEALER socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_dealer (const char *endpoint);
    
    //  Create a ROUTER socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_router (const char *endpoint);
    
    //  Create a PUSH socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_push (const char *endpoint);
    
    //  Create a PULL socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_pull (const char *endpoint);
    
    //  Create an XPUB socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_xpub (const char *endpoint);
    
    //  Create an XSUB socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_xsub (const char *endpoint);
    
    //  Create a PAIR socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_pair (const char *endpoint);
    
    //  Create a STREAM socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_stream (const char *endpoint);
    
    //  Destroy the socket. You must use this for any socket created via the
    //  zsock_new method.                                                   
    CZMQ_EXPORT void
        zsock_destroy (zsock_t **self_p);
    
    //  Bind a socket to a formatted endpoint. For tcp:// endpoints, supports   
    //  ephemeral ports, if you specify the port number as "*". By default      
    //  zsock uses the IANA designated range from C000 (49152) to FFFF (65535). 
    //  To override this range, follow the "*" with "[first-last]". Either or   
    //  both first and last may be empty. To bind to a random port within the   
    //  range, use "!" in place of "*".                                         
    //                                                                          
    //  Examples:                                                               
    //      tcp://127.0.0.1:*           bind to first free port from C000 up    
    //      tcp://127.0.0.1:!           bind to random port from C000 to FFFF   
    //      tcp://127.0.0.1:*[60000-]   bind to first free port from 60000 up   
    //      tcp://127.0.0.1:![-60000]   bind to random port from C000 to 60000  
    //      tcp://127.0.0.1:![55000-55999]                                      
    //                                  bind to random port from 55000 to 55999 
    //                                                                          
    //  On success, returns the actual port number used, for tcp:// endpoints,  
    //  and 0 for other transports. On failure, returns -1. Note that when using
    //  ephemeral ports, a port may be reused by different services without     
    //  clients being aware. Protocols that run on ephemeral ports should take  
    //  this into account.                                                      
    CZMQ_EXPORT int
        zsock_bind (zsock_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Returns last bound endpoint, if any.
    CZMQ_EXPORT const char *
        zsock_endpoint (zsock_t *self);
    
    //  Unbind a socket from a formatted endpoint.                     
    //  Returns 0 if OK, -1 if the endpoint was invalid or the function
    //  isn't supported.                                               
    CZMQ_EXPORT int
        zsock_unbind (zsock_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Connect a socket to a formatted endpoint        
    //  Returns 0 if OK, -1 if the endpoint was invalid.
    CZMQ_EXPORT int
        zsock_connect (zsock_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Disconnect a socket from a formatted endpoint                  
    //  Returns 0 if OK, -1 if the endpoint was invalid or the function
    //  isn't supported.                                               
    CZMQ_EXPORT int
        zsock_disconnect (zsock_t *self, const char *format, ...) CHECK_PRINTF (2);
    
    //  Attach a socket to zero or more endpoints. If endpoints is not null,     
    //  parses as list of ZeroMQ endpoints, separated by commas, and prefixed by 
    //  '@' (to bind the socket) or '>' (to connect the socket). Returns 0 if all
    //  endpoints were valid, or -1 if there was a syntax error. If the endpoint 
    //  does not start with '@' or '>', the serverish argument defines whether   
    //  it is used to bind (serverish = true) or connect (serverish = false).    
    CZMQ_EXPORT int
        zsock_attach (zsock_t *self, const char *endpoints, bool serverish);
    
    //  Returns socket type as printable constant string.
    CZMQ_EXPORT const char *
        zsock_type_str (zsock_t *self);
    
    //  Send a 'picture' message to the socket (or actor). The picture is a   
    //  string that defines the type of each frame. This makes it easy to send
    //  a complex multiframe message in one call. The picture can contain any 
    //  of these characters, each corresponding to one or two arguments:      
    //                                                                        
    //      i = int (signed)                                                  
    //      1 = uint8_t                                                       
    //      2 = uint16_t                                                      
    //      4 = uint32_t                                                      
    //      8 = uint64_t                                                      
    //      s = char *                                                        
    //      b = byte *, size_t (2 arguments)                                  
    //      c = zchunk_t *                                                    
    //      f = zframe_t *                                                    
    //      h = zhashx_t *                                                    
    //      U = zuuid_t *                                                     
    //      p = void * (sends the pointer value, only meaningful over inproc) 
    //      m = zmsg_t * (sends all frames in the zmsg)                       
    //      z = sends zero-sized frame (0 arguments)                          
    //      u = uint (deprecated)                                             
    //                                                                        
    //  Note that s, b, c, and f are encoded the same way and the choice is   
    //  offered as a convenience to the sender, which may or may not already  
    //  have data in a zchunk or zframe. Does not change or take ownership of 
    //  any arguments. Returns 0 if successful, -1 if sending failed for any  
    //  reason.                                                               
    CZMQ_EXPORT int
        zsock_send (void *self, const char *picture, ...);
    
    //  Send a 'picture' message to the socket (or actor). This is a va_list 
    //  version of zsock_send (), so please consult its documentation for the
    //  details.                                                             
    CZMQ_EXPORT int
        zsock_vsend (void *self, const char *picture, va_list argptr);
    
    //  Receive a 'picture' message to the socket (or actor). See zsock_send for
    //  the format and meaning of the picture. Returns the picture elements into
    //  a series of pointers as provided by the caller:                         
    //                                                                          
    //      i = int * (stores signed integer)                                   
    //      4 = uint32_t * (stores 32-bit unsigned integer)                     
    //      8 = uint64_t * (stores 64-bit unsigned integer)                     
    //      s = char ** (allocates new string)                                  
    //      b = byte **, size_t * (2 arguments) (allocates memory)              
    //      c = zchunk_t ** (creates zchunk)                                    
    //      f = zframe_t ** (creates zframe)                                    
    //      U = zuuid_t * (creates a zuuid with the data)                       
    //      h = zhashx_t ** (creates zhashx)                                    
    //      p = void ** (stores pointer)                                        
    //      m = zmsg_t ** (creates a zmsg with the remaing frames)              
    //      z = null, asserts empty frame (0 arguments)                         
    //      u = uint * (stores unsigned integer, deprecated)                    
    //                                                                          
    //  Note that zsock_recv creates the returned objects, and the caller must  
    //  destroy them when finished with them. The supplied pointers do not need 
    //  to be initialized. Returns 0 if successful, or -1 if it failed to recv  
    //  a message, in which case the pointers are not modified. When message    
    //  frames are truncated (a short message), sets return values to zero/null.
    //  If an argument pointer is NULL, does not store any value (skips it).    
    //  An 'n' picture matches an empty frame; if the message does not match,   
    //  the method will return -1.                                              
    CZMQ_EXPORT int
        zsock_recv (void *self, const char *picture, ...);
    
    //  Receive a 'picture' message from the socket (or actor). This is a    
    //  va_list version of zsock_recv (), so please consult its documentation
    //  for the details.                                                     
    CZMQ_EXPORT int
        zsock_vrecv (void *self, const char *picture, va_list argptr);
    
    //  Send a binary encoded 'picture' message to the socket (or actor). This 
    //  method is similar to zsock_send, except the arguments are encoded in a 
    //  binary format that is compatible with zproto, and is designed to reduce
    //  memory allocations. The pattern argument is a string that defines the  
    //  type of each argument. Supports these argument types:                  
    //                                                                         
    //   pattern    C type                  zproto type:                       
    //      1       uint8_t                 type = "number" size = "1"         
    //      2       uint16_t                type = "number" size = "2"         
    //      4       uint32_t                type = "number" size = "3"         
    //      8       uint64_t                type = "number" size = "4"         
    //      s       char *, 0-255 chars     type = "string"                    
    //      S       char *, 0-2^32-1 chars  type = "longstr"                   
    //      c       zchunk_t *              type = "chunk"                     
    //      f       zframe_t *              type = "frame"                     
    //      u       zuuid_t *               type = "uuid"                      
    //      m       zmsg_t *                type = "msg"                       
    //      p       void *, sends pointer value, only over inproc              
    //                                                                         
    //  Does not change or take ownership of any arguments. Returns 0 if       
    //  successful, -1 if sending failed for any reason.                       
    CZMQ_EXPORT int
        zsock_bsend (void *self, const char *picture, ...);
    
    //  Receive a binary encoded 'picture' message from the socket (or actor).  
    //  This method is similar to zsock_recv, except the arguments are encoded  
    //  in a binary format that is compatible with zproto, and is designed to   
    //  reduce memory allocations. The pattern argument is a string that defines
    //  the type of each argument. See zsock_bsend for the supported argument   
    //  types. All arguments must be pointers; this call sets them to point to  
    //  values held on a per-socket basis.                                      
    //  Note that zsock_brecv creates the returned objects, and the caller must 
    //  destroy them when finished with them. The supplied pointers do not need 
    //  to be initialized. Returns 0 if successful, or -1 if it failed to read  
    //  a message.                                                              
    CZMQ_EXPORT int
        zsock_brecv (void *self, const char *picture, ...);
    
    //  Set socket to use unbounded pipes (HWM=0); use this in cases when you are
    //  totally certain the message volume can fit in memory. This method works  
    //  across all versions of ZeroMQ. Takes a polymorphic socket reference.     
    CZMQ_EXPORT void
        zsock_set_unbounded (void *self);
    
    //  Send a signal over a socket. A signal is a short message carrying a   
    //  success/failure code (by convention, 0 means OK). Signals are encoded 
    //  to be distinguishable from "normal" messages. Accepts a zsock_t or a  
    //  zactor_t argument, and returns 0 if successful, -1 if the signal could
    //  not be sent. Takes a polymorphic socket reference.                    
    CZMQ_EXPORT int
        zsock_signal (void *self, byte status);
    
    //  Wait on a signal. Use this to coordinate between threads, over pipe  
    //  pairs. Blocks until the signal is received. Returns -1 on error, 0 or
    //  greater on success. Accepts a zsock_t or a zactor_t as argument.     
    //  Takes a polymorphic socket reference.                                
    CZMQ_EXPORT int
        zsock_wait (void *self);
    
    //  If there is a partial message still waiting on the socket, remove and    
    //  discard it. This is useful when reading partial messages, to get specific
    //  message types.                                                           
    CZMQ_EXPORT void
        zsock_flush (void *self);
    
    //  Probe the supplied object, and report if it looks like a zsock_t.
    //  Takes a polymorphic socket reference.                            
    CZMQ_EXPORT bool
        zsock_is (void *self);
    
    //  Probe the supplied reference. If it looks like a zsock_t instance, return
    //  the underlying libzmq socket handle; else if it looks like a file        
    //  descriptor, return NULL; else if it looks like a libzmq socket handle,   
    //  return the supplied value. Takes a polymorphic socket reference.         
    CZMQ_EXPORT void *
        zsock_resolve (void *self);
    
    //  Get socket option `heartbeat_ivl`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_heartbeat_ivl (void *self);
    
    //  Set socket option `heartbeat_ivl`.
    CZMQ_EXPORT void
        zsock_set_heartbeat_ivl (void *self, int heartbeat_ivl);
    
    //  Get socket option `heartbeat_ttl`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_heartbeat_ttl (void *self);
    
    //  Set socket option `heartbeat_ttl`.
    CZMQ_EXPORT void
        zsock_set_heartbeat_ttl (void *self, int heartbeat_ttl);
    
    //  Get socket option `heartbeat_timeout`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_heartbeat_timeout (void *self);
    
    //  Set socket option `heartbeat_timeout`.
    CZMQ_EXPORT void
        zsock_set_heartbeat_timeout (void *self, int heartbeat_timeout);
    
    //  Get socket option `use_fd`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_use_fd (void *self);
    
    //  Set socket option `use_fd`.
    CZMQ_EXPORT void
        zsock_set_use_fd (void *self, int use_fd);
    
    //  Set socket option `xpub_manual`.
    CZMQ_EXPORT void
        zsock_set_xpub_manual (void *self, int xpub_manual);
    
    //  Set socket option `xpub_welcome_msg`.
    CZMQ_EXPORT void
        zsock_set_xpub_welcome_msg (void *self, const char *xpub_welcome_msg);
    
    //  Set socket option `stream_notify`.
    CZMQ_EXPORT void
        zsock_set_stream_notify (void *self, int stream_notify);
    
    //  Get socket option `invert_matching`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_invert_matching (void *self);
    
    //  Set socket option `invert_matching`.
    CZMQ_EXPORT void
        zsock_set_invert_matching (void *self, int invert_matching);
    
    //  Set socket option `xpub_verboser`.
    CZMQ_EXPORT void
        zsock_set_xpub_verboser (void *self, int xpub_verboser);
    
    //  Get socket option `connect_timeout`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_connect_timeout (void *self);
    
    //  Set socket option `connect_timeout`.
    CZMQ_EXPORT void
        zsock_set_connect_timeout (void *self, int connect_timeout);
    
    //  Get socket option `tcp_maxrt`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_tcp_maxrt (void *self);
    
    //  Set socket option `tcp_maxrt`.
    CZMQ_EXPORT void
        zsock_set_tcp_maxrt (void *self, int tcp_maxrt);
    
    //  Get socket option `thread_safe`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_thread_safe (void *self);
    
    //  Get socket option `multicast_maxtpdu`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_multicast_maxtpdu (void *self);
    
    //  Set socket option `multicast_maxtpdu`.
    CZMQ_EXPORT void
        zsock_set_multicast_maxtpdu (void *self, int multicast_maxtpdu);
    
    //  Get socket option `vmci_buffer_size`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_vmci_buffer_size (void *self);
    
    //  Set socket option `vmci_buffer_size`.
    CZMQ_EXPORT void
        zsock_set_vmci_buffer_size (void *self, int vmci_buffer_size);
    
    //  Get socket option `vmci_buffer_min_size`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_vmci_buffer_min_size (void *self);
    
    //  Set socket option `vmci_buffer_min_size`.
    CZMQ_EXPORT void
        zsock_set_vmci_buffer_min_size (void *self, int vmci_buffer_min_size);
    
    //  Get socket option `vmci_buffer_max_size`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_vmci_buffer_max_size (void *self);
    
    //  Set socket option `vmci_buffer_max_size`.
    CZMQ_EXPORT void
        zsock_set_vmci_buffer_max_size (void *self, int vmci_buffer_max_size);
    
    //  Get socket option `vmci_connect_timeout`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_vmci_connect_timeout (void *self);
    
    //  Set socket option `vmci_connect_timeout`.
    CZMQ_EXPORT void
        zsock_set_vmci_connect_timeout (void *self, int vmci_connect_timeout);
    
    //  Get socket option `tos`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_tos (void *self);
    
    //  Set socket option `tos`.
    CZMQ_EXPORT void
        zsock_set_tos (void *self, int tos);
    
    //  Set socket option `router_handover`.
    CZMQ_EXPORT void
        zsock_set_router_handover (void *self, int router_handover);
    
    //  Set socket option `connect_rid`.
    CZMQ_EXPORT void
        zsock_set_connect_rid (void *self, const char *connect_rid);
    
    //  Set socket option `connect_rid` from 32-octet binary
    CZMQ_EXPORT void
        zsock_set_connect_rid_bin (void *self, const byte *connect_rid);
    
    //  Get socket option `handshake_ivl`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_handshake_ivl (void *self);
    
    //  Set socket option `handshake_ivl`.
    CZMQ_EXPORT void
        zsock_set_handshake_ivl (void *self, int handshake_ivl);
    
    //  Get socket option `socks_proxy`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_socks_proxy (void *self);
    
    //  Set socket option `socks_proxy`.
    CZMQ_EXPORT void
        zsock_set_socks_proxy (void *self, const char *socks_proxy);
    
    //  Set socket option `xpub_nodrop`.
    CZMQ_EXPORT void
        zsock_set_xpub_nodrop (void *self, int xpub_nodrop);
    
    //  Set socket option `router_mandatory`.
    CZMQ_EXPORT void
        zsock_set_router_mandatory (void *self, int router_mandatory);
    
    //  Set socket option `probe_router`.
    CZMQ_EXPORT void
        zsock_set_probe_router (void *self, int probe_router);
    
    //  Set socket option `req_relaxed`.
    CZMQ_EXPORT void
        zsock_set_req_relaxed (void *self, int req_relaxed);
    
    //  Set socket option `req_correlate`.
    CZMQ_EXPORT void
        zsock_set_req_correlate (void *self, int req_correlate);
    
    //  Set socket option `conflate`.
    CZMQ_EXPORT void
        zsock_set_conflate (void *self, int conflate);
    
    //  Get socket option `zap_domain`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_zap_domain (void *self);
    
    //  Set socket option `zap_domain`.
    CZMQ_EXPORT void
        zsock_set_zap_domain (void *self, const char *zap_domain);
    
    //  Get socket option `mechanism`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_mechanism (void *self);
    
    //  Get socket option `plain_server`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_plain_server (void *self);
    
    //  Set socket option `plain_server`.
    CZMQ_EXPORT void
        zsock_set_plain_server (void *self, int plain_server);
    
    //  Get socket option `plain_username`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_plain_username (void *self);
    
    //  Set socket option `plain_username`.
    CZMQ_EXPORT void
        zsock_set_plain_username (void *self, const char *plain_username);
    
    //  Get socket option `plain_password`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_plain_password (void *self);
    
    //  Set socket option `plain_password`.
    CZMQ_EXPORT void
        zsock_set_plain_password (void *self, const char *plain_password);
    
    //  Get socket option `curve_server`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_curve_server (void *self);
    
    //  Set socket option `curve_server`.
    CZMQ_EXPORT void
        zsock_set_curve_server (void *self, int curve_server);
    
    //  Get socket option `curve_publickey`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_curve_publickey (void *self);
    
    //  Set socket option `curve_publickey`.
    CZMQ_EXPORT void
        zsock_set_curve_publickey (void *self, const char *curve_publickey);
    
    //  Set socket option `curve_publickey` from 32-octet binary
    CZMQ_EXPORT void
        zsock_set_curve_publickey_bin (void *self, const byte *curve_publickey);
    
    //  Get socket option `curve_secretkey`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_curve_secretkey (void *self);
    
    //  Set socket option `curve_secretkey`.
    CZMQ_EXPORT void
        zsock_set_curve_secretkey (void *self, const char *curve_secretkey);
    
    //  Set socket option `curve_secretkey` from 32-octet binary
    CZMQ_EXPORT void
        zsock_set_curve_secretkey_bin (void *self, const byte *curve_secretkey);
    
    //  Get socket option `curve_serverkey`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_curve_serverkey (void *self);
    
    //  Set socket option `curve_serverkey`.
    CZMQ_EXPORT void
        zsock_set_curve_serverkey (void *self, const char *curve_serverkey);
    
    //  Set socket option `curve_serverkey` from 32-octet binary
    CZMQ_EXPORT void
        zsock_set_curve_serverkey_bin (void *self, const byte *curve_serverkey);
    
    //  Get socket option `gssapi_server`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_gssapi_server (void *self);
    
    //  Set socket option `gssapi_server`.
    CZMQ_EXPORT void
        zsock_set_gssapi_server (void *self, int gssapi_server);
    
    //  Get socket option `gssapi_plaintext`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_gssapi_plaintext (void *self);
    
    //  Set socket option `gssapi_plaintext`.
    CZMQ_EXPORT void
        zsock_set_gssapi_plaintext (void *self, int gssapi_plaintext);
    
    //  Get socket option `gssapi_principal`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_gssapi_principal (void *self);
    
    //  Set socket option `gssapi_principal`.
    CZMQ_EXPORT void
        zsock_set_gssapi_principal (void *self, const char *gssapi_principal);
    
    //  Get socket option `gssapi_service_principal`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_gssapi_service_principal (void *self);
    
    //  Set socket option `gssapi_service_principal`.
    CZMQ_EXPORT void
        zsock_set_gssapi_service_principal (void *self, const char *gssapi_service_principal);
    
    //  Get socket option `ipv6`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_ipv6 (void *self);
    
    //  Set socket option `ipv6`.
    CZMQ_EXPORT void
        zsock_set_ipv6 (void *self, int ipv6);
    
    //  Get socket option `immediate`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_immediate (void *self);
    
    //  Set socket option `immediate`.
    CZMQ_EXPORT void
        zsock_set_immediate (void *self, int immediate);
    
    //  Set socket option `router_raw`.
    CZMQ_EXPORT void
        zsock_set_router_raw (void *self, int router_raw);
    
    //  Get socket option `ipv4only`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_ipv4only (void *self);
    
    //  Set socket option `ipv4only`.
    CZMQ_EXPORT void
        zsock_set_ipv4only (void *self, int ipv4only);
    
    //  Set socket option `delay_attach_on_connect`.
    CZMQ_EXPORT void
        zsock_set_delay_attach_on_connect (void *self, int delay_attach_on_connect);
    
    //  Get socket option `type`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_type (void *self);
    
    //  Get socket option `sndhwm`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_sndhwm (void *self);
    
    //  Set socket option `sndhwm`.
    CZMQ_EXPORT void
        zsock_set_sndhwm (void *self, int sndhwm);
    
    //  Get socket option `rcvhwm`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_rcvhwm (void *self);
    
    //  Set socket option `rcvhwm`.
    CZMQ_EXPORT void
        zsock_set_rcvhwm (void *self, int rcvhwm);
    
    //  Get socket option `affinity`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_affinity (void *self);
    
    //  Set socket option `affinity`.
    CZMQ_EXPORT void
        zsock_set_affinity (void *self, int affinity);
    
    //  Set socket option `subscribe`.
    CZMQ_EXPORT void
        zsock_set_subscribe (void *self, const char *subscribe);
    
    //  Set socket option `unsubscribe`.
    CZMQ_EXPORT void
        zsock_set_unsubscribe (void *self, const char *unsubscribe);
    
    //  Get socket option `identity`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_identity (void *self);
    
    //  Set socket option `identity`.
    CZMQ_EXPORT void
        zsock_set_identity (void *self, const char *identity);
    
    //  Get socket option `rate`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_rate (void *self);
    
    //  Set socket option `rate`.
    CZMQ_EXPORT void
        zsock_set_rate (void *self, int rate);
    
    //  Get socket option `recovery_ivl`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_recovery_ivl (void *self);
    
    //  Set socket option `recovery_ivl`.
    CZMQ_EXPORT void
        zsock_set_recovery_ivl (void *self, int recovery_ivl);
    
    //  Get socket option `sndbuf`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_sndbuf (void *self);
    
    //  Set socket option `sndbuf`.
    CZMQ_EXPORT void
        zsock_set_sndbuf (void *self, int sndbuf);
    
    //  Get socket option `rcvbuf`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_rcvbuf (void *self);
    
    //  Set socket option `rcvbuf`.
    CZMQ_EXPORT void
        zsock_set_rcvbuf (void *self, int rcvbuf);
    
    //  Get socket option `linger`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_linger (void *self);
    
    //  Set socket option `linger`.
    CZMQ_EXPORT void
        zsock_set_linger (void *self, int linger);
    
    //  Get socket option `reconnect_ivl`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_reconnect_ivl (void *self);
    
    //  Set socket option `reconnect_ivl`.
    CZMQ_EXPORT void
        zsock_set_reconnect_ivl (void *self, int reconnect_ivl);
    
    //  Get socket option `reconnect_ivl_max`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_reconnect_ivl_max (void *self);
    
    //  Set socket option `reconnect_ivl_max`.
    CZMQ_EXPORT void
        zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max);
    
    //  Get socket option `backlog`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_backlog (void *self);
    
    //  Set socket option `backlog`.
    CZMQ_EXPORT void
        zsock_set_backlog (void *self, int backlog);
    
    //  Get socket option `maxmsgsize`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_maxmsgsize (void *self);
    
    //  Set socket option `maxmsgsize`.
    CZMQ_EXPORT void
        zsock_set_maxmsgsize (void *self, int maxmsgsize);
    
    //  Get socket option `multicast_hops`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_multicast_hops (void *self);
    
    //  Set socket option `multicast_hops`.
    CZMQ_EXPORT void
        zsock_set_multicast_hops (void *self, int multicast_hops);
    
    //  Get socket option `rcvtimeo`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_rcvtimeo (void *self);
    
    //  Set socket option `rcvtimeo`.
    CZMQ_EXPORT void
        zsock_set_rcvtimeo (void *self, int rcvtimeo);
    
    //  Get socket option `sndtimeo`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_sndtimeo (void *self);
    
    //  Set socket option `sndtimeo`.
    CZMQ_EXPORT void
        zsock_set_sndtimeo (void *self, int sndtimeo);
    
    //  Set socket option `xpub_verbose`.
    CZMQ_EXPORT void
        zsock_set_xpub_verbose (void *self, int xpub_verbose);
    
    //  Get socket option `tcp_keepalive`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_tcp_keepalive (void *self);
    
    //  Set socket option `tcp_keepalive`.
    CZMQ_EXPORT void
        zsock_set_tcp_keepalive (void *self, int tcp_keepalive);
    
    //  Get socket option `tcp_keepalive_idle`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_tcp_keepalive_idle (void *self);
    
    //  Set socket option `tcp_keepalive_idle`.
    CZMQ_EXPORT void
        zsock_set_tcp_keepalive_idle (void *self, int tcp_keepalive_idle);
    
    //  Get socket option `tcp_keepalive_cnt`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_tcp_keepalive_cnt (void *self);
    
    //  Set socket option `tcp_keepalive_cnt`.
    CZMQ_EXPORT void
        zsock_set_tcp_keepalive_cnt (void *self, int tcp_keepalive_cnt);
    
    //  Get socket option `tcp_keepalive_intvl`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_tcp_keepalive_intvl (void *self);
    
    //  Set socket option `tcp_keepalive_intvl`.
    CZMQ_EXPORT void
        zsock_set_tcp_keepalive_intvl (void *self, int tcp_keepalive_intvl);
    
    //  Get socket option `tcp_accept_filter`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_tcp_accept_filter (void *self);
    
    //  Set socket option `tcp_accept_filter`.
    CZMQ_EXPORT void
        zsock_set_tcp_accept_filter (void *self, const char *tcp_accept_filter);
    
    //  Get socket option `rcvmore`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_rcvmore (void *self);
    
    //  Get socket option `fd`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT SOCKET
        zsock_fd (void *self);
    
    //  Get socket option `events`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT int
        zsock_events (void *self);
    
    //  Get socket option `last_endpoint`.
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zsock_last_endpoint (void *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zsock_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  Create a SERVER socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_server (const char *endpoint);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Create a CLIENT socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_client (const char *endpoint);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Create a RADIO socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_radio (const char *endpoint);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Create a DISH socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_dish (const char *endpoint);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Create a GATHER socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_gather (const char *endpoint);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Create a SCATTER socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_scatter (const char *endpoint);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Return socket routing ID if any. This returns 0 if the socket is not
    //  of type ZMQ_SERVER or if no request was already received on it.     
    CZMQ_EXPORT uint32_t
        zsock_routing_id (zsock_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Set routing ID on socket. The socket MUST be of type ZMQ_SERVER.        
    //  This will be used when sending messages on the socket via the zsock API.
    CZMQ_EXPORT void
        zsock_set_routing_id (zsock_t *self, uint32_t routing_id);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Join a group for the RADIO-DISH pattern. Call only on ZMQ_DISH.
    //  Returns 0 if OK, -1 if failed.                                 
    CZMQ_EXPORT int
        zsock_join (void *self, const char *group);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Leave a group for the RADIO-DISH pattern. Call only on ZMQ_DISH.
    //  Returns 0 if OK, -1 if failed.                                  
    CZMQ_EXPORT int
        zsock_leave (void *self, const char *group);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    zsock_t *writer = zsock_new_push ("@tcp://127.0.0.1:5560");
    assert (writer);
    assert (zsock_resolve (writer) != writer);
    assert (streq (zsock_type_str (writer), "PUSH"));
    
    int rc;
    #if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 2, 0))
    //  Check unbind
    rc = zsock_unbind (writer, "tcp://127.0.0.1:%d", 5560);
    assert (rc == 0);
    
    //  In some cases and especially when running under Valgrind, doing
    //  a bind immediately after an unbind causes an EADDRINUSE error.
    //  Even a short sleep allows the OS to release the port for reuse.
    zclock_sleep (100);
    
    //  Bind again
    rc = zsock_bind (writer, "tcp://127.0.0.1:%d", 5560);
    assert (rc == 5560);
    assert (streq (zsock_endpoint (writer), "tcp://127.0.0.1:5560"));
    #endif
    
    zsock_t *reader = zsock_new_pull (">tcp://127.0.0.1:5560");
    assert (reader);
    assert (zsock_resolve (reader) != reader);
    assert (streq (zsock_type_str (reader), "PULL"));
    
    //  Basic Hello, World
    zstr_send (writer, "Hello, World");
    zmsg_t *msg = zmsg_recv (reader);
    assert (msg);
    char *string = zmsg_popstr (msg);
    assert (streq (string, "Hello, World"));
    free (string);
    zmsg_destroy (&msg);
    
    //  Test resolve libzmq socket
    #if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 2, 0))
    void *zmq_ctx = zmq_ctx_new ();
    #else
    void *zmq_ctx = zmq_ctx_new (1);
    #endif
    assert (zmq_ctx);
    void *zmq_sock = zmq_socket (zmq_ctx, ZMQ_PUB);
    assert (zmq_sock);
    assert (zsock_resolve (zmq_sock) == zmq_sock);
    zmq_close (zmq_sock);
    zmq_ctx_term (zmq_ctx);
    
    //  Test resolve zsock
    zsock_t *resolve = zsock_new_pub("@tcp://127.0.0.1:5561");
    assert (resolve);
    assert (zsock_resolve (resolve) == resolve->handle);
    zsock_destroy (&resolve);
    
    //  Test resolve FD
    SOCKET fd = zsock_fd (reader);
    assert (zsock_resolve ((void *) &fd) == NULL);
    
    //  Test binding to ephemeral ports, sequential and random
    int port = zsock_bind (writer, "tcp://127.0.0.1:*");
    assert (port >= DYNAMIC_FIRST && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[50000-]");
    assert (port >= 50000 && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[-50001]");
    assert (port >= DYNAMIC_FIRST && port <= 50001);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[60000-60050]");
    assert (port >= 60000 && port <= 60050);
    
    port = zsock_bind (writer, "tcp://127.0.0.1:!");
    assert (port >= DYNAMIC_FIRST && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:![50000-]");
    assert (port >= 50000 && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:![-50001]");
    assert (port >= DYNAMIC_FIRST && port <= 50001);
    port = zsock_bind (writer, "tcp://127.0.0.1:![60000-60050]");
    assert (port >= 60000 && port <= 60050);
    
    //  Test zsock_attach method
    zsock_t *server = zsock_new (ZMQ_DEALER);
    assert (server);
    rc = zsock_attach (server, "@inproc://myendpoint,tcp://127.0.0.1:5556,inproc://others", true);
    assert (rc == 0);
    rc = zsock_attach (server, "", false);
    assert (rc == 0);
    rc = zsock_attach (server, NULL, true);
    assert (rc == 0);
    rc = zsock_attach (server, ">a,@b, c,, ", false);
    assert (rc == -1);
    zsock_destroy (&server);
    
    //  Test zsock_endpoint method
    rc = zsock_bind (writer, "inproc://test.%s", "writer");
    assert (rc == 0);
    assert (streq (zsock_endpoint (writer), "inproc://test.writer"));
    
    //  Test error state when connecting to an invalid socket type
    //  ('txp://' instead of 'tcp://', typo intentional)
    rc = zsock_connect (reader, "txp://127.0.0.1:5560");
    assert (rc == -1);
    
    //  Test signal/wait methods
    rc = zsock_signal (writer, 123);
    assert (rc == 0);
    rc = zsock_wait (reader);
    assert (rc == 123);
    
    //  Test zsock_send/recv pictures
    uint8_t  number1 = 123;
    uint16_t number2 = 123 * 123;
    uint32_t number4 = 123 * 123 * 123;
    uint64_t number4_MAX = UINT32_MAX;
    uint64_t number8 = 123 * 123 * 123 * 123;
    uint64_t number8_MAX = UINT64_MAX;
    
    zchunk_t *chunk = zchunk_new ("HELLO", 5);
    assert (chunk);
    zframe_t *frame = zframe_new ("WORLD", 5);
    assert (frame);
    zhashx_t *hash = zhashx_new ();
    assert (hash);
    zuuid_t *uuid = zuuid_new ();
    assert (uuid);
    zhashx_set_destructor (hash, (zhashx_destructor_fn *) zstr_free);
    zhashx_set_duplicator (hash, (zhashx_duplicator_fn *) strdup);
    zhashx_insert (hash, "1", "value A");
    zhashx_insert (hash, "2", "value B");
    char *original = "pointer";
    
    //  Test zsock_recv into each supported type
    zsock_send (writer, "i124488zsbcfUhp",
                -12345, number1, number2, number4, number4_MAX,
                number8, number8_MAX,
                "This is a string", "ABCDE", 5,
                chunk, frame, uuid, hash, original);
    char *uuid_str = strdup (zuuid_str (uuid));
    zchunk_destroy (&chunk);
    zframe_destroy (&frame);
    zuuid_destroy (&uuid);
    zhashx_destroy (&hash);
    
    int integer;
    byte *data;
    size_t size;
    char *pointer;
    number8_MAX = number8 = number4 = number2 = number1 = 0;
    rc = zsock_recv (reader, "i124488zsbcfUhp",
                     &integer, &number1, &number2, &number4, &number4_MAX,
                     &number8, &number8_MAX, &string, &data, &size, &chunk,
                     &frame, &uuid, &hash, &pointer);
    assert (rc == 0);
    assert (integer == -12345);
    assert (number1 == 123);
    assert (number2 == 123 * 123);
    assert (number4 == 123 * 123 * 123);
    assert (number4_MAX == UINT32_MAX);
    assert (number8 == 123 * 123 * 123 * 123);
    assert (number8_MAX == UINT64_MAX);
    assert (streq (string, "This is a string"));
    assert (memcmp (data, "ABCDE", 5) == 0);
    assert (size == 5);
    assert (memcmp (zchunk_data (chunk), "HELLO", 5) == 0);
    assert (zchunk_size (chunk) == 5);
    assert (streq (uuid_str, zuuid_str (uuid)));
    assert (memcmp (zframe_data (frame), "WORLD", 5) == 0);
    assert (zframe_size (frame) == 5);
    char *value = (char *) zhashx_lookup (hash, "1");
    assert (streq (value, "value A"));
    value = (char *) zhashx_lookup (hash, "2");
    assert (streq (value, "value B"));
    assert (original == pointer);
    free (string);
    free (data);
    free (uuid_str);
    zframe_destroy (&frame);
    zchunk_destroy (&chunk);
    zhashx_destroy (&hash);
    zuuid_destroy (&uuid);
    
    //  Test zsock_recv of short message; this lets us return a failure
    //  with a status code and then nothing else; the receiver will get
    //  the status code and NULL/zero for all other values
    zsock_send (writer, "i", -1);
    zsock_recv (reader, "izsbcfp",
        &integer, &string, &data, &size, &chunk, &frame, &pointer);
    assert (integer == -1);
    assert (string == NULL);
    assert (data == NULL);
    assert (size == 0);
    assert (chunk == NULL);
    assert (frame == NULL);
    assert (pointer == NULL);
    
    msg = zmsg_new ();
    zmsg_addstr (msg, "frame 1");
    zmsg_addstr (msg, "frame 2");
    zsock_send (writer, "szm", "header", msg);
    zmsg_destroy (&msg);
    
    zsock_recv (reader, "szm", &string, &msg);
    
    assert (streq ("header", string));
    assert (zmsg_size (msg) == 2);
    assert (zframe_streq (zmsg_first (msg), "frame 1"));
    assert (zframe_streq (zmsg_next (msg), "frame 2"));
    zstr_free (&string);
    zmsg_destroy (&msg);
    
    //  Test zsock_recv with null arguments
    chunk = zchunk_new ("HELLO", 5);
    assert (chunk);
    frame = zframe_new ("WORLD", 5);
    assert (frame);
    zsock_send (writer, "izsbcfp",
                -12345, "This is a string", "ABCDE", 5, chunk, frame, original);
    zframe_destroy (&frame);
    zchunk_destroy (&chunk);
    zsock_recv (reader, "izsbcfp", &integer, NULL, NULL, NULL, &chunk, NULL, NULL);
    assert (integer == -12345);
    assert (memcmp (zchunk_data (chunk), "HELLO", 5) == 0);
    assert (zchunk_size (chunk) == 5);
    zchunk_destroy (&chunk);
    
    //  Test zsock_bsend/brecv pictures with binary encoding
    frame = zframe_new ("Hello", 5);
    chunk = zchunk_new ("World", 5);
    
    msg = zmsg_new ();
    zmsg_addstr (msg, "Hello");
    zmsg_addstr (msg, "World");
    
    zsock_bsend (writer, "1248sSpcfm",
                 number1, number2, number4, number8,
                 "Hello, World",
                 "Goodbye cruel World!",
                 original,
                 chunk, frame, msg);
    zchunk_destroy (&chunk);
    zframe_destroy (&frame);
    zmsg_destroy (&msg);
    
    number8 = number4 = number2 = number1 = 0;
    char *longstr;
    zsock_brecv (reader, "1248sSpcfm",
                 &number1, &number2, &number4, &number8,
                 &string, &longstr,
                 &pointer,
                 &chunk, &frame, &msg);
    assert (number1 == 123);
    assert (number2 == 123 * 123);
    assert (number4 == 123 * 123 * 123);
    assert (number8 == 123 * 123 * 123 * 123);
    assert (streq (string, "Hello, World"));
    assert (streq (longstr, "Goodbye cruel World!"));
    assert (pointer == original);
    zstr_free (&longstr);
    zchunk_destroy (&chunk);
    zframe_destroy (&frame);
    zmsg_destroy (&msg);
    
    #ifdef ZMQ_SERVER
    
    //  Test zsock_bsend/brecv pictures with binary encoding on SERVER and CLIENT sockets
    server = zsock_new_server ("tcp://127.0.0.1:5561");
    assert (server);
    zsock_t* client = zsock_new_client ("tcp://127.0.0.1:5561");
    assert (client);
    
    //  From client to server
    chunk = zchunk_new ("World", 5);
    zsock_bsend (client, "1248sSpc",
                 number1, number2, number4, number8,
                 "Hello, World",
                 "Goodbye cruel World!",
                 original,
                 chunk);
    zchunk_destroy (&chunk);
    
    number8 = number4 = number2 = number1 = 0;
    zsock_brecv (server, "1248sSpc",
                 &number1, &number2, &number4, &number8,
                 &string, &longstr,
                 &pointer,
                 &chunk);
    assert (number1 == 123);
    assert (number2 == 123 * 123);
    assert (number4 == 123 * 123 * 123);
    assert (number8 == 123 * 123 * 123 * 123);
    assert (streq (string, "Hello, World"));
    assert (streq (longstr, "Goodbye cruel World!"));
    assert (pointer == original);
    assert (zsock_routing_id (server));
    zstr_free (&longstr);
    zchunk_destroy (&chunk);
    
    //  From server to client
    chunk = zchunk_new ("World", 5);
    zsock_bsend (server, "1248sSpc",
                 number1, number2, number4, number8,
                 "Hello, World",
                 "Goodbye cruel World!",
                 original,
                 chunk);
    zchunk_destroy (&chunk);
    
    number8 = number4 = number2 = number1 = 0;
    zsock_brecv (client, "1248sSpc",
                 &number1, &number2, &number4, &number8,
                 &string, &longstr,
                 &pointer,
                 &chunk);
    assert (number1 == 123);
    assert (number2 == 123 * 123);
    assert (number4 == 123 * 123 * 123);
    assert (number8 == 123 * 123 * 123 * 123);
    assert (streq (string, "Hello, World"));
    assert (streq (longstr, "Goodbye cruel World!"));
    assert (pointer == original);
    assert (zsock_routing_id (client) == 0);
    zstr_free (&longstr);
    zchunk_destroy (&chunk);
    
    zsock_destroy (&client);
    zsock_destroy (&server);
    
    #endif
    
    #ifdef ZMQ_SCATTER
    
    zsock_t* gather = zsock_new_gather ("inproc://test-gather-scatter");
    assert (gather);
    zsock_t* scatter = zsock_new_scatter ("inproc://test-gather-scatter");
    assert (scatter);
    
    rc = zstr_send (scatter, "HELLO");
    assert (rc == 0);
    
    char* message;
    message = zstr_recv (gather);
    assert (streq(message, "HELLO"));
    zstr_free (&message);    
    
    zsock_destroy (&gather);
    zsock_destroy (&scatter);
    
    #endif
    
    //  Check that we can send a zproto format message
    zsock_bsend (writer, "1111sS4", 0xAA, 0xA0, 0x02, 0x01, "key", "value", 1234);
    zgossip_msg_t *gossip = zgossip_msg_new ();
    zgossip_msg_recv (gossip, reader);
    assert (zgossip_msg_id (gossip) == ZGOSSIP_MSG_PUBLISH);
    zgossip_msg_destroy (&gossip);
    
    zsock_destroy (&reader);
    zsock_destroy (&writer);
    
```

<A name="toc4-7161" title="zstr - sending and receiving strings" />
#### zstr - sending and receiving strings

The zstr class provides utility functions for sending and receiving C
strings across ØMQ sockets. It sends strings without a terminating null,
and appends a null byte on received strings. This class is for simple
message sending.

       Memory                       Wire
       +-------------+---+          +---+-------------+
Send   | S t r i n g | 0 |  ---->   | 6 | S t r i n g |
       +-------------+---+          +---+-------------+

       Wire                         Heap
       +---+-------------+          +-------------+---+
Recv   | 6 | S t r i n g |  ---->   | S t r i n g | 0 |
       +---+-------------+          +-------------+---+

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  This class has draft methods, which may change over time. They are not
    //  in stable releases, by default. Use --enable-drafts to enable.
    //  Receive C string from socket. Caller must free returned string using
    //  zstr_free(). Returns NULL if the context is being terminated or the 
    //  process was interrupted.                                            
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zstr_recv (void *source);
    
    //  Receive a series of strings (until NULL) from multipart data.    
    //  Each string is allocated and filled with string data; if there   
    //  are not enough frames, unallocated strings are set to NULL.      
    //  Returns -1 if the message could not be read, else returns the    
    //  number of strings filled, zero or more. Free each returned string
    //  using zstr_free(). If not enough strings are provided, remaining 
    //  multipart frames in the message are dropped.                     
    CZMQ_EXPORT int
        zstr_recvx (void *source, char **string_p, ...);
    
    //  Send a C string to a socket, as a frame. The string is sent without 
    //  trailing null byte; to read this you can use zstr_recv, or a similar
    //  method that adds a null terminator on the received string. String   
    //  may be NULL, which is sent as "".                                   
    CZMQ_EXPORT int
        zstr_send (void *dest, const char *string);
    
    //  Send a C string to a socket, as zstr_send(), with a MORE flag, so that
    //  you can send further strings in the same multi-part message.          
    CZMQ_EXPORT int
        zstr_sendm (void *dest, const char *string);
    
    //  Send a formatted string to a socket. Note that you should NOT use
    //  user-supplied strings in the format (they may contain '%' which  
    //  will create security holes).                                     
    CZMQ_EXPORT int
        zstr_sendf (void *dest, const char *format, ...) CHECK_PRINTF (2);
    
    //  Send a formatted string to a socket, as for zstr_sendf(), with a      
    //  MORE flag, so that you can send further strings in the same multi-part
    //  message.                                                              
    CZMQ_EXPORT int
        zstr_sendfm (void *dest, const char *format, ...) CHECK_PRINTF (2);
    
    //  Send a series of strings (until NULL) as multipart data   
    //  Returns 0 if the strings could be sent OK, or -1 on error.
    CZMQ_EXPORT int
        zstr_sendx (void *dest, const char *string, ...);
    
    //  Free a provided string, and nullify the parent pointer. Safe to call on
    //  a null pointer.                                                        
    CZMQ_EXPORT void
        zstr_free (char **string_p);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zstr_test (bool verbose);
    
    #ifdef CZMQ_BUILD_DRAFT_API
    //  *** Draft method, for development use, may change without warning ***
    //  Accepts a void pointer and returns a fresh character string. If source
    //  is null, returns an empty string.                                     
    //  Caller owns return value and must destroy it when done.
    CZMQ_EXPORT char *
        zstr_str (void *source);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new_pair ("@inproc://zstr.test");
    assert (output);
    zsock_t *input = zsock_new_pair (">inproc://zstr.test");
    assert (input);
    
    //  Send ten strings, five strings with MORE flag and then END
    int string_nbr;
    for (string_nbr = 0; string_nbr < 10; string_nbr++)
        zstr_sendf (output, "this is string %d", string_nbr);
    zstr_sendx (output, "This", "is", "almost", "the", "very", "END", NULL);
    
    //  Read and count until we receive END
    string_nbr = 0;
    for (string_nbr = 0;; string_nbr++) {
        char *string = zstr_recv (input);
        assert (string);
        if (streq (string, "END")) {
            zstr_free (&string);
            break;
        }
        zstr_free (&string);
    }
    assert (string_nbr == 15);
    
    zsock_destroy (&input);
    zsock_destroy (&output);
    
    #if defined (ZMQ_SERVER)
    //  Test SERVER/CLIENT over zstr
    zsock_t *server = zsock_new_server ("inproc://zstr-test-routing");
    zsock_t *client = zsock_new_client ("inproc://zstr-test-routing");;
    assert (server);
    assert (client);
    
    //  Try normal ping-pong to check reply routing ID
    int rc = zstr_send (client, "Hello");
    assert (rc == 0);
    char *request = zstr_recv (server);
    assert (streq (request, "Hello"));
    assert (zsock_routing_id (server));
    free (request);
    
    rc = zstr_send (server, "World");
    assert (rc == 0);
    char *reply = zstr_recv (client);
    assert (streq (reply, "World"));
    free (reply);
    
    rc = zstr_sendf (server, "%s", "World");
    assert (rc == 0);
    reply = zstr_recv (client);
    assert (streq (reply, "World"));
    free (reply);
    
    //  Try ping-pong using sendx and recx
    rc = zstr_sendx (client, "Hello", NULL);
    assert (rc == 0);
    rc = zstr_recvx (server, &request, NULL);
    assert (rc >= 0);
    assert (streq (request, "Hello"));
    free (request);
    
    rc = zstr_sendx (server, "World", NULL);
    assert (rc == 0);
    rc = zstr_recvx (client, &reply, NULL);
    assert (rc >= 0);
    assert (streq (reply, "World"));
    free (reply);
    
    //  Client and server disallow multipart
    rc = zstr_sendm (client, "Hello");
    assert (rc == -1);
    rc = zstr_sendm (server, "World");
    assert (rc == -1);
    
    zsock_destroy (&client);
    zsock_destroy (&server);
    #endif
```

<A name="toc4-7336" title="zsys - system-level methods" />
#### zsys - system-level methods

The zsys class provides a portable wrapper for system calls. We collect
them here to reduce the number of weird #ifdefs in other classes. As far
as possible, the bulk of CZMQ classes are fully portable.

Please add @discuss section in ../src/zsys.c.

This is the class interface:

```h
    #define UDP_FRAME_MAX   255         //  Max size of UDP frame
    
    //  Callback for interrupt signal handler
    typedef void (zsys_handler_fn) (int signal_value);
    
    //  Initialize CZMQ zsys layer; this happens automatically when you create
    //  a socket or an actor; however this call lets you force initialization
    //  earlier, so e.g. logging is properly set-up before you start working.
    //  Not threadsafe, so call only from main thread. Safe to call multiple
    //  times. Returns global CZMQ context.
    CZMQ_EXPORT void *
        zsys_init (void);
    
    //  Optionally shut down the CZMQ zsys layer; this normally happens automatically
    //  when the process exits; however this call lets you force a shutdown
    //  earlier, avoiding any potential problems with atexit() ordering, especially
    //  with Windows dlls.
    CZMQ_EXPORT void
        zsys_shutdown (void);
    
    //  Get a new ZMQ socket, automagically creating a ZMQ context if this is
    //  the first time. Caller is responsible for destroying the ZMQ socket
    //  before process exits, to avoid a ZMQ deadlock. Note: you should not use
    //  this method in CZMQ apps, use zsock_new() instead.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT void *
        zsys_socket (int type, const char *filename, size_t line_nbr);
    
    //  Destroy/close a ZMQ socket. You should call this for every socket you
    //  create using zsys_socket().
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT int
        zsys_close (void *handle, const char *filename, size_t line_nbr);
    
    //  Return ZMQ socket name for socket type
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT char *
        zsys_sockname (int socktype);
        
    //  Create a pipe, which consists of two PAIR sockets connected over inproc.
    //  The pipe is configured to use the zsys_pipehwm setting. Returns the
    //  frontend socket successful, NULL if failed.
    CZMQ_EXPORT zsock_t *
        zsys_create_pipe (zsock_t **backend_p);
        
    //  Set interrupt handler; this saves the default handlers so that a
    //  zsys_handler_reset () can restore them. If you call this multiple times
    //  then the last handler will take affect. If handler_fn is NULL, disables
    //  default SIGINT/SIGTERM handling in CZMQ.
    CZMQ_EXPORT void
        zsys_handler_set (zsys_handler_fn *handler_fn);
    
    //  Reset interrupt handler, call this at exit if needed
    CZMQ_EXPORT void
        zsys_handler_reset (void);
    
    //  Set default interrupt handler, so Ctrl-C or SIGTERM will set
    //  zsys_interrupted. Idempotent; safe to call multiple times.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT void
        zsys_catch_interrupts (void);
    
    //  Return 1 if file exists, else zero
    CZMQ_EXPORT bool
        zsys_file_exists (const char *filename);
    
    //  Return size of file, or -1 if not found
    CZMQ_EXPORT ssize_t
        zsys_file_size (const char *filename);
    
    //  Return file modification time. Returns 0 if the file does not exist.
    CZMQ_EXPORT time_t
        zsys_file_modified (const char *filename);
    
    //  Return file mode; provides at least support for the POSIX S_ISREG(m)
    //  and S_ISDIR(m) macros and the S_IRUSR and S_IWUSR bits, on all boxes.
    //  Returns a mode_t cast to int, or -1 in case of error.
    CZMQ_EXPORT int
        zsys_file_mode (const char *filename);
    
    //  Delete file. Does not complain if the file is absent
    CZMQ_EXPORT int
        zsys_file_delete (const char *filename);
    
    //  Check if file is 'stable'
    CZMQ_EXPORT bool
        zsys_file_stable (const char *filename);
    
    //  Create a file path if it doesn't exist. The file path is treated as a 
    //  printf format.
    CZMQ_EXPORT int
        zsys_dir_create (const char *pathname, ...);
    
    //  Remove a file path if empty; the pathname is treated as printf format.
    CZMQ_EXPORT int
        zsys_dir_delete (const char *pathname, ...);
    
    //  Move to a specified working directory. Returns 0 if OK, -1 if this failed.
    CZMQ_EXPORT int
        zsys_dir_change (const char *pathname);
    
    //  Set private file creation mode; all files created from here will be
    //  readable/writable by the owner only.
    CZMQ_EXPORT void
        zsys_file_mode_private (void);
    
    //  Reset default file creation mode; all files created from here will use
    //  process file mode defaults.
    CZMQ_EXPORT void
        zsys_file_mode_default (void);
    
    //  Return the CZMQ version for run-time API detection; returns version
    //  number into provided fields, providing reference isn't null in each case.
    CZMQ_EXPORT void
        zsys_version (int *major, int *minor, int *patch);
    
    //  Format a string using printf formatting, returning a freshly allocated
    //  buffer. If there was insufficient memory, returns NULL. Free the returned
    //  string using zstr_free().
    CZMQ_EXPORT char *
        zsys_sprintf (const char *format, ...);
    
    //  Format a string with a va_list argument, returning a freshly allocated
    //  buffer. If there was insufficient memory, returns NULL. Free the returned
    //  string using zstr_free().
    CZMQ_EXPORT char *
        zsys_vprintf (const char *format, va_list argptr);
    
    //  Create UDP beacon socket; if the routable option is true, uses
    //  multicast (not yet implemented), else uses broadcast. This method
    //  and related ones might _eventually_ be moved to a zudp class.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT SOCKET
        zsys_udp_new (bool routable);
    
    //  Close a UDP socket
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT int
        zsys_udp_close (SOCKET handle);
    
    //  Send zframe to UDP socket, return -1 if sending failed due to
    //  interface having disappeared (happens easily with WiFi)
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT int
        zsys_udp_send (SOCKET udpsock, zframe_t *frame, inaddr_t *address, int addrlen);
    
    //  Receive zframe from UDP socket, and set address of peer that sent it
    //  The peername must be a char [INET_ADDRSTRLEN] array.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT zframe_t *
        zsys_udp_recv (SOCKET udpsock, char *peername, int peerlen);
    
    //  Handle an I/O error on some socket operation; will report and die on
    //  fatal errors, and continue silently on "try again" errors.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT void
        zsys_socket_error (const char *reason);
    
    //  Return current host name, for use in public tcp:// endpoints. Caller gets
    //  a freshly allocated string, should free it using zstr_free(). If the host
    //  name is not resolvable, returns NULL.
    CZMQ_EXPORT char *
        zsys_hostname (void);
    
    //  Move the current process into the background. The precise effect depends
    //  on the operating system. On POSIX boxes, moves to a specified working
    //  directory (if specified), closes all file handles, reopens stdin, stdout,
    //  and stderr to the null device, and sets the process to ignore SIGHUP. On
    //  Windows, does nothing. Returns 0 if OK, -1 if there was an error.
    CZMQ_EXPORT int
        zsys_daemonize (const char *workdir);
    
    //  Drop the process ID into the lockfile, with exclusive lock, and switch
    //  the process to the specified group and/or user. Any of the arguments
    //  may be null, indicating a no-op. Returns 0 on success, -1 on failure.
    //  Note if you combine this with zsys_daemonize, run after, not before
    //  that method, or the lockfile will hold the wrong process ID.
    CZMQ_EXPORT int
        zsys_run_as (const char *lockfile, const char *group, const char *user);
    
    //  Returns true if the underlying libzmq supports CURVE security.
    //  Uses a heuristic probe according to the version of libzmq being used.
    CZMQ_EXPORT bool
        zsys_has_curve (void);
    
    //  Configure the number of I/O threads that ZeroMQ will use. A good
    //  rule of thumb is one thread per gigabit of traffic in or out. The
    //  default is 1, sufficient for most applications. If the environment
    //  variable ZSYS_IO_THREADS is defined, that provides the default.
    //  Note that this method is valid only before any socket is created.
    CZMQ_EXPORT void
        zsys_set_io_threads (size_t io_threads);
    
    //  Configure the number of sockets that ZeroMQ will allow. The default
    //  is 1024. The actual limit depends on the system, and you can query it
    //  by using zsys_socket_limit (). A value of zero means "maximum".
    //  Note that this method is valid only before any socket is created.
    CZMQ_EXPORT void
        zsys_set_max_sockets (size_t max_sockets);
    
    //  Return maximum number of ZeroMQ sockets that the system will support.
    CZMQ_EXPORT size_t
        zsys_socket_limit (void);
    
    //  Configure the maximum allowed size of a message sent.
    //  The default is INT_MAX.
    CZMQ_EXPORT void
        zsys_set_max_msgsz (int max_msgsz);
    
    //  Return maximum message size.
    CZMQ_EXPORT int
        zsys_max_msgsz (void);
    
    //  Configure the default linger timeout in msecs for new zsock instances.
    //  You can also set this separately on each zsock_t instance. The default
    //  linger time is zero, i.e. any pending messages will be dropped. If the
    //  environment variable ZSYS_LINGER is defined, that provides the default.
    //  Note that process exit will typically be delayed by the linger time.
    CZMQ_EXPORT void
        zsys_set_linger (size_t linger);
    
    //  Configure the default outgoing pipe limit (HWM) for new zsock instances.
    //  You can also set this separately on each zsock_t instance. The default
    //  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
    //  ZSYS_SNDHWM is defined, that provides the default. Note that a value of
    //  zero means no limit, i.e. infinite memory consumption.
    CZMQ_EXPORT void
        zsys_set_sndhwm (size_t sndhwm);
    
    //  Configure the default incoming pipe limit (HWM) for new zsock instances.
    //  You can also set this separately on each zsock_t instance. The default
    //  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
    //  ZSYS_RCVHWM is defined, that provides the default. Note that a value of
    //  zero means no limit, i.e. infinite memory consumption.
    CZMQ_EXPORT void
        zsys_set_rcvhwm (size_t rcvhwm);
    
    //  Configure the default HWM for zactor internal pipes; this is set on both
    //  ends of the pipe, for outgoing messages only (sndhwm). The default HWM is
    //  1,000, on all versions of ZeroMQ. If the environment var ZSYS_ACTORHWM is
    //  defined, that provides the default. Note that a value of zero means no
    //  limit, i.e. infinite memory consumption.
    CZMQ_EXPORT void
        zsys_set_pipehwm (size_t pipehwm);
    
    //  Return the HWM for zactor internal pipes.
    CZMQ_EXPORT size_t
        zsys_pipehwm (void);
    
    //  Configure use of IPv6 for new zsock instances. By default sockets accept
    //  and make only IPv4 connections. When you enable IPv6, sockets will accept
    //  and connect to both IPv4 and IPv6 peers. You can override the setting on
    //  each zsock_t instance. The default is IPv4 only (ipv6 set to 0). If the
    //  environment variable ZSYS_IPV6 is defined (as 1 or 0), this provides the
    //  default. Note: has no effect on ZMQ v2.
    CZMQ_EXPORT void
        zsys_set_ipv6 (int ipv6);
    
    //  Return use of IPv6 for zsock instances.
    CZMQ_EXPORT int
        zsys_ipv6 (void);
    
    //  Set network interface name to use for broadcasts, particularly zbeacon.
    //  This lets the interface be configured for test environments where required.
    //  For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is
    //  the default when there is no specified interface. If the environment
    //  variable ZSYS_INTERFACE is set, use that as the default interface name.
    //  Setting the interface to "*" means "use all available interfaces".
    CZMQ_EXPORT void
        zsys_set_interface (const char *value);
    
    //  Return network interface to use for broadcasts, or "" if none was set.
    CZMQ_EXPORT const char *
        zsys_interface (void);
    
    //  Set IPv6 address to use zbeacon socket, particularly for receiving zbeacon.
    //  This needs to be set IPv6 is enabled as IPv6 can have multiple addresses
    //  on a given interface. If the environment variable ZSYS_IPV6_ADDRESS is set,
    //  use that as the default IPv6 address.
    CZMQ_EXPORT void
        zsys_set_ipv6_address (const char *value);
    
    //  Return IPv6 address to use for zbeacon reception, or "" if none was set.
    CZMQ_EXPORT const char *
        zsys_ipv6_address (void);
    
    //  Set IPv6 milticast address to use for sending zbeacon messages. This needs
    //  to be set if IPv6 is enabled. If the environment variable
    //  ZSYS_IPV6_MCAST_ADDRESS is set, use that as the default IPv6 multicast
    //  address.
    CZMQ_EXPORT void
        zsys_set_ipv6_mcast_address (const char *value);
    
    //  Return IPv6 multicast address to use for sending zbeacon, or "" if none was
    //  set.
    CZMQ_EXPORT const char *
        zsys_ipv6_mcast_address (void);
    
    //  Configure the automatic use of pre-allocated FDs when creating new sockets.
    //  If 0 (default), nothing will happen. Else, when a new socket is bound, the
    //  system API will be used to check if an existing pre-allocated FD with a
    //  matching port (if TCP) or path (if IPC) exists, and if it does it will be
    //  set via the ZMQ_USE_FD socket option so that the library will use it
    //  instead of creating a new socket.
    CZMQ_EXPORT void
        zsys_set_auto_use_fd (int auto_use_fd);
    
    //  Return use of automatic pre-allocated FDs for zsock instances.
    CZMQ_EXPORT int
        zsys_auto_use_fd (void);
    
    //  Set log identity, which is a string that prefixes all log messages sent
    //  by this process. The log identity defaults to the environment variable
    //  ZSYS_LOGIDENT, if that is set.
    CZMQ_EXPORT void
        zsys_set_logident (const char *value);
    
    //  Set stream to receive log traffic. By default, log traffic is sent to
    //  stdout. If you set the stream to NULL, no stream will receive the log
    //  traffic (it may still be sent to the system facility).
    CZMQ_EXPORT void
        zsys_set_logstream (FILE *stream);
        
    //  Sends log output to a PUB socket bound to the specified endpoint. To
    //  collect such log output, create a SUB socket, subscribe to the traffic
    //  you care about, and connect to the endpoint. Log traffic is sent as a
    //  single string frame, in the same format as when sent to stdout. The
    //  log system supports a single sender; multiple calls to this method will
    //  bind the same sender to multiple endpoints. To disable the sender, call
    //  this method with a null argument.
    CZMQ_EXPORT void
        zsys_set_logsender (const char *endpoint);
    
    //  Enable or disable logging to the system facility (syslog on POSIX boxes,
    //  event log on Windows). By default this is disabled.
    CZMQ_EXPORT void
        zsys_set_logsystem (bool logsystem);
        
    //  Log error condition - highest priority
    CZMQ_EXPORT void
        zsys_error (const char *format, ...);
    
    //  Log warning condition - high priority
    CZMQ_EXPORT void
        zsys_warning (const char *format, ...);
        
    //  Log normal, but significant, condition - normal priority
    CZMQ_EXPORT void
        zsys_notice (const char *format, ...);
        
    //  Log informational message - low priority
    CZMQ_EXPORT void
        zsys_info (const char *format, ...);
        
    //  Log debug-level message - lowest priority
    CZMQ_EXPORT void
        zsys_debug (const char *format, ...);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zsys_test (bool verbose);
        
    //  Global signal indicator, TRUE when user presses Ctrl-C or the process
    //  gets a SIGTERM signal.
    CZMQ_EXPORT extern volatile int zsys_interrupted;
    //  Deprecated name for this variable
    CZMQ_EXPORT extern volatile int zctx_interrupted;
```

This is the class self test code:

```c
    zsys_catch_interrupts ();
    
    //  Check capabilities without using the return value
    int rc = zsys_has_curve ();
    
    if (verbose) {
        char *hostname = zsys_hostname ();
        zsys_info ("host name is %s", hostname);
        free (hostname);
        zsys_info ("system limit is %zu ZeroMQ sockets", zsys_socket_limit ());
    }
    zsys_set_linger (0);
    zsys_set_sndhwm (1000);
    zsys_set_rcvhwm (1000);
    zsys_set_pipehwm (2500);
    assert (zsys_pipehwm () == 2500);
    zsys_set_ipv6 (0);
    
    //  Test pipe creation
    zsock_t *pipe_back;
    zsock_t *pipe_front = zsys_create_pipe (&pipe_back);
    zstr_send (pipe_front, "Hello");
    char *string = zstr_recv (pipe_back);
    assert (streq (string, "Hello"));
    free (string);
    zsock_destroy (&pipe_back);
    zsock_destroy (&pipe_front);
    
    //  Test file manipulation
    rc = zsys_file_delete ("nosuchfile");
    assert (rc == -1);
    
    bool rc_bool = zsys_file_exists ("nosuchfile");
    assert (rc_bool != true);
    
    rc = (int) zsys_file_size ("nosuchfile");
    assert (rc == -1);
    
    time_t when = zsys_file_modified (".");
    assert (when > 0);
    
    int mode = zsys_file_mode (".");
    assert (S_ISDIR (mode));
    assert (mode & S_IRUSR);
    assert (mode & S_IWUSR);
    
    zsys_file_mode_private ();
    rc = zsys_dir_create ("%s/%s", ".", ".testsys/subdir");
    assert (rc == 0);
    when = zsys_file_modified ("./.testsys/subdir");
    assert (when > 0);
    assert (!zsys_file_stable ("./.testsys/subdir"));
    rc = zsys_dir_delete ("%s/%s", ".", ".testsys/subdir");
    assert (rc == 0);
    rc = zsys_dir_delete ("%s/%s", ".", ".testsys");
    assert (rc == 0);
    zsys_file_mode_default ();
    assert (zsys_dir_change (".") == 0);
    
    int major, minor, patch;
    zsys_version (&major, &minor, &patch);
    assert (major == CZMQ_VERSION_MAJOR);
    assert (minor == CZMQ_VERSION_MINOR);
    assert (patch == CZMQ_VERSION_PATCH);
    
    string = zsys_sprintf ("%s %02x", "Hello", 16);
    assert (streq (string, "Hello 10"));
    free (string);
    
    char *str64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.";
    int num10 = 1234567890;
    string = zsys_sprintf ("%s%s%s%s%d", str64, str64, str64, str64, num10);
    assert (strlen (string) == (4 * 64 + 10));
    free (string);
    
    //  Test logging system
    zsys_set_logident ("czmq_selftest");
    zsys_set_logsender ("inproc://logging");
    void *logger = zsys_socket (ZMQ_SUB, NULL, 0);
    assert (logger);
    rc = zmq_connect (logger, "inproc://logging");
    assert (rc == 0);
    rc = zmq_setsockopt (logger, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);
    
    if (verbose) {
        zsys_error ("This is an %s message", "error");
        zsys_warning ("This is a %s message", "warning");
        zsys_notice ("This is a %s message", "notice");
        zsys_info ("This is a %s message", "info");
        zsys_debug ("This is a %s message", "debug");
        zsys_set_logident ("hello, world");
        zsys_info ("This is a %s message", "info");
        zsys_debug ("This is a %s message", "debug");
    
        //  Check that logsender functionality is working
        char *received = zstr_recv (logger);
        assert (received);
        zstr_free (&received);
    }
    zsys_close (logger, NULL, 0);
```

<A name="toc4-7823" title="ztimerset - timer set" />
#### ztimerset - timer set

ztimerset - timer set

Please add @discuss section in ../src/ztimerset.c.

This is the class interface:

```h
    //  This is a draft class, and may change without notice. It is disabled in
    //  stable builds by default. If you use this in applications, please ask
    //  for it to be pushed to stable state. Use --enable-drafts to enable.
    #ifdef CZMQ_BUILD_DRAFT_API
    // Callback function for timer event.
    typedef void (ztimerset_fn) (
        int timer_id, void *arg);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Create new timer set.
    CZMQ_EXPORT ztimerset_t *
        ztimerset_new (void);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Destroy a timer set
    CZMQ_EXPORT void
        ztimerset_destroy (ztimerset_t **self_p);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Add a timer to the set. Returns timer id if OK, -1 on failure.
    CZMQ_EXPORT int
        ztimerset_add (ztimerset_t *self, size_t interval, ztimerset_fn handler, void *arg);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Cancel a timer. Returns 0 if OK, -1 on failure.
    CZMQ_EXPORT int
        ztimerset_cancel (ztimerset_t *self, int timer_id);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Set timer interval. Returns 0 if OK, -1 on failure.                                    
    //  This method is slow, canceling the timer and adding a new one yield better performance.
    CZMQ_EXPORT int
        ztimerset_set_interval (ztimerset_t *self, int timer_id, size_t interval);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Reset timer to start interval counting from current time. Returns 0 if OK, -1 on failure.
    //  This method is slow, canceling the timer and adding a new one yield better performance.  
    CZMQ_EXPORT int
        ztimerset_reset (ztimerset_t *self, int timer_id);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Return the time until the next interval.                        
    //  Should be used as timeout parameter for the zpoller wait method.
    //  The timeout is in msec.                                         
    CZMQ_EXPORT int
        ztimerset_timeout (ztimerset_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Invoke callback function of all timers which their interval has elapsed.
    //  Should be call after zpoller wait method.                               
    //  Returns 0 if OK, -1 on failure.                                         
    CZMQ_EXPORT int
        ztimerset_execute (ztimerset_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Self test of this class.
    CZMQ_EXPORT void
        ztimerset_test (bool verbose);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Simple create/destroy test
    ztimerset_t *self = ztimerset_new ();
    assert (self);
    
    //  Adding timer
    bool timer_invoked = false;
    int timer_id = ztimerset_add (self, 100, handler, &timer_invoked);
    assert (timer_id != -1);
    int rc = ztimerset_execute (self);
    assert (rc == 0);
    assert (!timer_invoked);
    int timeout = ztimerset_timeout (self);
    assert (timeout > 0);
    zclock_sleep (timeout);
    rc = ztimerset_execute (self);
    assert (rc == 0);
    assert (timer_invoked);
    
    //  Cancel timer
    timeout = ztimerset_timeout (self);
    assert (timeout > 0);
    rc = ztimerset_cancel (self, timer_id);
    assert (rc == 0);
    timeout = ztimerset_timeout (self);
    assert(timeout == -1);
    
    //  Reset a timer
    timer_id = ztimerset_add (self, 100, handler, &timer_invoked);
    assert (timer_id != -1);
    timeout = ztimerset_timeout (self);
    assert (timeout > 0);
    zclock_sleep (timeout / 2);
    timeout = ztimerset_timeout (self);
    rc = ztimerset_reset(self, timer_id);
    assert (rc == 0);
    int timeout2 = ztimerset_timeout (self);
    assert (timeout2 > timeout);
    rc = ztimerset_cancel (self, timer_id);
    assert (rc == 0);
    
    //  Set interval
    timer_id = ztimerset_add (self, 100, handler, &timer_invoked);
    assert (timer_id != -1);
    timeout = ztimerset_timeout (self);
    rc = ztimerset_set_interval(self, timer_id, 200);
    timeout2 = ztimerset_timeout (self);
    assert (timeout2 > timeout);
    
    ztimerset_destroy (&self);
```

<A name="toc4-7949" title="ztrie - simple trie for tokenizable strings" />
#### ztrie - simple trie for tokenizable strings

This is a variant of a trie or prefix tree where all the descendants of a
node have a common prefix of the string associated with that node. This
implementation is specialized for strings that can be tokenized by a delimiter
like a URL, URI or URN. Routes in the tree can be matched by regular expressions
and by using capturing groups parts of a matched route can be easily obtained.

Note that the performance for pure string based matching is okay but on short
strings zhash and zhashx are 3-4 times faster.

This is the class interface:

```h
    //  This is a draft class, and may change without notice. It is disabled in
    //  stable builds by default. If you use this in applications, please ask
    //  for it to be pushed to stable state. Use --enable-drafts to enable.
    #ifdef CZMQ_BUILD_DRAFT_API
    // Callback function for ztrie_node to destroy node data.
    typedef void (ztrie_destroy_data_fn) (
        void **data);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Creates a new ztrie.
    CZMQ_EXPORT ztrie_t *
        ztrie_new (char delimiter);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Destroy the ztrie.
    CZMQ_EXPORT void
        ztrie_destroy (ztrie_t **self_p);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Inserts a new route into the tree and attaches the data. Returns -1     
    //  if the route already exists, otherwise 0. This method takes ownership of
    //  the provided data if a destroy_data_fn is provided.                     
    CZMQ_EXPORT int
        ztrie_insert_route (ztrie_t *self, const char *path, void *data, ztrie_destroy_data_fn destroy_data_fn);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Removes a route from the trie and destroys its data. Returns -1 if the
    //  route does not exists, otherwise 0.                                   
    //  the start of the list call zlist_first (). Advances the cursor.       
    CZMQ_EXPORT int
        ztrie_remove_route (ztrie_t *self, const char *path);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Returns true if the path matches a route in the tree, otherwise false.
    CZMQ_EXPORT bool
        ztrie_matches (ztrie_t *self, const char *path);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Returns the data of a matched route from last ztrie_matches. If the path
    //  did not match, returns NULL. Do not delete the data as it's owned by    
    //  ztrie.                                                                  
    CZMQ_EXPORT void *
        ztrie_hit_data (ztrie_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Returns the count of parameters that a matched route has.
    CZMQ_EXPORT size_t
        ztrie_hit_parameter_count (ztrie_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Returns the parameters of a matched route with named regexes from last   
    //  ztrie_matches. If the path did not match or the route did not contain any
    //  named regexes, returns NULL.                                             
    CZMQ_EXPORT zhashx_t *
        ztrie_hit_parameters (ztrie_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Returns the asterisk matched part of a route, if there has been no match
    //  or no asterisk match, returns NULL.                                     
    CZMQ_EXPORT const char *
        ztrie_hit_asterisk_match (ztrie_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Print the trie
    CZMQ_EXPORT void
        ztrie_print (ztrie_t *self);
    
    //  *** Draft method, for development use, may change without warning ***
    //  Self test of this class.
    CZMQ_EXPORT void
        ztrie_test (bool verbose);
    
    #endif // CZMQ_BUILD_DRAFT_API
```

This is the class self test code:

```c
    //  Create a new trie for matching strings that can be tokenized by a slash
    //  (e.g. URLs minus the protocol, address and port).
    ztrie_t *self = ztrie_new ('/');
    assert (self);
    
    int ret = 0;
    
    //  Let's start by inserting a couple of routes into the trie.
    //  This one is for the route '/foo/bar' the slash at the beginning of the
    //  route is important because everything before the first delimiter will be
    //  discarded. A slash at the end of a route is optional though. The data
    //  associated with this node is passed without destroy function which means
    //  it must be destroyed by the caller.
    int foo_bar_data = 10;
    ret = ztrie_insert_route (self, "/foo/bar", &foo_bar_data, NULL);
    assert (ret == 0);
    
    //  Now suppose we like to match all routes with two tokens that start with
    //  '/foo/' but aren't '/foo/bar'. This is possible by using regular
    //  expressions which are enclosed in an opening and closing curly bracket.
    //  Tokens that contain regular  expressions are always match after string
    //  based tokens.
    //  Note: There is no order in which regular expressions are sorted thus
    //  if you enter multiple expressions for a route you will have to make
    //  sure they don't have overlapping results. For example '/foo/{[^/]+}'
    //  and '/foo/{\d+} having could turn out badly.
    int foo_other_data = 100;
    ret = ztrie_insert_route (self, "/foo/{[^/]+}", &foo_other_data, NULL);
    assert (ret == 0);
    
    //  Regular expression are only matched against tokens of the same level.
    //  This allows us to append to are route with a regular expression as if
    //  it were a string.
    ret = ztrie_insert_route (self, "/foo/{[^/]+}/gulp", NULL, NULL);
    assert (ret == 0);
    
    //  Routes are identified by their endpoint, which is the last token of the route.
    //  It is possible to insert routes for a node that already exists but isn't an
    //  endpoint yet. The delimiter at the end of a route is optional and has no effect.
    ret = ztrie_insert_route (self, "/foo/", NULL, NULL);
    assert (ret == 0);
    
    //  If you try to insert a route which already exists the method will return -1.
    ret = ztrie_insert_route (self, "/foo", NULL, NULL);
    assert (ret == -1);
    
    //  It is not allowed to insert routes with empty tokens.
    ret = ztrie_insert_route (self, "//foo", NULL, NULL);
    assert (ret == -1);
    
    //  Everything before the first delimiter is ignored so 'foo/bar/baz' is equivalent
    //  to '/bar/baz'.
    ret = ztrie_insert_route (self, "foo/bar/baz", NULL, NULL);
    assert (ret == 0);
    ret = ztrie_insert_route (self, "/bar/baz", NULL, NULL);
    assert (ret == -1);
    
    //  Of course you are allowed to remove routes, in case there is data associated with a
    //  route and a destroy data function has been supplied that data will be destroyed.
    ret = ztrie_remove_route (self, "/foo");
    assert (ret == 0);
    
    //  Removing a non existent route will  as well return -1.
    ret = ztrie_remove_route (self, "/foo");
    assert (ret == -1);
    
    //  Removing a route with a regular expression must exactly match the entered one.
    ret = ztrie_remove_route (self, "/foo/{[^/]+}");
    assert (ret == 0);
    
    //  Next we like to match a path by regular expressions and also extract matched
    //  parts of a route. This can be done by naming the regular expression. The name of a
    //  regular expression is entered at the beginning of the curly brackets and separated
    //  by a colon from the regular expression. The first one in this examples is named
    //  'name' and names the expression '[^/]'. If there is no capturing group defined in
    //  the expression the whole matched string will be associated with this parameter. In
    //  case you don't like the get the whole matched string use a capturing group, like
    //  it has been done for the 'id' parameter. This is nice but you can even match as
    //  many parameter for a token as you like. Therefore simply put the parameter names
    //  separated by colons in front of the regular expression and make sure to add a
    //  capturing group for each parameter. The first parameter will be associated with
    //  the first capturing and so on.
    char *data = (char *) malloc (80);
    sprintf (data, "%s", "Hello World!");
    ret = ztrie_insert_route (self, "/baz/{name:[^/]+}/{id:--(\\d+)}/{street:nr:(\\a+)(\\d+)}", data, NULL);
    assert (ret == 0);
    
    //  There is a lot you can do with regular expression but matching routes
    //  of arbitrary length wont work. Therefore we make use of the asterisk
    //  operator. Just place it at the end of your route, e.g. '/config/bar/*'.
    ret = ztrie_insert_route (self, "/config/bar/*", NULL, NULL);
    assert (ret == 0);
    
    //  Appending to an asterisk as you would to with a regular expression
    //  isn't valid.
    ret = ztrie_insert_route (self, "/config/bar/*/bar", NULL, NULL);
    assert (ret == -1);
    
    //  The asterisk operator will only work as a leaf in the tree. If you
    //  enter an asterisk in the middle of your route it will simply be
    //  interpreted as a string.
    ret = ztrie_insert_route (self, "/test/*/bar", NULL, NULL);
    assert (ret == 0);
    
    //  If a parent has an asterisk as child it is not allowed to have
    //  other siblings.
    ret = ztrie_insert_route (self, "/config/bar/foo/glup", NULL, NULL);
    assert (ret != 0);
    
    //  Test matches
    bool hasMatch = false;
    
    //  The route '/bar/foo' will fail to match as this route has never been inserted.
    hasMatch = ztrie_matches (self, "/bar/foo");
    assert (!hasMatch);
    
    //  The route '/foo/bar' will match and we can obtain the data associated with it.
    hasMatch = ztrie_matches (self, "/foo/bar");
    assert (hasMatch);
    int foo_bar_hit_data = *((int *) ztrie_hit_data (self));
    assert (foo_bar_data == foo_bar_hit_data);
    
    //  This route is part of another but is no endpoint itself thus the matches will fail.
    hasMatch = ztrie_matches (self, "/baz/blub");
    assert (!hasMatch);
    
    //  This route will match our named regular expressions route. Thus we can extract data
    //  from the route by their names.
    hasMatch = ztrie_matches (self, "/baz/blub/--11/abc23");
    assert (hasMatch);
    char *match_data = (char *) ztrie_hit_data (self);
    assert (streq ("Hello World!", match_data));
    zhashx_t *parameters = ztrie_hit_parameters (self);
    assert (zhashx_size (parameters) == 4);
    assert (streq ("blub", (char *) zhashx_lookup (parameters, "name")));
    assert (streq ("11", (char *) zhashx_lookup (parameters, "id")));
    assert (streq ("abc", (char *) zhashx_lookup (parameters, "street")));
    assert (streq ("23", (char *) zhashx_lookup (parameters, "nr")));
    zhashx_destroy (&parameters);
    
    //  This will match our asterisk route '/config/bar/*'. As the result we
    //  can obtain the asterisk matched part of the route.
    hasMatch = ztrie_matches (self, "/config/bar/foo/bar");
    assert (hasMatch);
    assert (streq (ztrie_hit_asterisk_match (self), "foo/bar"));
    
    zstr_free (&data);
    ztrie_destroy (&self);
```

<A name="toc4-8192" title="zuuid - UUID support class" />
#### zuuid - UUID support class

The zuuid class generates UUIDs and provides methods for working with
them. If you build CZMQ with libuuid, on Unix/Linux, it will use that
library. On Windows it will use UuidCreate(). Otherwise it will use a
random number generator to produce convincing imitations of UUIDs.

Please add @discuss section in ../src/zuuid.c.

This is the class interface:

```h
    //  This is a stable class, and may not change except for emergencies. It
    //  is provided in stable builds.
    //  Create a new UUID object.
    CZMQ_EXPORT zuuid_t *
        zuuid_new (void);
    
    //  Create UUID object from supplied ZUUID_LEN-octet value.
    CZMQ_EXPORT zuuid_t *
        zuuid_new_from (const byte *source);
    
    //  Destroy a specified UUID object.
    CZMQ_EXPORT void
        zuuid_destroy (zuuid_t **self_p);
    
    //  Set UUID to new supplied ZUUID_LEN-octet value.
    CZMQ_EXPORT void
        zuuid_set (zuuid_t *self, const byte *source);
    
    //  Set UUID to new supplied string value skipping '-' and '{' '}'
    //  optional delimiters. Return 0 if OK, else returns -1.         
    CZMQ_EXPORT int
        zuuid_set_str (zuuid_t *self, const char *source);
    
    //  Return UUID binary data.
    CZMQ_EXPORT const byte *
        zuuid_data (zuuid_t *self);
    
    //  Return UUID binary size
    CZMQ_EXPORT size_t
        zuuid_size (zuuid_t *self);
    
    //  Returns UUID as string
    CZMQ_EXPORT const char *
        zuuid_str (zuuid_t *self);
    
    //  Return UUID in the canonical string format: 8-4-4-4-12, in lower
    //  case. Caller does not modify or free returned value. See        
    //  http://en.wikipedia.org/wiki/Universally_unique_identifier      
    CZMQ_EXPORT const char *
        zuuid_str_canonical (zuuid_t *self);
    
    //  Store UUID blob in target array
    CZMQ_EXPORT void
        zuuid_export (zuuid_t *self, byte *target);
    
    //  Check if UUID is same as supplied value
    CZMQ_EXPORT bool
        zuuid_eq (zuuid_t *self, const byte *compare);
    
    //  Check if UUID is different from supplied value
    CZMQ_EXPORT bool
        zuuid_neq (zuuid_t *self, const byte *compare);
    
    //  Make copy of UUID object; if uuid is null, or memory was exhausted,
    //  returns null.                                                      
    CZMQ_EXPORT zuuid_t *
        zuuid_dup (zuuid_t *self);
    
    //  Self test of this class.
    CZMQ_EXPORT void
        zuuid_test (bool verbose);
    
```

This is the class self test code:

```c
    //  Simple create/destroy test
    assert (ZUUID_LEN == 16);
    assert (ZUUID_STR_LEN == 32);
    
    zuuid_t *uuid = zuuid_new ();
    assert (uuid);
    assert (zuuid_size (uuid) == ZUUID_LEN);
    assert (strlen (zuuid_str (uuid)) == ZUUID_STR_LEN);
    zuuid_t *copy = zuuid_dup (uuid);
    assert (streq (zuuid_str (uuid), zuuid_str (copy)));
    
    //  Check set/set_str/export methods
    const char *myuuid = "8CB3E9A9649B4BEF8DE225E9C2CEBB38";
    const char *myuuid2 = "8CB3E9A9-649B-4BEF-8DE2-25E9C2CEBB38";
    const char *myuuid3 = "{8CB3E9A9-649B-4BEF-8DE2-25E9C2CEBB38}";
    const char *myuuid4 = "8CB3E9A9649B4BEF8DE225E9C2CEBB3838";
    int rc = zuuid_set_str (uuid, myuuid);
    assert (rc == 0);
    assert (streq (zuuid_str (uuid), myuuid));
    rc = zuuid_set_str (uuid, myuuid2);
    assert (rc == 0);
    assert (streq (zuuid_str (uuid), myuuid));
    rc = zuuid_set_str (uuid, myuuid3);
    assert (rc == 0);
    assert (streq (zuuid_str (uuid), myuuid));
    rc = zuuid_set_str (uuid, myuuid4);
    assert (rc == -1);
    byte copy_uuid [ZUUID_LEN];
    zuuid_export (uuid, copy_uuid);
    zuuid_set (uuid, copy_uuid);
    assert (streq (zuuid_str (uuid), myuuid));
    
    //  Check the canonical string format
    assert (streq (zuuid_str_canonical (uuid),
                   "8cb3e9a9-649b-4bef-8de2-25e9c2cebb38"));
    
    zuuid_destroy (&uuid);
    zuuid_destroy (&copy);
```


<A name="toc2-8313" title="Error Handling" />
## Error Handling

The CZMQ policy is to reduce the error flow to 0/-1 where possible. libzmq still does a lot of errno setting. CZMQ does not do that, as it creates a fuzzy API. Things either work as expected, or they fail, and the application's best strategy is usually to assert on non-zero return codes.

Some older libraries still return plethora of error codes, to indicate different types of failure. This ironically makes both library and application more likely to be buggy. The reason is simply that it needs more code on both sides of the API, and the more code, the more bugs.

The use of black/white error handling fits the CLASS style for APIs where each call is explicit and without side effects of any kind, and where damage is either impossible, or fatal.

The one exception is running out of resources (memory, sockets). In that case, there are two strategies that work, for different types of app. One is to assert, to force better sizing of the machine and/or limits such as max connections. Two is to degrade carefully, e.g. refuse new connections, however that is considerably harder to do correctly and probably unrealistic for most developers.

Some CZMQ methods used to actually assert, e.g. in zsocket_bind, if the action failed, instead of returning -1. That was just closer to the majority case where the action MUST work, or nothing can continue. However there's a small slice of cases where failure means something positive, and for these cases, such calls return -1 on failure. 99% of calling code simply asserts the return value is not -1.

There are a few cases where the return value is overloaded to return -1, 0, or other values. These are somewhat confusing special cases and we aim to eliminate them over time.

The overall goal with this strategy is robustness, and absolute minimal and predictable expression in the code. You can see that it works: the CZMQ code is generally very simple and clear, with a few exceptions of places where people have used their old C style (we fix these over time).

<A name="toc2-8330" title="CZMQ Actors" />
## CZMQ Actors

The v2 API had a zthread class that let you create "attached threads" connected to their parent by an inproc:// PIPE socket. In v3 this has been simplified and better wrapped as the zactor class. CZMQ actors are in effect threads with a socket interface. A zactor_t instance works like a socket, and the CZMQ classes that deal with sockets (like zmsg and zpoller) all accept zactor_t references as well as zsock_t and libzmq void * socket handles.

To write an actor, use this template. Note that your actor is a single function "void myname (zsock_t *pipe, void *args)" function:

    /*  =========================================================================
        someclass - some description

        Copyright (c) the Contributors as noted in the AUTHORS file.
        This file is part of CZMQ, the high-level C binding for ØMQ:
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
            zsock_t *which = (zsock_t *) zpoller_wait (self->poller, -1);
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

<A name="toc2-8476" title="Under the Hood" />
## Under the Hood

<A name="toc3-8479" title="Adding a New Class" />
### Adding a New Class

If you define a new CZMQ class `myclass` you need to:

* Write the `zmyclass.c` and `zmyclass.h` source files, in `src` and `include` respectively.
* Add`#include <zmyclass.h>` to `include/czmq.h`.
* Add the myclass header and test call to `src/czmq_selftest.c`.
* Add a reference documentation to 'doc/zmyclass.txt'.
* Add myclass to 'model/projects.xml` and read model/README.txt.
* Add a section to README.txt.

<A name="toc3-8491" title="Documentation" />
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

<A name="toc3-8530" title="Development" />
### Development

CZMQ is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

<A name="toc3-8540" title="Porting CZMQ" />
### Porting CZMQ

When you try CZMQ on an OS that it's not been used on (ever, or for a while), you will hit code that does not compile. In some cases the patches are trivial, in other cases (usually when porting to Windows), the work needed to build equivalent functionality may be non-trivial. In any case, the benefit is that once ported, the functionality is available to all applications.

Before attempting to patch code for portability, please read the `czmq_prelude.h` header file. There are several typical types of changes you may need to make to get functionality working on a specific operating system:

* Defining typedefs which are missing on that specific compiler: do this in czmq_prelude.h.
* Defining macros that rename exotic library functions to more conventional names: do this in czmq_prelude.h.
* Reimplementing specific methods to use a non-standard API: this is typically needed on Windows. Do this in the relevant class, using #ifdefs to properly differentiate code for different platforms.

<A name="toc3-8551" title="Hints to Contributors" />
### Hints to Contributors

CZMQ is a nice, neat library, and you may not immediately appreciate why. Read the CLASS style guide please, and write your code to make it indistinguishable from the rest of the code in the library. That is the only real criteria for good style: it's invisible.

Don't include system headers in source files. The right place for these is czmq_prelude.h. If you need to check against configured libraries and/or headers, include platform.h in the source before including czmq.h.

Do read your code after you write it and ask, "Can I make this simpler?" We do use a nice minimalist and yet readable style. Learn it, adopt it, use it.

Before opening a pull request read our [contribution guidelines](https://github.com/zeromq/czmq/blob/master/CONTRIBUTING.md). Thanks!

<A name="toc3-8562" title="Code Generation" />
### Code Generation

We generate the zsockopt class using [GSL](https://github.com/imatix/gsl), using a code generator script in scripts/sockopts.gsl. We also generate the project files.

<A name="toc3-8567" title="This Document" />
### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).

_This documentation was generated from libczmq/README.txt using [Gitdown](https://github.com/zeromq/gitdown)_
