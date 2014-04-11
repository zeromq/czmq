
<A name="toc1-3" title="CZMQ - High-level C binding for ØMQ" />
# CZMQ - High-level C binding for ØMQ

[![Build Status](https://travis-ci.org/zeromq/czmq.png?branch=master)](https://travis-ci.org/zeromq/czmq)

<A name="toc2-8" title="Contents" />
## Contents


**<a href="#toc2-13">Overview</a>**
&emsp;<a href="#toc3-16">Scope and Goals</a>
&emsp;<a href="#toc3-57">Ownership and License</a>

**<a href="#toc2-68">Using CZMQ</a>**
&emsp;<a href="#toc3-71">Building and Installing</a>
&emsp;<a href="#toc3-115">Linking with an Application</a>
&emsp;<a href="#toc3-122">Use from Other Languages</a>
&emsp;<a href="#toc3-135">API Summary</a>
&emsp;<a href="#toc4-138">zauth - authentication for ZeroMQ security mechanisms</a>
&emsp;<a href="#toc4-149">zbeacon - LAN service announcement and discovery</a>
&emsp;<a href="#toc4-160">zcert - work with CURVE security certificates</a>
&emsp;<a href="#toc4-171">zcertstore - work with CURVE security certificate stores</a>
&emsp;<a href="#toc4-182">zchunk - work with memory chunks</a>
&emsp;<a href="#toc4-193">zclock - millisecond clocks and delays</a>
&emsp;<a href="#toc4-204">zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.</a>
&emsp;<a href="#toc4-215">zctx - working with ØMQ contexts</a>
&emsp;<a href="#toc4-226">zdir - work with file-system directories</a>
&emsp;<a href="#toc4-237">zfile - work with files</a>
&emsp;<a href="#toc4-248">zframe - working with single message frames</a>
&emsp;<a href="#toc4-259">zhash - generic hash table container</a>
&emsp;<a href="#toc4-270">zlist - generic list container</a>
&emsp;<a href="#toc4-281">zloop - event-driven reactor</a>
&emsp;<a href="#toc4-292">zmonitor - socket event monitor</a>
&emsp;<a href="#toc4-303">zmsg - working with multipart messages</a>
&emsp;<a href="#toc4-314">zmutex - wrap lightweight mutexes</a>
&emsp;<a href="#toc4-325">zpoller - trivial socket poller class</a>
&emsp;<a href="#toc4-336">zproxy - convenient zmq_proxy API</a>
&emsp;<a href="#toc4-347">zsocket - working with ØMQ sockets</a>
&emsp;<a href="#toc4-358">zsockopt - working with ØMQ socket options</a>
&emsp;<a href="#toc4-369">zstr - sending and receiving strings</a>
&emsp;<a href="#toc4-394">zsys - system-level methods</a>
&emsp;<a href="#toc4-405">zrex - working with regular expressions</a>
&emsp;<a href="#toc4-416">zthread - working with system threads</a>
&emsp;<a href="#toc4-427">zuuid - UUID management</a>

**<a href="#toc2-438">Under the Hood</a>**
&emsp;<a href="#toc3-441">Adding a New Class</a>
&emsp;<a href="#toc3-453">Documentation</a>
&emsp;<a href="#toc3-492">Development</a>
&emsp;<a href="#toc3-502">Porting CZMQ</a>
&emsp;<a href="#toc3-513">Code Generation</a>
&emsp;<a href="#toc3-518">This Document</a>

<A name="toc2-13" title="Overview" />
## Overview

<A name="toc3-16" title="Scope and Goals" />
### Scope and Goals

CZMQ has these goals:

* To wrap the ØMQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of ØMQ, particularly 2.x and 3.x.
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

You will need the libtool and autotools packages. After building, run the CZMQ selftests:

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

<A name="toc3-135" title="API Summary" />
### API Summary

<A name="toc4-138" title="zauth - authentication for ZeroMQ security mechanisms" />
#### zauth - authentication for ZeroMQ security mechanisms

A zauth object takes over authentication for all incoming connections in 
its context. Note that libzmq provides four levels of security: default 
NULL (which zauth does not see), and authenticated NULL, PLAIN, and CURVE,
which zauth can see.

This is the class interface:

    #define CURVE_ALLOW_ANY "*"
    
    //  Constructor
    //  Install authentication for the specified context. Returns a new zauth
    //  object that you can use to configure authentication. Note that until you
    //  add policies, all incoming NULL connections are allowed (classic ZeroMQ
    //  behaviour), and all PLAIN and CURVE connections are denied. If there was
    //  an error during initialization, returns NULL.
    CZMQ_EXPORT zauth_t *
        zauth_new (zctx_t *ctx);
        
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
        
    //  Enable verbose tracing of commands and activity
    CZMQ_EXPORT void
        zauth_set_verbose (zauth_t *self, bool verbose);
        
    //  Destructor
    CZMQ_EXPORT void
        zauth_destroy (zauth_t **self_p);
    
    //  Selftest
    CZMQ_EXPORT int
        zauth_test (bool verbose);



<A name="toc4-149" title="zbeacon - LAN service announcement and discovery" />
#### zbeacon - LAN service announcement and discovery

The zbeacon class implements a peer-to-peer discovery service for local
networks. A beacon can broadcast and/or capture service announcements
using UDP messages on the local area network. This implementation uses
IPv4 UDP broadcasts. You can define the format of your outgoing beacons,
and set a filter that validates incoming beacons. Beacons are sent and
received asynchronously in the background. The zbeacon API provides a
incoming beacons on a ZeroMQ socket (the pipe) that you can configure,
poll on, and receive messages on. Incoming beacons are always delivered
as two frames: the ipaddress of the sender (a string), and the beacon
data itself (binary, as published).

This is the class interface:

    //  Create a new beacon on a certain UDP port
    CZMQ_EXPORT zbeacon_t *
        zbeacon_new (zctx_t *ctx, int port_nbr);
        
    //  Destroy a beacon
    CZMQ_EXPORT void
        zbeacon_destroy (zbeacon_t **self_p);
    
    //  Return our own IP address as printable string
    CZMQ_EXPORT char *
        zbeacon_hostname (zbeacon_t *self);
    
    //  Set broadcast interval in milliseconds (default is 1000 msec)
    CZMQ_EXPORT void
        zbeacon_set_interval (zbeacon_t *self, int interval);
    
    //  Filter out any beacon that looks exactly like ours
    CZMQ_EXPORT void
        zbeacon_noecho (zbeacon_t *self);
    
    //  Start broadcasting beacon to peers at the specified interval
    CZMQ_EXPORT void
        zbeacon_publish (zbeacon_t *self, byte *transmit, size_t size);
        
    //  Stop broadcasting beacons
    CZMQ_EXPORT void
        zbeacon_silence (zbeacon_t *self);
    
    //  Start listening to other peers; zero-sized filter means get everything
    CZMQ_EXPORT void
        zbeacon_subscribe (zbeacon_t *self, byte *filter, size_t size);
    
    //  Stop listening to other peers
    CZMQ_EXPORT void
        zbeacon_unsubscribe (zbeacon_t *self);
    
    //  Get beacon ZeroMQ socket, for polling or receiving messages
    CZMQ_EXPORT void *
        zbeacon_socket (zbeacon_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zbeacon_test (bool verbose);


<A name="toc4-160" title="zcert - work with CURVE security certificates" />
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
    
    //  Return copy of certificate
    CZMQ_EXPORT zcert_t *
        zcert_dup (zcert_t *self);
    
    //  Return true if two certificates have the same keys
    CZMQ_EXPORT bool
        zcert_eq (zcert_t *self, zcert_t *compare);
    
    //  Print certificate contents to open stream
    CZMQ_EXPORT void
        zcert_fprint (zcert_t *self, FILE *file);
    
    //  Print certificate contents to stdout
    CZMQ_EXPORT void
        zcert_print (zcert_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zcert_test (bool verbose);

Certificates are stored in the ZPL (ZMQ RFC 4) format. They have two
sections, "metadata" and "curve". The first contains a list of 'name =
value' pairs, one per line. Values may be enclosed in quotes. The curve
section has a 'public-key = keyvalue' and, for secret certificates, a
'secret-key = keyvalue' line. The keyvalue is a Z85-encoded CURVE key.

<A name="toc4-171" title="zcertstore - work with CURVE security certificate stores" />
#### zcertstore - work with CURVE security certificate stores

To authenticate new clients using the ZeroMQ CURVE security mechanism,
we have to check that the client's public key matches a key we know and
accept. There are numerous ways to store accepted client public keys. 
The mechanism CZMQ implements is "certificates" (plain text files) held 
in a "certificate store" (a disk directory). This class works with such
certificate stores, and lets you easily load them from disk, and check
if a given client public key is known or not. The zcert class does the
work of managing a single certificate.

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
    
    //  Print list of certificates in store to open stream
    CZMQ_EXPORT void
        zcertstore_fprint (zcertstore_t *self, FILE *file);
    
    //  Print list of certificates in store to stdout
    CZMQ_EXPORT void
        zcertstore_print (zcertstore_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zcertstore_test (bool verbose);

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

<A name="toc4-182" title="zchunk - work with memory chunks" />
#### zchunk - work with memory chunks

The zchunk class works with variable sized blobs. Not as efficient as
ØMQ's messages but they do less weirdness and so are easier to understand.
The chunk class has methods to read and write chunks from disk.

This is the class interface:

    //  Create new chunk
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
    
    //  Create copy of chunk, as new chunk object. Returns a fresh zchunk_t
    //  object, or NULL if there was not enough heap memory.
    CZMQ_EXPORT zchunk_t *
        zchunk_dup (zchunk_t *self);
    
    //  Dump chunk to FILE stream, for debugging and tracing.
    CZMQ_EXPORT void
        zchunk_fprint (zchunk_t *self, FILE *file);
    
    //  Dump message to stderr, for debugging and tracing.
    //  See zchunk_fprint for details
    CZMQ_EXPORT void
        zchunk_print (zchunk_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zchunk_test (bool verbose);


<A name="toc4-193" title="zclock - millisecond clocks and delays" />
#### zclock - millisecond clocks and delays

The zclock class provides essential sleep and system time functions, used
to slow down threads for testing, and calculate timers for polling. Wraps
the non-portable system calls in a simple portable API.

This is the class interface:

    //  Sleep for a number of milliseconds
    CZMQ_EXPORT void
        zclock_sleep (int msecs);
    
    //  Return current system clock as milliseconds
    CZMQ_EXPORT int64_t
        zclock_time (void);
    
    //  Print formatted string to stdout, prefixed by date/time and
    //  terminated with a newline.
    CZMQ_EXPORT void
        zclock_log (const char *format, ...);
    
    //  Return formatted date/time as fresh string. Free using zstr_free().
    CZMQ_EXPORT char *
        zclock_timestr (void);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zclock_test (bool verbose);

The Win32 Sleep() call defaults to 16ms resolution unless the system timer
resolution is increased with a call to timeBeginPeriod() permitting 1ms
granularity.

<A name="toc4-204" title="zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL." />
#### zconfig - work with config files written in rfc.zeromq.org/spec:4/ZPL.

Lets applications load, work with, and save configuration files.
This implements rfc.zeromq.org/spec:4/ZPL, which is a simple structured
text format for configuration files.

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
    
    //  Execute a callback for each config item in the tree
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
    
    //  Print the config file to open stream
    CZMQ_EXPORT void
        zconfig_fprint (zconfig_t *self, FILE *file);
    
    //  Print the config file to stdout
    CZMQ_EXPORT void
        zconfig_print (zconfig_t *self);
        
    //  Load a config tree from a memory chunk
    CZMQ_EXPORT zconfig_t *
        zconfig_chunk_load (zchunk_t *chunk);
    
    //  Save a config tree to a new memory chunk
    CZMQ_EXPORT zchunk_t *
        zconfig_chunk_save (zconfig_t *self);
    

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

<A name="toc4-215" title="zctx - working with ØMQ contexts" />
#### zctx - working with ØMQ contexts

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
    CZMQ_EXPORT int
        zctx_test (bool verbose);
    
    //  Global signal indicator, TRUE when user presses Ctrl-C or the process
    //  gets a SIGTERM signal.
    CZMQ_EXPORT extern volatile int zctx_interrupted;


<A name="toc4-226" title="zdir - work with file-system directories" />
#### zdir - work with file-system directories

The zdir class gives access to the file system index. It will load
a directory tree (a directory plus all child directories) into a
zdir structure and then let you navigate that structure. It exists
mainly to wrap non-portable OS functions to do this.

This is the class interface:

    //  Create a new directory item that loads in the full tree of the
    //  specified path, optionally located under some parent path.
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
    CZMQ_EXPORT int
        zdir_test (bool verbose);


<A name="toc4-237" title="zfile - work with files" />
#### zfile - work with files

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
    
    //  Duplicate a file item, returns a newly constructed item
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
    CZMQ_EXPORT int
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

This class is a new API, deprecating the old zfile class (which still
exists but is implemented in zsys now).

<A name="toc4-248" title="zframe - working with single message frames" />
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
        zframe_recv (void *socket);
    
    //  Receive a new frame off the socket. Returns newly allocated frame, or
    //  NULL if there was no input waiting, or if the read was interrupted.
    CZMQ_EXPORT zframe_t *
        zframe_recv_nowait (void *socket);
    
    // Send a frame to a socket, destroy frame after sending.
    // Return -1 on error, 0 on success.
    CZMQ_EXPORT int
        zframe_send (zframe_t **self_p, void *socket, int flags);
    
    //  Return number of bytes in frame data
    CZMQ_EXPORT size_t
        zframe_size (zframe_t *self);
    
    //  Return address of frame data
    CZMQ_EXPORT byte *
        zframe_data (zframe_t *self);
    
    //  Create a new frame that duplicates an existing frame
    CZMQ_EXPORT zframe_t *
        zframe_dup (zframe_t *self);
    
    //  Return frame data encoded as printable hex string
    CZMQ_EXPORT char *
        zframe_strhex (zframe_t *self);
    
    //  Return frame data copied into freshly allocated string
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
    
    //   Print contents of the frame to FILE stream.
    CZMQ_EXPORT void
        zframe_fprint (zframe_t *self, const char *prefix, FILE *file);
    
    //  Print contents of frame to stderr
    CZMQ_EXPORT void
        zframe_print (zframe_t *self, const char *prefix);
    
    //  Set new contents for frame
    CZMQ_EXPORT void
        zframe_reset (zframe_t *self, const void *data, size_t size);
    
    //  Put a block of data to the frame payload.
    CZMQ_EXPORT int
        zframe_put_block (zframe_t *self, byte *data, size_t size);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zframe_test (bool verbose);


<A name="toc4-259" title="zhash - generic hash table container" />
#### zhash - generic hash table container

Expandable hash table container

This is the class interface:

    //  Callback function for zhash_foreach method
    typedef int (zhash_foreach_fn) (const char *key, void *item, void *argument);
    //  Callback function for zhash_freefn method
    typedef void (zhash_free_fn) (void *data);
    
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
        zhash_freefn (zhash_t *self, const char *key, zhash_free_fn *free_fn);
    
    //  Return the number of keys/items in the hash table
    CZMQ_EXPORT size_t
        zhash_size (zhash_t *self);
    
    //  Make copy of hash table
    CZMQ_EXPORT zhash_t *
        zhash_dup (zhash_t *self);
    
    //  Return keys for items in table
    CZMQ_EXPORT zlist_t *
        zhash_keys (zhash_t *self);
        
    //  Apply function to each item in the hash table. Items are iterated in no
    //  defined order. Stops if callback function returns non-zero and returns
    //  final return code from callback function (zero = success).
    CZMQ_EXPORT int
        zhash_foreach (zhash_t *self, zhash_foreach_fn *callback, void *argument);
    
    //  Add comment to hash table before saving to disk. You can add as many
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
    
    //  Set hash for automatic value destruction
    CZMQ_EXPORT void
        zhash_autofree (zhash_t *self);
        
    //  Serialize hash table to a binary frame that can be sent in a message.
    //  The packed format is compatible with the 'dictionary' type defined in
    //  rfc.zeromq.org/spec:19/FILEMQ. Comments are not included in the packed
    //  data. Item values MUST be strings.
    CZMQ_EXPORT zframe_t *
        zhash_pack (zhash_t *self);
        
    //  Unpack binary frame into a new hash table. Packed data must follow format
    //  defined by zhash_pack. Hash table is set to autofree. An empty frame
    //  unpacks to an empty hash table.
    zhash_t *
        zhash_unpack (zframe_t *frame);
    
    //  Self test of this class
    CZMQ_EXPORT void
        zhash_test (int verbose);

Note that it's relatively slow (~50k insertions/deletes per second), so
don't do inserts/updates on the critical path for message I/O. It can
do ~2.5M lookups per second for 16-char keys. Timed on a 1.6GHz CPU.

<A name="toc4-270" title="zlist - generic list container" />
#### zlist - generic list container

Provides a generic container implementing a fast singly-linked list. You
can use this to construct multi-dimensional lists, and other structures
together with other generic containers like zhash.

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
    
    //  Return first item in the list, or null
    CZMQ_EXPORT void *
        zlist_first (zlist_t *self);
    
    //  Return last item in the list, or null
    CZMQ_EXPORT void *
        zlist_last (zlist_t *self);
    
    //  Return first item in the list, or null, leaves the cursor
    CZMQ_EXPORT void *
        zlist_head (zlist_t *self);
    
    //  Return last item in the list, or null, leaves the cursor
    CZMQ_EXPORT void *
        zlist_tail (zlist_t *self);
    
    //  Return next item in the list, or null
    CZMQ_EXPORT void *
        zlist_next (zlist_t *self);
    
    //  Append an item to the end of the list, return 0 if OK
    //  or -1 if this failed for some reason (out of memory).
    CZMQ_EXPORT int
        zlist_append (zlist_t *self, void *item);
    
    //  Push an item to the start of the list, return 0 if OK
    //  or -1 if this failed for some reason (out of memory).
    CZMQ_EXPORT int
        zlist_push (zlist_t *self, void *item);
    
    //  Pop the item off the start of the list, if any
    CZMQ_EXPORT void *
        zlist_pop (zlist_t *self);
    
    //  Remove the specified item from the list if present
    CZMQ_EXPORT void
        zlist_remove (zlist_t *self, void *item);
    
    // Add an explicit free function to the item including a hint as to
    // whether it can be found at the tail
    CZMQ_EXPORT void *
        zlist_freefn (zlist_t *self, void *item, zlist_free_fn *fn, bool at_tail);
    
    //  Make a copy of list. If the list has autofree set, the copied list will
    //  duplicate all items, which must be strings. Otherwise, the list will hold
    //  pointers back to the items in the original list.
    CZMQ_EXPORT zlist_t *
        zlist_dup (zlist_t *self);
    
    //  Return number of items in the list
    CZMQ_EXPORT size_t
        zlist_size (zlist_t *self);
    
    //  Sort the list by ascending key value using a straight ASCII comparison.
    //  The sort is not stable, so may reorder items with the same keys.
    CZMQ_EXPORT void
        zlist_sort (zlist_t *self, zlist_compare_fn *compare);
    
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
    
    //  Self test of this class
    CZMQ_EXPORT void
        zlist_test (int verbose);

To iterate through a list, use zlist_first to get the first item, then
loop while not null, and do zlist_next at the end of each iteration.

<A name="toc4-281" title="zloop - event-driven reactor" />
#### zloop - event-driven reactor

The zloop class provides an event-driven reactor pattern. The reactor
handles zmq_pollitem_t items (pollers or writers, sockets or fds), and
once-off or repeated timers. Its resolution is 1 msec. It uses a tickless
timer to reduce CPU interrupts in inactive processes.

This is the class interface:

    //  Callback function for reactor events
    typedef int (zloop_fn) (zloop_t *loop, zmq_pollitem_t *item, void *arg);
    
    // Callback for reactor timer events
    typedef int (zloop_timer_fn) (zloop_t *loop, int timer_id, void *arg);
    
    //  Create a new zloop reactor
    CZMQ_EXPORT zloop_t *
        zloop_new (void);
    
    //  Destroy a reactor
    CZMQ_EXPORT void
        zloop_destroy (zloop_t **self_p);
    
    //  Register pollitem with the reactor. When the pollitem is ready, will call
    //  the handler, passing the arg. Returns 0 if OK, -1 if there was an error.
    //  If you register the pollitem more than once, each instance will invoke its
    //  corresponding handler.
    CZMQ_EXPORT int
        zloop_poller (zloop_t *self, zmq_pollitem_t *item, zloop_fn handler, void *arg);
    
    //  Cancel a pollitem from the reactor, specified by socket or FD. If both
    //  are specified, uses only socket. If multiple poll items exist for same
    //  socket/FD, cancels ALL of them.
    CZMQ_EXPORT void
        zloop_poller_end (zloop_t *self, zmq_pollitem_t *item);
    
    //  Configure a registered pollitem to ignore errors. If you do not set this, 
    //  then pollitems that have errors are removed from the reactor silently.
    CZMQ_EXPORT void
        zloop_set_tolerant (zloop_t *self, zmq_pollitem_t *item);
    
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


<A name="toc4-292" title="zmonitor - socket event monitor" />
#### zmonitor - socket event monitor

The zmonitor class provides an API for obtaining socket events such as
connected, listen, disconnected, etc. Socket events are only available
for sockets connecting or bound to ipc:// and tcp:// endpoints.

This is the class interface:

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
    
    // Self test of this class
    CZMQ_EXPORT void
        zmonitor_test (bool verbose);
    

This class wraps the ZMQ socket monitor API, see zmq_socket_monitor for
details. Currently this class requires libzmq v4.0 or later and is empty
on older versions of libzmq.

<A name="toc4-303" title="zmsg - working with multipart messages" />
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
    //  was interrupted. Does a blocking recv, if you want to not block then use
    //  the zloop class or zmsg_recv_nowait() or zmq_poll to check for socket input before receiving.
    CZMQ_EXPORT zmsg_t *
        zmsg_recv (void *socket);
    
    //  Receive message from socket, returns zmsg_t object, or NULL either if there was
    //  no input waiting, or the recv was interrupted.
    CZMQ_EXPORT zmsg_t *
        zmsg_recv_nowait (void *socket);
    
    //  Send message to socket, destroy after sending. If the message has no
    //  frames, sends nothing but destroys the message anyhow. Safe to call
    //  if zmsg is null.
    CZMQ_EXPORT int
        zmsg_send (zmsg_t **self_p, void *socket);
    
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
    
    //  Pop frame off front of message, caller now owns frame
    //  If next frame is empty, pops and destroys that empty frame.
    CZMQ_EXPORT zframe_t *
        zmsg_unwrap (zmsg_t *self);
    
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
    //  object, or NULL if there was not enough heap memory.
    CZMQ_EXPORT zmsg_t *
        zmsg_dup (zmsg_t *self);
    
    //  Print message to open stream
    //  Truncates to first 10 frames, for readability.
    CZMQ_EXPORT void
        zmsg_fprint (zmsg_t *self, FILE *file);
    
    //  Print message to stdout
    CZMQ_EXPORT void
        zmsg_print (zmsg_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zmsg_test (bool verbose);


<A name="toc4-314" title="zmutex - wrap lightweight mutexes" />
#### zmutex - wrap lightweight mutexes

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
    CZMQ_EXPORT int
        zmutex_test (bool verbose);


<A name="toc4-325" title="zpoller - trivial socket poller class" />
#### zpoller - trivial socket poller class

The zpoller class provides a minimalist interface to ZeroMQ's zmq_poll
API, for the very common case of reading from a number of sockets.
It does not provide polling for output, nor polling on file handles.
If you need either of these, use the zmq_poll API directly.

This is the class interface:

    //  Create new poller
    CZMQ_EXPORT zpoller_t *
        zpoller_new (void *reader, ...);
    
    //  Destroy a poller
    CZMQ_EXPORT void
        zpoller_destroy (zpoller_t **self_p);
    
    //  Add a reader to be polled
    CZMQ_EXPORT int
        zpoller_add (zpoller_t *self, void *reader);
    
    //  Poll the registered readers for I/O, return first socket that has input.
    //  This means the order that sockets are defined in the poll list affects
    //  their priority. If you need a balanced poll, use the low level zmq_poll
    //  method directly. If the poll call was interrupted (SIGINT), or the ZMQ
    //  context was destroyed, or the timeout expired, returns NULL. You can
    //  test the actual exit condition by calling zpoller_expired () and
    //  zpoller_terminated (). Timeout is in msec.
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
    CZMQ_EXPORT int
        zpoller_test (bool verbose);


<A name="toc4-336" title="zproxy - convenient zmq_proxy API" />
#### zproxy - convenient zmq_proxy API

The zproxy class provides an equivalent to the ZMQ steerable proxy, on
all versions of ZeroMQ.

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
        zproxy_test (bool verbose);


<A name="toc4-347" title="zsocket - working with ØMQ sockets" />
#### zsocket - working with ØMQ sockets

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
        zsocket_destroy (zctx_t *self, void *socket);
    
    //  Bind a socket to a formatted endpoint. If the port is specified as
    //  '*', binds to any free port from ZSOCKET_DYNFROM to ZSOCKET_DYNTO
    //  and returns the actual port number used. Otherwise asserts that the
    //  bind succeeded with the specified port number. Always returns the
    //  port number if successful.
    CZMQ_EXPORT int
        zsocket_bind (void *socket, const char *format, ...);
    
    //  Unbind a socket from a formatted endpoint.
    //  Returns 0 if OK, -1 if the endpoint was invalid or the function
    //  isn't supported.
    CZMQ_EXPORT int
        zsocket_unbind (void *socket, const char *format, ...);
    
    //  Connect a socket to a formatted endpoint
    //  Returns 0 if OK, -1 if the endpoint was invalid.
    CZMQ_EXPORT int
        zsocket_connect (void *socket, const char *format, ...);
    
    //  Disconnect a socket from a formatted endpoint
    //  Returns 0 if OK, -1 if the endpoint was invalid or the function
    //  isn't supported.
    CZMQ_EXPORT int
        zsocket_disconnect (void *socket, const char *format, ...);
    
    //  Poll for input events on the socket. Returns TRUE if there is input
    //  ready on the socket, else FALSE.
    CZMQ_EXPORT bool
        zsocket_poll (void *socket, int msecs);
    
    //  Returns socket type as printable constant string
    CZMQ_EXPORT const char *
        zsocket_type_str (void *socket);
    
    //  Send data over a socket as a single message frame.
    //  Accepts these flags: ZFRAME_MORE and ZFRAME_DONTWAIT.
    CZMQ_EXPORT int
        zsocket_sendmem (void *socket, const void *data, size_t size, int flags);
    
    //  Send a signal over a socket. A signal is a zero-byte message.
    //  Signals are used primarily between threads, over pipe sockets.
    //  Returns -1 if there was an error sending the signal.
    CZMQ_EXPORT int
        zsocket_signal (void *socket);
    
    //  Wait on a signal. Use this to coordinate between threads, over
    //  pipe pairs. Returns -1 on error, 0 on success.
    CZMQ_EXPORT int
        zsocket_wait (void *socket);
    
    //  Send data over a socket as a single message frame.
    //  Returns -1 on error, 0 on success
    CZMQ_EXPORT int
        zsocket_sendmem (void *socket, const void *data, size_t size, int flags);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zsocket_test (bool verbose);


<A name="toc4-358" title="zsockopt - working with ØMQ socket options" />
#### zsockopt - working with ØMQ socket options

The zsockopt class provides access to the ØMQ getsockopt/setsockopt API.

This is the class interface:

    #if (ZMQ_VERSION_MAJOR == 4)
    //  Get socket options
    CZMQ_EXPORT int zsocket_tos (void *zocket);
    CZMQ_EXPORT int zsocket_plain_server (void *zocket);
    CZMQ_EXPORT char * zsocket_plain_username (void *zocket);
    CZMQ_EXPORT char * zsocket_plain_password (void *zocket);
    CZMQ_EXPORT int zsocket_curve_server (void *zocket);
    CZMQ_EXPORT char * zsocket_curve_publickey (void *zocket);
    CZMQ_EXPORT char * zsocket_curve_secretkey (void *zocket);
    CZMQ_EXPORT char * zsocket_curve_serverkey (void *zocket);
    CZMQ_EXPORT char * zsocket_zap_domain (void *zocket);
    CZMQ_EXPORT int zsocket_mechanism (void *zocket);
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
    CZMQ_EXPORT void zsocket_set_zap_domain (void *zocket, const char * zap_domain);
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
    
    //  Emulation of widely-used 2.x socket options
    CZMQ_EXPORT void zsocket_set_hwm (void *zocket, int hwm);
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
    
    //  Emulation of widely-used 2.x socket options
    CZMQ_EXPORT void zsocket_set_hwm (void *zocket, int hwm);
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

This class is generated, using the GSL code generator. See the sockopts
XML file, which provides the metadata, and the sockopts.gsl template,
which does the work.

<A name="toc4-369" title="zstr - sending and receiving strings" />
#### zstr - sending and receiving strings

The zstr class provides utility functions for sending and receiving C
strings across ØMQ sockets. It sends strings without a terminating null,
and appends a null byte on received strings. This class is for simple
message sending.

<center>
<img src="https://github.com/zeromq/czmq/raw/master/images/README_2.png" alt="2">
</center>

This is the class interface:

    //  Receive C string from socket. Caller must free returned string using
    //  zstr_free(). Returns NULL if the context is being terminated or the
    //  process was interrupted.
    CZMQ_EXPORT char *
        zstr_recv (void *socket);
    
    //  Receive C string from socket, if socket had input ready. Caller must
    //  free returned string using zstr_free. Returns NULL if there was no input
    //  waiting, or if the context was terminated. Use zctx_interrupted to exit
    //  any loop that relies on this method.
    CZMQ_EXPORT char *
        zstr_recv_nowait (void *socket);
    
    //  Send a C string to a socket, as a frame. The string is sent without
    //  trailing null byte; to read this you can use zstr_recv, or a similar
    //  method that adds a null terminator on the received string.
    CZMQ_EXPORT int
        zstr_send (void *socket, const char *string);
    
    //  Send a C string to a socket, as zstr_send(), with a MORE flag, so that
    //  you can send further strings in the same multi-part message.
    CZMQ_EXPORT int
        zstr_sendm (void *socket, const char *string);
    
    //  Send a formatted string to a socket. Note that you should NOT use
    //  user-supplied strings in the format (they may contain '%' which
    //  will create security holes).
    CZMQ_EXPORT int
        zstr_sendf (void *socket, const char *format, ...);
    
    //  Send a formatted string to a socket, as for zstr_sendf(), with a
    //  MORE flag, so that you can send further strings in the same multi-part
    //  message.
    CZMQ_EXPORT int
        zstr_sendfm (void *socket, const char *format, ...);
    
    //  Send a series of strings (until NULL) as multipart data
    //  Returns 0 if the strings could be sent OK, or -1 on error.
    CZMQ_EXPORT int
        zstr_sendx (void *socket, const char *string, ...);
    
    //  Receive a series of strings (until NULL) from multipart data.
    //  Each string is allocated and filled with string data; if there
    //  are not enough frames, unallocated strings are set to NULL.
    //  Returns -1 if the message could not be read, else returns the
    //  number of strings filled, zero or more. Free each returned string
    //  using zstr_free().
    CZMQ_EXPORT int
        zstr_recvx (void *socket, char **string_p, ...);
    
    //  Free a provided string, and nullify the parent pointer. Safe to call on
    //  a null pointer.
    CZMQ_EXPORT void
        zstr_free (char **string_p);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zstr_test (bool verbose);


<A name="toc4-394" title="zsys - system-level methods" />
#### zsys - system-level methods

The zsys class provides a portable wrapper for system calls. We collect
them here to reduce the number of weird #ifdefs in other classes. As far
as possible, the bulk of CZMQ classes are fully portable.

This is the class interface:

    #define UDP_FRAME_MAX   255         //  Max size of UDP frame
    
    //  Callback for interrupt signal handler
    typedef void (zsys_handler_fn) (int signal_value);
    
    //  Set interrupt handler (NULL means external handler)
    CZMQ_EXPORT void
        zsys_handler_set (zsys_handler_fn *handler_fn);
    
    //  Reset interrupt handler, call this at exit if needed
    CZMQ_EXPORT void
        zsys_handler_reset (void);
    
    //  Set network interface name to use for broadcasts
    //  Use this to force the interface for beacons
    //  This is experimental; may be merged into zbeacon class.
    CZMQ_EXPORT void
        zsys_set_interface (const char *interface_name);
    
    //  Return network interface name to use for broadcasts.
    //  Returns "" if no interface was set.
    //  This is experimental; may be merged into zbeacon class.
    CZMQ_EXPORT char *
        zsys_interface (void);
    
    //  Return 1 if file exists, else zero
    CZMQ_EXPORT bool
        zsys_file_exists (const char *filename);
    
    //  Return size of file, or -1 if not found
    CZMQ_EXPORT ssize_t
        zsys_file_size (const char *filename);
    
    //  Return file modification time. Returns 0 if the file does not exist.
    CZMQ_EXPORT time_t
        zsys_file_modified (const char *filename);
    
    //  Return file mode
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
    
    //  Return the czmq version for run-time API detection
    CZMQ_EXPORT void
        zsys_version (int *major, int *minor, int *patch);
    
    //  Format a string with variable arguments, returning a freshly allocated
    //  buffer. If there was insufficient memory, returns NULL. Free the returned
    //  string using zstr_free().
    CZMQ_EXPORT char *
        zsys_vprintf (const char *format, va_list argptr);
    
    //  Create UDP beacon socket; if the routable option is true, uses
    //  multicast (not yet implemented), else uses broadcast. This method
    //  and related ones might _eventually_ be moved to a zudp class.
    CZMQ_EXPORT SOCKET
        zsys_udp_new (bool routable);
    
    //  Send zframe to UDP socket
    CZMQ_EXPORT void
        zsys_udp_send (SOCKET udpsock, zframe_t *frame, inaddr_t *address);
    
    //  Receive zframe from UDP socket, and set address of peer that sent it
    //  The peername must be a char [INET_ADDRSTRLEN] array.
    CZMQ_EXPORT zframe_t *
        zsys_udp_recv (SOCKET udpsock, char *peername);
    
    //  Handle an I/O error on some socket operation; will report and die on
    //  fatal errors, and continue silently on "try again" errors.
    CZMQ_EXPORT void
        zsys_socket_error (const char *reason);
        
    //  Move the current process into the background. The precise effect depends
    //  on the operating system. On POSIX boxes, moves to a specified working
    //  directory (if specified), closes all file handles, reopens stdin, stdout,
    //  and stderr to the null device, and sets the process to ignore SIGHUP. On
    //  Windows, does nothing. Returns 0 if OK, -1 if there was an error.
    CZMQ_EXPORT int
        zsys_daemonize (const char *workdir);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zsys_test (bool verbose);


<A name="toc4-405" title="zrex - working with regular expressions" />
#### zrex - working with regular expressions

The zrex class provides a simple API for regular expressions, wrapping
Alberto Demichelis's T-Rex library from http://tiny-rex.sourceforge.net/.

This is the class interface:

    //  Constructor. Optionally, sets an expression against which we can match
    //  text and capture hits. If there is an error in the expression, reports
    //  zrex_valid() as false and provides the error in zrex_strerror(). If you
    //  set a pattern, you can call zrex_hits() to test it against text.
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
    
    //  Matches the text against a previously set expression, and reports the
    //  number of hits (aka "capture groups" in e.g. Perl). If the text does
    //  not match, returns 0. If it matches, returns 1 or greater, depending on
    //  how many "(...)" groups the expression has. An expression with one group
    //  will produce 2 hits, one for the whole expression and one for the group.
    //  To retrieve the individual hits, call zrex_hit ().
    CZMQ_EXPORT int
        zrex_hits (zrex_t *self, const char *text);
    
    //  Matches the text against a new expression, and reports the number of
    //  hits. If the text does not match, returns 0. If it matches, returns 1 or
    //  greater, depending on how many "(...)" groups the expression has. An
    //  expression with one group will produce 2 hits, one for the whole
    //  expression and one for the group. To retrieve the individual hits, call
    //  zrex_hit ().
    CZMQ_EXPORT int
        zrex_eq (zrex_t *self, const char *text, const char *expression);
    
    //  Returns the Nth sequence captured from the last expression match, where
    //  N is 0 to the value returned by zrex_hits() or zrex_eq(). Sequence 0
    //  is always the whole matching string. Sequence 1 is the first capture
    //  group, if any, and so on.
    CZMQ_EXPORT const char *
        zrex_hit (zrex_t *self, uint index);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zrex_test (bool verbose);

The underlying TRex class implements the following expressions:

\   Quote the next metacharacter
^   Match the beginning of the string
.   Match any character
$   Match the end of the string
|   Alternation
()  Grouping (captures a 'hit')
[]  Character class

==GREEDY CLOSURES==
*      Match 0 or more times
+      Match 1 or more times
?      Match 1 or 0 times
{n}    Match exactly n times
{n,}   Match at least n times
{n,m}  Match at least n but not more than m times

==ESCAPE CHARACTERS==
\t      tab                   (HT, TAB)
\n      newline               (LF, NL)
\r      return                (CR)
\f      form feed             (FF)

==PREDEFINED CLASSES==
\l      lowercase next char
\u      uppercase next char
\a      alpha [a-zA-Z]
\w      alphanumeric [0-9a-zA-Z]
\s      space characters
\d      decimal digits
\x      hexadecimal digits
\c      control characters
\p      punctuation
\b      word boundary
\A      non letters
\W      non alphanumeric
\S      non space
\D      non decimal digits
\X      non hexadecimal digits
\C      non control characters
\P      non punctuation
\B      non word boundary

<A name="toc4-416" title="zthread - working with system threads" />
#### zthread - working with system threads

The zthread class wraps OS thread creation. It creates detached threads
that look like normal OS threads, or attached threads that share the
caller's ØMQ context, and get an inproc pipe to talk back to the parent
thread. Detached threads create their own ØMQ contexts as needed.

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
    CZMQ_EXPORT int
        zthread_test (bool verbose);

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

<A name="toc4-427" title="zuuid - UUID management" />
#### zuuid - UUID management

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
    
    //  Set UUID to new supplied value 
    CZMQ_EXPORT void
        zuuid_set (zuuid_t *self, byte *source);
        
    //  Store UUID blob in target array
    CZMQ_EXPORT void
        zuuid_export (zuuid_t *self, byte *target);
    
    //  Check if UUID is same as supplied value
    CZMQ_EXPORT bool
        zuuid_eq (zuuid_t *self, byte *compare);
    
    //  Check if UUID is different from supplied value
    CZMQ_EXPORT bool
        zuuid_neq (zuuid_t *self, byte *compare);
    
    //  Make copy of UUID object
    CZMQ_EXPORT zuuid_t *
        zuuid_dup (zuuid_t *self);
    
    //  Self test of this class
    CZMQ_EXPORT int
        zuuid_test (bool verbose);


<A name="toc2-438" title="Under the Hood" />
## Under the Hood

<A name="toc3-441" title="Adding a New Class" />
### Adding a New Class

If you define a new CZMQ class `myclass` you need to:

* Write the `zmyclass.c` and `zmyclass.h` source files, in `src` and `include` respectively.
* Add`#include <zmyclass.h>` to `include/czmq.h`.
* Add the myclass header and test call to `src/czmq_selftest.c`.
* Add a reference documentation to 'doc/zmyclass.txt'.
* Add myclass to 'model/projects.xml` and read model/README.txt.
* Add a section to README.txt.

<A name="toc3-453" title="Documentation" />
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

<A name="toc3-492" title="Development" />
### Development

CZMQ is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

<A name="toc3-502" title="Porting CZMQ" />
### Porting CZMQ

When you try CZMQ on an OS that it's not been used on (ever, or for a while), you will hit code that does not compile. In some cases the patches are trivial, in other cases (usually when porting to Windows), the work needed to build equivalent functionality may be non-trivial. In any case, the benefit is that once ported, the functionality is available to all applications.

Before attempting to patch code for portability, please read the `czmq_prelude.h` header file. There are several typical types of changes you may need to make to get functionality working on a specific operating system:

* Defining typedefs which are missing on that specific compiler: do this in czmq_prelude.h.
* Defining macros that rename exotic library functions to more conventional names: do this in czmq_prelude.h.
* Reimplementing specific methods to use a non-standard API: this is typically needed on Windows. Do this in the relevant class, using #ifdefs to properly differentiate code for different platforms.

<A name="toc3-513" title="Code Generation" />
### Code Generation

We generate the zsockopt class using [https://github.com/imatix/gsl GSL], using a code generator script in scripts/sockopts.gsl.

<A name="toc3-518" title="This Document" />
### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
