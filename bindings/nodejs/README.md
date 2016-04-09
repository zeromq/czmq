# Node.js Binding for CZMQ

This is a development kit. Note: this README is generated automatically
by zproject from project.xml. Please DO NOT modify by hand except for test
purposes.

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
npm install -g nan
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

### The Zarmour class - armoured text encoding and decoding

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zarmour = new czmq.Zarmour ()
```

You *must* call the destructor on every Zarmour instance:

```
my_zarmour.destroy ()
```

Methods:

```
string my_zarmour.encode (String)
```

Encode a stream of bytes into an armoured string. Returns the armoured
string, or NULL if there was insufficient memory available to allocate
a new string.

```
zchunk my_zarmour.decode (String)
```

Decode an armoured string into a chunk. The decoded output is
null-terminated, so it may be treated as a string, if that's what
it was prior to encoding.

```
integer my_zarmour.mode ()
```

Get the mode property.

```
string my_zarmour.modeStr ()
```

Get printable string for mode.

```
boolean my_zarmour.pad ()
```

Return true if padding is turned on.

```
char my_zarmour.padChar ()
```

Get the padding character.

```
boolean my_zarmour.lineBreaks ()
```

Return if splitting output into lines is turned on. Default is off.

```
size my_zarmour.lineLength ()
```

Get the line length used for splitting lines.

### The Zcert class - work with CURVE security certificates

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zcert = new czmq.Zcert ()
```

You *must* call the destructor on every Zcert instance:

```
my_zcert.destroy ()
```

Methods:

```
buffer my_zcert.publicKey ()
```

Return public part of key pair as 32-byte binary string

```
buffer my_zcert.secretKey ()
```

Return secret part of key pair as 32-byte binary string

```
string my_zcert.publicTxt ()
```

Return public part of key pair as Z85 armored string

```
string my_zcert.secretTxt ()
```

Return secret part of key pair as Z85 armored string

```
string my_zcert.meta (String)
```

Get metadata value from certificate; if the metadata value doesn't
exist, returns NULL.

```
zlist my_zcert.metaKeys ()
```

Get list of metadata fields from certificate. Caller is responsible for
destroying list. Caller should not modify the values of list items.

```
integer my_zcert.save (String)
```

Save full certificate (public + secret) to file for persistent storage
This creates one public file and one secret file (filename + "_secret").

```
integer my_zcert.savePublic (String)
```

Save public certificate only to file for persistent storage

```
integer my_zcert.saveSecret (String)
```

Save secret certificate only to file for persistent storage

```
zcert my_zcert.dup ()
```

Return copy of certificate; if certificate is NULL or we exhausted
heap memory, returns NULL.

```
boolean my_zcert.eq (Zcert)
```

Return true if two certificates have the same keys

### The Zcertstore class - work with CURVE security certificate stores

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zcertstore = new czmq.Zcertstore (String)
```

You *must* call the destructor on every Zcertstore instance:

```
my_zcertstore.destroy ()
```

Methods:

```
zcert my_zcertstore.lookup (String)
```

Look up certificate by public key, returns zcert_t object if found,
else returns NULL. The public key is provided in Z85 text format.

### The Zchunk class - work with memory chunks

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zchunk = new czmq.Zchunk (String)
```

You *must* call the destructor on every Zchunk instance:

```
my_zchunk.destroy ()
```

Methods:

```
size my_zchunk.size ()
```

Return chunk cur size

```
size my_zchunk.maxSize ()
```

Return chunk max size

```
buffer my_zchunk.data ()
```

Return chunk data

```
size my_zchunk.set (String)
```

Set chunk data from user-supplied data; truncate if too large. Data may
be null. Returns actual size of chunk

```
size my_zchunk.append (String)
```

Append user-supplied data to chunk, return resulting chunk size. If the
data would exceeded the available space, it is truncated. If you want to
grow the chunk to accommodate new data, use the zchunk_extend method.

```
size my_zchunk.extend (String)
```

Append user-supplied data to chunk, return resulting chunk size. If the
data would exceeded the available space, the chunk grows in size.

```
size my_zchunk.consume (Zchunk)
```

Copy as much data from 'source' into the chunk as possible; returns the
new size of chunk. If all data from 'source' is used, returns exhausted
on the source chunk. Source can be consumed as many times as needed until
it is exhausted. If source was already exhausted, does not change chunk.

```
boolean my_zchunk.exhausted ()
```

Returns true if the chunk was exhausted by consume methods, or if the
chunk has a size of zero.

```
zchunk my_zchunk.slurp (String)
```

Try to slurp an entire file into a chunk. Will read up to maxsize of
the file. If maxsize is 0, will attempt to read the entire file and
fail with an assertion if that cannot fit into memory. Returns a new
chunk containing the file data, or NULL if the file could not be read.

```
zchunk my_zchunk.dup ()
```

Create copy of chunk, as new chunk object. Returns a fresh zchunk_t
object, or null if there was not enough heap memory. If chunk is null,
returns null.

```
string my_zchunk.strhex ()
```

Return chunk data encoded as printable hex string. Caller must free
string when finished with it.

```
string my_zchunk.strdup ()
```

Return chunk data copied into freshly allocated string
Caller must free string when finished with it.

```
boolean my_zchunk.streq (String)
```

Return TRUE if chunk body is equal to string, excluding terminator

```
zframe my_zchunk.pack ()
```

Transform zchunk into a zframe that can be sent in a message.

```
zchunk my_zchunk.unpack (Zframe)
```

Transform a zframe into a zchunk.

```
string my_zchunk.digest ()
```

Calculate SHA1 digest for chunk, using zdigest class.

### The Zclock class - millisecond clocks and delays

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zclock = new czmq.Zclock ()
```

Methods:

```
msecs my_zclock.time ()
```

Return current system clock as milliseconds. Note that this clock can
jump backwards (if the system clock is changed) so is unsafe to use for
timers and time offsets. Use zclock_mono for that instead.

```
msecs my_zclock.mono ()
```

Return current monotonic clock in milliseconds. Use this when you compute
time offsets. The monotonic clock is not affected by system changes and
so will never be reset backwards, unlike a system clock.

```
msecs my_zclock.usecs ()
```

Return current monotonic clock in microseconds. Use this when you compute
time offsets. The monotonic clock is not affected by system changes and
so will never be reset backwards, unlike a system clock.

```
string my_zclock.timestr ()
```

Return formatted date/time as fresh string. Free using zstr_free().

### The Zconfig class - work with config files written in rfc.zeromq.org/spec:4/ZPL.

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zconfig = new czmq.Zconfig (String, Zconfig)
```

You *must* call the destructor on every Zconfig instance:

```
my_zconfig.destroy ()
```

Methods:

```
string my_zconfig.name ()
```

Return name of config item

```
string my_zconfig.value ()
```

Return value of config item

```
string my_zconfig.get (String, String)
```

Get value for config item into a string value; leading slash is optional
and ignored.

```
zconfig my_zconfig.child ()
```

Find our first child, if any

```
zconfig my_zconfig.next ()
```

Find our first sibling, if any

```
zconfig my_zconfig.locate (String)
```

Find a config item along a path; leading slash is optional and ignored.

```
zconfig my_zconfig.atDepth (Number)
```

Locate the last config item at a specified depth

```
zlist my_zconfig.comments ()
```

Return comments of config item, as zlist.

```
integer my_zconfig.save (String)
```

Save a config tree to a specified ZPL text file, where a filename
"-" means dump to standard output.

```
integer my_zconfig.savef (String)
```

Equivalent to zconfig_save, taking a format string instead of a fixed
filename.

```
string my_zconfig.filename ()
```

Report filename used during zconfig_load, or NULL if none

```
integer my_zconfig.reload (Zconfig)
```

Reload config tree from same file that it was previously loaded from.
Returns 0 if OK, -1 if there was an error (and then does not change
existing data).

```
zconfig my_zconfig.chunkLoad (Zchunk)
```

Load a config tree from a memory chunk

```
zchunk my_zconfig.chunkSave ()
```

Save a config tree to a new memory chunk

```
zconfig my_zconfig.strLoad (String)
```

Load a config tree from a null-terminated string

```
string my_zconfig.strSave ()
```

Save a config tree to a new null terminated string

```
boolean my_zconfig.hasChanged ()
```

Return true if a configuration tree was loaded from a file and that
file has changed in since the tree was loaded.

### The Zdigest class - provides hashing functions (SHA-1 at present)

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zdigest = new czmq.Zdigest ()
```

You *must* call the destructor on every Zdigest instance:

```
my_zdigest.destroy ()
```

Methods:

```
buffer my_zdigest.data ()
```

Return final digest hash data. If built without crypto support,
returns NULL.

```
size my_zdigest.size ()
```

Return final digest hash size

```
string my_zdigest.string ()
```

Return digest as printable hex string; caller should not modify nor
free this string. After calling this, you may not use zdigest_update()
on the same digest. If built without crypto support, returns NULL.

### The Zdir class - work with file-system directories

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zdir = new czmq.Zdir (String, String)
```

You *must* call the destructor on every Zdir instance:

```
my_zdir.destroy ()
```

Methods:

```
string my_zdir.path ()
```

Return directory path

```
time my_zdir.modified ()
```

Return last modification time for directory.

```
file_size my_zdir.cursize ()
```

Return total hierarchy size, in bytes of data contained in all files
in the directory tree.

```
size my_zdir.count ()
```

Return directory count

```
zlist my_zdir.list ()
```

Returns a sorted list of zfile objects; Each entry in the list is a pointer
to a zfile_t item already allocated in the zdir tree. Do not destroy the
original zdir tree until you are done with this list.

```
zlist my_zdir.diff (Zdir, Zdir, String)
```

Calculate differences between two versions of a directory tree.
Returns a list of zdir_patch_t patches. Either older or newer may
be null, indicating the directory is empty/absent. If alias is set,
generates virtual filename (minus path, plus alias).

```
zlist my_zdir.resync (String)
```

Return full contents of directory as a zdir_patch list.

```
zhash my_zdir.cache ()
```

Load directory cache; returns a hash table containing the SHA-1 digests
of every file in the tree. The cache is saved between runs in .cache.

### The ZdirPatch class - work with directory patches

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zdir_patch = new czmq.ZdirPatch (String, Zfile, Number, String)
```

You *must* call the destructor on every ZdirPatch instance:

```
my_zdir_patch.destroy ()
```

Methods:

```
zdir_patch my_zdir_patch.dup ()
```

Create copy of a patch. If the patch is null, or memory was exhausted,
returns null.

```
string my_zdir_patch.path ()
```

Return patch file directory path

```
zfile my_zdir_patch.file ()
```

Return patch file item

```
integer my_zdir_patch.op ()
```

Return operation

```
string my_zdir_patch.vpath ()
```

Return patch virtual file path

```
string my_zdir_patch.digest ()
```

Return hash digest for patch file

### The Zfile class - helper functions for working with files.

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zfile = new czmq.Zfile (String, String)
```

You *must* call the destructor on every Zfile instance:

```
my_zfile.destroy ()
```

Methods:

```
zfile my_zfile.dup ()
```

Duplicate a file item, returns a newly constructed item. If the file
is null, or memory was exhausted, returns null.

```
string my_zfile.filename (String)
```

Return file name, remove path if provided

```
time my_zfile.modified ()
```

Return when the file was last modified. If you want this to reflect the
current situation, call zfile_restat before checking this property.

```
file_size my_zfile.cursize ()
```

Return the last-known size of the file. If you want this to reflect the
current situation, call zfile_restat before checking this property.

```
boolean my_zfile.isDirectory ()
```

Return true if the file is a directory. If you want this to reflect
any external changes, call zfile_restat before checking this property.

```
boolean my_zfile.isRegular ()
```

Return true if the file is a regular file. If you want this to reflect
any external changes, call zfile_restat before checking this property.

```
boolean my_zfile.isReadable ()
```

Return true if the file is readable by this process. If you want this to
reflect any external changes, call zfile_restat before checking this
property.

```
boolean my_zfile.isWriteable ()
```

Return true if the file is writeable by this process. If you want this
to reflect any external changes, call zfile_restat before checking this
property.

```
boolean my_zfile.isStable ()
```

Check if file has stopped changing and can be safely processed.
Updates the file statistics from disk at every call.

```
boolean my_zfile.hasChanged ()
```

Return true if the file was changed on disk since the zfile_t object
was created, or the last zfile_restat() call made on it.

```
integer my_zfile.input ()
```

Open file for reading
Returns 0 if OK, -1 if not found or not accessible

```
integer my_zfile.output ()
```

Open file for writing, creating directory if needed
File is created if necessary; chunks can be written to file at any
location. Returns 0 if OK, -1 if error.

```
zchunk my_zfile.read (, Number)
```

Read chunk from file at specified position. If this was the last chunk,
sets the eof property. Returns a null chunk in case of error.

```
boolean my_zfile.eof ()
```

Returns true if zfile_read() just read the last chunk in the file.

```
integer my_zfile.write (Zchunk, Number)
```

Write chunk to file at specified position
Return 0 if OK, else -1

```
string my_zfile.readln ()
```

Read next line of text from file. Returns a pointer to the text line,
or NULL if there was nothing more to read from the file.

```
string my_zfile.digest ()
```

Calculate SHA1 digest for file, using zdigest class.

### The Zframe class - working with single message frames

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zframe = new czmq.Zframe (String)
```

You *must* call the destructor on every Zframe instance:

```
my_zframe.destroy ()
```

Methods:

```
integer my_zframe.send (Zframe, Zsock, Number)
```

Send a frame to a socket, destroy frame after sending.
Return -1 on error, 0 on success.

```
size my_zframe.size ()
```

Return number of bytes in frame data

```
buffer my_zframe.data ()
```

Return address of frame data

```
zframe my_zframe.dup ()
```

Create a new frame that duplicates an existing frame. If frame is null,
or memory was exhausted, returns null.

```
string my_zframe.strhex ()
```

Return frame data encoded as printable hex string, useful for 0MQ UUIDs.
Caller must free string when finished with it.

```
string my_zframe.strdup ()
```

Return frame data copied into freshly allocated string
Caller must free string when finished with it.

```
boolean my_zframe.streq (String)
```

Return TRUE if frame body is equal to string, excluding terminator

```
integer my_zframe.more ()
```

Return frame MORE indicator (1 or 0), set when reading frame from socket
or by the zframe_set_more() method

```
number my_zframe.routingId ()
```

Return frame routing ID, if the frame came from a ZMQ_SERVER socket.
Else returns zero.

```
string my_zframe.group ()
```

Return frame group of radio-dish pattern.

```
integer my_zframe.setGroup (String)
```

Set group on frame. This is used if/when the frame is sent to a
ZMQ_RADIO socket.
Return -1 on error, 0 on success.

```
boolean my_zframe.eq (Zframe)
```

Return TRUE if two frames have identical size and data
If either frame is NULL, equality is always false.

### The Zhash class - generic type-free hash container (simple)

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zhash = new czmq.Zhash ()
```

You *must* call the destructor on every Zhash instance:

```
my_zhash.destroy ()
```

Methods:

```
integer my_zhash.rename (String, String)
```

Reindexes an item from an old key to a new key. If there was no such
item, does nothing. Returns 0 if successful, else -1.

```
size my_zhash.size ()
```

Return the number of keys/items in the hash table

```
zhash my_zhash.dup ()
```

Make copy of hash table; if supplied table is null, returns null.
Does not copy items themselves. Rebuilds new table so may be slow on
very large tables. NOTE: only works with item values that are strings
since there's no other way to know how to duplicate the item value.

```
zlist my_zhash.keys ()
```

Return keys for items in table

```
string my_zhash.cursor ()
```

After a successful first/next method, returns the key for the item that
was returned. This is a constant string that you may not modify or
deallocate, and which lasts as long as the item in the hash. After an
unsuccessful first/next, returns NULL.

```
zframe my_zhash.pack ()
```

Serialize hash table to a binary frame that can be sent in a message.
The packed format is compatible with the 'dictionary' type defined in
http://rfc.zeromq.org/spec:35/FILEMQ, and implemented by zproto:

   ; A list of name/value pairs
   dictionary      = dict-count *( dict-name dict-value )
   dict-count      = number-4
   dict-value      = longstr
   dict-name       = string

   ; Strings are always length + text contents
   longstr         = number-4 *VCHAR
   string          = number-1 *VCHAR

   ; Numbers are unsigned integers in network byte order
   number-1        = 1OCTET
   number-4        = 4OCTET

Comments are not included in the packed data. Item values MUST be
strings.

```
integer my_zhash.save (String)
```

Save hash table to a text file in name=value format. Hash values must be
printable strings; keys may not contain '=' character. Returns 0 if OK,
else -1 if a file error occurred.

```
integer my_zhash.load (String)
```

Load hash table from a text file in name=value format; hash table must
already exist. Hash values must printable strings; keys may not contain
'=' character. Returns 0 if OK, else -1 if a file was not readable.

```
integer my_zhash.refresh ()
```

When a hash table was loaded from a file by zhash_load, this method will
reload the file if it has been modified since, and is "stable", i.e. not
still changing. Returns 0 if OK, -1 if there was an error reloading the 
file.

### The Zhashx class - extended generic type-free hash container

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zhashx = new czmq.Zhashx ()
```

You *must* call the destructor on every Zhashx instance:

```
my_zhashx.destroy ()
```

Methods:

```
size my_zhashx.size ()
```

Return the number of keys/items in the hash table

```
zlistx my_zhashx.keys ()
```

Return a zlistx_t containing the keys for the items in the
table. Uses the key_duplicator to duplicate all keys and sets the
key_destructor as destructor for the list.

```
zlistx my_zhashx.values ()
```

Return a zlistx_t containing the values for the items in the
table. Uses the duplicator to duplicate all items and sets the
destructor as destructor for the list.

```
integer my_zhashx.save (String)
```

Save hash table to a text file in name=value format. Hash values must be
printable strings; keys may not contain '=' character. Returns 0 if OK,
else -1 if a file error occurred.

```
integer my_zhashx.load (String)
```

Load hash table from a text file in name=value format; hash table must
already exist. Hash values must printable strings; keys may not contain
'=' character. Returns 0 if OK, else -1 if a file was not readable.

```
integer my_zhashx.refresh ()
```

When a hash table was loaded from a file by zhashx_load, this method will
reload the file if it has been modified since, and is "stable", i.e. not
still changing. Returns 0 if OK, -1 if there was an error reloading the 
file.

```
zframe my_zhashx.pack ()
```

Serialize hash table to a binary frame that can be sent in a message.
The packed format is compatible with the 'dictionary' type defined in
http://rfc.zeromq.org/spec:35/FILEMQ, and implemented by zproto:

   ; A list of name/value pairs
   dictionary      = dict-count *( dict-name dict-value )
   dict-count      = number-4
   dict-value      = longstr
   dict-name       = string

   ; Strings are always length + text contents
   longstr         = number-4 *VCHAR
   string          = number-1 *VCHAR

   ; Numbers are unsigned integers in network byte order
   number-1        = 1OCTET
   number-4        = 4OCTET

Comments are not included in the packed data. Item values MUST be
strings.

```
zhashx my_zhashx.dup ()
```

Make a copy of the list; items are duplicated if you set a duplicator
for the list, otherwise not. Copying a null reference returns a null
reference. Note that this method's behavior changed slightly for CZMQ
v3.x, as it does not set nor respect autofree. It does however let you
duplicate any hash table safely. The old behavior is in zhashx_dup_v2.

```
zhashx my_zhashx.dupV2 ()
```

Make copy of hash table; if supplied table is null, returns null.
Does not copy items themselves. Rebuilds new table so may be slow on
very large tables. NOTE: only works with item values that are strings
since there's no other way to know how to duplicate the item value.

### The Ziflist class - List of network interfaces available on system

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_ziflist = new czmq.Ziflist ()
```

You *must* call the destructor on every Ziflist instance:

```
my_ziflist.destroy ()
```

Methods:

```
size my_ziflist.size ()
```

Return the number of network interfaces on system

```
string my_ziflist.first ()
```

Get first network interface, return NULL if there are none

```
string my_ziflist.next ()
```

Get next network interface, return NULL if we hit the last one

```
string my_ziflist.address ()
```

Return the current interface IP address as a printable string

```
string my_ziflist.broadcast ()
```

Return the current interface broadcast address as a printable string

```
string my_ziflist.netmask ()
```

Return the current interface network mask as a printable string

### The Zlist class - simple generic list container

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zlist = new czmq.Zlist ()
```

You *must* call the destructor on every Zlist instance:

```
my_zlist.destroy ()
```

Methods:

```
zlist my_zlist.dup ()
```

Make a copy of list. If the list has autofree set, the copied list will
duplicate all items, which must be strings. Otherwise, the list will hold
pointers back to the items in the original list. If list is null, returns
NULL.

```
size my_zlist.size ()
```

Return number of items in the list

### The Zlistx class - extended generic list container

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zlistx = new czmq.Zlistx ()
```

You *must* call the destructor on every Zlistx instance:

```
my_zlistx.destroy ()
```

Methods:

```
size my_zlistx.size ()
```

Return the number of items in the list

```
zlistx my_zlistx.dup ()
```

Make a copy of the list; items are duplicated if you set a duplicator
for the list, otherwise not. Copying a null reference returns a null
reference.

### The Zloop class - event-driven reactor

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zloop = new czmq.Zloop ()
```

You *must* call the destructor on every Zloop instance:

```
my_zloop.destroy ()
```

Methods:

```
integer my_zloop.timerEnd (Number)
```

Cancel a specific timer identified by a specific timer_id (as returned by
zloop_timer).

```
integer my_zloop.start ()
```

Start the reactor. Takes control of the thread and returns when the 0MQ
context is terminated or the process is interrupted, or any event handler
returns -1. Event handlers may register new sockets and timers, and
cancel sockets. Returns 0 if interrupted, -1 if canceled by a handler.

### The Zmsg class - working with multipart messages

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zmsg = new czmq.Zmsg ()
```

You *must* call the destructor on every Zmsg instance:

```
my_zmsg.destroy ()
```

Methods:

```
integer my_zmsg.send (Zmsg, Zsock)
```

Send message to destination socket, and destroy the message after sending
it successfully. If the message has no frames, sends nothing but destroys
the message anyhow. Nullifies the caller's reference to the message (as
it is a destructor).

```
integer my_zmsg.sendm (Zmsg, Zsock)
```

Send message to destination socket as part of a multipart sequence, and
destroy the message after sending it successfully. Note that after a
zmsg_sendm, you must call zmsg_send or another method that sends a final
message part. If the message has no frames, sends nothing but destroys
the message anyhow. Nullifies the caller's reference to the message (as
it is a destructor).

```
size my_zmsg.size ()
```

Return size of message, i.e. number of frames (0 or more).

```
size my_zmsg.contentSize ()
```

Return total size of all frames in message.

```
number my_zmsg.routingId ()
```

Return message routing ID, if the message came from a ZMQ_SERVER socket.
Else returns zero.

```
integer my_zmsg.prepend (Zframe)
```

Push frame to the front of the message, i.e. before all other frames.
Message takes ownership of frame, will destroy it when message is sent.
Returns 0 on success, -1 on error. Deprecates zmsg_push, which did not
nullify the caller's frame reference.

```
integer my_zmsg.append (Zframe)
```

Add frame to the end of the message, i.e. after all other frames.
Message takes ownership of frame, will destroy it when message is sent.
Returns 0 on success. Deprecates zmsg_add, which did not nullify the
caller's frame reference.

```
zframe my_zmsg.pop ()
```

Remove first frame from message, if any. Returns frame, or NULL.

```
integer my_zmsg.pushmem (String)
```

Push block of memory to front of message, as a new frame.
Returns 0 on success, -1 on error.

```
integer my_zmsg.addmem (String)
```

Add block of memory to the end of the message, as a new frame.
Returns 0 on success, -1 on error.

```
integer my_zmsg.pushstr (String)
```

Push string as new frame to front of message.
Returns 0 on success, -1 on error.

```
integer my_zmsg.addstr (String)
```

Push string as new frame to end of message.
Returns 0 on success, -1 on error.

```
integer my_zmsg.pushstrf (String)
```

Push formatted string as new frame to front of message.
Returns 0 on success, -1 on error.

```
integer my_zmsg.addstrf (String)
```

Push formatted string as new frame to end of message.
Returns 0 on success, -1 on error.

```
string my_zmsg.popstr ()
```

Pop frame off front of message, return as fresh string. If there were
no more frames in the message, returns NULL.

```
integer my_zmsg.addmsg (Zmsg)
```

Push encoded message as a new frame. Message takes ownership of
submessage, so the original is destroyed in this call. Returns 0 on
success, -1 on error.

```
zmsg my_zmsg.popmsg ()
```

Remove first submessage from message, if any. Returns zmsg_t, or NULL if
decoding was not successful.

```
zframe my_zmsg.first ()
```

Set cursor to first frame in message. Returns frame, or NULL, if the
message is empty. Use this to navigate the frames as a list.

```
zframe my_zmsg.next ()
```

Return the next frame. If there are no more frames, returns NULL. To move
to the first frame call zmsg_first(). Advances the cursor.

```
zframe my_zmsg.last ()
```

Return the last frame. If there are no frames, returns NULL.

```
zframe my_zmsg.encode ()
```

Serialize multipart message to a single message frame. Use this method
to send structured messages across transports that do not support
multipart data. Allocates and returns a new frame containing the
serialized message. To decode a serialized message frame, use
zmsg_decode ().

```
zmsg my_zmsg.dup ()
```

Create copy of message, as new message object. Returns a fresh zmsg_t
object. If message is null, or memory was exhausted, returns null.

```
boolean my_zmsg.eq (Zmsg)
```

Return true if the two messages have the same number of frames and each
frame in the first message is identical to the corresponding frame in the
other message. As with zframe_eq, return false if either message is NULL.

```
integer my_zmsg.signal ()
```

Return signal value, 0 or greater, if message is a signal, -1 if not.

### The Zpoller class - event-driven reactor

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zpoller = new czmq.Zpoller (Zsock)
```

You *must* call the destructor on every Zpoller instance:

```
my_zpoller.destroy ()
```

Methods:

```
integer my_zpoller.add (Zsock)
```

Add a reader to be polled. Returns 0 if OK, -1 on failure. The reader may
be a libzmq void * socket, a zsock_t instance, or a zactor_t instance.

```
boolean my_zpoller.expired ()
```

Return true if the last zpoller_wait () call ended because the timeout
expired, without any error.

```
boolean my_zpoller.terminated ()
```

Return true if the last zpoller_wait () call ended because the process
was interrupted, or the parent context was destroyed.

### The Zproc class - process configuration and status

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zproc = new czmq.Zproc ()
```

Methods:

```
integer my_zproc.czmqVersion ()
```

Returns CZMQ version as a single 6-digit integer encoding the major
version (x 10000), the minor version (x 100) and the patch.

```
boolean my_zproc.interrupted ()
```

Returns true if the process received a SIGINT or SIGTERM signal.
It is good practice to use this method to exit any infinite loop
processing messages.

```
boolean my_zproc.hasCurve ()
```

Returns true if the underlying libzmq supports CURVE security.

```
string my_zproc.hostname ()
```

Return current host name, for use in public tcp:// endpoints.
If the host name is not resolvable, returns NULL.

```
string my_zproc.biface ()
```

Return network interface to use for broadcasts, or "" if none was set.

### The Zsock class - high-level socket API that hides libzmq contexts and sockets

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zsock = new czmq.Zsock (Number)
```

You *must* call the destructor on every Zsock instance:

```
my_zsock.destroy ()
```

Methods:

```
integer my_zsock.bind (String)
```

Bind a socket to a formatted endpoint. For tcp:// endpoints, supports
ephemeral ports, if you specify the port number as "*". By default
zsock uses the IANA designated range from C000 (49152) to FFFF (65535).
To override this range, follow the "*" with "[first-last]". Either or
both first and last may be empty. To bind to a random port within the
range, use "!" in place of "*".

Examples:
    tcp://127.0.0.1:*           bind to first free port from C000 up
    tcp://127.0.0.1:!           bind to random port from C000 to FFFF
    tcp://127.0.0.1:*[60000-]   bind to first free port from 60000 up
    tcp://127.0.0.1:![-60000]   bind to random port from C000 to 60000
    tcp://127.0.0.1:![55000-55999]
                                bind to random port from 55000 to 55999

On success, returns the actual port number used, for tcp:// endpoints,
and 0 for other transports. On failure, returns -1. Note that when using
ephemeral ports, a port may be reused by different services without
clients being aware. Protocols that run on ephemeral ports should take
this into account.

```
string my_zsock.endpoint ()
```

Returns last bound endpoint, if any.

```
integer my_zsock.unbind (String)
```

Unbind a socket from a formatted endpoint.
Returns 0 if OK, -1 if the endpoint was invalid or the function
isn't supported.

```
integer my_zsock.connect (String)
```

Connect a socket to a formatted endpoint
Returns 0 if OK, -1 if the endpoint was invalid.

```
integer my_zsock.disconnect (String)
```

Disconnect a socket from a formatted endpoint
Returns 0 if OK, -1 if the endpoint was invalid or the function
isn't supported.

```
integer my_zsock.attach (String, Boolean)
```

Attach a socket to zero or more endpoints. If endpoints is not null,
parses as list of ZeroMQ endpoints, separated by commas, and prefixed by
'@' (to bind the socket) or '>' (to connect the socket). Returns 0 if all
endpoints were valid, or -1 if there was a syntax error. If the endpoint
does not start with '@' or '>', the serverish argument defines whether
it is used to bind (serverish = true) or connect (serverish = false).

```
string my_zsock.typeStr ()
```

Returns socket type as printable constant string.

```
integer my_zsock.send (String)
```

Send a 'picture' message to the socket (or actor). The picture is a
string that defines the type of each frame. This makes it easy to send
a complex multiframe message in one call. The picture can contain any
of these characters, each corresponding to one or two arguments:

    i = int (signed)
    1 = uint8_t
    2 = uint16_t
    4 = uint32_t
    8 = uint64_t
    s = char *
    b = byte *, size_t (2 arguments)
    c = zchunk_t *
    f = zframe_t *
    h = zhashx_t *
    U = zuuid_t *
    p = void * (sends the pointer value, only meaningful over inproc)
    m = zmsg_t * (sends all frames in the zmsg)
    z = sends zero-sized frame (0 arguments)
    u = uint (deprecated)

Note that s, b, c, and f are encoded the same way and the choice is
offered as a convenience to the sender, which may or may not already
have data in a zchunk or zframe. Does not change or take ownership of
any arguments. Returns 0 if successful, -1 if sending failed for any
reason.

```
integer my_zsock.recv (String)
```

Receive a 'picture' message to the socket (or actor). See zsock_send for
the format and meaning of the picture. Returns the picture elements into
a series of pointers as provided by the caller:

    i = int * (stores signed integer)
    4 = uint32_t * (stores 32-bit unsigned integer)
    8 = uint64_t * (stores 64-bit unsigned integer)
    s = char ** (allocates new string)
    b = byte **, size_t * (2 arguments) (allocates memory)
    c = zchunk_t ** (creates zchunk)
    f = zframe_t ** (creates zframe)
    U = zuuid_t * (creates a zuuid with the data)
    h = zhashx_t ** (creates zhashx)
    p = void ** (stores pointer)
    m = zmsg_t ** (creates a zmsg with the remaing frames)
    z = null, asserts empty frame (0 arguments)
    u = uint * (stores unsigned integer, deprecated)

Note that zsock_recv creates the returned objects, and the caller must
destroy them when finished with them. The supplied pointers do not need
to be initialized. Returns 0 if successful, or -1 if it failed to recv
a message, in which case the pointers are not modified. When message
frames are truncated (a short message), sets return values to zero/null.
If an argument pointer is NULL, does not store any value (skips it).
An 'n' picture matches an empty frame; if the message does not match,
the method will return -1.

```
integer my_zsock.bsend (String)
```

Send a binary encoded 'picture' message to the socket (or actor). This
method is similar to zsock_send, except the arguments are encoded in a
binary format that is compatible with zproto, and is designed to reduce
memory allocations. The pattern argument is a string that defines the
type of each argument. Supports these argument types:

 pattern    C type                  zproto type:
    1       uint8_t                 type = "number" size = "1"
    2       uint16_t                type = "number" size = "2"
    4       uint32_t                type = "number" size = "3"
    8       uint64_t                type = "number" size = "4"
    s       char *, 0-255 chars     type = "string"
    S       char *, 0-2^32-1 chars  type = "longstr"
    c       zchunk_t *              type = "chunk"
    f       zframe_t *              type = "frame"
    u       zuuid_t *               type = "uuid"
    m       zmsg_t *                type = "msg"
    p       void *, sends pointer value, only over inproc

Does not change or take ownership of any arguments. Returns 0 if
successful, -1 if sending failed for any reason.

```
integer my_zsock.brecv (String)
```

Receive a binary encoded 'picture' message from the socket (or actor).
This method is similar to zsock_recv, except the arguments are encoded
in a binary format that is compatible with zproto, and is designed to
reduce memory allocations. The pattern argument is a string that defines
the type of each argument. See zsock_bsend for the supported argument
types. All arguments must be pointers; this call sets them to point to
values held on a per-socket basis.
Note that zsock_brecv creates the returned objects, and the caller must
destroy them when finished with them. The supplied pointers do not need
to be initialized. Returns 0 if successful, or -1 if it failed to read
a message.

```
number my_zsock.routingId ()
```

Return socket routing ID if any. This returns 0 if the socket is not
of type ZMQ_SERVER or if no request was already received on it.

```
integer my_zsock.wait ()
```

Wait on a signal. Use this to coordinate between threads, over pipe
pairs. Blocks until the signal is received. Returns -1 on error, 0 or
greater on success. Accepts a zsock_t or a zactor_t as argument.
Takes a polymorphic socket reference.

```
integer my_zsock.join (String)
```

Join a group for the RADIO-DISH pattern. Call only on ZMQ_DISH.
Returns 0 if OK, -1 if failed.

```
integer my_zsock.leave (String)
```

Leave a group for the RADIO-DISH pattern. Call only on ZMQ_DISH.
Returns 0 if OK, -1 if failed.

```
integer my_zsock.heartbeatIvl ()
```

Get socket option `heartbeat_ivl`.

```
integer my_zsock.heartbeatTtl ()
```

Get socket option `heartbeat_ttl`.

```
integer my_zsock.heartbeatTimeout ()
```

Get socket option `heartbeat_timeout`.

```
integer my_zsock.useFd ()
```

Get socket option `use_fd`.

```
integer my_zsock.tos ()
```

Get socket option `tos`.

```
string my_zsock.zapDomain ()
```

Get socket option `zap_domain`.

```
integer my_zsock.mechanism ()
```

Get socket option `mechanism`.

```
integer my_zsock.plainServer ()
```

Get socket option `plain_server`.

```
string my_zsock.plainUsername ()
```

Get socket option `plain_username`.

```
string my_zsock.plainPassword ()
```

Get socket option `plain_password`.

```
integer my_zsock.curveServer ()
```

Get socket option `curve_server`.

```
string my_zsock.curvePublickey ()
```

Get socket option `curve_publickey`.

```
string my_zsock.curveSecretkey ()
```

Get socket option `curve_secretkey`.

```
string my_zsock.curveServerkey ()
```

Get socket option `curve_serverkey`.

```
integer my_zsock.gssapiServer ()
```

Get socket option `gssapi_server`.

```
integer my_zsock.gssapiPlaintext ()
```

Get socket option `gssapi_plaintext`.

```
string my_zsock.gssapiPrincipal ()
```

Get socket option `gssapi_principal`.

```
string my_zsock.gssapiServicePrincipal ()
```

Get socket option `gssapi_service_principal`.

```
integer my_zsock.ipv6 ()
```

Get socket option `ipv6`.

```
integer my_zsock.immediate ()
```

Get socket option `immediate`.

```
integer my_zsock.ipv4only ()
```

Get socket option `ipv4only`.

```
integer my_zsock.type ()
```

Get socket option `type`.

```
integer my_zsock.sndhwm ()
```

Get socket option `sndhwm`.

```
integer my_zsock.rcvhwm ()
```

Get socket option `rcvhwm`.

```
integer my_zsock.affinity ()
```

Get socket option `affinity`.

```
string my_zsock.identity ()
```

Get socket option `identity`.

```
integer my_zsock.rate ()
```

Get socket option `rate`.

```
integer my_zsock.recoveryIvl ()
```

Get socket option `recovery_ivl`.

```
integer my_zsock.sndbuf ()
```

Get socket option `sndbuf`.

```
integer my_zsock.rcvbuf ()
```

Get socket option `rcvbuf`.

```
integer my_zsock.linger ()
```

Get socket option `linger`.

```
integer my_zsock.reconnectIvl ()
```

Get socket option `reconnect_ivl`.

```
integer my_zsock.reconnectIvlMax ()
```

Get socket option `reconnect_ivl_max`.

```
integer my_zsock.backlog ()
```

Get socket option `backlog`.

```
integer my_zsock.maxmsgsize ()
```

Get socket option `maxmsgsize`.

```
integer my_zsock.multicastHops ()
```

Get socket option `multicast_hops`.

```
integer my_zsock.rcvtimeo ()
```

Get socket option `rcvtimeo`.

```
integer my_zsock.sndtimeo ()
```

Get socket option `sndtimeo`.

```
integer my_zsock.tcpKeepalive ()
```

Get socket option `tcp_keepalive`.

```
integer my_zsock.tcpKeepaliveIdle ()
```

Get socket option `tcp_keepalive_idle`.

```
integer my_zsock.tcpKeepaliveCnt ()
```

Get socket option `tcp_keepalive_cnt`.

```
integer my_zsock.tcpKeepaliveIntvl ()
```

Get socket option `tcp_keepalive_intvl`.

```
string my_zsock.tcpAcceptFilter ()
```

Get socket option `tcp_accept_filter`.

```
integer my_zsock.rcvmore ()
```

Get socket option `rcvmore`.

```
integer my_zsock.events ()
```

Get socket option `events`.

```
string my_zsock.lastEndpoint ()
```

Get socket option `last_endpoint`.

### The Zstr class - sending and receiving strings

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zstr = new czmq.Zstr ()
```

Methods:

```
string my_zstr.recv (Zsock)
```

Receive C string from socket. Caller must free returned string using
zstr_free(). Returns NULL if the context is being terminated or the
process was interrupted.

```
integer my_zstr.recvx (Zsock, String)
```

Receive a series of strings (until NULL) from multipart data.
Each string is allocated and filled with string data; if there
are not enough frames, unallocated strings are set to NULL.
Returns -1 if the message could not be read, else returns the
number of strings filled, zero or more. Free each returned string
using zstr_free(). If not enough strings are provided, remaining
multipart frames in the message are dropped.

```
integer my_zstr.send (Zsock, String)
```

Send a C string to a socket, as a frame. The string is sent without
trailing null byte; to read this you can use zstr_recv, or a similar
method that adds a null terminator on the received string. String
may be NULL, which is sent as "".

```
integer my_zstr.sendm (Zsock, String)
```

Send a C string to a socket, as zstr_send(), with a MORE flag, so that
you can send further strings in the same multi-part message.

```
integer my_zstr.sendf (Zsock, String)
```

Send a formatted string to a socket. Note that you should NOT use
user-supplied strings in the format (they may contain '%' which
will create security holes).

```
integer my_zstr.sendfm (Zsock, String)
```

Send a formatted string to a socket, as for zstr_sendf(), with a
MORE flag, so that you can send further strings in the same multi-part
message.

```
integer my_zstr.sendx (Zsock, String)
```

Send a series of strings (until NULL) as multipart data
Returns 0 if the strings could be sent OK, or -1 on error.

```
string my_zstr.str (Zsock)
```

Accepts a void pointer and returns a fresh character string. If source
is null, returns an empty string.

### The Ztrie class - simple trie for tokenizable strings

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_ztrie = new czmq.Ztrie (String)
```

You *must* call the destructor on every Ztrie instance:

```
my_ztrie.destroy ()
```

Methods:

```
integer my_ztrie.removeRoute (String)
```

Removes a route from the trie and destroys its data. Returns -1 if the
route does not exists, otherwise 0.
the start of the list call zlist_first (). Advances the cursor.

```
boolean my_ztrie.matches (String)
```

Returns true if the path matches a route in the tree, otherwise false.

```
size my_ztrie.hitParameterCount ()
```

Returns the count of parameters that a matched route has.

```
zhashx my_ztrie.hitParameters ()
```

Returns the parameters of a matched route with named regexes from last
ztrie_matches. If the path did not match or the route did not contain any
named regexes, returns NULL.

```
string my_ztrie.hitAsteriskMatch ()
```

Returns the asterisk matched part of a route, if there has been no match
or no asterisk match, returns NULL.

### The Zuuid class - UUID support class

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zuuid = new czmq.Zuuid ()
```

You *must* call the destructor on every Zuuid instance:

```
my_zuuid.destroy ()
```

Methods:

```
integer my_zuuid.setStr (String)
```

Set UUID to new supplied string value skipping '-' and '{' '}'
optional delimiters. Return 0 if OK, else returns -1.

```
buffer my_zuuid.data ()
```

Return UUID binary data.

```
size my_zuuid.size ()
```

Return UUID binary size

```
string my_zuuid.str ()
```

Returns UUID as string

```
string my_zuuid.strCanonical ()
```

Return UUID in the canonical string format: 8-4-4-4-12, in lower
case. Caller does not modify or free returned value. See
http://en.wikipedia.org/wiki/Universally_unique_identifier

```
boolean my_zuuid.eq (String)
```

Check if UUID is same as supplied value

```
boolean my_zuuid.neq (String)
```

Check if UUID is different from supplied value

```
zuuid my_zuuid.dup ()
```

Make copy of UUID object; if uuid is null, or memory was exhausted,
returns null.
