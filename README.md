
<A name="toc1-3" title="CZMQ - High-level C binding for ØMQ" />
# CZMQ - High-level C binding for ØMQ

[![Build Status](https://travis-ci.org/zeromq/czmq.png?branch=master)](https://travis-ci.org/zeromq/czmq)

<A name="toc2-8" title="Contents" />
## Contents


**<a href="#toc2-13">Overview</a>**

**<a href="#toc3-16">Scope and Goals</a>**

**<a href="#toc3-57">Ownership and License</a>**

**<a href="#toc2-68">Using CZMQ</a>**

**<a href="#toc3-71">Building and Installing</a>**

**<a href="#toc3-115">Linking with an Application</a>**

**<a href="#toc3-122">Use from Other Languages</a>**

**<a href="#toc3-135">API v3 Summary</a>**
&emsp;<a href="#toc4-140">zactor - simple actor framework</a>
&emsp;<a href="#toc4-212">zauth - authentication for ZeroMQ security mechanisms</a>
&emsp;<a href="#toc4-425">zbeacon - LAN discovery and presence</a>
&emsp;<a href="#toc4-609">zcert - work with CURVE security certificates</a>
&emsp;<a href="#toc4-770">zcertstore - work with CURVE security certificate stores</a>
&emsp;<a href="#toc4-869">zchunk - work with memory chunks</a>
&emsp;<a href="#toc4-1048">zclock - millisecond clocks and delays</a>
&emsp;<a href="#toc4-1106">zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.</a>
&emsp;<a href="#toc4-1339">zdigest - provides hashing functions (SHA-1 at present)</a>
&emsp;<a href="#toc4-1402">zdir - work with file-system directories</a>
&emsp;<a href="#toc4-1511">zdir_patch - work with directory patches</a>
&emsp;<a href="#toc4-1576">zfile - provides methods to work with files in a portable fashion.</a>
&emsp;<a href="#toc4-1809">zframe - working with single message frames</a>
&emsp;<a href="#toc4-1979">zgossip - decentralized configuration management</a>
&emsp;<a href="#toc4-2169">zhash - generic type-free hash container</a>
&emsp;<a href="#toc4-2549">ziflist - List of network interfaces available on system</a>
&emsp;<a href="#toc4-2620">zlist - generic type-free list container</a>
&emsp;<a href="#toc4-2840">zloop - event-driven reactor</a>
&emsp;<a href="#toc4-2965">zmonitor - socket event monitor</a>
&emsp;<a href="#toc4-3069">zmsg - working with multipart messages</a>
&emsp;<a href="#toc4-3442">zpoller - trivial socket poller class</a>
&emsp;<a href="#toc4-3555">zproxy - run a steerable proxy in the background</a>
&emsp;<a href="#toc4-3701">zrex - work with regular expressions</a>
&emsp;<a href="#toc4-3792">zring - generic type-free doubly linked ring container</a>
&emsp;<a href="#toc4-4072">zsock - high-level socket API that hides libzmq contexts and sockets</a>
&emsp;<a href="#toc4-4606">zsock_option - get/set ØMQ socket options</a>
&emsp;<a href="#toc4-5597">zstr - sending and receiving strings</a>
&emsp;<a href="#toc4-5701">zsys - system-level methods</a>
&emsp;<a href="#toc4-6129">zuuid - UUID support class</a>

**<a href="#toc3-6216">API v2 Summary</a>**
&emsp;<a href="#toc4-6221">zauth_v2 - authentication for ZeroMQ servers (deprecated)</a>
&emsp;<a href="#toc4-6413">zctx - working with ØMQ contexts (deprecated)</a>
&emsp;<a href="#toc4-6541">zmonitor_v2 - socket event monitor (deprecated)</a>
&emsp;<a href="#toc4-6625">zmutex - working with mutexes (deprecated)</a>
&emsp;<a href="#toc4-6669">zproxy_v2 - run a steerable proxy in the background (deprecated)</a>
&emsp;<a href="#toc4-6777">zsocket - working with ØMQ sockets (deprecated)</a>
&emsp;<a href="#toc4-6941">zsockopt - get/set ØMQ socket options (deprecated)</a>
&emsp;<a href="#toc4-7933">zthread - working with system threads (deprecated)</a>

**<a href="#toc2-8050">Error Handling</a>**

**<a href="#toc2-8067">CZMQ Actors</a>**

**<a href="#toc2-8213">Under the Hood</a>**

**<a href="#toc3-8216">Adding a New Class</a>**

**<a href="#toc3-8228">Documentation</a>**

**<a href="#toc3-8267">Development</a>**

**<a href="#toc3-8277">Porting CZMQ</a>**

**<a href="#toc3-8288">Hints to Contributors</a>**

**<a href="#toc3-8300">Code Generation</a>**

**<a href="#toc3-8305">This Document</a>**

<A name="toc2-13" title="Overview" />
## Overview

<A name="toc3-16" title="Scope and Goals" />
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

<A name="toc3-57" title="Ownership and License" />
### Ownership and License

The contributors are listed in AUTHORS. This project uses the MPL v2 license, see LICENSE.

CZMQ uses the [C4.1 (Collective Code Construction Contract)](http://rfc.zeromq.org/spec:22) process for contributions.

CZMQ uses the [CLASS (C Language Style for Scalabilty)](http://rfc.zeromq.org/spec:21) guide for code style.

To report an issue, use the [CZMQ issue tracker](https://github.com/zeromq/czmq/issues) at github.com.

<A name="toc2-68" title="Using CZMQ" />
## Using CZMQ

<A name="toc3-71" title="Building and Installing" />
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

<A name="toc3-115" title="Linking with an Application" />
### Linking with an Application

Include `czmq.h` in your application and link with libczmq. Here is a typical gcc link command:

    gcc -lczmq -lzmq myapp.c -o myapp

<A name="toc3-122" title="Use from Other Languages" />
### Use from Other Languages

This is a list of known higher-level wrappers around CZMQ:

* https://github.com/1100110/CZMQ - D bindings
* https://github.com/methodmissing/rbczmq - Ruby
* https://github.com/zeromq/pyczmq - Python
* https://github.com/lhope/cl-czmq - Common Lisp
* https://github.com/fmp88/ocaml-czmq - Ocaml
* https://github.com/gar1t/erlang-czmq - Erlang
* https://github.com/mtortonesi/ruby-czmq-ffi - Ruby FFI

<A name="toc3-135" title="API v3 Summary" />
### API v3 Summary

This is the API provided by CZMQ v3.x, in alphabetical order.

<A name="toc4-140" title="zactor - simple actor framework" />
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
zmsg, zstr, zpoller, and zloop.

An actor function MUST call zsock_signal (pipe) when initialized
and MUST listen to pipe and exit on $TERM command.


This is the class interface:

    //  Actors get a pipe and arguments from caller
    typedef void (zactor_fn) (zsock_t *pipe, void *args);
    
    //  Create a new actor passing arbitrary arguments reference.
    CZMQ_EXPORT zactor_t *
        zactor_new (zactor_fn *task, void *args);
    
    //  Destroy an actor.
    CZMQ_EXPORT void
        zactor_destroy (zactor_t **p_self);
    
    //  Send a zmsg message to the actor, take ownership of the message
    //  and destroy when it has been sent.
    CZMQ_EXPORT int
        zactor_send (zactor_t *self, zmsg_t **msg_p);
    
    //  Receive a zmsg message from the actor. Returns NULL if the actor
    //  was interrupted before the message could be received, or if there
    //  was a timeout on the actor.
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
    
    //  Self test of this class
    CZMQ_EXPORT void
        zactor_test (bool verbose);

This is the class self test code:

    zactor_t *actor = zactor_new (echo_actor, "Hello, World");
    assert (actor);
    zstr_sendx (actor, "ECHO", "This is a string", NULL);
    char *string = zstr_recv (actor);
    assert (streq (string, "This is a string"));
    free (string);
    zactor_destroy (&actor);

<A name="toc4-212" title="zauth - authentication for ZeroMQ security mechanisms" />
#### zauth - authentication for ZeroMQ security mechanisms

A zauth actor takes over authentication for all incoming connections in
its context. You can whitelist or blacklist peers based on IP address,
and define policies for securing PLAIN, CURVE, and GSSAPI connections.

This class replaces zauth_v2, and is meant for applications that use the
CZMQ v3 API (meaning, zsock).

This is the class interface:

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
    //  or nmore addresses, any non-whitelisted addresses are treated as
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
        zauth (zsock_t *pipe, void *unused);
    
    //  Selftest
    CZMQ_EXPORT void
        zauth_test (bool verbose);

This is the class self test code:

    //  Create temporary directory for test files
    #   define TESTDIR ".test_zauth"
    zsys_dir_create (TESTDIR);
    
    //  Check there's no authentication
    zsock_t *server = zsock_new (ZMQ_PUSH);
    assert (server);
    zsock_t *client = zsock_new (ZMQ_PULL);
    assert (client);
    bool success = s_can_connect (&server, &client);
    assert (success);
    
    //  Install the authenticator
    zactor_t *auth = zactor_new (zauth, NULL);
    assert (auth);
    if (verbose) {
        zstr_sendx (auth, "VERBOSE", NULL);
        zsock_wait (auth);
    }
    //  Check there's no authentication on a default NULL server
    success = s_can_connect (&server, &client);
    assert (success);
    
    //  When we set a domain on the server, we switch on authentication
    //  for NULL sockets, but with no policies, the client connection
    //  will be allowed.
    zsock_set_zap_domain (server, "global");
    success = s_can_connect (&server, &client);
    assert (success);
    
    //  Blacklist 127.0.0.1, connection should fail
    zsock_set_zap_domain (server, "global");
    zstr_sendx (auth, "DENY", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&server, &client);
    assert (!success);
    
    //  Whitelist our address, which overrides the blacklist
    zsock_set_zap_domain (server, "global");
    zstr_sendx (auth, "ALLOW", "127.0.0.1", NULL);
    zsock_wait (auth);
    success = s_can_connect (&server, &client);
    assert (success);
    
    //  Try PLAIN authentication
    zsock_set_plain_server (server, 1);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "Password");
    success = s_can_connect (&server, &client);
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
    success = s_can_connect (&server, &client);
    assert (success);
    
    zsock_set_plain_server (server, 1);
    zsock_set_plain_username (client, "admin");
    zsock_set_plain_password (client, "Bogus");
    success = s_can_connect (&server, &client);
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
        char *server_key = zcert_public_txt (server_cert);
    
        //  Test without setting-up any authentication
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        success = s_can_connect (&server, &client);
        assert (!success);
    
        //  Test CURVE_ALLOW_ANY
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        zstr_sendx (auth, "CURVE", CURVE_ALLOW_ANY, NULL);
        zsock_wait (auth);
        success = s_can_connect (&server, &client);
        assert (success);
    
        //  Test full client authentication using certificates
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsock_set_curve_server (server, 1);
        zsock_set_curve_serverkey (client, server_key);
        zcert_save_public (client_cert, TESTDIR "/mycert.txt");
        zstr_sendx (auth, "CURVE", TESTDIR, NULL);
        zsock_wait (auth);
        success = s_can_connect (&server, &client);
        assert (success);
    
        zcert_destroy (&server_cert);
        zcert_destroy (&client_cert);
    }
    //  Remove the authenticator and check a normal connection works
    zactor_destroy (&auth);
    success = s_can_connect (&server, &client);
    assert (success);
    
    zsock_destroy (&client);
    zsock_destroy (&server);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);

<A name="toc4-425" title="zbeacon - LAN discovery and presence" />
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

This is the class self test code:

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

<A name="toc4-609" title="zcert - work with CURVE security certificates" />
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

    
    //  Create and initialize a new certificate in memory
    CZMQ_EXPORT zcert_t *
        zcert_new (void);
    
    //  Constructor, accepts public/secret key pair from caller
    CZMQ_EXPORT zcert_t *
        zcert_new_from (byte *public_key, byte *secret_key);
    
    //  Destroy a certificate in memory
    CZMQ_EXPORT void
        zcert_destroy (zcert_t **self_p);
    
    //  Return public part of key pair as 32-byte binary string
    CZMQ_EXPORT byte *
        zcert_public_key (zcert_t *self);
    
    //  Return secret part of key pair as 32-byte binary string
    CZMQ_EXPORT byte *
        zcert_secret_key (zcert_t *self);
    
    //  Return public part of key pair as Z85 armored string
    CZMQ_EXPORT char *
        zcert_public_txt (zcert_t *self);
    
    //  Return secret part of key pair as Z85 armored string
    CZMQ_EXPORT char *
        zcert_secret_txt (zcert_t *self);
    
    //  Set certificate metadata from formatted string.
    CZMQ_EXPORT void
        zcert_set_meta (zcert_t *self, const char *name, const char *format, ...);
    
    //  Get metadata value from certificate; if the metadata value doesn't 
    //  exist, returns NULL.
    CZMQ_EXPORT char *
        zcert_meta (zcert_t *self, const char *name);
    
    //  Get list of metadata fields from certificate. Caller is responsible for
    //  destroying list. Caller should not modify the values of list items.
    CZMQ_EXPORT zlist_t *
        zcert_meta_keys (zcert_t *self);
    
    //  Load certificate from file (constructor)
    CZMQ_EXPORT zcert_t *
        zcert_load (const char *filename);
    
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
        zcert_apply (zcert_t *self, void *zocket);
    
    //  Return copy of certificate; if certificate is null or we exhausted
    //  heap memory, returns null.
    CZMQ_EXPORT zcert_t *
        zcert_dup (zcert_t *self);
    
    //  Return true if two certificates have the same keys
    CZMQ_EXPORT bool
        zcert_eq (zcert_t *self, zcert_t *compare);
    
    //  Print certificate contents to stdout
    CZMQ_EXPORT void
        zcert_print (zcert_t *self);
    
    //  DEPRECATED as incompatible with centralized logging
    //  Print certificate contents to open stream
    CZMQ_EXPORT void
        zcert_fprint (zcert_t *self, FILE *file);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zcert_test (bool verbose);

This is the class self test code:

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

<A name="toc4-770" title="zcertstore - work with CURVE security certificate stores" />
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
tools/makecert.c command line tool, or any text editor. The format of a
certificate file is defined in the zcert man page.

This is the class interface:

    
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
    
    //  DEPRECATED as incompatible with centralized logging
    //  Print list of certificates in store to open stream
    CZMQ_EXPORT void
        zcertstore_fprint (zcertstore_t *self, FILE *file);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zcertstore_test (bool verbose);

This is the class self test code:

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
    free (client_key);
    
    if (verbose)
        zcertstore_print (certstore);
    zcertstore_destroy (&certstore);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);

<A name="toc4-869" title="zchunk - work with memory chunks" />
#### zchunk - work with memory chunks

The zchunk class works with variable sized blobs. Not as efficient as
MQ's messages but they do less weirdness and so are easier to understand.
The chunk class has methods to read and write chunks from disk.


This is the class interface:

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
    
    //  Append user-supplied data to chunk, return resulting chunk size
    CZMQ_EXPORT size_t
        zchunk_append (zchunk_t *self, const void *data, size_t size);
    
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
    CZMQ_EXPORT zchunk_t *
        zchunk_read (FILE *handle, size_t bytes);
        
    //  Write chunk to an open file descriptor
    CZMQ_EXPORT int
        zchunk_write (zchunk_t *self, FILE *handle);
    
    //  Try to slurp an entire file into a chunk. Will read up to maxsize of
    //  the file. If maxsize is 0, will attempt to read the entire file and
    //  fail with an assertion if that cannot fit into memory. Returns a new
    //  chunk containing the file data, or NULL if the file could not be read.
    CZMQ_EXPORT zchunk_t *
        zchunk_slurp (const char *filename, size_t maxsize);
    
    //  Create copy of chunk, as new chunk object. Returns a fresh zchunk_t
    //  object, or null if there was not enough heap memory. If chunk is null,
    //  returns null.
    CZMQ_EXPORT zchunk_t *
        zchunk_dup (zchunk_t *self);
    
    //  Return chunk data encoded as printable hex string. Caller must free
    //  string when finished with it.
    CZMQ_EXPORT char *
        zchunk_strhex (zchunk_t *self);
    
    //  Return chunk data copied into freshly allocated string
    //  Caller must free string when finished with it.
    CZMQ_EXPORT char *
        zchunk_strdup (zchunk_t *self);
    
    //  Return TRUE if chunk body is equal to string, excluding terminator
    CZMQ_EXPORT bool
        zchunk_streq (zchunk_t *self, const char *string);
    
    //  Transform zchunk into a zframe that can be sent in a message.
    CZMQ_EXPORT zframe_t *
        zchunk_pack (zchunk_t *self);
    
    //  Transform a zframe into a zchunk.
    CZMQ_EXPORT zchunk_t *
        zchunk_unpack (zframe_t *frame);
    
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
    
    //  Self test of this class
    CZMQ_EXPORT void
        zchunk_test (bool verbose);

This is the class self test code:

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

<A name="toc4-1048" title="zclock - millisecond clocks and delays" />
#### zclock - millisecond clocks and delays

The zclock class provides essential sleep and system time functions, used
to slow down threads for testing, and calculate timers for polling. Wraps
the non-portable system calls in a simple portable API.

The Win32 Sleep() call defaults to 16ms resolution unless the system timer
resolution is increased with a call to timeBeginPeriod() permitting 1ms
granularity.

This is the class interface:

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
    CZMQ_EXPORT char *
        zclock_timestr (void);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zclock_test (bool verbose);

This is the class self test code:

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

<A name="toc4-1106" title="zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL." />
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

    //  Function that executes config
    typedef int (zconfig_fct) (zconfig_t *self, void *arg, int level);
    
    //  Create new config item
    CZMQ_EXPORT zconfig_t *
        zconfig_new (const char *name, zconfig_t *parent);
    
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
    
    //  Set config item name, name may be NULL
    CZMQ_EXPORT void
        zconfig_set_name (zconfig_t *self, const char *name);
    
    //  Set new value for config item. The new value may be a string, a printf
    //  format, or NULL. Note that if string may possibly contain '%', or if it
    //  comes from an insecure source, you must use '%s' as the format, followed
    //  by the string.
    CZMQ_EXPORT void
        zconfig_set_value (zconfig_t *self, const char *format, ...);
        
    //  Find our first child, if any
    CZMQ_EXPORT zconfig_t *
        zconfig_child (zconfig_t *self);
    
    //  Find our first sibling, if any
    CZMQ_EXPORT zconfig_t *
        zconfig_next (zconfig_t *self);
    
    //  Find a config item along a path; leading slash is optional and ignored.
    CZMQ_EXPORT zconfig_t *
        zconfig_locate (zconfig_t *self, const char *path);
    
    //  Resolve a config path into a string value; leading slash is optional
    //  and ignored.
    CZMQ_EXPORT char *
        zconfig_resolve (zconfig_t *self, const char *path, const char *default_value);
    
    //  Set config item name, name may be NULL
    CZMQ_EXPORT void
        zconfig_set_path (zconfig_t *self, const char *path, const char *value);
    
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
        zconfig_set_comment (zconfig_t *self, const char *format, ...);
    
    //  Return comments of config item, as zlist.
    CZMQ_EXPORT zlist_t *
        zconfig_comments (zconfig_t *self);
    
    //  Load a config tree from a specified ZPL text file
    CZMQ_EXPORT zconfig_t *
        zconfig_load (const char *filename);
    
    //  Save a config tree to a specified ZPL text file, where a filename
    //  "-" means dump to standard output.
    CZMQ_EXPORT int
        zconfig_save (zconfig_t *self, const char *filename);
    
    //  Report filename used during zconfig_load, or NULL if none
    CZMQ_EXPORT char *
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
    
    //  Return true if a configuration tree was loaded from a file and that 
    //  file has changed in since the tree was loaded.
    CZMQ_EXPORT bool
        zconfig_has_changed (zconfig_t *self);
    
    //  Print the config file to open stream
    CZMQ_EXPORT void
        zconfig_fprint (zconfig_t *self, FILE *file);
    
    //  Print the config file to stdout
    CZMQ_EXPORT void
        zconfig_print (zconfig_t *self);
    

This is the class self test code:

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
    zconfig_put (root, "/curve/secret-key", "Top Secret");
    zconfig_set_comment (root, "   CURVE certificate");
    zconfig_set_comment (root, "   -----------------");
    assert (zconfig_comments (root));
    zconfig_save (root, TESTDIR "/test.cfg");
    zconfig_destroy (&root);
    root = zconfig_load (TESTDIR "/test.cfg");
    if (verbose)
        zconfig_save (root, "-");
    assert (streq (zconfig_filename (root), TESTDIR "/test.cfg"));
    
    char *email = zconfig_resolve (root, "/headers/email", NULL);
    assert (email);
    assert (streq (email, "some@random.com"));
    char *passwd = zconfig_resolve (root, "/curve/secret-key", NULL);
    assert (passwd);
    assert (streq (passwd, "Top Secret"));
    
    zconfig_save (root, TESTDIR "/test.cfg");
    assert (zconfig_has_changed (root));
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
    zchunk_t *chunk = zconfig_chunk_save (root);
    assert (chunk);
    zconfig_destroy (&root);
    
    root = zconfig_chunk_load (chunk);
    assert (root);
    char *value = zconfig_resolve (root, "/section/value", NULL);
    assert (value);
    assert (streq (value, "somevalue"));
    
    //  Test config can't be saved to a file in a path that doesn't
    //  exist or isn't writable
    rc = zconfig_save (root, TESTDIR "/path/that/doesnt/exist/test.cfg");
    assert (rc == -1);
    
    zconfig_destroy (&root);
    zchunk_destroy (&chunk);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);

<A name="toc4-1339" title="zdigest - provides hashing functions (SHA-1 at present)" />
#### zdigest - provides hashing functions (SHA-1 at present)

The zdigest class generates a hash from zchunks of data. The current
algorithm is SHA-1, chosen for speed. We are aiming to generate a
unique digest for a file, and there are no security issues in this
use case.

The current code depends on OpenSSL, which might be replaced by hard
coded SHA-1 implementation to reduce build dependencies.

This is the class interface:

    //  Constructor - creates new digest object, which you use to build up a
    //  digest by repeatedly calling zdigest_update() on chunks of data.
    CZMQ_EXPORT zdigest_t *
        zdigest_new (void);
        
    //  Destroy a digest object
    CZMQ_EXPORT void
        zdigest_destroy (zdigest_t **self_p);
        
    //  Add buffer into digest calculation
    CZMQ_EXPORT void
        zdigest_update (zdigest_t *self, byte *buffer, size_t length);
        
    //  Return final digest hash data. If built without crypto support, returns
    //  NULL.
    CZMQ_EXPORT byte *
        zdigest_data (zdigest_t *self);
        
    //  Return final digest hash size
    CZMQ_EXPORT size_t
        zdigest_size (zdigest_t *self);
        
    //  Return digest as printable hex string; caller should not modify nor
    //  free this string. After calling this, you may not use zdigest_update()
    //  on the same digest. If built without crypto support, returns NULL.
    CZMQ_EXPORT char *
        zdigest_string (zdigest_t *self);
        
    //  Self test of this class
    CZMQ_EXPORT void
        zdigest_test (bool verbose);

This is the class self test code:

    byte *buffer = (byte *) zmalloc (1024);
    memset (buffer, 0xAA, 1024);
    
    zdigest_t *digest = zdigest_new ();
    assert (digest);
    zdigest_update (digest, buffer, 1024);
    byte *data = zdigest_data (digest);
    assert (data [0] == 0xDE);
    assert (data [1] == 0xB2);
    assert (data [2] == 0x38);
    assert (data [3] == 0x07);
    assert (streq (zdigest_string (digest),
                   "DEB23807D4FE025E900FE9A9C7D8410C3DDE9671"));
    zdigest_destroy (&digest);
    free (buffer);

<A name="toc4-1402" title="zdir - work with file-system directories" />
#### zdir - work with file-system directories

The zdir class gives access to the file system index. It will load
a directory tree (a directory plus all child directories) into a
zdir structure and then let you navigate that structure. It exists
mainly to wrap non-portable OS functions to do this.


This is the class interface:

    //  Create a new directory item that loads in the full tree of the specified
    //  path, optionally located under some parent path. If parent is "-", then
    //  loads only the top-level directory (and does not use parent as a path).
    CZMQ_EXPORT zdir_t *
        zdir_new (const char *path, const char *parent);
    
    //  Destroy a directory tree and all children it contains.
    CZMQ_EXPORT void
        zdir_destroy (zdir_t **self_p);
    
    //  Return directory path
    CZMQ_EXPORT char *
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
        
    //  Returns a sorted array of zfile objects; returns a single block of memory,
    //  that you destroy by calling zstr_free(). Each entry in the array is a pointer
    //  to a zfile_t item already allocated in the zdir tree. The array ends with
    //  a null pointer. Do not destroy the original zdir tree until you are done
    //  with this array.
    CZMQ_EXPORT zfile_t **
        zdir_flatten (zdir_t *self);
    
    //  Free a provided string, and nullify the parent pointer. Safe to call on
    //  a null pointer.
    CZMQ_EXPORT void
        zdir_flatten_free (zfile_t ***files_p);
    
    //  Remove directory, optionally including all files that it contains, at
    //  all levels. If force is false, will only remove the directory if empty.
    //  If force is true, will remove all files and all subdirectories.
    CZMQ_EXPORT void
        zdir_remove (zdir_t *self, bool force);
    
    //  Calculate differences between two versions of a directory tree.
    //  Returns a list of zdir_patch_t patches. Either older or newer may
    //  be null, indicating the directory is empty/absent. If alias is set,
    //  generates virtual filename (minus path, plus alias).
    CZMQ_EXPORT zlist_t *
        zdir_diff (zdir_t *older, zdir_t *newer, const char *alias);
    
    //  Return full contents of directory as a zdir_patch list.
    CZMQ_EXPORT zlist_t *
        zdir_resync (zdir_t *self, const char *alias);
    
    //  Load directory cache; returns a hash table containing the SHA-1 digests
    //  of every file in the tree. The cache is saved between runs in .cache.
    //  The caller must destroy the hash table when done with it.
    CZMQ_EXPORT zhash_t *
        zdir_cache (zdir_t *self);
    
    //  Print contents of directory to open stream
    CZMQ_EXPORT void
        zdir_fprint (zdir_t *self, FILE *file, int indent);
    
    //  Print contents of directory to stdout
    CZMQ_EXPORT void
        zdir_print (zdir_t *self, int indent);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zdir_test (bool verbose);

This is the class self test code:

    zdir_t *older = zdir_new (".", NULL);
    assert (older);
    if (verbose) {
        printf ("\n");
        zdir_dump (older, 0);
    }
    zdir_t *newer = zdir_new ("..", NULL);
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

<A name="toc4-1511" title="zdir_patch - work with directory patches" />
#### zdir_patch - work with directory patches

The zdir_patch class works with one patch, which says "create this
file" or "delete this file" (referring to a zfile item each time).


This is the class interface:

    //  Create new patch
    CZMQ_EXPORT zdir_patch_t *
        zdir_patch_new (const char *path, zfile_t *file,
                        zdir_patch_op_t op, const char *alias);
    
    //  Destroy a patch
    CZMQ_EXPORT void
        zdir_patch_destroy (zdir_patch_t **self_p);
    
    //  Create copy of a patch. If the patch is null, or memory was exhausted,
    //  returns null.
    CZMQ_EXPORT zdir_patch_t *
        zdir_patch_dup (zdir_patch_t *self);
    
    //  Return patch file directory path
    CZMQ_EXPORT char *
        zdir_patch_path (zdir_patch_t *self);
    
    //  Return patch file item
    CZMQ_EXPORT zfile_t *
        zdir_patch_file (zdir_patch_t *self);
    
    //  Return operation
    CZMQ_EXPORT zdir_patch_op_t
        zdir_patch_op (zdir_patch_t *self);
        
    //  Return patch virtual file path
    CZMQ_EXPORT char *
        zdir_patch_vpath (zdir_patch_t *self);
    
    //  Calculate hash digest for file (create only)
    CZMQ_EXPORT void
        zdir_patch_digest_set (zdir_patch_t *self);
    
    //  Return hash digest for patch file 
    CZMQ_EXPORT char *
        zdir_patch_digest (zdir_patch_t *self);
        
    //  Self test of this class
    CZMQ_EXPORT void
        zdir_patch_test (bool verbose);

This is the class self test code:

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

<A name="toc4-1576" title="zfile - provides methods to work with files in a portable fashion." />
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
    CZMQ_EXPORT zfile_t *
        zfile_dup (zfile_t *self);
    
    //  Return file name, remove path if provided
    CZMQ_EXPORT char *
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
    //  sets self->eof. Returns a null chunk in case of error.
    CZMQ_EXPORT zchunk_t *
        zfile_read (zfile_t *self, size_t bytes, off_t offset);
    
    //  Write chunk to file at specified position
    //  Return 0 if OK, else -1
    CZMQ_EXPORT int
        zfile_write (zfile_t *self, zchunk_t *chunk, off_t offset);
    
    //  Close file, if open
    CZMQ_EXPORT void
        zfile_close (zfile_t *self);
    
    //  Return file handle, if opened
    CZMQ_EXPORT FILE *
        zfile_handle (zfile_t *self);
    
    //  Calculate SHA1 digest for file, using zdigest class. Caller should not
    //  modify digest.
    CZMQ_EXPORT char *
        zfile_digest (zfile_t *self);
    
    //  Self test of this class
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

This is the class self test code:

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
    
    //  Now append one byte to file from outside
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

<A name="toc4-1809" title="zframe - working with single message frames" />
#### zframe - working with single message frames

The zframe class provides methods to send and receive single message
frames across ØMQ sockets. A 'frame' corresponds to one zmq_msg_t. When
you read a frame from a socket, the zframe_more() method indicates if the
frame is part of an unfinished multipart message. The zframe_send method
normally destroys the frame, but with the ZFRAME_REUSE flag, you can send
the same frame many times. Frames are binary, and this class has no
special support for text data.


This is the class interface:

    #define ZFRAME_MORE     1
    #define ZFRAME_REUSE    2
    #define ZFRAME_DONTWAIT 4
    
    //  Create a new frame with optional size, and optional data
    CZMQ_EXPORT zframe_t *
        zframe_new (const void *data, size_t size);
    
    //  Create an empty (zero-sized) frame
    CZMQ_EXPORT zframe_t *
        zframe_new_empty (void);
    
    //  Destroy a frame
    CZMQ_EXPORT void
        zframe_destroy (zframe_t **self_p);
    
    //  Receive frame from socket, returns zframe_t object or NULL if the recv
    //  was interrupted. Does a blocking recv, if you want to not block then use
    //  zframe_recv_nowait().
    CZMQ_EXPORT zframe_t *
        zframe_recv (void *source);
    
    // Send a frame to a socket, destroy frame after sending.
    // Return -1 on error, 0 on success.
    CZMQ_EXPORT int
        zframe_send (zframe_t **self_p, void *dest, int flags);
    
    //  Return number of bytes in frame data
    CZMQ_EXPORT size_t
        zframe_size (zframe_t *self);
    
    //  Return address of frame data
    CZMQ_EXPORT byte *
        zframe_data (zframe_t *self);
    
    //  Create a new frame that duplicates an existing frame. If frame is null,
    //  or memory was exhausted, returns null.
    CZMQ_EXPORT zframe_t *
        zframe_dup (zframe_t *self);
    
    //  Return frame data encoded as printable hex string, useful for ØMQ UUIDs.
    //  Caller must free string when finished with it.
    CZMQ_EXPORT char *
        zframe_strhex (zframe_t *self);
    
    //  Return frame data copied into freshly allocated string
    //  Caller must free string when finished with it.
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
    
    //  Self test of this class
    CZMQ_EXPORT void
        zframe_test (bool verbose);

This is the class self test code:

    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new_pair ("@inproc://zframe.test");
    assert (output);
    zsock_t *input = zsock_new_pair (">inproc://zframe.test");
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
    
    zsock_destroy (&input);
    zsock_destroy (&output);
    

<A name="toc4-1979" title="zgossip - decentralized configuration management" />
#### zgossip - decentralized configuration management

Implements a gossip protocol for decentralized configuration management.
Your applications nodes form a loosely connected network (which can have
cycles), and publish name/value tuples. Each node re-distributes the new
tuples it receives, so that the entire network eventually achieves a
consistent state. The current design does not expire tuples.

Provides these commands (sent as multipart strings to the actor):

* BIND endpoint -- binds the gossip service to specified endpoint
* PORT -- returns the last TCP port, if any, used for binding
* CONFIGURE configfile -- load configuration from specified file
* SET configpath value -- set configuration path = value
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
    //      zstr_sendx (zgossip, "CONFIGURE", filename, NULL);
    //
    //  Set configuration path value:
    //
    //      zstr_sendx (zgossip, "SET", path, value, NULL);
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
    void
        zgossip (zsock_t *pipe, void *args);
    
    //  Self test of this class
    void
        zgossip_test (bool verbose);

This is the class self test code:

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
    

<A name="toc4-2169" title="zhash - generic type-free hash container" />
#### zhash - generic type-free hash container

zhash is an expandable hash table container.

The hash table always has a size that is prime and roughly doubles its
size when 75% full. In case of hash collisions items are chained in a
linked list. The hash table size is increased slightly (up to 5 times
before roughly doubling the size) when an overly long chain (between 1
and 63 items depending on table size) is detected.

This is the class interface:

    //  Callback function for zhash_freefn method
    typedef void (zhash_free_fn) (void *data);
    
    //  Callback function for hashing keys
    typedef size_t (zhash_hash_fn) (const void *key);
    
    //  Create a new, empty hash container
    CZMQ_EXPORT zhash_t *
        zhash_new (void);
    
    //  Destroy a hash container and all items in it
    CZMQ_EXPORT void
        zhash_destroy (zhash_t **self_p);
    
    //  Insert item into hash table with specified key and item.
    //  If key is already present returns -1 and leaves existing item unchanged
    //  Returns 0 on success.
    CZMQ_EXPORT int
        zhash_insert (zhash_t *self, const void *key, void *item);
    
    //  Update or insert item into hash table with specified key and item. If the
    //  key is already present, destroys old item and inserts new one. If you set
    //  a container item destructor, this is called on the old value. If the key
    //  was not already present, inserts a new item. Sets the hash cursor to the
    //  new item.
    CZMQ_EXPORT void
        zhash_update (zhash_t *self, const void *key, void *item);
    
    //  Remove an item specified by key from the hash table. If there was no such
    //  item, this function does nothing.
    CZMQ_EXPORT void
        zhash_delete (zhash_t *self, const void *key);
    
    //  Delete all items from the hash table. If the key destructor is
    //  set, calls it on every key. If the item destructor is set, calls
    //  it on every item.
    CZMQ_EXPORT void
        zhash_purge (zhash_t *self);
    
    //  Return the item at the specified key, or null
    CZMQ_EXPORT void *
        zhash_lookup (zhash_t *self, const void *key);
    
    //  Reindexes an item from an old key to a new key. If there was no such
    //  item, does nothing. Returns 0 if successful, else -1.
    CZMQ_EXPORT int
        zhash_rename (zhash_t *self, const void *old_key, const void *new_key);
    
    //  Set a free function for the specified hash table item. When the item is
    //  destroyed, the free function, if any, is called on that item.
    //  Use this when hash items are dynamically allocated, to ensure that
    //  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
    //  Returns the item, or NULL if there is no such item.
    CZMQ_EXPORT void *
        zhash_freefn (zhash_t *self, const void *key, zhash_free_fn *free_fn);
    
    //  Return the number of keys/items in the hash table
    CZMQ_EXPORT size_t
        zhash_size (zhash_t *self);
    
    //  Return a zlist_t containing the keys for the items in the
    //  table. Uses the key_duplicator to duplicate all keys and sets the
    //  key_destructor as destructor for the list.
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
    
    //  After a successful insert, update, or first/next method, returns the key
    //  for the item that was returned. This is a constant string that you may
    //  not modify or deallocate, and which lasts as long as the item in the hash.
    //  After an unsuccessful first/next, returns NULL.
    CZMQ_EXPORT const void *
        zhash_cursor (zhash_t *self);
    
    //  Add a comment to hash table before saving to disk. You can add as many
    //  comment lines as you like. These comment lines are discarded when loading
    //  the file. If you use a null format, all comments are deleted.
    CZMQ_EXPORT void
        zhash_comment (zhash_t *self, const char *format, ...);
    
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
    CZMQ_EXPORT zframe_t *
        zhash_pack (zhash_t *self);
        
    //  Unpack binary frame into a new hash table. Packed data must follow format
    //  defined by zhash_pack. Hash table is set to autofree. An empty frame
    //  unpacks to an empty hash table.
    CZMQ_EXPORT zhash_t *
        zhash_unpack (zframe_t *frame);
    
    //  Make a copy of the list; items are duplicated if you set a duplicator
    //  for the list, otherwise not. Copying a null reference returns a null
    //  reference. Note that this method's behavior changed slightly for CZMQ
    //  v3.x, as it does not set nor respect autofree. It does however let you
    //  duplicate any hash table safely. The old behavior is in zhash_dup_v2.
    CZMQ_EXPORT zhash_t *
        zhash_dup (zhash_t *self);
    
    //  Set a user-defined deallocator for hash items; by default items are not
    //  freed when the hash is destroyed.
    CZMQ_EXPORT void
        zhash_set_destructor (zhash_t *self, czmq_destructor destructor);
    
    //  Set a user-defined duplicator for hash items; by default items are not
    //  copied when the hash is duplicated.
    CZMQ_EXPORT void
        zhash_set_duplicator (zhash_t *self, czmq_duplicator duplicator);
    
    //  Set a user-defined deallocator for keys; by default keys are freed
    //  when the hash is destroyed using free().
    CZMQ_EXPORT void
        zhash_set_key_destructor (zhash_t *self, czmq_destructor destructor);
    
    //  Set a user-defined duplicator for keys; by default keys are duplicated
    //  using strdup.
    CZMQ_EXPORT void
        zhash_set_key_duplicator (zhash_t *self, czmq_duplicator duplicator);
    
    //  Set a user-defined comparator for keys; by default keys are
    //  compared using strcmp.
    CZMQ_EXPORT void
        zhash_set_key_comparator (zhash_t *self, czmq_comparator comparator);
    
    //  Set a user-defined hash function for keys; by default keys are
    //  hashed by a modified Bernstein hashing function.
    CZMQ_EXPORT void
        zhash_set_key_hasher (zhash_t *self, zhash_hash_fn hasher);
    
    //  DEPRECATED by zhash_dup
    //  Make copy of hash table; if supplied table is null, returns null.
    //  Does not copy items themselves. Rebuilds new table so may be slow on
    //  very large tables. NOTE: only works with item values that are strings
    //  since there's no other way to know how to duplicate the item value.
    CZMQ_EXPORT zhash_t *
        zhash_dup_v2 (zhash_t *self);
    
    //  DEPRECATED as clumsy -- use set_destructor instead
    //  Set hash for automatic value destruction
    CZMQ_EXPORT void
        zhash_autofree (zhash_t *self);
    
    //  DEPRECATED as clumsy -- use zhash_first/_next instead
    typedef int (zhash_foreach_fn) (const char *key, void *item, void *argument);
    
    //  DEPRECATED as clumsy -- use zhash_first/_next instead
    //  Apply function to each item in the hash table. Items are iterated in no
    //  defined order. Stops if callback function returns non-zero and returns
    //  final return code from callback function (zero = success).
    //  Callback function for zhash_foreach method
    CZMQ_EXPORT int
        zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zhash_test (int verbose);

This is the class self test code:

    zhash_t *hash = zhash_new ();
    assert (hash);
    assert (zhash_size (hash) == 0);
    assert (zhash_first (hash) == NULL);
    assert (zhash_cursor (hash) == NULL);
    
    //  Insert some items
    int rc;
    rc = zhash_insert (hash, "DEADBEEF", "dead beef");
    char *item = (char *) zhash_first (hash);
    assert (streq ((char *) zhash_cursor (hash), "DEADBEEF"));
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
    
    //  Test autofree; automatically copies and frees string values
    hash = zhash_new ();
    assert (hash);
    zhash_autofree (hash);
    char value [255];
    strcpy (value, "This is a string");
    rc = zhash_insert (hash, "key1", value);
    assert (rc == 0);
    strcpy (value, "Ring a ding ding");
    rc = zhash_insert (hash, "key2", value);
    assert (rc == 0);
    assert (streq ((char *) zhash_lookup (hash, "key1"), "This is a string"));
    assert (streq ((char *) zhash_lookup (hash, "key2"), "Ring a ding ding"));
    zhash_destroy (&hash);

<A name="toc4-2549" title="ziflist - List of network interfaces available on system" />
#### ziflist - List of network interfaces available on system

The ziflist class takes a snapshot of the network interfaces that the
system currently supports (this can change arbitrarily, especially on
mobile devices). The caller can then access the network interface
information using an iterator that works like zlist. Only stores those
interfaces with broadcast capability, and ignores the loopback interface.


This is the class interface:

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
    
    //  Selftest for this class
    CZMQ_EXPORT void
        ziflist_test (bool verbose);

This is the class self test code:

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

<A name="toc4-2620" title="zlist - generic type-free list container" />
#### zlist - generic type-free list container

Provides a generic container implementing a fast singly-linked list. You
can use this to construct multi-dimensional lists, and other structures
together with other generic containers like zhash.

To iterate through a list, use zlist_first to get the first item, then
loop while not null, and do zlist_next at the end of each iteration.

This is the class interface:

    //  Comparison function for zlist_sort method
    typedef bool (zlist_compare_fn) (void *item1, void *item2);
    
    // Callback function for zlist_freefn method
    typedef void (zlist_free_fn) (void *data);
    
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
    
    //  Remove the specified item from the list if present
    CZMQ_EXPORT void
        zlist_remove (zlist_t *self, void *item);
    
    //  Make a copy of list. If the list has autofree set, the copied list will
    //  duplicate all items, which must be strings. Otherwise, the list will hold
    //  pointers back to the items in the original list.
    CZMQ_EXPORT zlist_t *
        zlist_dup (zlist_t *self);
    
    //  Purge all items from list
    CZMQ_EXPORT void
    zlist_purge (zlist_t *self);
    
    //  Return number of items in the list
    CZMQ_EXPORT size_t
        zlist_size (zlist_t *self);
    
    //  Sort the list by ascending key value using a straight ASCII comparison.
    //  The sort is not stable, so may reorder items with the same keys.
    CZMQ_EXPORT void
        zlist_sort (zlist_t *self, zlist_compare_fn *compare);
    
    //  Set a user-defined destructor for items; by default items are not
    //  freed when the list is destroyed.
    CZMQ_EXPORT void
        zlist_set_destructor (zlist_t *self, czmq_destructor destructor);
    
    //  Set a user-defined duplicator for items; by default items are not
    //  copied when the list is duplicated.
    CZMQ_EXPORT void
        zlist_set_duplicator (zlist_t *self, czmq_duplicator duplicator);
    
    //  DEPRECATED by zlist_set_duplicator/zlist_set_destructor
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
    
    //  DEPRECATED by zlist_set_duplicator/zlist_set_destructor
    //  Set a free function for the specified list item. When the item is
    //  destroyed, the free function, if any, is called on that item.
    //  Use this when list items are dynamically allocated, to ensure that
    //  you don't have memory leaks. You can pass 'free' or NULL as a free_fn.
    //  Returns the item, or NULL if there is no such item.
    CZMQ_EXPORT void *
        zlist_freefn (zlist_t *self, void *item, zlist_free_fn *fn, bool at_tail);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zlist_test (int verbose);

This is the class self test code:

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
    zlist_append (list, bread);
    assert (zlist_size (list) == 2);
    zlist_append (list, wine);
    assert (zlist_size (list) == 3);
    
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
    
    zlist_sort (list, s_compare);
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
    
    //  Destructor should be safe to call twice
    zlist_destroy (&list);
    zlist_destroy (&list);
    assert (list == NULL);

<A name="toc4-2840" title="zloop - event-driven reactor" />
#### zloop - event-driven reactor

The zloop class provides an event-driven reactor pattern. The reactor
handles zmq_pollitem_t items (pollers or writers, sockets or fds), and
once-off or repeated timers. Its resolution is 1 msec. It uses a tickless
timer to reduce CPU interrupts in inactive processes.


This is the class interface:

    //  Callback function for reactor socket activity
    typedef int (zloop_reader_fn) (zloop_t *loop, zsock_t *reader, void *arg);
    
    //  Callback function for reactor events (low-level)
    typedef int (zloop_fn) (zloop_t *loop, zmq_pollitem_t *item, void *arg);
    
    // Callback for reactor timer events
    typedef int (zloop_timer_fn) (zloop_t *loop, int timer_id, void *arg);
    
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
    
    //  Set verbose tracing of reactor on/off
    CZMQ_EXPORT void
        zloop_set_verbose (zloop_t *self, bool verbose);
    
    //  Start the reactor. Takes control of the thread and returns when the ØMQ
    //  context is terminated or the process is interrupted, or any event handler
    //  returns -1. Event handlers may register new sockets and timers, and
    //  cancel sockets. Returns 0 if interrupted, -1 if cancelled by a handler.
    CZMQ_EXPORT int
        zloop_start (zloop_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zloop_test (bool verbose);

This is the class self test code:

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
    
    //  When we get the ping message, end the reactor
    rc = zloop_reader (loop, input, s_socket_event, NULL);
    assert (rc == 0);
    zloop_reader_set_tolerant (loop, input);
    zloop_start (loop);
    
    zloop_destroy (&loop);
    assert (loop == NULL);
    
    zsock_destroy (&input);
    zsock_destroy (&output);

<A name="toc4-2965" title="zmonitor - socket event monitor" />
#### zmonitor - socket event monitor

The zmonitor actor provides an API for obtaining socket events such as
connected, listen, disconnected, etc. Socket events are only available
for sockets connecting or bound to ipc:// and tcp:// endpoints.

This class wraps the ZMQ socket monitor API, see zmq_socket_monitor for
details. Works on all versions of libzmq from 3.2 onwards. This class
replaces zproxy_v2, and is meant for applications that use the CZMQ v3
API (meaning, zsock).

This is the class interface:

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

This is the class self test code:

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

<A name="toc4-3069" title="zmsg - working with multipart messages" />
#### zmsg - working with multipart messages

The zmsg class provides methods to send and receive multipart messages
across ØMQ sockets. This class provides a list-like container interface,
with methods to work with the overall container. zmsg_t messages are
composed of zero or more zframe_t frames.


This is the class interface:

    //  Create a new empty message object
    CZMQ_EXPORT zmsg_t *
        zmsg_new (void);
    
    //  Destroy a message object and all frames it contains
    CZMQ_EXPORT void
        zmsg_destroy (zmsg_t **self_p);
    
    //  Receive message from socket, returns zmsg_t object or NULL if the recv
    //  was interrupted. Does a blocking recv. If you want to not block then use
    //  the zloop class or zmsg_recv_nowait or zmq_poll to check for socket input
    //  before receiving.
    CZMQ_EXPORT zmsg_t *
        zmsg_recv (void *source);
    
    //  Send message to destination socket, and destroy the message after sending
    //  it successfully. If the message has no frames, sends nothing but destroys
    //  the message anyhow. Nullifies the caller's reference to the message (as
    //  it is a destructor).
    CZMQ_EXPORT int
        zmsg_send (zmsg_t **self_p, void *dest);
    
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
    
    //  Remove first frame from message, if any. Returns frame, or NULL. Caller
    //  now owns frame and must destroy it when finished with it.
    CZMQ_EXPORT zframe_t *
        zmsg_pop (zmsg_t *self);
    
    //  Push block of memory to front of message, as a new frame.
    //  Returns 0 on success, -1 on error.
    CZMQ_EXPORT int
        zmsg_pushmem (zmsg_t *self, const void *src, size_t size);
    
    //  Add block of memory to the end of the message, as a new frame.
    //  Returns 0 on success, -1 on error.
    CZMQ_EXPORT int
        zmsg_addmem (zmsg_t *self, const void *src, size_t size);
        
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
        zmsg_pushstrf (zmsg_t *self, const char *format, ...);
    
    //  Push formatted string as new frame to end of message.
    //  Returns 0 on success, -1 on error.
    CZMQ_EXPORT int
        zmsg_addstrf (zmsg_t *self, const char *format, ...);
    
    //  Pop frame off front of message, return as fresh string. If there were
    //  no more frames in the message, returns NULL.
    CZMQ_EXPORT char *
        zmsg_popstr (zmsg_t *self);
    
    //  Pop frame off front of message, return as integer value. If there were
    //  no more frames in the message, returns 0.
    CZMQ_EXPORT int
        zmsg_popint (zmsg_t *self);
    
    //  Push encoded message as a new frame. Message takes ownership of
    //  submessage, so the original is destroyed in this call. Returns 0 on
    //  success, -1 on error.
    CZMQ_EXPORT int
        zmsg_addmsg (zmsg_t *self, zmsg_t **msg_p);
    
    //  Remove first submessage from message, if any. Returns zmsg_t, or NULL if
    //  decoding was not succesfull. Caller now owns message and must destroy it
    //  when finished with it.
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
    
    //  Load/append an open file into message, create new message if
    //  null message provided. Returns NULL if the message could not 
    //  be loaded.
    CZMQ_EXPORT zmsg_t *
        zmsg_load (zmsg_t *self, FILE *file);
    
    //  Serialize multipart message to a single buffer. Use this method to send
    //  structured messages across transports that do not support multipart data.
    //  Allocates and returns a new buffer containing the serialized message.
    //  To decode a serialized message buffer, use zmsg_decode ().
    CZMQ_EXPORT size_t
        zmsg_encode (zmsg_t *self, byte **buffer);
    
    //  Decodes a serialized message buffer created by zmsg_encode () and returns
    //  a new zmsg_t object. Returns NULL if the buffer was badly formatted or 
    //  there was insufficient memory to work.
    CZMQ_EXPORT zmsg_t *
        zmsg_decode (byte *buffer, size_t buffer_size);
    
    //  Create copy of message, as new message object. Returns a fresh zmsg_t
    //  object. If message is null, or memory was exhausted, returns null.
    CZMQ_EXPORT zmsg_t *
        zmsg_dup (zmsg_t *self);
    
    //  Send message to zsys log sink (may be stdout, or system facility as
    //  configured by zsys_set_logstream).
    CZMQ_EXPORT void
        zmsg_print (zmsg_t *self);
    
    //  Probe the supplied object, and report if it looks like a zmsg_t.
    CZMQ_EXPORT bool
        zmsg_is (void *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zmsg_test (bool verbose);

This is the class self test code:

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
    
    // create empty file for null test
    FILE *file = fopen ("zmsg.test", "w");
    assert (file);
    fclose (file);
    
    file = fopen ("zmsg.test", "r");
    zmsg_t *null_msg = zmsg_load (NULL, file);
    assert (null_msg == NULL);
    fclose (file);
    remove ("zmsg.test");
    
    //  Save to a file, read back
    file = fopen ("zmsg.test", "w");
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
    msg = zmsg_load (NULL, file);
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
    byte *buffer;
    size_t buffer_size = zmsg_encode (msg, &buffer);
    zmsg_destroy (&msg);
    msg = zmsg_decode (buffer, buffer_size);
    assert (msg);
    free (buffer);
    zmsg_destroy (&msg);
    
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
    

<A name="toc4-3442" title="zpoller - trivial socket poller class" />
#### zpoller - trivial socket poller class

The zpoller class provides a minimalist interface to ZeroMQ's zmq_poll
API, for the very common case of reading from a number of sockets.
It does not provide polling for output, nor polling on file handles.
If you need either of these, use the zmq_poll API directly.


This is the class interface:

    //  Create new poller; the reader can be a libzmq socket (void *), a zsock_t
    //  instance, or a zactor_t instance.
    CZMQ_EXPORT zpoller_t *
        zpoller_new (void *reader, ...);
    
    //  Destroy a poller
    CZMQ_EXPORT void
        zpoller_destroy (zpoller_t **self_p);
    
    //  Add a reader to be polled. Returns 0 if OK, -1 on failure. The reader may
    //  be a libzmq void * socket, a zsock_t instance, or a zactor_t instance.
    CZMQ_EXPORT int
        zpoller_add (zpoller_t *self, void *reader);
    
    //  Remove a reader from the poller; returns 0 if OK, -1 on failure. The
    //  reader may be a libzmq void * socket, a zsock_t instance, or a zactor_t
    //  instance.
    CZMQ_EXPORT int
        zpoller_remove (zpoller_t *self, void *reader);
    
    //  Poll the registered readers for I/O, return first reader that has input.
    //  The reader will be a libzmq void * socket, or a zsock_t or zactor_t
    //  instance as specified in zpoller_new/zpoller_add. The order that
    //  sockets are defined in the poll list affects their priority. If you
    //  need a balanced poll, use the low level zmq_poll method directly. If
    //  the poll call was interrupted (SIGINT), or the ZMQ context was
    //  destroyed, or the timeout expired, returns NULL. You can test the
    //  actual exit condition by calling zpoller_expired () and
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
    
    //  Self test of this class
    CZMQ_EXPORT void
        zpoller_test (bool verbose);

This is the class self test code:

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
    
    //  Set-up poller
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
    
    // Stop polling reader
    rc = zpoller_remove (poller, sink);
    assert (rc == 0);
    
    // Check fd works
    rc = zsock_connect (bowl, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc != -1);
    int fd = zsock_fd (bowl);
    rc = zpoller_add (poller, (void *) &fd);
    assert (rc != -1);
    zstr_send (vent, "Hello again, world");
    assert (zpoller_wait (poller, 500) == &fd);
    
    //  Destroy poller and sockets
    zpoller_destroy (&poller);
    
    zsock_destroy (&vent);
    zsock_destroy (&sink);
    zsock_destroy (&bowl);
    zsock_destroy (&dish);

<A name="toc4-3555" title="zproxy - run a steerable proxy in the background" />
#### zproxy - run a steerable proxy in the background

A zproxy actor switches messages between a frontend and a backend socket.
It acts much like the zmq_proxy_steerable method, though it makes benefit
of CZMQ's facilities, to be somewhat simpler to set-up.

This class replaces zproxy_v2, and is meant for applications that use the
CZMQ v3 API (meaning, zsock).

This is the class interface:

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
    //  This is the zproxy constructor as a zactor_fn; the argument is a
    //  character string specifying frontend and backend socket types as two
    //  uppercase strings separated by a hyphen:
    CZMQ_EXPORT void
        zproxy (zsock_t *pipe, void *unused);
    
    //  Selftest
    CZMQ_EXPORT void
        zproxy_test (bool verbose);

This is the class self test code:

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

<A name="toc4-3701" title="zrex - work with regular expressions" />
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


This is the class interface:

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

This is the class self test code:


<A name="toc4-3792" title="zring - generic type-free doubly linked ring container" />
#### zring - generic type-free doubly linked ring container

Provides a generic "ring" container, which mixes a doubly-linked list
with a hash table to provide both ordered and direct keyed access to
items. The zring class is more complex and powerful than zlist, which
is considered a 'basic' list container. This container provides hooks
for duplicator, comparator, and destructor functions. These tie into
CZMQ and standard C semantics, so e.g. for string items you can use
strdup, strcmp, and zstr_free. To store custom objects, define your
own duplicator and comparator, and use the standard object destructor.


This is the class interface:

    //  Create a new ring container (a ring is a doubly-linked ring)
    CZMQ_EXPORT zring_t *
        zring_new (void);
    
    //  Destroy a ring container
    CZMQ_EXPORT void
        zring_destroy (zring_t **self_p);
    
    //  Prepend an item to the start of the ring, return 0 if OK, else -1.
    //  Leaves cursor at newly inserted item.
    CZMQ_EXPORT int
        zring_prepend (zring_t *self, void *item);
    
    //  Append an item to the end of the ring, return 0 if OK, else -1.
    //  Leaves cursor at newly inserted item.
    CZMQ_EXPORT int
        zring_append (zring_t *self, void *item);
    
    //  Append an item to the end of the ring, and insert into the ring
    //  dictionary, so that you can find the item rapidly using zring_lookup.
    //  If you do a lot of item searches, this is faster than zring_find,
    //  which is at worst an O(N) operation. When items leave the ring, they
    //  are always removed from the dictionary. Returns 0 on success, -1 if
    //  the key already existed in the dictionary, or heap memory ran out.
    CZMQ_EXPORT int
        zring_insert (zring_t *self, const void *key, void *item);
        
    //  Find an item in the ring, looking first at the cursor, and then from the
    //  first to last item. If a comparator was set on container, calls this to
    //  compare each item in the ring with the supplied target item. If none
    //  was set, compares the two item pointers for equality. If the item is
    //  found, leaves the cursor at the found item. Returns the item if found,
    //  else null.
    CZMQ_EXPORT void *
        zring_find (zring_t *self, void *item);
    
    //  Search the ring dictionary for an item, by key. If the item is in the
    //  dictionary (via zring_insert), then sets the ring cursor to the item,
    //  and returns the item value. If not, leaves the cursor unchanged, and
    //  returns NULL.
    CZMQ_EXPORT void *
        zring_lookup (zring_t *self, const void *key);
    
    //  Detach an item from the ring, without destroying the item. Searches the
    //  ring for the item, always starting with the cursor, if any is set, and
    //  then from the start of the ring. If item is null, detaches the item at the
    //  cursor, if set. If the item was found and detached, leaves the cursor at
    //  the next item, if any, and returns the item. Else, returns null.
    CZMQ_EXPORT void *
        zring_detach (zring_t *self, void *item);
    
    //  Remove an item from the ring, and destroy it, if the item destructor is
    //  set. Searches the ring for the item, always starting with the cursor, if
    //  any is set, and then from the start of the ring. If item is null, removes
    //  the item at the cursor, if set. If the item was found and removed, leaves
    //  the cursor at the next item, if any, and returns 0. Else, returns -1.
    CZMQ_EXPORT int
        zring_remove (zring_t *self, void *item);
    
    //  Search the ring dictionary for an item, by key. If the item is in the
    //  dictionary (via zring_insert), then removes the item from the ring and
    //  calls the item destructor, if any is found. Returns 0 if the item was
    //  found and removed, else -1 if not found.
    CZMQ_EXPORT int
        zring_delete (zring_t *self, const void *key);
        
    //  Delete all items from the ring. If the item destructor is set, calls it
    //  on every item.
    CZMQ_EXPORT void
        zring_purge (zring_t *self);
    
    //  Return number of items in the ring
    CZMQ_EXPORT size_t
        zring_size (zring_t *self);
    
    //  Return the item at the head of ring. If the ring is empty, returns NULL.
    //  Leaves cursor pointing at the head item, or NULL if the ring is empty.
    CZMQ_EXPORT void *
        zring_first (zring_t *self);
    
    //  Return the item at the tail of ring. If the ring is empty, returns NULL.
    //  Leaves cursor pointing at the tail item, or NULL if the ring is empty.
    CZMQ_EXPORT void *
        zring_last (zring_t *self);
        
    //  Return the next item. At the end of the ring (or in an empty ring),
    //  returns NULL. Use repeated zring_next () calls to work through the ring
    //  from zring_first ().
    CZMQ_EXPORT void *
        zring_next (zring_t *self);
        
    //  Return the previous item. At the start of the ring (or in an empty ring),
    //  returns NULL. Use repeated zring_prev () calls to work through the ring
    //  backwards from zring_last ().
    CZMQ_EXPORT void *
        zring_prev (zring_t *self);
    
    //  Return the key of the current item in the ring. If the ring is empty, or the
    //  cursor passed the end of the ring, returns NULL. Does not change the cursor.
    CZMQ_EXPORT const void *
        zring_key (zring_t *self);
    
    //  Return current item in the ring. If the ring is empty, or the cursor
    //  passed the end of the ring, returns NULL. Does not change the cursor.
    CZMQ_EXPORT void *
        zring_item (zring_t *self);
    
    //  Sort the list using the compare function.
    //  The sort is not stable, so may reorder items with the same keys.
    CZMQ_EXPORT void
        zlist_sort (zlist_t *self, zlist_compare_fn *compare);
    
    //  Make a copy of the ring; items are duplicated if you set a duplicator
    //  for the ring, otherwise not. Copying a null reference returns a null
    //  reference.
    CZMQ_EXPORT zring_t *
        zring_dup (zring_t *self);
        
    //  Set a user-defined deallocator for ring items; by default items are not
    //  freed when the ring is destroyed.
    CZMQ_EXPORT void
        zring_set_destructor (zring_t *self, czmq_destructor destructor);
    
    //  Set a user-defined duplicator for ring items; by default items are not
    //  copied when the ring is duplicated.
    CZMQ_EXPORT void
        zring_set_duplicator (zring_t *self, czmq_duplicator duplicator);
    
    //  Set a user-defined comparator for zring_find and zring_sort; the method
    //  must return -1, 0, or 1 depending on whether item1 is less than, equal to,
    //  or greater than, item2.
    CZMQ_EXPORT void
        zring_set_comparator (zring_t *self, czmq_comparator comparator);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zring_test (int verbose);

This is the class self test code:

    zring_t *ring = zring_new ();
    assert (ring);
    assert (zring_size (ring) == 0);
    
    //  Three items we'll use as test data
    //  Ring items are void *, not particularly strings
    char *cheese = "boursin";
    char *bread = "baguette";
    char *wine = "bordeaux";
    
    int rc = zring_append (ring, cheese);
    assert (!rc);
    assert (zring_size (ring) == 1);
    assert (zring_item (ring) == cheese);
    rc = zring_append (ring, bread);
    assert (!rc);
    assert (zring_size (ring) == 2);
    assert (zring_item (ring) == bread);
    rc = zring_append (ring, wine);
    assert (!rc);
    assert (zring_size (ring) == 3);
    assert (zring_item (ring) == wine);
    
    assert (zring_first (ring) == cheese);
    assert (zring_next (ring) == bread);
    assert (zring_next (ring) == wine);
    assert (zring_next (ring) == NULL);
    //  After we reach end of ring, next wraps around
    assert (zring_next (ring) == cheese);
    
    assert (zring_last (ring) == wine);
    assert (zring_prev (ring) == bread);
    assert (zring_prev (ring) == cheese);
    assert (zring_prev (ring) == NULL);
    //  After we reach start of ring, prev wraps around
    assert (zring_prev (ring) == wine);
    zring_purge (ring);
    
    //  Test some list insertion-deletion combos
    assert (zring_size (ring) == 0);
    zring_prepend (ring, "4");
    zring_append (ring, "3");
    zring_prepend (ring, "5");
    zring_append (ring, "2");
    zring_prepend (ring, "0");
    zring_append (ring, "1");
    assert (zring_size (ring) == 6);
    
    //  Try the comparator functionality
    zring_set_comparator (ring, (czmq_comparator *) strcmp);
    zring_sort (ring);
    
    char *item = (char *) zring_first (ring);
    assert (streq (item, "0"));
    item = (char *) zring_find (ring, "5");
    assert (streq (item, "5"));
    
    //  Try the duplicator and destructor
    zring_set_duplicator (ring, (czmq_duplicator *) strdup);
    zring_t *dup = zring_dup (ring);
    assert (dup);
    zring_set_destructor (dup, (czmq_destructor *) zstr_free);
    assert (zring_size (dup) == 6);
    zring_destroy (&dup);
    
    //  We're comparing as strings, not item pointers
    rc = zring_remove (ring, "2");
    assert (rc == 0);
    rc = zring_remove (ring, "5");
    assert (rc == 0);
    rc = zring_remove (ring, "3");
    assert (rc == 0);
    item = (char *) zring_detach (ring, NULL);
    zring_purge (ring);
    
    //  Try the dictionary insert/delete functionality
    rc = zring_insert (ring, "1", "one");
    assert (rc == 0);
    rc = zring_insert (ring, "3", "three");
    assert (rc == 0);
    rc = zring_insert (ring, "2", "two");
    assert (rc == 0);
    rc = zring_insert (ring, "2", "two");
    assert (rc == -1);
    
    const char *key;
    item = (char *) zring_lookup (ring, "2");
    assert (streq (item, "two"));
    key =  (char *) zring_key (ring);
    assert (streq (key, "2"));
    item = (char *) zring_lookup (ring, "1");
    assert (streq (item, "one"));
    key =  (char *) zring_key (ring);
    assert (streq (key, "1"));
    item = (char *) zring_item (ring);
    assert (streq (item, "one"));
    key = (char *) zring_key (ring);
    assert (streq (key, "1"));
    
    item = (char *) zring_first (ring);
    key = (char *) zring_key (ring);
    assert (streq (key, "1"));
    item = (char *) zring_next (ring);
    key = (char *) zring_key (ring);
    assert (streq (key, "3"));
    item = (char *) zring_next (ring);
    key = (char *) zring_key (ring);
    assert (streq (key, "2"));
    
    rc = zring_delete (ring, "3");
    assert (rc == 0);
    rc = zring_delete (ring, "3");
    assert (rc == -1);
    //  Using detach/remove will also remove from dictionary
    rc = zring_remove (ring, "two");
    assert (rc == 0);
    rc = zring_delete (ring, "2");
    assert (rc == -1);
    zring_purge (ring);
    
    //  Destructor should be safe to call twice
    zring_destroy (&ring);
    assert (ring == NULL);
    zring_destroy (&ring);

<A name="toc4-4072" title="zsock - high-level socket API that hides libzmq contexts and sockets" />
#### zsock - high-level socket API that hides libzmq contexts and sockets

The zsock class wraps the libzmq socket handle (a void *) with a proper
structure that follows the CLASS rules for construction and destruction.
Some zsock methods take a void * "polymorphic" reference, which can be
either a zsock_t or a zactor_r reference, or a libzmq void *.


This is the class interface:

    
    #if defined (ZSOCK_NOCHECK)
    #   define zsock_new(t) zsock_new_((t), NULL, 0)
    #   define zsock_new_pub(e) zsock_new_pub_((e), NULL, 0)
    #   define zsock_new_sub(e,s) zsock_new_sub_((e), (s), NULL, 0)
    #   define zsock_new_req(e) zsock_new_req_((e), NULL, 0)
    #   define zsock_new_rep(e) zsock_new_rep_((e), NULL, 0)
    #   define zsock_new_dealer(e) zsock_new_dealer_((e), NULL, 0)
    #   define zsock_new_router(e) zsock_new_router_((e), NULL, 0)
    #   define zsock_new_pull(e) zsock_new_pull_((e), NULL, 0)
    #   define zsock_new_push(e) zsock_new_push_((e), NULL, 0)
    #   define zsock_new_xpub(e) zsock_new_xpub_((e), NULL, 0)
    #   define zsock_new_xsub(e) zsock_new_xsub_((e), NULL, 0)
    #   define zsock_new_pair(e) zsock_new_pair_((e), NULL, 0)
    #   define zsock_new_stream(e) zsock_new_stream_((e), NULL, 0)
    #   define zsock_destroy(t) zsock_destroy_((t), NULL, 0)
    #else
    #   define zsock_new(t) zsock_new_((t), __FILE__, __LINE__)
    #   define zsock_new_pub(e) zsock_new_pub_((e), __FILE__, __LINE__)
    #   define zsock_new_sub(e,s) zsock_new_sub_((e), (s), __FILE__, __LINE__)
    #   define zsock_new_req(e) zsock_new_req_((e), __FILE__, __LINE__)
    #   define zsock_new_rep(e) zsock_new_rep_((e), __FILE__, __LINE__)
    #   define zsock_new_dealer(e) zsock_new_dealer_((e), __FILE__, __LINE__)
    #   define zsock_new_router(e) zsock_new_router_((e), __FILE__, __LINE__)
    #   define zsock_new_pull(e) zsock_new_pull_((e), __FILE__, __LINE__)
    #   define zsock_new_push(e) zsock_new_push_((e), __FILE__, __LINE__)
    #   define zsock_new_xpub(e) zsock_new_xpub_((e), __FILE__, __LINE__)
    #   define zsock_new_xsub(e) zsock_new_xsub_((e), __FILE__, __LINE__)
    #   define zsock_new_pair(e) zsock_new_pair_((e), __FILE__, __LINE__)
    #   define zsock_new_stream(e) zsock_new_stream_((e), __FILE__, __LINE__)
    #   define zsock_destroy(t) zsock_destroy_((t), __FILE__, __LINE__)
    #endif
    
    //  Create a new socket. This macro passes the caller source and line
    //  number so that CZMQ can report socket leaks intelligently. To switch
    //  off this checking, which may be costly if you use a LOT of sockets,
    //  define ZSOCK_NOCHECK before compiling your code. Returns the new
    //  socket, or NULL if the new socket could not be created.
    CZMQ_EXPORT zsock_t *
        zsock_new_ (int type, const char *filename, size_t line_nbr);
    
    //  Destroy the socket. You must use this for any socket created via the
    //  zsock_new method.
    CZMQ_EXPORT void
        zsock_destroy_ (zsock_t **self_p, const char *filename, size_t line_nbr);
    
    //  Smart constructors, which create sockets with additional set-up. In all of
    //  these, the endpoint is NULL, or starts with '@' (connect) or '>' (bind).
    //  Multiple endpoints are allowed, separated by commas. If endpoint does not
    //  start with '@' or '>', default action depends on socket type.
    
    //  Create a PUB socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_pub_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create a SUB socket, and optionally subscribe to some prefix string. Default
    //  action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_sub_ (const char *endpoint, const char *subscribe, const char *filename, size_t line_nbr);
    
    //  Create a REQ socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_req_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create a REP socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_rep_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create a DEALER socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_dealer_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create a ROUTER socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_router_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create a PUSH socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_push_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create a PULL socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_pull_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create an XPUB socket. Default action is bind.
    CZMQ_EXPORT zsock_t *
        zsock_new_xpub_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create an XSUB socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_xsub_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create a PAIR socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_pair_ (const char *endpoint, const char *filename, size_t line_nbr);
    
    //  Create a STREAM socket. Default action is connect.
    CZMQ_EXPORT zsock_t *
        zsock_new_stream_ (const char *endpoint, const char *filename, size_t line_nbr);
    
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
        zsock_bind (zsock_t *self, const char *format, ...);
    
    //  Returns last bound endpoint, if any.
    CZMQ_EXPORT char *
        zsock_endpoint (zsock_t *self);
    
    //  Unbind a socket from a formatted endpoint.
    //  Returns 0 if OK, -1 if the endpoint was invalid or the function
    //  isn't supported.
    CZMQ_EXPORT int
        zsock_unbind (zsock_t *self, const char *format, ...);
    
    //  Connect a socket to a formatted endpoint
    //  Returns 0 if OK, -1 if the endpoint was invalid.
    CZMQ_EXPORT int
        zsock_connect (zsock_t *self, const char *format, ...);
    
    //  Disconnect a socket from a formatted endpoint
    //  Returns 0 if OK, -1 if the endpoint was invalid or the function
    //  isn't supported.
    CZMQ_EXPORT int
        zsock_disconnect (zsock_t *self, const char *format, ...);
    
    //  Attach a socket to zero or more endpoints. If endpoints is not null,
    //  parses as list of ZeroMQ endpoints, separated by commas, and prefixed by
    //  '@' (to bind the socket) or '>' (to attach the socket). Returns 0 if all
    //  endpoints were valid, or -1 if there was a syntax error. If the endpoint
    //  does not start with '@' or '>', the serverish argument defines whether
    //  it is used to bind (serverish = true) or connect (serverish = false).
    CZMQ_EXPORT int
        zsock_attach (zsock_t *self, const char *endpoints, bool serverish);
    
    //  Returns socket type as printable constant string. Takes a polymorphic
    //  socket reference.
    CZMQ_EXPORT const char *
        zsock_type_str (void *self);
    
    //  Send a 'picture' message to the socket (or actor). The picture is a
    //  string that defines the type of each frame. This makes it easy to send
    //  a complex multiframe message in one call. The picture can contain any
    //  of these characters, each corresponding to one or two arguments:
    //
    //      i = int
    //      u = uint
    //      s = char *
    //      b = byte *, size_t (2 arguments)
    //      c = zchunk_t *
    //      f = zframe_t *
    //      h = zhash_t *
    //      p = void * (sends the pointer value, only meaningful over inproc)
    //      m = zmsg_t * (sends all frames in the zmsg)
    //      z = sends zero-sized frame (0 arguments)
    //
    //  Note that s, b, c, and f are encoded the same way and the choice is
    //  offered as a convenience to the sender, which may or may not already
    //  have data in a zchunk or zframe. Does not change or take ownership of
    //  any arguments. Returns 0 if successful, -1 if sending failed for any
    //  reason.
    CZMQ_EXPORT int
        zsock_send (void *self, const char *picture, ...);
    
    //  Receive a 'picture' message to the socket (or actor). See zsock_send for
    //  the format and meaning of the picture. Returns the picture elements into
    //  a series of pointers as provided by the caller:
    //
    //      i = int * (stores integer)
    //      u = uint * (stores unsigned integer)
    //      s = char ** (allocates new string)
    //      b = byte **, size_t * (2 arguments) (allocates memory)
    //      c = zchunk_t ** (creates zchunk)
    //      f = zframe_t ** (creates zframe)
    //      p = void ** (stores pointer)
    //      h = zhash_t ** (creates zhash)
    //      m = zmsg_t ** (creates a zmsg with the remaing frames)
    //      z = null, asserts empty frame (0 arguments)
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
    //  values held on a per-socket basis. Do not modify or destroy the returned
    //  values. Returns 0 if successful, or -1 if it failed to read a message.
    CZMQ_EXPORT int
        zsock_brecv (void *self, const char *picture, ...);
        
    //  Set socket to use unbounded pipes (HWM=0); use this in cases when you are
    //  totally certain the message volume can fit in memory. This method works
    //  across all versions of ZeroMQ. Takes a polymorphic socket reference.
    CZMQ_EXPORT void
        zsock_set_unbounded (void *self);
    
    //  Send a signal over a socket. A signal is a short message carrying a
    //  success/failure code (by convention, 0 means OK). Signals are encoded
    //  to be distinguishable from "normal" messages. Accepts a zock_t or a
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
    //  the underlying libzmq socket handle; elsie if it looks like a file
    //  descriptor, return NULL; else if it looks like a libzmq socket handle,
    //  return the supplied value. Takes a polymorphic socket reference.
    CZMQ_EXPORT void *
        zsock_resolve (void *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zsock_test (bool verbose);

This is the class self test code:

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
    
    //  Test resolve FD
    int fd = zsock_fd (reader);
    assert (zsock_resolve ((void *) &fd) == NULL);
    
    //  Test binding to ephemeral ports, sequential and random
    int port = zsock_bind (writer, "tcp://127.0.0.1:*");
    assert (port >= DYNAMIC_FIRST && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[50000-]");
    assert (port >= 50000 && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[-50001]");
    assert (port >= DYNAMIC_FIRST && port <= 50001);
    port = zsock_bind (writer, "tcp://127.0.0.1:*[60000-60010]");
    assert (port >= 60000 && port <= 60010);
    
    port = zsock_bind (writer, "tcp://127.0.0.1:!");
    assert (port >= DYNAMIC_FIRST && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:![50000-]");
    assert (port >= 50000 && port <= DYNAMIC_LAST);
    port = zsock_bind (writer, "tcp://127.0.0.1:![-50001]");
    assert (port >= DYNAMIC_FIRST && port <= 50001);
    port = zsock_bind (writer, "tcp://127.0.0.1:![60000-60010]");
    assert (port >= 60000 && port <= 60010);
    
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
    zchunk_t *chunk = zchunk_new ("HELLO", 5);
    assert (chunk);
    zframe_t *frame = zframe_new ("WORLD", 5);
    assert (frame);
    zhash_t *hash = zhash_new ();
    assert (hash);
    zhash_autofree (hash);
    zhash_insert (hash, "1", "value A");
    zhash_insert (hash, "2", "value B");
    char *original = "pointer";
    
    //  We can send signed integers, strings, blocks of memory, chunks,
    //  frames, hashes and pointers
    zsock_send (writer, "izsbcfp",
                -12345, "This is a string", "ABCDE", 5, chunk, frame, original);
    msg = zmsg_recv (reader);
    assert (msg);
    if (verbose)
        zmsg_print (msg);
    zmsg_destroy (&msg);
    
    //  Test zsock_recv into each supported type
    zsock_send (writer, "izsbcfhp",
                -12345, "This is a string", "ABCDE", 5, chunk, frame, hash, original);
    zframe_destroy (&frame);
    zchunk_destroy (&chunk);
    zhash_destroy (&hash);
    int integer;
    byte *data;
    size_t size;
    char *pointer;
    rc = zsock_recv (reader, "izsbcfhp", &integer, &string, &data, &size, &chunk, &frame, &hash, &pointer);
    assert (rc == 0);
    assert (integer == -12345);
    assert (streq (string, "This is a string"));
    assert (memcmp (data, "ABCDE", 5) == 0);
    assert (size == 5);
    assert (memcmp (zchunk_data (chunk), "HELLO", 5) == 0);
    assert (zchunk_size (chunk) == 5);
    assert (memcmp (zframe_data (frame), "WORLD", 5) == 0);
    assert (zframe_size (frame) == 5);
    char *value = (char *) zhash_lookup (hash, "1");
    assert (streq (value, "value A"));
    value = (char *) zhash_lookup (hash, "2");
    assert (streq (value, "value B"));
    assert (original == pointer);
    free (string);
    free (data);
    zframe_destroy (&frame);
    zchunk_destroy (&chunk);
    zhash_destroy (&hash);
    
    //  Test zsock_recv of short message; this lets us return a failure
    //  with a status code and then nothing else; the receiver will get
    //  the status code and NULL/zero for all other values
    zsock_send (writer, "i", -1);
    zsock_recv (reader, "izsbcfp", &integer, &string, &data, &size, &chunk, &frame, &pointer);
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
    uint8_t  number1 = 123;
    uint16_t number2 = 123 * 123;
    uint32_t number4 = 123 * 123 * 123;
    uint64_t number8 = 123 * 123 * 123 * 123;
    
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
    
    number1 = number2 = number4 = number8 = 0;
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
    
    //  Check that we can send a zproto format message
    zsock_bsend (writer, "1111sS4", 0xAA, 0xA0, 0x02, 0x01, "key", "value", 1234);
    zgossip_msg_t *gossip = zgossip_msg_new ();
    zgossip_msg_recv (gossip, reader);
    assert (zgossip_msg_id (gossip) == ZGOSSIP_MSG_PUBLISH);
    zgossip_msg_destroy (&gossip);
    
    zsock_destroy (&reader);
    zsock_destroy (&writer);
    

<A name="toc4-4606" title="zsock_option - get/set ØMQ socket options" />
#### zsock_option - get/set ØMQ socket options

The zsock_option class provides access to the ØMQ getsockopt/setsockopt API.
All methods in this class take a void * "polymorphic" reference, which
can be either a zsock_t or a zactor_r reference, or a libzmq void *.

This class is generated, using the GSL code generator. See the sockopts
XML file, which provides the metadata, and the sock_option.gsl template,
which does the work.

This is the class interface:

    #if (ZMQ_VERSION_MAJOR == 4)
    //  Get socket options
    CZMQ_EXPORT int zsock_tos (void *self);
    CZMQ_EXPORT char * zsock_zap_domain (void *self);
    CZMQ_EXPORT int zsock_mechanism (void *self);
    CZMQ_EXPORT int zsock_plain_server (void *self);
    CZMQ_EXPORT char * zsock_plain_username (void *self);
    CZMQ_EXPORT char * zsock_plain_password (void *self);
    CZMQ_EXPORT int zsock_curve_server (void *self);
    CZMQ_EXPORT char * zsock_curve_publickey (void *self);
    CZMQ_EXPORT char * zsock_curve_secretkey (void *self);
    CZMQ_EXPORT char * zsock_curve_serverkey (void *self);
    CZMQ_EXPORT int zsock_gssapi_server (void *self);
    CZMQ_EXPORT int zsock_gssapi_plaintext (void *self);
    CZMQ_EXPORT char * zsock_gssapi_principal (void *self);
    CZMQ_EXPORT char * zsock_gssapi_service_principal (void *self);
    CZMQ_EXPORT int zsock_ipv6 (void *self);
    CZMQ_EXPORT int zsock_immediate (void *self);
    CZMQ_EXPORT int zsock_ipv4only (void *self);
    CZMQ_EXPORT int zsock_type (void *self);
    CZMQ_EXPORT int zsock_sndhwm (void *self);
    CZMQ_EXPORT int zsock_rcvhwm (void *self);
    CZMQ_EXPORT int zsock_affinity (void *self);
    CZMQ_EXPORT char * zsock_identity (void *self);
    CZMQ_EXPORT int zsock_rate (void *self);
    CZMQ_EXPORT int zsock_recovery_ivl (void *self);
    CZMQ_EXPORT int zsock_sndbuf (void *self);
    CZMQ_EXPORT int zsock_rcvbuf (void *self);
    CZMQ_EXPORT int zsock_linger (void *self);
    CZMQ_EXPORT int zsock_reconnect_ivl (void *self);
    CZMQ_EXPORT int zsock_reconnect_ivl_max (void *self);
    CZMQ_EXPORT int zsock_backlog (void *self);
    CZMQ_EXPORT int zsock_maxmsgsize (void *self);
    CZMQ_EXPORT int zsock_multicast_hops (void *self);
    CZMQ_EXPORT int zsock_rcvtimeo (void *self);
    CZMQ_EXPORT int zsock_sndtimeo (void *self);
    CZMQ_EXPORT int zsock_tcp_keepalive (void *self);
    CZMQ_EXPORT int zsock_tcp_keepalive_idle (void *self);
    CZMQ_EXPORT int zsock_tcp_keepalive_cnt (void *self);
    CZMQ_EXPORT int zsock_tcp_keepalive_intvl (void *self);
    CZMQ_EXPORT char * zsock_tcp_accept_filter (void *self);
    CZMQ_EXPORT int zsock_rcvmore (void *self);
    CZMQ_EXPORT int zsock_fd (void *self);
    CZMQ_EXPORT int zsock_events (void *self);
    CZMQ_EXPORT char * zsock_last_endpoint (void *self);
    
    //  Set socket options
    CZMQ_EXPORT void zsock_set_tos (void *self, int tos);
    CZMQ_EXPORT void zsock_set_router_handover (void *self, int router_handover);
    CZMQ_EXPORT void zsock_set_router_mandatory (void *self, int router_mandatory);
    CZMQ_EXPORT void zsock_set_probe_router (void *self, int probe_router);
    CZMQ_EXPORT void zsock_set_req_relaxed (void *self, int req_relaxed);
    CZMQ_EXPORT void zsock_set_req_correlate (void *self, int req_correlate);
    CZMQ_EXPORT void zsock_set_conflate (void *self, int conflate);
    CZMQ_EXPORT void zsock_set_zap_domain (void *self, const char * zap_domain);
    CZMQ_EXPORT void zsock_set_plain_server (void *self, int plain_server);
    CZMQ_EXPORT void zsock_set_plain_username (void *self, const char * plain_username);
    CZMQ_EXPORT void zsock_set_plain_password (void *self, const char * plain_password);
    CZMQ_EXPORT void zsock_set_curve_server (void *self, int curve_server);
    CZMQ_EXPORT void zsock_set_curve_publickey (void *self, const char * curve_publickey);
    CZMQ_EXPORT void zsock_set_curve_publickey_bin (void *self, const byte *curve_publickey);
    CZMQ_EXPORT void zsock_set_curve_secretkey (void *self, const char * curve_secretkey);
    CZMQ_EXPORT void zsock_set_curve_secretkey_bin (void *self, const byte *curve_secretkey);
    CZMQ_EXPORT void zsock_set_curve_serverkey (void *self, const char * curve_serverkey);
    CZMQ_EXPORT void zsock_set_curve_serverkey_bin (void *self, const byte *curve_serverkey);
    CZMQ_EXPORT void zsock_set_gssapi_server (void *self, int gssapi_server);
    CZMQ_EXPORT void zsock_set_gssapi_plaintext (void *self, int gssapi_plaintext);
    CZMQ_EXPORT void zsock_set_gssapi_principal (void *self, const char * gssapi_principal);
    CZMQ_EXPORT void zsock_set_gssapi_service_principal (void *self, const char * gssapi_service_principal);
    CZMQ_EXPORT void zsock_set_ipv6 (void *self, int ipv6);
    CZMQ_EXPORT void zsock_set_immediate (void *self, int immediate);
    CZMQ_EXPORT void zsock_set_router_raw (void *self, int router_raw);
    CZMQ_EXPORT void zsock_set_ipv4only (void *self, int ipv4only);
    CZMQ_EXPORT void zsock_set_delay_attach_on_connect (void *self, int delay_attach_on_connect);
    CZMQ_EXPORT void zsock_set_sndhwm (void *self, int sndhwm);
    CZMQ_EXPORT void zsock_set_rcvhwm (void *self, int rcvhwm);
    CZMQ_EXPORT void zsock_set_affinity (void *self, int affinity);
    CZMQ_EXPORT void zsock_set_subscribe (void *self, const char * subscribe);
    CZMQ_EXPORT void zsock_set_unsubscribe (void *self, const char * unsubscribe);
    CZMQ_EXPORT void zsock_set_identity (void *self, const char * identity);
    CZMQ_EXPORT void zsock_set_rate (void *self, int rate);
    CZMQ_EXPORT void zsock_set_recovery_ivl (void *self, int recovery_ivl);
    CZMQ_EXPORT void zsock_set_sndbuf (void *self, int sndbuf);
    CZMQ_EXPORT void zsock_set_rcvbuf (void *self, int rcvbuf);
    CZMQ_EXPORT void zsock_set_linger (void *self, int linger);
    CZMQ_EXPORT void zsock_set_reconnect_ivl (void *self, int reconnect_ivl);
    CZMQ_EXPORT void zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max);
    CZMQ_EXPORT void zsock_set_backlog (void *self, int backlog);
    CZMQ_EXPORT void zsock_set_maxmsgsize (void *self, int maxmsgsize);
    CZMQ_EXPORT void zsock_set_multicast_hops (void *self, int multicast_hops);
    CZMQ_EXPORT void zsock_set_rcvtimeo (void *self, int rcvtimeo);
    CZMQ_EXPORT void zsock_set_sndtimeo (void *self, int sndtimeo);
    CZMQ_EXPORT void zsock_set_xpub_verbose (void *self, int xpub_verbose);
    CZMQ_EXPORT void zsock_set_tcp_keepalive (void *self, int tcp_keepalive);
    CZMQ_EXPORT void zsock_set_tcp_keepalive_idle (void *self, int tcp_keepalive_idle);
    CZMQ_EXPORT void zsock_set_tcp_keepalive_cnt (void *self, int tcp_keepalive_cnt);
    CZMQ_EXPORT void zsock_set_tcp_keepalive_intvl (void *self, int tcp_keepalive_intvl);
    CZMQ_EXPORT void zsock_set_tcp_accept_filter (void *self, const char * tcp_accept_filter);
    #endif
    
    #if (ZMQ_VERSION_MAJOR == 3)
    //  Get socket options
    CZMQ_EXPORT int zsock_ipv4only (void *self);
    CZMQ_EXPORT int zsock_type (void *self);
    CZMQ_EXPORT int zsock_sndhwm (void *self);
    CZMQ_EXPORT int zsock_rcvhwm (void *self);
    CZMQ_EXPORT int zsock_affinity (void *self);
    CZMQ_EXPORT char * zsock_identity (void *self);
    CZMQ_EXPORT int zsock_rate (void *self);
    CZMQ_EXPORT int zsock_recovery_ivl (void *self);
    CZMQ_EXPORT int zsock_sndbuf (void *self);
    CZMQ_EXPORT int zsock_rcvbuf (void *self);
    CZMQ_EXPORT int zsock_linger (void *self);
    CZMQ_EXPORT int zsock_reconnect_ivl (void *self);
    CZMQ_EXPORT int zsock_reconnect_ivl_max (void *self);
    CZMQ_EXPORT int zsock_backlog (void *self);
    CZMQ_EXPORT int zsock_maxmsgsize (void *self);
    CZMQ_EXPORT int zsock_multicast_hops (void *self);
    CZMQ_EXPORT int zsock_rcvtimeo (void *self);
    CZMQ_EXPORT int zsock_sndtimeo (void *self);
    CZMQ_EXPORT int zsock_tcp_keepalive (void *self);
    CZMQ_EXPORT int zsock_tcp_keepalive_idle (void *self);
    CZMQ_EXPORT int zsock_tcp_keepalive_cnt (void *self);
    CZMQ_EXPORT int zsock_tcp_keepalive_intvl (void *self);
    CZMQ_EXPORT char * zsock_tcp_accept_filter (void *self);
    CZMQ_EXPORT int zsock_rcvmore (void *self);
    CZMQ_EXPORT int zsock_fd (void *self);
    CZMQ_EXPORT int zsock_events (void *self);
    CZMQ_EXPORT char * zsock_last_endpoint (void *self);
    
    //  Set socket options
    CZMQ_EXPORT void zsock_set_router_raw (void *self, int router_raw);
    CZMQ_EXPORT void zsock_set_ipv4only (void *self, int ipv4only);
    CZMQ_EXPORT void zsock_set_delay_attach_on_connect (void *self, int delay_attach_on_connect);
    CZMQ_EXPORT void zsock_set_sndhwm (void *self, int sndhwm);
    CZMQ_EXPORT void zsock_set_rcvhwm (void *self, int rcvhwm);
    CZMQ_EXPORT void zsock_set_affinity (void *self, int affinity);
    CZMQ_EXPORT void zsock_set_subscribe (void *self, const char * subscribe);
    CZMQ_EXPORT void zsock_set_unsubscribe (void *self, const char * unsubscribe);
    CZMQ_EXPORT void zsock_set_identity (void *self, const char * identity);
    CZMQ_EXPORT void zsock_set_rate (void *self, int rate);
    CZMQ_EXPORT void zsock_set_recovery_ivl (void *self, int recovery_ivl);
    CZMQ_EXPORT void zsock_set_sndbuf (void *self, int sndbuf);
    CZMQ_EXPORT void zsock_set_rcvbuf (void *self, int rcvbuf);
    CZMQ_EXPORT void zsock_set_linger (void *self, int linger);
    CZMQ_EXPORT void zsock_set_reconnect_ivl (void *self, int reconnect_ivl);
    CZMQ_EXPORT void zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max);
    CZMQ_EXPORT void zsock_set_backlog (void *self, int backlog);
    CZMQ_EXPORT void zsock_set_maxmsgsize (void *self, int maxmsgsize);
    CZMQ_EXPORT void zsock_set_multicast_hops (void *self, int multicast_hops);
    CZMQ_EXPORT void zsock_set_rcvtimeo (void *self, int rcvtimeo);
    CZMQ_EXPORT void zsock_set_sndtimeo (void *self, int sndtimeo);
    CZMQ_EXPORT void zsock_set_xpub_verbose (void *self, int xpub_verbose);
    CZMQ_EXPORT void zsock_set_tcp_keepalive (void *self, int tcp_keepalive);
    CZMQ_EXPORT void zsock_set_tcp_keepalive_idle (void *self, int tcp_keepalive_idle);
    CZMQ_EXPORT void zsock_set_tcp_keepalive_cnt (void *self, int tcp_keepalive_cnt);
    CZMQ_EXPORT void zsock_set_tcp_keepalive_intvl (void *self, int tcp_keepalive_intvl);
    CZMQ_EXPORT void zsock_set_tcp_accept_filter (void *self, const char * tcp_accept_filter);
    #endif
    
    #if (ZMQ_VERSION_MAJOR == 2)
    //  Get socket options
    CZMQ_EXPORT int zsock_hwm (void *self);
    CZMQ_EXPORT int zsock_swap (void *self);
    CZMQ_EXPORT int zsock_affinity (void *self);
    CZMQ_EXPORT char * zsock_identity (void *self);
    CZMQ_EXPORT int zsock_rate (void *self);
    CZMQ_EXPORT int zsock_recovery_ivl (void *self);
    CZMQ_EXPORT int zsock_recovery_ivl_msec (void *self);
    CZMQ_EXPORT int zsock_mcast_loop (void *self);
    #   if (ZMQ_VERSION_MINOR == 2)
    CZMQ_EXPORT int zsock_rcvtimeo (void *self);
    #   endif
    #   if (ZMQ_VERSION_MINOR == 2)
    CZMQ_EXPORT int zsock_sndtimeo (void *self);
    #   endif
    CZMQ_EXPORT int zsock_sndbuf (void *self);
    CZMQ_EXPORT int zsock_rcvbuf (void *self);
    CZMQ_EXPORT int zsock_linger (void *self);
    CZMQ_EXPORT int zsock_reconnect_ivl (void *self);
    CZMQ_EXPORT int zsock_reconnect_ivl_max (void *self);
    CZMQ_EXPORT int zsock_backlog (void *self);
    CZMQ_EXPORT int zsock_type (void *self);
    CZMQ_EXPORT int zsock_rcvmore (void *self);
    CZMQ_EXPORT int zsock_fd (void *self);
    CZMQ_EXPORT int zsock_events (void *self);
    
    //  Set socket options
    CZMQ_EXPORT void zsock_set_hwm (void *self, int hwm);
    CZMQ_EXPORT void zsock_set_swap (void *self, int swap);
    CZMQ_EXPORT void zsock_set_affinity (void *self, int affinity);
    CZMQ_EXPORT void zsock_set_identity (void *self, const char * identity);
    CZMQ_EXPORT void zsock_set_rate (void *self, int rate);
    CZMQ_EXPORT void zsock_set_recovery_ivl (void *self, int recovery_ivl);
    CZMQ_EXPORT void zsock_set_recovery_ivl_msec (void *self, int recovery_ivl_msec);
    CZMQ_EXPORT void zsock_set_mcast_loop (void *self, int mcast_loop);
    #   if (ZMQ_VERSION_MINOR == 2)
    CZMQ_EXPORT void zsock_set_rcvtimeo (void *self, int rcvtimeo);
    #   endif
    #   if (ZMQ_VERSION_MINOR == 2)
    CZMQ_EXPORT void zsock_set_sndtimeo (void *self, int sndtimeo);
    #   endif
    CZMQ_EXPORT void zsock_set_sndbuf (void *self, int sndbuf);
    CZMQ_EXPORT void zsock_set_rcvbuf (void *self, int rcvbuf);
    CZMQ_EXPORT void zsock_set_linger (void *self, int linger);
    CZMQ_EXPORT void zsock_set_reconnect_ivl (void *self, int reconnect_ivl);
    CZMQ_EXPORT void zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max);
    CZMQ_EXPORT void zsock_set_backlog (void *self, int backlog);
    CZMQ_EXPORT void zsock_set_subscribe (void *self, const char * subscribe);
    CZMQ_EXPORT void zsock_set_unsubscribe (void *self, const char * unsubscribe);
    #endif
    
    //  Self test of this class
    CZMQ_EXPORT void zsock_option_test (bool verbose);

This is the class self test code:

    zsock_t *self;
    #if (ZMQ_VERSION_MAJOR == 4)
    #     if defined (ZMQ_TOS)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_tos (self, 1);
    assert (zsock_tos (self) == 1);
    zsock_tos (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_ROUTER_HANDOVER)
    self = zsock_new (ZMQ_ROUTER);
    assert (self);
    zsock_set_router_handover (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_ROUTER_MANDATORY)
    self = zsock_new (ZMQ_ROUTER);
    assert (self);
    zsock_set_router_mandatory (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_PROBE_ROUTER)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_probe_router (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_REQ_RELAXED)
    self = zsock_new (ZMQ_REQ);
    assert (self);
    zsock_set_req_relaxed (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_REQ_CORRELATE)
    self = zsock_new (ZMQ_REQ);
    assert (self);
    zsock_set_req_correlate (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_CONFLATE)
    self = zsock_new (ZMQ_PUSH);
    assert (self);
    zsock_set_conflate (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_ZAP_DOMAIN)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_zap_domain (self, "test");
    char *zap_domain = zsock_zap_domain (self);
    assert (zap_domain);
    free (zap_domain);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_MECHANISM)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_mechanism (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_PLAIN_SERVER)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_plain_server (self, 1);
    assert (zsock_plain_server (self) == 1);
    zsock_plain_server (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_PLAIN_USERNAME)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_plain_username (self, "test");
    char *plain_username = zsock_plain_username (self);
    assert (plain_username);
    free (plain_username);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_PLAIN_PASSWORD)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_plain_password (self, "test");
    char *plain_password = zsock_plain_password (self);
    assert (plain_password);
    free (plain_password);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_IPV6)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_ipv6 (self, 1);
    assert (zsock_ipv6 (self) == 1);
    zsock_ipv6 (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_IMMEDIATE)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_immediate (self, 1);
    assert (zsock_immediate (self) == 1);
    zsock_immediate (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_ROUTER_RAW)
    self = zsock_new (ZMQ_ROUTER);
    assert (self);
    zsock_set_router_raw (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_IPV4ONLY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_ipv4only (self, 1);
    assert (zsock_ipv4only (self) == 1);
    zsock_ipv4only (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_delay_attach_on_connect (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TYPE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_type (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SNDHWM)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_sndhwm (self, 1);
    assert (zsock_sndhwm (self) == 1);
    zsock_sndhwm (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVHWM)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvhwm (self, 1);
    assert (zsock_rcvhwm (self) == 1);
    zsock_rcvhwm (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_AFFINITY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_affinity (self, 1);
    assert (zsock_affinity (self) == 1);
    zsock_affinity (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_subscribe (self, "test");
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_UNSUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_unsubscribe (self, "test");
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_IDENTITY)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_identity (self, "test");
    char *identity = zsock_identity (self);
    assert (identity);
    free (identity);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RATE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rate (self, 1);
    assert (zsock_rate (self) == 1);
    zsock_rate (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECOVERY_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_recovery_ivl (self, 1);
    assert (zsock_recovery_ivl (self) == 1);
    zsock_recovery_ivl (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SNDBUF)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_sndbuf (self, 1);
    assert (zsock_sndbuf (self) == 1);
    zsock_sndbuf (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVBUF)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvbuf (self, 1);
    assert (zsock_rcvbuf (self) == 1);
    zsock_rcvbuf (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_LINGER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_linger (self, 1);
    assert (zsock_linger (self) == 1);
    zsock_linger (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl (self, 1);
    assert (zsock_reconnect_ivl (self) == 1);
    zsock_reconnect_ivl (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL_MAX)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl_max (self, 1);
    assert (zsock_reconnect_ivl_max (self) == 1);
    zsock_reconnect_ivl_max (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_BACKLOG)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_backlog (self, 1);
    assert (zsock_backlog (self) == 1);
    zsock_backlog (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_MAXMSGSIZE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_maxmsgsize (self, 1);
    assert (zsock_maxmsgsize (self) == 1);
    zsock_maxmsgsize (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_MULTICAST_HOPS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_multicast_hops (self, 1);
    assert (zsock_multicast_hops (self) == 1);
    zsock_multicast_hops (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvtimeo (self, 1);
    assert (zsock_rcvtimeo (self) == 1);
    zsock_rcvtimeo (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SNDTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_sndtimeo (self, 1);
    assert (zsock_sndtimeo (self) == 1);
    zsock_sndtimeo (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_XPUB_VERBOSE)
    self = zsock_new (ZMQ_XPUB);
    assert (self);
    zsock_set_xpub_verbose (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive (self, 1);
    assert (zsock_tcp_keepalive (self) == 1);
    zsock_tcp_keepalive (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_idle (self, 1);
    assert (zsock_tcp_keepalive_idle (self) == 1);
    zsock_tcp_keepalive_idle (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_CNT)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_cnt (self, 1);
    assert (zsock_tcp_keepalive_cnt (self) == 1);
    zsock_tcp_keepalive_cnt (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_intvl (self, 1);
    assert (zsock_tcp_keepalive_intvl (self) == 1);
    zsock_tcp_keepalive_intvl (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_ACCEPT_FILTER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_accept_filter (self, "127.0.0.1");
    char *tcp_accept_filter = zsock_tcp_accept_filter (self);
    assert (tcp_accept_filter);
    free (tcp_accept_filter);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVMORE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_rcvmore (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_FD)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_fd (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_EVENTS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_events (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_LAST_ENDPOINT)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    char *last_endpoint = zsock_last_endpoint (self);
    assert (last_endpoint);
    free (last_endpoint);
    zsock_destroy (&self);
    #     endif
    #endif
    
    #if (ZMQ_VERSION_MAJOR == 3)
    #     if defined (ZMQ_ROUTER_RAW)
    self = zsock_new (ZMQ_ROUTER);
    assert (self);
    zsock_set_router_raw (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_IPV4ONLY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_ipv4only (self, 1);
    assert (zsock_ipv4only (self) == 1);
    zsock_ipv4only (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_delay_attach_on_connect (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TYPE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_type (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SNDHWM)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_sndhwm (self, 1);
    assert (zsock_sndhwm (self) == 1);
    zsock_sndhwm (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVHWM)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvhwm (self, 1);
    assert (zsock_rcvhwm (self) == 1);
    zsock_rcvhwm (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_AFFINITY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_affinity (self, 1);
    assert (zsock_affinity (self) == 1);
    zsock_affinity (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_subscribe (self, "test");
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_UNSUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_unsubscribe (self, "test");
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_IDENTITY)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_identity (self, "test");
    char *identity = zsock_identity (self);
    assert (identity);
    free (identity);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RATE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rate (self, 1);
    assert (zsock_rate (self) == 1);
    zsock_rate (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECOVERY_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_recovery_ivl (self, 1);
    assert (zsock_recovery_ivl (self) == 1);
    zsock_recovery_ivl (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SNDBUF)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_sndbuf (self, 1);
    assert (zsock_sndbuf (self) == 1);
    zsock_sndbuf (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVBUF)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvbuf (self, 1);
    assert (zsock_rcvbuf (self) == 1);
    zsock_rcvbuf (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_LINGER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_linger (self, 1);
    assert (zsock_linger (self) == 1);
    zsock_linger (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl (self, 1);
    assert (zsock_reconnect_ivl (self) == 1);
    zsock_reconnect_ivl (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL_MAX)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl_max (self, 1);
    assert (zsock_reconnect_ivl_max (self) == 1);
    zsock_reconnect_ivl_max (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_BACKLOG)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_backlog (self, 1);
    assert (zsock_backlog (self) == 1);
    zsock_backlog (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_MAXMSGSIZE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_maxmsgsize (self, 1);
    assert (zsock_maxmsgsize (self) == 1);
    zsock_maxmsgsize (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_MULTICAST_HOPS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_multicast_hops (self, 1);
    assert (zsock_multicast_hops (self) == 1);
    zsock_multicast_hops (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvtimeo (self, 1);
    assert (zsock_rcvtimeo (self) == 1);
    zsock_rcvtimeo (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SNDTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_sndtimeo (self, 1);
    assert (zsock_sndtimeo (self) == 1);
    zsock_sndtimeo (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_XPUB_VERBOSE)
    self = zsock_new (ZMQ_XPUB);
    assert (self);
    zsock_set_xpub_verbose (self, 1);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive (self, 1);
    assert (zsock_tcp_keepalive (self) == 1);
    zsock_tcp_keepalive (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_idle (self, 1);
    assert (zsock_tcp_keepalive_idle (self) == 1);
    zsock_tcp_keepalive_idle (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_CNT)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_cnt (self, 1);
    assert (zsock_tcp_keepalive_cnt (self) == 1);
    zsock_tcp_keepalive_cnt (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_intvl (self, 1);
    assert (zsock_tcp_keepalive_intvl (self) == 1);
    zsock_tcp_keepalive_intvl (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TCP_ACCEPT_FILTER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_accept_filter (self, "127.0.0.1");
    char *tcp_accept_filter = zsock_tcp_accept_filter (self);
    assert (tcp_accept_filter);
    free (tcp_accept_filter);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVMORE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_rcvmore (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_FD)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_fd (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_EVENTS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_events (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_LAST_ENDPOINT)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    char *last_endpoint = zsock_last_endpoint (self);
    assert (last_endpoint);
    free (last_endpoint);
    zsock_destroy (&self);
    #     endif
    #endif
    
    #if (ZMQ_VERSION_MAJOR == 2)
    #     if defined (ZMQ_HWM)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_hwm (self, 1);
    assert (zsock_hwm (self) == 1);
    zsock_hwm (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SWAP)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_swap (self, 1);
    assert (zsock_swap (self) == 1);
    zsock_swap (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_AFFINITY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_affinity (self, 1);
    assert (zsock_affinity (self) == 1);
    zsock_affinity (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_IDENTITY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_identity (self, "test");
    char *identity = zsock_identity (self);
    assert (identity);
    free (identity);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RATE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rate (self, 1);
    assert (zsock_rate (self) == 1);
    zsock_rate (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECOVERY_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_recovery_ivl (self, 1);
    assert (zsock_recovery_ivl (self) == 1);
    zsock_recovery_ivl (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECOVERY_IVL_MSEC)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_recovery_ivl_msec (self, 1);
    assert (zsock_recovery_ivl_msec (self) == 1);
    zsock_recovery_ivl_msec (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_MCAST_LOOP)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_mcast_loop (self, 1);
    assert (zsock_mcast_loop (self) == 1);
    zsock_mcast_loop (self);
    zsock_destroy (&self);
    #     endif
    #   if (ZMQ_VERSION_MINOR == 2)
    #     if defined (ZMQ_RCVTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvtimeo (self, 1);
    assert (zsock_rcvtimeo (self) == 1);
    zsock_rcvtimeo (self);
    zsock_destroy (&self);
    #     endif
    #   endif
    #   if (ZMQ_VERSION_MINOR == 2)
    #     if defined (ZMQ_SNDTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_sndtimeo (self, 1);
    assert (zsock_sndtimeo (self) == 1);
    zsock_sndtimeo (self);
    zsock_destroy (&self);
    #     endif
    #   endif
    #     if defined (ZMQ_SNDBUF)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_sndbuf (self, 1);
    assert (zsock_sndbuf (self) == 1);
    zsock_sndbuf (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVBUF)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvbuf (self, 1);
    assert (zsock_rcvbuf (self) == 1);
    zsock_rcvbuf (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_LINGER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_linger (self, 1);
    assert (zsock_linger (self) == 1);
    zsock_linger (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl (self, 1);
    assert (zsock_reconnect_ivl (self) == 1);
    zsock_reconnect_ivl (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL_MAX)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl_max (self, 1);
    assert (zsock_reconnect_ivl_max (self) == 1);
    zsock_reconnect_ivl_max (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_BACKLOG)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_backlog (self, 1);
    assert (zsock_backlog (self) == 1);
    zsock_backlog (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_SUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_subscribe (self, "test");
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_UNSUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_unsubscribe (self, "test");
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_TYPE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_type (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_RCVMORE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_rcvmore (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_FD)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_fd (self);
    zsock_destroy (&self);
    #     endif
    #     if defined (ZMQ_EVENTS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_events (self);
    zsock_destroy (&self);
    #     endif
    #endif
    

<A name="toc4-5597" title="zstr - sending and receiving strings" />
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

    //  Receive C string from socket. Caller must free returned string using
    //  zstr_free(). Returns NULL if the context is being terminated or the
    //  process was interrupted.
    CZMQ_EXPORT char *
        zstr_recv (void *source);
    
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
        zstr_sendf (void *dest, const char *format, ...);
    
    //  Send a formatted string to a socket, as for zstr_sendf(), with a
    //  MORE flag, so that you can send further strings in the same multi-part
    //  message.
    CZMQ_EXPORT int
        zstr_sendfm (void *dest, const char *format, ...);
    
    //  Send a series of strings (until NULL) as multipart data
    //  Returns 0 if the strings could be sent OK, or -1 on error.
    CZMQ_EXPORT int
        zstr_sendx (void *dest, const char *string, ...);
    
    //  Receive a series of strings (until NULL) from multipart data.
    //  Each string is allocated and filled with string data; if there
    //  are not enough frames, unallocated strings are set to NULL.
    //  Returns -1 if the message could not be read, else returns the
    //  number of strings filled, zero or more. Free each returned string
    //  using zstr_free(). If not enough strings are provided, remaining
    //  multipart frames in the message are dropped.
    CZMQ_EXPORT int
        zstr_recvx (void *source, char **string_p, ...);
    
    //  Free a provided string, and nullify the parent pointer. Safe to call on
    //  a null pointer.
    CZMQ_EXPORT void
        zstr_free (char **string_p);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zstr_test (bool verbose);

This is the class self test code:

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

<A name="toc4-5701" title="zsys - system-level methods" />
#### zsys - system-level methods

The zsys class provides a portable wrapper for system calls. We collect
them here to reduce the number of weird #ifdefs in other classes. As far
as possible, the bulk of CZMQ classes are fully portable.


This is the class interface:

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
    CZMQ_EXPORT mode_t
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
        zsys_udp_send (SOCKET udpsock, zframe_t *frame, inaddr_t *address);
    
    //  Receive zframe from UDP socket, and set address of peer that sent it
    //  The peername must be a char [INET_ADDRSTRLEN] array.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT zframe_t *
        zsys_udp_recv (SOCKET udpsock, char *peername);
    
    //  Handle an I/O error on some socket operation; will report and die on
    //  fatal errors, and continue silently on "try again" errors.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    CZMQ_EXPORT void
        zsys_socket_error (const char *reason);
    
    //  Return current host name, for use in public tcp:// endpoints. Caller gets
    //  a freshly allocated string, should free it using zstr_free().
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

This is the class self test code:

    zsys_catch_interrupts ();
    
    //  Check capabilities without using the return value
    int rc = zsys_has_curve ();
    
    if (verbose) {
        char *hostname = zsys_hostname ();
        zsys_info ("host name is %s", hostname);
        free (hostname);
        zsys_info ("system limit is %zd ZeroMQ sockets", zsys_socket_limit ());
    }
    zsys_set_io_threads (1);
    zsys_set_max_sockets (0);
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
    
    mode_t mode = zsys_file_mode (".");
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
    rc = zsocket_connect (logger, "inproc://logging");
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

<A name="toc4-6129" title="zuuid - UUID support class" />
#### zuuid - UUID support class

The zuuid class generates UUIDs and provides methods for working with
them. If you build CZMQ with libuuid, on Unix/Linux, it will use that
library. On Windows it will use UuidCreate(). Otherwise it will use a
random number generator to produce convincing imitations of uuids.


This is the class interface:

    //  Constructor
    CZMQ_EXPORT zuuid_t *
        zuuid_new (void);
    
    //  Destructor
    CZMQ_EXPORT void
        zuuid_destroy (zuuid_t **self_p);
    
    //  Return UUID binary data
    CZMQ_EXPORT byte *
        zuuid_data (zuuid_t *self);
    
    //  Return UUID binary size
    CZMQ_EXPORT size_t
        zuuid_size (zuuid_t *self);
    
    //  Returns UUID as string
    CZMQ_EXPORT char *
        zuuid_str (zuuid_t *self);
    
    //  Set UUID to new supplied ZUUID_LEN-octet value
    CZMQ_EXPORT void
        zuuid_set (zuuid_t *self, byte *source);
        
    //  Set UUID to new supplied ZUUID_STR_LEN-char string value;
    //  return 0 if OK, else returns -1.
    CZMQ_EXPORT int
        zuuid_set_str (zuuid_t *self, const char *source);
    
    //  Store UUID blob in target array
    CZMQ_EXPORT void
        zuuid_export (zuuid_t *self, byte *target);
    
    //  Check if UUID is same as supplied value
    CZMQ_EXPORT bool
        zuuid_eq (zuuid_t *self, byte *compare);
    
    //  Check if UUID is different from supplied value
    CZMQ_EXPORT bool
        zuuid_neq (zuuid_t *self, byte *compare);
    
    //  Make copy of UUID object; if uuid is null, or memory was exhausted,
    //  returns null.
    CZMQ_EXPORT zuuid_t *
        zuuid_dup (zuuid_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zuuid_test (bool verbose);

This is the class self test code:

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
    zuuid_set_str (uuid, myuuid);
    assert (streq (zuuid_str (uuid), myuuid));
    byte copy_uuid [ZUUID_LEN];
    zuuid_export (uuid, copy_uuid);
    zuuid_set (uuid, copy_uuid);
    assert (streq (zuuid_str (uuid), myuuid));
    
    zuuid_destroy (&uuid);
    zuuid_destroy (&copy);


<A name="toc3-6216" title="API v2 Summary" />
### API v2 Summary

This is the deprecated API provided by CZMQ v2.x, in alphabetical order.

<A name="toc4-6221" title="zauth_v2 - authentication for ZeroMQ servers (deprecated)" />
#### zauth_v2 - authentication for ZeroMQ servers (deprecated)

A zauth object takes over authentication for all incoming connections in
its context.

This class is deprecated in CZMQ v3; it works together with zctx, zsocket,
and other deprecated V2 classes. New applications should use the V3 zauth
interface, based on zactor, together with the zsock class for sockets.

This is the class interface:

    #ifndef CURVE_ALLOW_ANY
    #   define CURVE_ALLOW_ANY "*"
    #endif
    
    //  Constructor
    //  Install authentication for the specified context. Returns a new zauth
    //  object that you can use to configure authentication. Note that until you
    //  add policies, all incoming NULL connections are allowed (classic ZeroMQ
    //  behaviour), and all PLAIN and CURVE connections are denied. If there was
    //  an error during initialization, returns NULL.
    CZMQ_EXPORT zauth_t *
        zauth_new (zctx_t *ctx);
        
    //  Destructor
    CZMQ_EXPORT void
        zauth_destroy (zauth_t **self_p);
    
    //  Allow (whitelist) a single IP address. For NULL, all clients from this
    //  address will be accepted. For PLAIN and CURVE, they will be allowed to
    //  continue with authentication. You can call this method multiple times 
    //  to whitelist multiple IP addresses. If you whitelist a single address,
    //  any non-whitelisted addresses are treated as blacklisted.
    CZMQ_EXPORT void
        zauth_allow (zauth_t *self, const char *address);
    
    //  Deny (blacklist) a single IP address. For all security mechanisms, this
    //  rejects the connection without any further authentication. Use either a
    //  whitelist, or a blacklist, not not both. If you define both a whitelist 
    //  and a blacklist, only the whitelist takes effect.
    CZMQ_EXPORT void
        zauth_deny (zauth_t *self, const char *address);
    
    //  Configure PLAIN authentication for a given domain. PLAIN authentication
    //  uses a plain-text password file. To cover all domains, use "*". You can
    //  modify the password file at any time; it is reloaded automatically.
    CZMQ_EXPORT void
        zauth_configure_plain (zauth_t *self, const char *domain, const char *filename);
        
    //  Configure CURVE authentication for a given domain. CURVE authentication
    //  uses a directory that holds all public client certificates, i.e. their
    //  public keys. The certificates must be in zcert_save () format. To cover
    //  all domains, use "*". You can add and remove certificates in that
    //  directory at any time. To allow all client keys without checking, specify
    //  CURVE_ALLOW_ANY for the location.
    CZMQ_EXPORT void
        zauth_configure_curve (zauth_t *self, const char *domain, const char *location);
        
    //  Configure GSSAPI authentication for a given domain. GSSAPI authentication
    //  uses an underlying mechanism (usually Kerberos) to establish a secure
    //  context and perform mutual authentication. To cover all domains, use "*".
    CZMQ_EXPORT void
        zauth_configure_gssapi (zauth_t *self, char *domain);
    
    //  Enable verbose tracing of commands and activity
    CZMQ_EXPORT void
        zauth_set_verbose (zauth_t *self, bool verbose);
        
    //  Selftest
    CZMQ_EXPORT void
        zauth_v2_test (bool verbose);

This is the class self test code:

    //  Create temporary directory for test files
    #   define TESTDIR ".test_zauth"
    zsys_dir_create (TESTDIR);
    
    //  Install the authenticator
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    zauth_t *auth = zauth_new (ctx);
    assert (auth);
    zauth_set_verbose (auth, verbose);
    
    //  A default NULL connection should always success, and not
    //  go through our authentication infrastructure at all.
    void *server = zsocket_new (ctx, ZMQ_PUSH);
    assert (server);
    void *client = zsocket_new (ctx, ZMQ_PULL);
    assert (client);
    bool success = s_can_connect (ctx, &server, &client);
    assert (success);
    
    //  When we set a domain on the server, we switch on authentication
    //  for NULL sockets, but with no policies, the client connection
    //  will be allowed.
    zsocket_set_zap_domain (server, "global");
    success = s_can_connect (ctx, &server, &client);
    assert (success);
    
    //  Blacklist 127.0.0.1, connection should fail
    zsocket_set_zap_domain (server, "global");
    zauth_deny (auth, "127.0.0.1");
    success = s_can_connect (ctx, &server, &client);
    assert (!success);
    
    //  Whitelist our address, which overrides the blacklist
    zsocket_set_zap_domain (server, "global");
    zauth_allow (auth, "127.0.0.1");
    success = s_can_connect (ctx, &server, &client);
    assert (success);
    
    //  Try PLAIN authentication
    zsocket_set_plain_server (server, 1);
    zsocket_set_plain_username (client, "admin");
    zsocket_set_plain_password (client, "Password");
    success = s_can_connect (ctx, &server, &client);
    assert (!success);
    
    FILE *password = fopen (TESTDIR "/password-file", "w");
    assert (password);
    fprintf (password, "admin=Password\n");
    fclose (password);
    zsocket_set_plain_server (server, 1);
    zsocket_set_plain_username (client, "admin");
    zsocket_set_plain_password (client, "Password");
    zauth_configure_plain (auth, "*", TESTDIR "/password-file");
    success = s_can_connect (ctx, &server, &client);
    assert (success);
    
    zsocket_set_plain_server (server, 1);
    zsocket_set_plain_username (client, "admin");
    zsocket_set_plain_password (client, "Bogus");
    success = s_can_connect (ctx, &server, &client);
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
        char *server_key = zcert_public_txt (server_cert);
    
        //  Test without setting-up any authentication
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsocket_set_curve_server (server, 1);
        zsocket_set_curve_serverkey (client, server_key);
        success = s_can_connect (ctx, &server, &client);
        assert (!success);
    
        //  Test CURVE_ALLOW_ANY
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsocket_set_curve_server (server, 1);
        zsocket_set_curve_serverkey (client, server_key);
        zauth_configure_curve (auth, "*", CURVE_ALLOW_ANY);
        success = s_can_connect (ctx, &server, &client);
        assert (success);
    
        //  Test full client authentication using certificates
        zcert_apply (server_cert, server);
        zcert_apply (client_cert, client);
        zsocket_set_curve_server (server, 1);
        zsocket_set_curve_serverkey (client, server_key);
        zcert_save_public (client_cert, TESTDIR "/mycert.txt");
        zauth_configure_curve (auth, "*", TESTDIR);
        success = s_can_connect (ctx, &server, &client);
        assert (success);
    
        zcert_destroy (&server_cert);
        zcert_destroy (&client_cert);
    }
    //  Remove the authenticator and check a normal connection works
    zauth_destroy (&auth);
    success = s_can_connect (ctx, &server, &client);
    assert (success);
    
    zctx_destroy (&ctx);
    
    //  Delete all test files
    zdir_t *dir = zdir_new (TESTDIR, NULL);
    assert (dir);
    zdir_remove (dir, true);
    zdir_destroy (&dir);

<A name="toc4-6413" title="zctx - working with ØMQ contexts (deprecated)" />
#### zctx - working with ØMQ contexts (deprecated)

The zctx class wraps ØMQ contexts. It manages open sockets in the context
and automatically closes these before terminating the context. It provides
a simple way to set the linger timeout on sockets, and configure contexts
for number of I/O threads. Sets-up signal (interrupt) handling for the
process.

The zctx class has these main features:

* Tracks all open sockets and automatically closes them before calling
zmq_term(). This avoids an infinite wait on open sockets.

* Automatically configures sockets with a ZMQ_LINGER timeout you can
define, and which defaults to zero. The default behavior of zctx is
therefore like ØMQ/2.0, immediate termination with loss of any pending
messages. You can set any linger timeout you like by calling the
zctx_set_linger() method.

* Moves the iothreads configuration to a separate method, so that default
usage is 1 I/O thread. Lets you configure this value.

* Sets up signal (SIGINT and SIGTERM) handling so that blocking calls
such as zmq_recv() and zmq_poll() will return when the user presses
Ctrl-C.

NOTE: this class is deprecated in favor of zsock, which does not expose
contexts in the API at all. All zsock instances use the same global
context.

This is the class interface:

    //  Create new context, returns context object, replaces zmq_init
    CZMQ_EXPORT zctx_t *
        zctx_new (void);
    
    //  Destroy context and all sockets in it, replaces zmq_term
    CZMQ_EXPORT void
        zctx_destroy (zctx_t **self_p);
    
    //  Create new shadow context, returns context object
    CZMQ_EXPORT zctx_t *
        zctx_shadow (zctx_t *self);
    //  Raise default I/O threads from 1, for crazy heavy applications
    //  The rule of thumb is one I/O thread per gigabyte of traffic in
    //  or out. Call this method before creating any sockets on the context,
    //  or calling zctx_shadow, or the setting will have no effect.
    CZMQ_EXPORT void
        zctx_set_iothreads (zctx_t *self, int iothreads);
    
    //  Set msecs to flush sockets when closing them, see the ZMQ_LINGER
    //  man page section for more details. By default, set to zero, so
    //  any in-transit messages are discarded when you destroy a socket or
    //  a context.
    CZMQ_EXPORT void
        zctx_set_linger (zctx_t *self, int linger);
    
    //  Set initial high-water mark for inter-thread pipe sockets. Note that
    //  this setting is separate from the default for normal sockets. You 
    //  should change the default for pipe sockets *with care*. Too low values
    //  will cause blocked threads, and an infinite setting can cause memory
    //  exhaustion. The default, no matter the underlying ZeroMQ version, is
    //  1,000.
    CZMQ_EXPORT void
        zctx_set_pipehwm (zctx_t *self, int pipehwm);
        
    //  Set initial send HWM for all new normal sockets created in context.
    //  You can set this per-socket after the socket is created.
    //  The default, no matter the underlying ZeroMQ version, is 1,000.
    CZMQ_EXPORT void
        zctx_set_sndhwm (zctx_t *self, int sndhwm);
        
    //  Set initial receive HWM for all new normal sockets created in context.
    //  You can set this per-socket after the socket is created.
    //  The default, no matter the underlying ZeroMQ version, is 1,000.
    CZMQ_EXPORT void
        zctx_set_rcvhwm (zctx_t *self, int rcvhwm);
    
    //  Return low-level ØMQ context object, will be NULL before first socket
    //  is created. Use with care.
    CZMQ_EXPORT void *
        zctx_underlying (zctx_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zctx_test (bool verbose);

This is the class self test code:

    //  Create and destroy a context without using it
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    zctx_destroy (&ctx);
    assert (ctx == NULL);
    
    //  Create a context with many busy sockets, destroy it
    ctx = zctx_new ();
    assert (ctx);
    zctx_set_iothreads (ctx, 1);
    zctx_set_linger (ctx, 5);       //  5 msecs
    void *s1 = zctx__socket_new (ctx, ZMQ_PAIR);
    assert (s1);
    void *s2 = zctx__socket_new (ctx, ZMQ_XREQ);
    assert (s2);
    void *s3 = zctx__socket_new (ctx, ZMQ_REQ);
    assert (s3);
    void *s4 = zctx__socket_new (ctx, ZMQ_REP);
    assert (s4);
    void *s5 = zctx__socket_new (ctx, ZMQ_PUB);
    assert (s5);
    void *s6 = zctx__socket_new (ctx, ZMQ_SUB);
    assert (s6);
    int rc = zsocket_connect (s1, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s2, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s3, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s4, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s5, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    rc = zsocket_connect (s6, "tcp://127.0.0.1:5555");
    assert (rc == 0);
    assert (zctx_underlying (ctx));
    zctx_destroy (&ctx);

<A name="toc4-6541" title="zmonitor_v2 - socket event monitor (deprecated)" />
#### zmonitor_v2 - socket event monitor (deprecated)

The zmonitor class provides an API for obtaining socket events such as
connected, listen, disconnected, etc. Socket events are only available
for sockets connecting or bound to ipc:// and tcp:// endpoints.
This class wraps the ZMQ socket monitor API, see zmq_socket_monitor for
details. Currently this class requires libzmq v4.0 or later and is empty
on older versions of libzmq.

This class is deprecated in CZMQ v3; it works together with zctx, zsocket,
and other deprecated V2 classes. New applications should use the V3 zmonitor
interface, based on zactor, together with the zsock class for sockets.

This is the class interface:

    //  This code needs backporting to work with ZMQ v3.2
    #if (ZMQ_VERSION_MAJOR == 4)
    
    //  Create a new socket monitor
    CZMQ_EXPORT zmonitor_t *
        zmonitor_new (zctx_t *ctx, void *socket, int events);
    
    //  Destroy a socket monitor
    CZMQ_EXPORT void
        zmonitor_destroy (zmonitor_t **self_p);
    
    //  Receive a status message from the monitor; if no message arrives within
    //  500 msec, or the call was interrupted, returns NULL.
    CZMQ_EXPORT zmsg_t *
        zmonitor_recv (zmonitor_t *self);
    
    //  Get the ZeroMQ socket, for polling 
    CZMQ_EXPORT void *
        zmonitor_socket (zmonitor_t *self);
    
    //  Enable verbose tracing of commands and activity
    CZMQ_EXPORT void
        zmonitor_set_verbose (zmonitor_t *self, bool verbose);
    #endif          //  ZeroMQ 4.0 or later
    
    // Self test of this class
    CZMQ_EXPORT void
        zmonitor_v2_test (bool verbose);

This is the class self test code:

    zctx_t *ctx = zctx_new ();
    assert (ctx);
    bool result;
    
    void *sink = zsocket_new (ctx, ZMQ_PULL);
    assert (sink);
    zmonitor_t *sinkmon = zmonitor_new (ctx,
                                        sink, ZMQ_EVENT_LISTENING | ZMQ_EVENT_ACCEPTED);
    assert (sinkmon);
    zmonitor_set_verbose (sinkmon, verbose);
    
    //  Check sink is now listening
    int port_nbr = zsocket_bind (sink, "tcp://127.0.0.1:*");
    assert (port_nbr != -1);
    result = s_check_event (sinkmon, ZMQ_EVENT_LISTENING);
    assert (result);
    
    void *source = zsocket_new (ctx, ZMQ_PUSH);
    assert (source);
    zmonitor_t *sourcemon = zmonitor_new (ctx,
                                          source, ZMQ_EVENT_CONNECTED | ZMQ_EVENT_DISCONNECTED);
    assert (sourcemon);
    zmonitor_set_verbose (sourcemon, verbose);
    zsocket_connect (source, "tcp://127.0.0.1:%d", port_nbr);
    
    //  Check source connected to sink
    result = s_check_event (sourcemon, ZMQ_EVENT_CONNECTED);
    assert (result);
    
    //  Check sink accepted connection
    result = s_check_event (sinkmon, ZMQ_EVENT_ACCEPTED);
    assert (result);
    
    zmonitor_destroy (&sinkmon);
    zmonitor_destroy (&sourcemon);
    zctx_destroy (&ctx);

<A name="toc4-6625" title="zmutex - working with mutexes (deprecated)" />
#### zmutex - working with mutexes (deprecated)

The zmutex class provides a portable wrapper for mutexes. Please do not
use this class to do multi-threading. It is for the rare case where you
absolutely need thread-safe global state. This should happen in system
code only. DO NOT USE THIS TO SHARE SOCKETS BETWEEN THREADS, OR DARK
THINGS WILL HAPPEN TO YOUR CODE.


This is the class interface:

    //  Create a new mutex container
    CZMQ_EXPORT zmutex_t *
        zmutex_new (void);
    
    //  Destroy a mutex container
    CZMQ_EXPORT void
        zmutex_destroy (zmutex_t **self_p);
    
    //  Lock mutex
    CZMQ_EXPORT void
        zmutex_lock (zmutex_t *self);
    
    //  Unlock mutex
    CZMQ_EXPORT void
        zmutex_unlock (zmutex_t *self);
    
    //  Try to lock mutex
    CZMQ_EXPORT int
        zmutex_try_lock (zmutex_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zmutex_test (bool verbose);

This is the class self test code:

    zmutex_t *mutex = zmutex_new ();
    assert (mutex);
    zmutex_lock (mutex);
    zmutex_unlock (mutex);
    zmutex_destroy (&mutex);

<A name="toc4-6669" title="zproxy_v2 - run a steerable proxy in the background (deprecated)" />
#### zproxy_v2 - run a steerable proxy in the background (deprecated)

The zproxy class provides an equivalent to the ZMQ steerable proxy, on
all versions of ZeroMQ.

This class is deprecated in CZMQ v3; it works together with zctx, zsocket,
and other deprecated V2 classes. New applications should use the V3 zproxy
interface, based on zactor, together with the zsock class for sockets.

This is the class interface:

    
    //  Constructor
    //  Create a new zproxy object. You must create the frontend and backend
    //  sockets, configure them, and connect or bind them, before you pass them
    //  to the constructor. Do NOT use the sockets again, after passing them to
    //  this method.
    CZMQ_EXPORT zproxy_t *
        zproxy_new (zctx_t *ctx, void *frontend, void *backend);
    
    //  Destructor
    //  Destroy a zproxy object; note this first stops the proxy.
    CZMQ_EXPORT void
        zproxy_destroy (zproxy_t **self_p);
    
    //  Copy all proxied messages to specified endpoint; if this is NULL, any
    //  in-progress capturing will be stopped. You must already have bound the
    //  endpoint to a PULL socket.
    CZMQ_EXPORT void
        zproxy_capture (zproxy_t *self, const char *endpoint);
    
    //  Pauses a zproxy object; a paused proxy will cease processing messages,
    //  causing them to be queued up and potentially hit the high-water mark on
    //  the frontend socket, causing messages to be dropped, or writing
    //  applications to block.
    CZMQ_EXPORT void
        zproxy_pause (zproxy_t *self);
    
    //  Resume a zproxy object
    CZMQ_EXPORT void
        zproxy_resume (zproxy_t *self);
    
    // Self test of this class
    CZMQ_EXPORT void
        zproxy_v2_test (bool verbose);

This is the class self test code:

    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *frontend = zsocket_new (ctx, ZMQ_PULL);
    assert (frontend);
    int rc = zsocket_bind (frontend, "inproc://frontend");
    assert (rc == 0);
    void *backend = zsocket_new (ctx, ZMQ_PUSH);
    assert (backend);
    rc = zsocket_bind (backend, "inproc://backend");
    assert (rc == 0);
    zproxy_t *proxy = zproxy_new (ctx, frontend, backend);
    assert (proxy);
    
    //  Connect application sockets to proxy
    void *faucet = zsocket_new (ctx, ZMQ_PUSH);
    assert (faucet);
    rc = zsocket_connect (faucet, "inproc://frontend");
    assert (rc == 0);
    void *sink = zsocket_new (ctx, ZMQ_PULL);
    assert (sink);
    rc = zsocket_connect (sink, "inproc://backend");
    assert (rc == 0);
    
    //  Send some messages and check they arrived
    zstr_send (faucet, "Hello");
    char *string = zstr_recv (sink);
    assert (streq (string, "Hello"));
    zstr_free (&string);
    
    //  Check pause/resume functionality
    zproxy_pause (proxy);
    zstr_send (faucet, "World");
    
    zproxy_resume (proxy);
    string = zstr_recv (sink);
    assert (streq (string, "World"));
    zstr_free (&string);
    
    //  Create capture socket, must be a PULL socket
    void *capture = zsocket_new (ctx, ZMQ_PULL);
    assert (capture);
    rc = zsocket_bind (capture, "inproc://capture");
    assert (rc == 0);
    
    //  Switch on capturing, check that it works
    zproxy_capture (proxy, "inproc://capture");
    zstr_send (faucet, "Hello");
    
    string = zstr_recv (sink);
    assert (streq (string, "Hello"));
    zstr_free (&string);
    
    string = zstr_recv (capture);
    assert (streq (string, "Hello"));
    zstr_free (&string);
    zproxy_destroy (&proxy);
    zctx_destroy (&ctx);
    

<A name="toc4-6777" title="zsocket - working with ØMQ sockets (deprecated)" />
#### zsocket - working with ØMQ sockets (deprecated)

The zsocket class provides helper functions for ØMQ sockets. It doesn't
wrap the ØMQ socket type, to avoid breaking all libzmq socket-related
calls.


This is the class interface:

    //  This port range is defined by IANA for dynamic or private ports
    //  We use this when choosing a port for dynamic binding.
    #define ZSOCKET_DYNFROM     0xc000
    #define ZSOCKET_DYNTO       0xffff
    
    //  Callback function for zero-copy methods
    typedef void (zsocket_free_fn) (void *data, void *arg);
    
    //  Create a new socket within our CZMQ context, replaces zmq_socket.
    //  Use this to get automatic management of the socket at shutdown.
    //  Note: SUB sockets do not automatically subscribe to everything; you
    //  must set filters explicitly.
    CZMQ_EXPORT void *
        zsocket_new (zctx_t *self, int type);
    
    //  Destroy a socket within our CZMQ context, replaces zmq_close.
    CZMQ_EXPORT void
        zsocket_destroy (zctx_t *ctx, void *self);
    
    //  Bind a socket to a formatted endpoint. If the port is specified as
    //  '*', binds to any free port from ZSOCKET_DYNFROM to ZSOCKET_DYNTO
    //  and returns the actual port number used. Otherwise asserts that the
    //  bind succeeded with the specified port number. Always returns the
    //  port number if successful.
    CZMQ_EXPORT int
        zsocket_bind (void *self, const char *format, ...);
    
    //  Unbind a socket from a formatted endpoint.
    //  Returns 0 if OK, -1 if the endpoint was invalid or the function
    //  isn't supported.
    CZMQ_EXPORT int
        zsocket_unbind (void *self, const char *format, ...);
    
    //  Connect a socket to a formatted endpoint
    //  Returns 0 if OK, -1 if the endpoint was invalid.
    CZMQ_EXPORT int
        zsocket_connect (void *self, const char *format, ...);
    
    //  Disconnect a socket from a formatted endpoint
    //  Returns 0 if OK, -1 if the endpoint was invalid or the function
    //  isn't supported.
    CZMQ_EXPORT int
        zsocket_disconnect (void *self, const char *format, ...);
    
    //  Poll for input events on the socket. Returns TRUE if there is input
    //  ready on the socket, else FALSE.
    CZMQ_EXPORT bool
        zsocket_poll (void *self, int msecs);
    
    //  Returns socket type as printable constant string
    CZMQ_EXPORT const char *
        zsocket_type_str (void *self);
    
    //  Send data over a socket as a single message frame.
    //  Accepts these flags: ZFRAME_MORE and ZFRAME_DONTWAIT.
    //  Returns -1 on error, 0 on success
    CZMQ_EXPORT int
        zsocket_sendmem (void *self, const void *data, size_t size, int flags);
    
    //  Send a signal over a socket. A signal is a zero-byte message.
    //  Signals are used primarily between threads, over pipe sockets.
    //  Returns -1 if there was an error sending the signal.
    CZMQ_EXPORT int
        zsocket_signal (void *self);
    
    //  Wait on a signal. Use this to coordinate between threads, over
    //  pipe pairs. Returns -1 on error, 0 on success.
    CZMQ_EXPORT int
        zsocket_wait (void *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zsocket_test (bool verbose);

This is the class self test code:

    zctx_t *ctx = zctx_new ();
    assert (ctx);
    
    //  Create a detached thread, let it run
    char *interf = "127.0.0.1";
    char *domain = "localhost";
    int service = 5560;
    
    void *writer = zsocket_new (ctx, ZMQ_PUSH);
    assert (writer);
    void *reader = zsocket_new (ctx, ZMQ_PULL);
    assert (reader);
    assert (streq (zsocket_type_str (writer), "PUSH"));
    assert (streq (zsocket_type_str (reader), "PULL"));
    int rc = zsocket_bind (writer, "tcp://%s:%d", interf, service);
    assert (rc == service);
    
    #if (ZMQ_VERSION >= ZMQ_MAKE_VERSION (3, 2, 0))
    //  Check unbind
    rc = zsocket_unbind (writer, "tcp://%s:%d", interf, service);
    assert (rc == 0);
    
    //  In some cases and especially when running under Valgrind, doing
    //  a bind immediately after an unbind causes an EADDRINUSE error.
    //  Even a short sleep allows the OS to release the port for reuse.
    zclock_sleep (100);
    
    //  Bind again
    rc = zsocket_bind (writer, "tcp://%s:%d", interf, service);
    assert (rc == service);
    #endif
    
    rc = zsocket_connect (reader, "tcp://%s:%d", domain, service);
    assert (rc == 0);
    zstr_send (writer, "HELLO");
    char *message = zstr_recv (reader);
    assert (message);
    assert (streq (message, "HELLO"));
    free (message);
    
    //  Test binding to ports
    int port = zsocket_bind (writer, "tcp://%s:*", interf);
    assert (port >= ZSOCKET_DYNFROM && port <= ZSOCKET_DYNTO);
    
    assert (zsocket_poll (writer, 100) == false);
    
    //  Test error state when connecting to an invalid socket type
    //  ('txp://' instead of 'tcp://', typo intentional)
    rc = zsocket_connect (reader, "txp://%s:%d", domain, service);
    assert (rc == -1);
    
    //  Test sending frames to socket
    rc = zsocket_sendmem (writer, "ABC", 3, ZFRAME_MORE);
    assert (rc == 0);
    rc = zsocket_sendmem (writer, "DEFG", 4, 0);
    assert (rc == 0);
    
    zframe_t *frame = zframe_recv (reader);
    assert (frame);
    assert (zframe_streq (frame, "ABC"));
    assert (zframe_more (frame));
    zframe_destroy (&frame);
    
    frame = zframe_recv (reader);
    assert (frame);
    assert (zframe_streq (frame, "DEFG"));
    assert (!zframe_more (frame));
    zframe_destroy (&frame);
    
    rc = zsocket_signal (writer);
    assert (rc == 0);
    rc = zsocket_wait (reader);
    assert (rc == 0);
    
    zsocket_destroy (ctx, reader);
    zsocket_destroy (ctx, writer);
    zctx_destroy (&ctx);

<A name="toc4-6941" title="zsockopt - get/set ØMQ socket options (deprecated)" />
#### zsockopt - get/set ØMQ socket options (deprecated)

The zsockopt class provides access to the ØMQ getsockopt/setsockopt API.

This class is generated, using the GSL code generator. See the sockopts
XML file, which provides the metadata, and the zsockopt.gsl template,
which does the work.

This is the class interface:

    #if (ZMQ_VERSION_MAJOR == 4)
    //  Get socket options
    CZMQ_EXPORT int zsocket_tos (void *zocket);
    CZMQ_EXPORT char * zsocket_zap_domain (void *zocket);
    CZMQ_EXPORT int zsocket_mechanism (void *zocket);
    CZMQ_EXPORT int zsocket_plain_server (void *zocket);
    CZMQ_EXPORT char * zsocket_plain_username (void *zocket);
    CZMQ_EXPORT char * zsocket_plain_password (void *zocket);
    CZMQ_EXPORT int zsocket_curve_server (void *zocket);
    CZMQ_EXPORT char * zsocket_curve_publickey (void *zocket);
    CZMQ_EXPORT char * zsocket_curve_secretkey (void *zocket);
    CZMQ_EXPORT char * zsocket_curve_serverkey (void *zocket);
    CZMQ_EXPORT int zsocket_gssapi_server (void *zocket);
    CZMQ_EXPORT int zsocket_gssapi_plaintext (void *zocket);
    CZMQ_EXPORT char * zsocket_gssapi_principal (void *zocket);
    CZMQ_EXPORT char * zsocket_gssapi_service_principal (void *zocket);
    CZMQ_EXPORT int zsocket_ipv6 (void *zocket);
    CZMQ_EXPORT int zsocket_immediate (void *zocket);
    CZMQ_EXPORT int zsocket_ipv4only (void *zocket);
    CZMQ_EXPORT int zsocket_type (void *zocket);
    CZMQ_EXPORT int zsocket_sndhwm (void *zocket);
    CZMQ_EXPORT int zsocket_rcvhwm (void *zocket);
    CZMQ_EXPORT int zsocket_affinity (void *zocket);
    CZMQ_EXPORT char * zsocket_identity (void *zocket);
    CZMQ_EXPORT int zsocket_rate (void *zocket);
    CZMQ_EXPORT int zsocket_recovery_ivl (void *zocket);
    CZMQ_EXPORT int zsocket_sndbuf (void *zocket);
    CZMQ_EXPORT int zsocket_rcvbuf (void *zocket);
    CZMQ_EXPORT int zsocket_linger (void *zocket);
    CZMQ_EXPORT int zsocket_reconnect_ivl (void *zocket);
    CZMQ_EXPORT int zsocket_reconnect_ivl_max (void *zocket);
    CZMQ_EXPORT int zsocket_backlog (void *zocket);
    CZMQ_EXPORT int zsocket_maxmsgsize (void *zocket);
    CZMQ_EXPORT int zsocket_multicast_hops (void *zocket);
    CZMQ_EXPORT int zsocket_rcvtimeo (void *zocket);
    CZMQ_EXPORT int zsocket_sndtimeo (void *zocket);
    CZMQ_EXPORT int zsocket_tcp_keepalive (void *zocket);
    CZMQ_EXPORT int zsocket_tcp_keepalive_idle (void *zocket);
    CZMQ_EXPORT int zsocket_tcp_keepalive_cnt (void *zocket);
    CZMQ_EXPORT int zsocket_tcp_keepalive_intvl (void *zocket);
    CZMQ_EXPORT char * zsocket_tcp_accept_filter (void *zocket);
    CZMQ_EXPORT int zsocket_rcvmore (void *zocket);
    CZMQ_EXPORT int zsocket_fd (void *zocket);
    CZMQ_EXPORT int zsocket_events (void *zocket);
    CZMQ_EXPORT char * zsocket_last_endpoint (void *zocket);
    
    //  Set socket options
    CZMQ_EXPORT void zsocket_set_tos (void *zocket, int tos);
    CZMQ_EXPORT void zsocket_set_router_handover (void *zocket, int router_handover);
    CZMQ_EXPORT void zsocket_set_router_mandatory (void *zocket, int router_mandatory);
    CZMQ_EXPORT void zsocket_set_probe_router (void *zocket, int probe_router);
    CZMQ_EXPORT void zsocket_set_req_relaxed (void *zocket, int req_relaxed);
    CZMQ_EXPORT void zsocket_set_req_correlate (void *zocket, int req_correlate);
    CZMQ_EXPORT void zsocket_set_conflate (void *zocket, int conflate);
    CZMQ_EXPORT void zsocket_set_zap_domain (void *zocket, const char * zap_domain);
    CZMQ_EXPORT void zsocket_set_plain_server (void *zocket, int plain_server);
    CZMQ_EXPORT void zsocket_set_plain_username (void *zocket, const char * plain_username);
    CZMQ_EXPORT void zsocket_set_plain_password (void *zocket, const char * plain_password);
    CZMQ_EXPORT void zsocket_set_curve_server (void *zocket, int curve_server);
    CZMQ_EXPORT void zsocket_set_curve_publickey (void *zocket, const char * curve_publickey);
    CZMQ_EXPORT void zsocket_set_curve_publickey_bin (void *zocket, const byte *curve_publickey);
    CZMQ_EXPORT void zsocket_set_curve_secretkey (void *zocket, const char * curve_secretkey);
    CZMQ_EXPORT void zsocket_set_curve_secretkey_bin (void *zocket, const byte *curve_secretkey);
    CZMQ_EXPORT void zsocket_set_curve_serverkey (void *zocket, const char * curve_serverkey);
    CZMQ_EXPORT void zsocket_set_curve_serverkey_bin (void *zocket, const byte *curve_serverkey);
    CZMQ_EXPORT void zsocket_set_gssapi_server (void *zocket, int gssapi_server);
    CZMQ_EXPORT void zsocket_set_gssapi_plaintext (void *zocket, int gssapi_plaintext);
    CZMQ_EXPORT void zsocket_set_gssapi_principal (void *zocket, const char * gssapi_principal);
    CZMQ_EXPORT void zsocket_set_gssapi_service_principal (void *zocket, const char * gssapi_service_principal);
    CZMQ_EXPORT void zsocket_set_ipv6 (void *zocket, int ipv6);
    CZMQ_EXPORT void zsocket_set_immediate (void *zocket, int immediate);
    CZMQ_EXPORT void zsocket_set_router_raw (void *zocket, int router_raw);
    CZMQ_EXPORT void zsocket_set_ipv4only (void *zocket, int ipv4only);
    CZMQ_EXPORT void zsocket_set_delay_attach_on_connect (void *zocket, int delay_attach_on_connect);
    CZMQ_EXPORT void zsocket_set_sndhwm (void *zocket, int sndhwm);
    CZMQ_EXPORT void zsocket_set_rcvhwm (void *zocket, int rcvhwm);
    CZMQ_EXPORT void zsocket_set_affinity (void *zocket, int affinity);
    CZMQ_EXPORT void zsocket_set_subscribe (void *zocket, const char * subscribe);
    CZMQ_EXPORT void zsocket_set_unsubscribe (void *zocket, const char * unsubscribe);
    CZMQ_EXPORT void zsocket_set_identity (void *zocket, const char * identity);
    CZMQ_EXPORT void zsocket_set_rate (void *zocket, int rate);
    CZMQ_EXPORT void zsocket_set_recovery_ivl (void *zocket, int recovery_ivl);
    CZMQ_EXPORT void zsocket_set_sndbuf (void *zocket, int sndbuf);
    CZMQ_EXPORT void zsocket_set_rcvbuf (void *zocket, int rcvbuf);
    CZMQ_EXPORT void zsocket_set_linger (void *zocket, int linger);
    CZMQ_EXPORT void zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl);
    CZMQ_EXPORT void zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max);
    CZMQ_EXPORT void zsocket_set_backlog (void *zocket, int backlog);
    CZMQ_EXPORT void zsocket_set_maxmsgsize (void *zocket, int maxmsgsize);
    CZMQ_EXPORT void zsocket_set_multicast_hops (void *zocket, int multicast_hops);
    CZMQ_EXPORT void zsocket_set_rcvtimeo (void *zocket, int rcvtimeo);
    CZMQ_EXPORT void zsocket_set_sndtimeo (void *zocket, int sndtimeo);
    CZMQ_EXPORT void zsocket_set_xpub_verbose (void *zocket, int xpub_verbose);
    CZMQ_EXPORT void zsocket_set_tcp_keepalive (void *zocket, int tcp_keepalive);
    CZMQ_EXPORT void zsocket_set_tcp_keepalive_idle (void *zocket, int tcp_keepalive_idle);
    CZMQ_EXPORT void zsocket_set_tcp_keepalive_cnt (void *zocket, int tcp_keepalive_cnt);
    CZMQ_EXPORT void zsocket_set_tcp_keepalive_intvl (void *zocket, int tcp_keepalive_intvl);
    CZMQ_EXPORT void zsocket_set_tcp_accept_filter (void *zocket, const char * tcp_accept_filter);
    #endif
    
    #if (ZMQ_VERSION_MAJOR == 3)
    //  Get socket options
    CZMQ_EXPORT int zsocket_ipv4only (void *zocket);
    CZMQ_EXPORT int zsocket_type (void *zocket);
    CZMQ_EXPORT int zsocket_sndhwm (void *zocket);
    CZMQ_EXPORT int zsocket_rcvhwm (void *zocket);
    CZMQ_EXPORT int zsocket_affinity (void *zocket);
    CZMQ_EXPORT char * zsocket_identity (void *zocket);
    CZMQ_EXPORT int zsocket_rate (void *zocket);
    CZMQ_EXPORT int zsocket_recovery_ivl (void *zocket);
    CZMQ_EXPORT int zsocket_sndbuf (void *zocket);
    CZMQ_EXPORT int zsocket_rcvbuf (void *zocket);
    CZMQ_EXPORT int zsocket_linger (void *zocket);
    CZMQ_EXPORT int zsocket_reconnect_ivl (void *zocket);
    CZMQ_EXPORT int zsocket_reconnect_ivl_max (void *zocket);
    CZMQ_EXPORT int zsocket_backlog (void *zocket);
    CZMQ_EXPORT int zsocket_maxmsgsize (void *zocket);
    CZMQ_EXPORT int zsocket_multicast_hops (void *zocket);
    CZMQ_EXPORT int zsocket_rcvtimeo (void *zocket);
    CZMQ_EXPORT int zsocket_sndtimeo (void *zocket);
    CZMQ_EXPORT int zsocket_tcp_keepalive (void *zocket);
    CZMQ_EXPORT int zsocket_tcp_keepalive_idle (void *zocket);
    CZMQ_EXPORT int zsocket_tcp_keepalive_cnt (void *zocket);
    CZMQ_EXPORT int zsocket_tcp_keepalive_intvl (void *zocket);
    CZMQ_EXPORT char * zsocket_tcp_accept_filter (void *zocket);
    CZMQ_EXPORT int zsocket_rcvmore (void *zocket);
    CZMQ_EXPORT int zsocket_fd (void *zocket);
    CZMQ_EXPORT int zsocket_events (void *zocket);
    CZMQ_EXPORT char * zsocket_last_endpoint (void *zocket);
    
    //  Set socket options
    CZMQ_EXPORT void zsocket_set_router_raw (void *zocket, int router_raw);
    CZMQ_EXPORT void zsocket_set_ipv4only (void *zocket, int ipv4only);
    CZMQ_EXPORT void zsocket_set_delay_attach_on_connect (void *zocket, int delay_attach_on_connect);
    CZMQ_EXPORT void zsocket_set_sndhwm (void *zocket, int sndhwm);
    CZMQ_EXPORT void zsocket_set_rcvhwm (void *zocket, int rcvhwm);
    CZMQ_EXPORT void zsocket_set_affinity (void *zocket, int affinity);
    CZMQ_EXPORT void zsocket_set_subscribe (void *zocket, const char * subscribe);
    CZMQ_EXPORT void zsocket_set_unsubscribe (void *zocket, const char * unsubscribe);
    CZMQ_EXPORT void zsocket_set_identity (void *zocket, const char * identity);
    CZMQ_EXPORT void zsocket_set_rate (void *zocket, int rate);
    CZMQ_EXPORT void zsocket_set_recovery_ivl (void *zocket, int recovery_ivl);
    CZMQ_EXPORT void zsocket_set_sndbuf (void *zocket, int sndbuf);
    CZMQ_EXPORT void zsocket_set_rcvbuf (void *zocket, int rcvbuf);
    CZMQ_EXPORT void zsocket_set_linger (void *zocket, int linger);
    CZMQ_EXPORT void zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl);
    CZMQ_EXPORT void zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max);
    CZMQ_EXPORT void zsocket_set_backlog (void *zocket, int backlog);
    CZMQ_EXPORT void zsocket_set_maxmsgsize (void *zocket, int maxmsgsize);
    CZMQ_EXPORT void zsocket_set_multicast_hops (void *zocket, int multicast_hops);
    CZMQ_EXPORT void zsocket_set_rcvtimeo (void *zocket, int rcvtimeo);
    CZMQ_EXPORT void zsocket_set_sndtimeo (void *zocket, int sndtimeo);
    CZMQ_EXPORT void zsocket_set_xpub_verbose (void *zocket, int xpub_verbose);
    CZMQ_EXPORT void zsocket_set_tcp_keepalive (void *zocket, int tcp_keepalive);
    CZMQ_EXPORT void zsocket_set_tcp_keepalive_idle (void *zocket, int tcp_keepalive_idle);
    CZMQ_EXPORT void zsocket_set_tcp_keepalive_cnt (void *zocket, int tcp_keepalive_cnt);
    CZMQ_EXPORT void zsocket_set_tcp_keepalive_intvl (void *zocket, int tcp_keepalive_intvl);
    CZMQ_EXPORT void zsocket_set_tcp_accept_filter (void *zocket, const char * tcp_accept_filter);
    #endif
    
    #if (ZMQ_VERSION_MAJOR == 2)
    //  Get socket options
    CZMQ_EXPORT int zsocket_hwm (void *zocket);
    CZMQ_EXPORT int zsocket_swap (void *zocket);
    CZMQ_EXPORT int zsocket_affinity (void *zocket);
    CZMQ_EXPORT char * zsocket_identity (void *zocket);
    CZMQ_EXPORT int zsocket_rate (void *zocket);
    CZMQ_EXPORT int zsocket_recovery_ivl (void *zocket);
    CZMQ_EXPORT int zsocket_recovery_ivl_msec (void *zocket);
    CZMQ_EXPORT int zsocket_mcast_loop (void *zocket);
    #   if (ZMQ_VERSION_MINOR == 2)
    CZMQ_EXPORT int zsocket_rcvtimeo (void *zocket);
    #   endif
    #   if (ZMQ_VERSION_MINOR == 2)
    CZMQ_EXPORT int zsocket_sndtimeo (void *zocket);
    #   endif
    CZMQ_EXPORT int zsocket_sndbuf (void *zocket);
    CZMQ_EXPORT int zsocket_rcvbuf (void *zocket);
    CZMQ_EXPORT int zsocket_linger (void *zocket);
    CZMQ_EXPORT int zsocket_reconnect_ivl (void *zocket);
    CZMQ_EXPORT int zsocket_reconnect_ivl_max (void *zocket);
    CZMQ_EXPORT int zsocket_backlog (void *zocket);
    CZMQ_EXPORT int zsocket_type (void *zocket);
    CZMQ_EXPORT int zsocket_rcvmore (void *zocket);
    CZMQ_EXPORT int zsocket_fd (void *zocket);
    CZMQ_EXPORT int zsocket_events (void *zocket);
    
    //  Set socket options
    CZMQ_EXPORT void zsocket_set_hwm (void *zocket, int hwm);
    CZMQ_EXPORT void zsocket_set_swap (void *zocket, int swap);
    CZMQ_EXPORT void zsocket_set_affinity (void *zocket, int affinity);
    CZMQ_EXPORT void zsocket_set_identity (void *zocket, const char * identity);
    CZMQ_EXPORT void zsocket_set_rate (void *zocket, int rate);
    CZMQ_EXPORT void zsocket_set_recovery_ivl (void *zocket, int recovery_ivl);
    CZMQ_EXPORT void zsocket_set_recovery_ivl_msec (void *zocket, int recovery_ivl_msec);
    CZMQ_EXPORT void zsocket_set_mcast_loop (void *zocket, int mcast_loop);
    #   if (ZMQ_VERSION_MINOR == 2)
    CZMQ_EXPORT void zsocket_set_rcvtimeo (void *zocket, int rcvtimeo);
    #   endif
    #   if (ZMQ_VERSION_MINOR == 2)
    CZMQ_EXPORT void zsocket_set_sndtimeo (void *zocket, int sndtimeo);
    #   endif
    CZMQ_EXPORT void zsocket_set_sndbuf (void *zocket, int sndbuf);
    CZMQ_EXPORT void zsocket_set_rcvbuf (void *zocket, int rcvbuf);
    CZMQ_EXPORT void zsocket_set_linger (void *zocket, int linger);
    CZMQ_EXPORT void zsocket_set_reconnect_ivl (void *zocket, int reconnect_ivl);
    CZMQ_EXPORT void zsocket_set_reconnect_ivl_max (void *zocket, int reconnect_ivl_max);
    CZMQ_EXPORT void zsocket_set_backlog (void *zocket, int backlog);
    CZMQ_EXPORT void zsocket_set_subscribe (void *zocket, const char * subscribe);
    CZMQ_EXPORT void zsocket_set_unsubscribe (void *zocket, const char * unsubscribe);
    #endif
    
    //  Self test of this class
    CZMQ_EXPORT void zsockopt_test (bool verbose);

This is the class self test code:

    zctx_t *ctx = zctx_new ();
    assert (ctx);
    void *zocket;
    #if (ZMQ_VERSION_MAJOR == 4)
    #     if defined (ZMQ_TOS)
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_set_tos (zocket, 1);
    assert (zsocket_tos (zocket) == 1);
    zsocket_tos (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_ROUTER_HANDOVER)
    zocket = zsocket_new (ctx, ZMQ_ROUTER);
    assert (zocket);
    zsocket_set_router_handover (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_ROUTER_MANDATORY)
    zocket = zsocket_new (ctx, ZMQ_ROUTER);
    assert (zocket);
    zsocket_set_router_mandatory (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_PROBE_ROUTER)
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_set_probe_router (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_REQ_RELAXED)
    zocket = zsocket_new (ctx, ZMQ_REQ);
    assert (zocket);
    zsocket_set_req_relaxed (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_REQ_CORRELATE)
    zocket = zsocket_new (ctx, ZMQ_REQ);
    assert (zocket);
    zsocket_set_req_correlate (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_CONFLATE)
    zocket = zsocket_new (ctx, ZMQ_PUSH);
    assert (zocket);
    zsocket_set_conflate (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_ZAP_DOMAIN)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_zap_domain (zocket, "test");
    char *zap_domain = zsocket_zap_domain (zocket);
    assert (zap_domain);
    free (zap_domain);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_MECHANISM)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_mechanism (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_PLAIN_SERVER)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_plain_server (zocket, 1);
    assert (zsocket_plain_server (zocket) == 1);
    zsocket_plain_server (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_PLAIN_USERNAME)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_plain_username (zocket, "test");
    char *plain_username = zsocket_plain_username (zocket);
    assert (plain_username);
    free (plain_username);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_PLAIN_PASSWORD)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_plain_password (zocket, "test");
    char *plain_password = zsocket_plain_password (zocket);
    assert (plain_password);
    free (plain_password);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_IPV6)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_ipv6 (zocket, 1);
    assert (zsocket_ipv6 (zocket) == 1);
    zsocket_ipv6 (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_IMMEDIATE)
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_set_immediate (zocket, 1);
    assert (zsocket_immediate (zocket) == 1);
    zsocket_immediate (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_ROUTER_RAW)
    zocket = zsocket_new (ctx, ZMQ_ROUTER);
    assert (zocket);
    zsocket_set_router_raw (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_IPV4ONLY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_ipv4only (zocket, 1);
    assert (zsocket_ipv4only (zocket) == 1);
    zsocket_ipv4only (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_delay_attach_on_connect (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TYPE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_type (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SNDHWM)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_sndhwm (zocket, 1);
    assert (zsocket_sndhwm (zocket) == 1);
    zsocket_sndhwm (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVHWM)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvhwm (zocket, 1);
    assert (zsocket_rcvhwm (zocket) == 1);
    zsocket_rcvhwm (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_AFFINITY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_affinity (zocket, 1);
    assert (zsocket_affinity (zocket) == 1);
    zsocket_affinity (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_subscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_UNSUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_unsubscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_IDENTITY)
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_set_identity (zocket, "test");
    char *identity = zsocket_identity (zocket);
    assert (identity);
    free (identity);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RATE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rate (zocket, 1);
    assert (zsocket_rate (zocket) == 1);
    zsocket_rate (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECOVERY_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl (zocket, 1);
    assert (zsocket_recovery_ivl (zocket) == 1);
    zsocket_recovery_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SNDBUF)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_sndbuf (zocket, 1);
    assert (zsocket_sndbuf (zocket) == 1);
    zsocket_sndbuf (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVBUF)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvbuf (zocket, 1);
    assert (zsocket_rcvbuf (zocket) == 1);
    zsocket_rcvbuf (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_LINGER)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_linger (zocket, 1);
    assert (zsocket_linger (zocket) == 1);
    zsocket_linger (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl (zocket, 1);
    assert (zsocket_reconnect_ivl (zocket) == 1);
    zsocket_reconnect_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL_MAX)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl_max (zocket, 1);
    assert (zsocket_reconnect_ivl_max (zocket) == 1);
    zsocket_reconnect_ivl_max (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_BACKLOG)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_backlog (zocket, 1);
    assert (zsocket_backlog (zocket) == 1);
    zsocket_backlog (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_MAXMSGSIZE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_maxmsgsize (zocket, 1);
    assert (zsocket_maxmsgsize (zocket) == 1);
    zsocket_maxmsgsize (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_MULTICAST_HOPS)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_multicast_hops (zocket, 1);
    assert (zsocket_multicast_hops (zocket) == 1);
    zsocket_multicast_hops (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvtimeo (zocket, 1);
    assert (zsocket_rcvtimeo (zocket) == 1);
    zsocket_rcvtimeo (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SNDTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndtimeo (zocket, 1);
    assert (zsocket_sndtimeo (zocket) == 1);
    zsocket_sndtimeo (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_XPUB_VERBOSE)
    zocket = zsocket_new (ctx, ZMQ_XPUB);
    assert (zocket);
    zsocket_set_xpub_verbose (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive (zocket, 1);
    assert (zsocket_tcp_keepalive (zocket) == 1);
    zsocket_tcp_keepalive (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_idle (zocket, 1);
    assert (zsocket_tcp_keepalive_idle (zocket) == 1);
    zsocket_tcp_keepalive_idle (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_CNT)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_cnt (zocket, 1);
    assert (zsocket_tcp_keepalive_cnt (zocket) == 1);
    zsocket_tcp_keepalive_cnt (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_intvl (zocket, 1);
    assert (zsocket_tcp_keepalive_intvl (zocket) == 1);
    zsocket_tcp_keepalive_intvl (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_ACCEPT_FILTER)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_accept_filter (zocket, "127.0.0.1");
    char *tcp_accept_filter = zsocket_tcp_accept_filter (zocket);
    assert (tcp_accept_filter);
    free (tcp_accept_filter);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVMORE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_rcvmore (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_FD)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_fd (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_EVENTS)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_events (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_LAST_ENDPOINT)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    char *last_endpoint = zsocket_last_endpoint (zocket);
    assert (last_endpoint);
    free (last_endpoint);
    zsocket_destroy (ctx, zocket);
    #     endif
    #endif
    
    #if (ZMQ_VERSION_MAJOR == 3)
    #     if defined (ZMQ_ROUTER_RAW)
    zocket = zsocket_new (ctx, ZMQ_ROUTER);
    assert (zocket);
    zsocket_set_router_raw (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_IPV4ONLY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_ipv4only (zocket, 1);
    assert (zsocket_ipv4only (zocket) == 1);
    zsocket_ipv4only (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_delay_attach_on_connect (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TYPE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_type (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SNDHWM)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_sndhwm (zocket, 1);
    assert (zsocket_sndhwm (zocket) == 1);
    zsocket_sndhwm (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVHWM)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvhwm (zocket, 1);
    assert (zsocket_rcvhwm (zocket) == 1);
    zsocket_rcvhwm (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_AFFINITY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_affinity (zocket, 1);
    assert (zsocket_affinity (zocket) == 1);
    zsocket_affinity (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_subscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_UNSUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_unsubscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_IDENTITY)
    zocket = zsocket_new (ctx, ZMQ_DEALER);
    assert (zocket);
    zsocket_set_identity (zocket, "test");
    char *identity = zsocket_identity (zocket);
    assert (identity);
    free (identity);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RATE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rate (zocket, 1);
    assert (zsocket_rate (zocket) == 1);
    zsocket_rate (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECOVERY_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl (zocket, 1);
    assert (zsocket_recovery_ivl (zocket) == 1);
    zsocket_recovery_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SNDBUF)
    zocket = zsocket_new (ctx, ZMQ_PUB);
    assert (zocket);
    zsocket_set_sndbuf (zocket, 1);
    assert (zsocket_sndbuf (zocket) == 1);
    zsocket_sndbuf (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVBUF)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvbuf (zocket, 1);
    assert (zsocket_rcvbuf (zocket) == 1);
    zsocket_rcvbuf (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_LINGER)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_linger (zocket, 1);
    assert (zsocket_linger (zocket) == 1);
    zsocket_linger (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl (zocket, 1);
    assert (zsocket_reconnect_ivl (zocket) == 1);
    zsocket_reconnect_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL_MAX)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl_max (zocket, 1);
    assert (zsocket_reconnect_ivl_max (zocket) == 1);
    zsocket_reconnect_ivl_max (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_BACKLOG)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_backlog (zocket, 1);
    assert (zsocket_backlog (zocket) == 1);
    zsocket_backlog (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_MAXMSGSIZE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_maxmsgsize (zocket, 1);
    assert (zsocket_maxmsgsize (zocket) == 1);
    zsocket_maxmsgsize (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_MULTICAST_HOPS)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_multicast_hops (zocket, 1);
    assert (zsocket_multicast_hops (zocket) == 1);
    zsocket_multicast_hops (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvtimeo (zocket, 1);
    assert (zsocket_rcvtimeo (zocket) == 1);
    zsocket_rcvtimeo (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SNDTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndtimeo (zocket, 1);
    assert (zsocket_sndtimeo (zocket) == 1);
    zsocket_sndtimeo (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_XPUB_VERBOSE)
    zocket = zsocket_new (ctx, ZMQ_XPUB);
    assert (zocket);
    zsocket_set_xpub_verbose (zocket, 1);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive (zocket, 1);
    assert (zsocket_tcp_keepalive (zocket) == 1);
    zsocket_tcp_keepalive (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_idle (zocket, 1);
    assert (zsocket_tcp_keepalive_idle (zocket) == 1);
    zsocket_tcp_keepalive_idle (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_CNT)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_cnt (zocket, 1);
    assert (zsocket_tcp_keepalive_cnt (zocket) == 1);
    zsocket_tcp_keepalive_cnt (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_keepalive_intvl (zocket, 1);
    assert (zsocket_tcp_keepalive_intvl (zocket) == 1);
    zsocket_tcp_keepalive_intvl (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TCP_ACCEPT_FILTER)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_tcp_accept_filter (zocket, "127.0.0.1");
    char *tcp_accept_filter = zsocket_tcp_accept_filter (zocket);
    assert (tcp_accept_filter);
    free (tcp_accept_filter);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVMORE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_rcvmore (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_FD)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_fd (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_EVENTS)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_events (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_LAST_ENDPOINT)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    char *last_endpoint = zsocket_last_endpoint (zocket);
    assert (last_endpoint);
    free (last_endpoint);
    zsocket_destroy (ctx, zocket);
    #     endif
    #endif
    
    #if (ZMQ_VERSION_MAJOR == 2)
    #     if defined (ZMQ_HWM)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_hwm (zocket, 1);
    assert (zsocket_hwm (zocket) == 1);
    zsocket_hwm (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SWAP)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_swap (zocket, 1);
    assert (zsocket_swap (zocket) == 1);
    zsocket_swap (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_AFFINITY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_affinity (zocket, 1);
    assert (zsocket_affinity (zocket) == 1);
    zsocket_affinity (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_IDENTITY)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_identity (zocket, "test");
    char *identity = zsocket_identity (zocket);
    assert (identity);
    free (identity);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RATE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rate (zocket, 1);
    assert (zsocket_rate (zocket) == 1);
    zsocket_rate (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECOVERY_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl (zocket, 1);
    assert (zsocket_recovery_ivl (zocket) == 1);
    zsocket_recovery_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECOVERY_IVL_MSEC)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_recovery_ivl_msec (zocket, 1);
    assert (zsocket_recovery_ivl_msec (zocket) == 1);
    zsocket_recovery_ivl_msec (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_MCAST_LOOP)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_mcast_loop (zocket, 1);
    assert (zsocket_mcast_loop (zocket) == 1);
    zsocket_mcast_loop (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #   if (ZMQ_VERSION_MINOR == 2)
    #     if defined (ZMQ_RCVTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvtimeo (zocket, 1);
    assert (zsocket_rcvtimeo (zocket) == 1);
    zsocket_rcvtimeo (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #   endif
    #   if (ZMQ_VERSION_MINOR == 2)
    #     if defined (ZMQ_SNDTIMEO)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndtimeo (zocket, 1);
    assert (zsocket_sndtimeo (zocket) == 1);
    zsocket_sndtimeo (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #   endif
    #     if defined (ZMQ_SNDBUF)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_sndbuf (zocket, 1);
    assert (zsocket_sndbuf (zocket) == 1);
    zsocket_sndbuf (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVBUF)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_rcvbuf (zocket, 1);
    assert (zsocket_rcvbuf (zocket) == 1);
    zsocket_rcvbuf (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_LINGER)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_linger (zocket, 1);
    assert (zsocket_linger (zocket) == 1);
    zsocket_linger (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl (zocket, 1);
    assert (zsocket_reconnect_ivl (zocket) == 1);
    zsocket_reconnect_ivl (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RECONNECT_IVL_MAX)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_reconnect_ivl_max (zocket, 1);
    assert (zsocket_reconnect_ivl_max (zocket) == 1);
    zsocket_reconnect_ivl_max (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_BACKLOG)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_backlog (zocket, 1);
    assert (zsocket_backlog (zocket) == 1);
    zsocket_backlog (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_SUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_subscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_UNSUBSCRIBE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_set_unsubscribe (zocket, "test");
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_TYPE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_type (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_RCVMORE)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_rcvmore (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_FD)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_fd (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #     if defined (ZMQ_EVENTS)
    zocket = zsocket_new (ctx, ZMQ_SUB);
    assert (zocket);
    zsocket_events (zocket);
    zsocket_destroy (ctx, zocket);
    #     endif
    #endif
    
    zctx_destroy (&ctx);

<A name="toc4-7933" title="zthread - working with system threads (deprecated)" />
#### zthread - working with system threads (deprecated)

The zthread class wraps OS thread creation. It creates detached threads
that look like normal OS threads, or attached threads that share the
caller's ØMQ context, and get an inproc pipe to talk back to the parent
thread. Detached threads create their own ØMQ contexts as needed.
NOTE: this class is deprecated in favor of zactor.

We have several use cases for multiple threads. One is to simulate many
processes, so we can test ØMQ designs and flows more easily. Another is
to create APIs that can send and receive ØMQ messages in the background.

zthread solves these two use cases separately, using the zthread_new
and zthead_fork methods respectively. These methods wrap the native
system calls needed to start threads, so your code can remain fully
portable.

Detached threads follow the POSIX pthreads API; they accept a void *
argument and return a void * result (always NULL in our case).

Attached thread receive a void * argument, a zctx_t context, and a pipe
socket. The pipe socket is a PAIR socket that is connected back to the
caller. When you call zthread_fork, it returns you a PAIR socket that
is the other end of this pipe. Thus attached threads can talk back to
their parent threads over the pipe. We use this very heavily when making
so-called "asynchronous" APIs, which you can see in the Guide examples
like 'clone'.

To recap some rules about threading: do not share sockets between
threads or your code will crash. You can migrate a socket from one
thread to a child thread, if you stop using it in the parent thread
immediately after creating the child thread. If you want to connect
sockets over inproc:// they must share the same ØMQ context, i.e. be
attached threads. You should always use zthread_fork to create an
attached thread; it is not sufficient to pass a zctx_t structure to
a detached thread (this will crash).

If you want to communicate over ipc:// or tcp:// you may be sharing
the same context, or use separate contexts. Thus, every detached thread
usually starts by creating its own zctx_t instance.

This is the class interface:

    //  Detached threads follow POSIX pthreads API
    typedef void *(zthread_detached_fn) (void *args);
    
    //  Attached threads get context and pipe from parent
    typedef void (zthread_attached_fn) (void *args, zctx_t *ctx, void *pipe);
    
    //  Create a detached thread. A detached thread operates autonomously
    //  and is used to simulate a separate process. It gets no ctx, and no
    //  pipe.
    CZMQ_EXPORT int
        zthread_new (zthread_detached_fn *thread_fn, void *args);
    
    //  Create an attached thread. An attached thread gets a ctx and a PAIR
    //  pipe back to its parent. It must monitor its pipe, and exit if the
    //  pipe becomes unreadable. Do not destroy the ctx, the thread does this
    //  automatically when it ends.
    CZMQ_EXPORT void *
        zthread_fork (zctx_t *ctx, zthread_attached_fn *thread_fn, void *args);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zthread_test (bool verbose);

This is the class self test code:

    static void *
    s_test_detached (void *args)
    {
    //  Create a socket to check it'll be automatically deleted
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    
    void *push = zsocket_new (ctx, ZMQ_PUSH);
    assert (push);
    zctx_destroy (&ctx);
    return NULL;
    }
    
    static void
    s_test_attached (void *args, zctx_t *ctx, void *pipe)
    {
    //  Create a socket to check it'll be automatically deleted
    zsocket_new (ctx, ZMQ_PUSH);
    assert (ctx);
    //  Wait for our parent to ping us, and pong back
    char *ping = zstr_recv (pipe);
    assert (ping);
    zstr_free (&ping);
    zstr_send (pipe, "pong");
    }
    
    zctx_t *ctx = zctx_new ();
    assert (ctx);
    int rc = 0;
    
    //  Create a detached thread, let it run
    rc = zthread_new (s_test_detached, NULL);
    assert (rc == 0);
    zclock_sleep (100);
    
    //  Create an attached thread, check it's safely alive
    void *pipe = zthread_fork (ctx, s_test_attached, NULL);
    assert (pipe);
    zstr_send (pipe, "ping");
    char *pong = zstr_recv (pipe);
    assert (pong);
    assert (streq (pong, "pong"));
    zstr_free (&pong);
    
    //  Everything should be cleanly closed now
    zctx_destroy (&ctx);


<A name="toc2-8050" title="Error Handling" />
## Error Handling

The CZMQ policy is to reduce the error flow to 0/-1 where possible. libzmq still does a lot of errno setting. CZMQ does not do that, as it creates a fuzzy API. Things either work as expected, or they fail, and the application's best strategy is usually to assert on non-zero return codes.

Some older libraries still return plethora of error codes, to indicate different types of failure. This ironically makes both library and application more likely to be buggy. The reason is simply that it needs more code on both sides of the API, and the more code, the more bugs.

The use of black/white error handling fits the CLASS style for APIs where each call is explicit and without side effects of any kind, and where damage is either impossible, or fatal.

The one exception is running out of resources (memory, sockets). In that case, there are two strategies that work, for different types of app. One is to assert, to force better sizing of the machine and/or limits such as max connections. Two is to degrade carefully, e.g. refuse new connections, however that is considerably harder to do correctly and probably unrealistic for most developers.

Some CZMQ methods used to actually assert, e.g. in zsocket_bind, if the action failed, instead of returning -1. That was just closer to the majority case where the action MUST work, or nothing can continue. However there's a small slice of cases where failure means something positive, and for these cases, such calls return -1 on failure. 99% of calling code simply asserts the return value is not -1.

There are a few cases where the return value is overloaded to return -1, 0, or other values. These are somewhat confusing special cases and we aim to eliminate them over time.

The overall goal with this strategy is robustness, and absolute minimal and predictable expression in the code. You can see that it works: the CZMQ code is generally very simple and clear, with a few exceptions of places where people have used their old C style (we fix these over time).

<A name="toc2-8067" title="CZMQ Actors" />
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

<A name="toc2-8213" title="Under the Hood" />
## Under the Hood

<A name="toc3-8216" title="Adding a New Class" />
### Adding a New Class

If you define a new CZMQ class `myclass` you need to:

* Write the `zmyclass.c` and `zmyclass.h` source files, in `src` and `include` respectively.
* Add`#include <zmyclass.h>` to `include/czmq.h`.
* Add the myclass header and test call to `src/czmq_selftest.c`.
* Add a reference documentation to 'doc/zmyclass.txt'.
* Add myclass to 'model/projects.xml` and read model/README.txt.
* Add a section to README.txt.

<A name="toc3-8228" title="Documentation" />
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

<A name="toc3-8267" title="Development" />
### Development

CZMQ is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

<A name="toc3-8277" title="Porting CZMQ" />
### Porting CZMQ

When you try CZMQ on an OS that it's not been used on (ever, or for a while), you will hit code that does not compile. In some cases the patches are trivial, in other cases (usually when porting to Windows), the work needed to build equivalent functionality may be non-trivial. In any case, the benefit is that once ported, the functionality is available to all applications.

Before attempting to patch code for portability, please read the `czmq_prelude.h` header file. There are several typical types of changes you may need to make to get functionality working on a specific operating system:

* Defining typedefs which are missing on that specific compiler: do this in czmq_prelude.h.
* Defining macros that rename exotic library functions to more conventional names: do this in czmq_prelude.h.
* Reimplementing specific methods to use a non-standard API: this is typically needed on Windows. Do this in the relevant class, using #ifdefs to properly differentiate code for different platforms.

<A name="toc3-8288" title="Hints to Contributors" />
### Hints to Contributors

CZMQ is a nice, neat library, and you may not immediately appreciate why. Read the CLASS style guide please, and write your code to make it indistinguishable from the rest of the code in the library. That is the only real criteria for good style: it's invisible.

Don't include system headers in source files. The right place for these is czmq_prelude.h. If you need to check against configured libraries and/or headers, include platform.h in the source before including czmq.h.

Do read your code after you write it and ask, "Can I make this simpler?" We do use a nice minimalist and yet readable style. Learn it, adopt it, use it.

Before opening a pull request read our [contribution guidelines](https://github.com/zeromq/czmq/blob/master/CONTRIBUTING.md). Thanks!


<A name="toc3-8300" title="Code Generation" />
### Code Generation

We generate the zsockopt class using [GSL](https://github.com/imatix/gsl), using a code generator script in scripts/sockopts.gsl. We also generate the project files.

<A name="toc3-8305" title="This Document" />
### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
