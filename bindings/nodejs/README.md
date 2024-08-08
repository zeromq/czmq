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

### The Zargs class - Platform independent command line argument parsing helpers

There are two kind of elements provided by this class
Named parameters, accessed by param_get and param_has methods
  * --named-parameter
  * --parameter with_value
  * -a val
Positional arguments, accessed by zargs_first, zargs_next

It DOES:
* provide easy to use CLASS compatible API for accessing argv
* is platform independent
* provide getopt_long style -- argument, which delimits parameters from arguments
* makes parameters position independent

It does NOT
* change argv
* provide a "declarative" way to define command line interface

In future it SHALL
* hide several formats of command line to one (-Idir, --include=dir,
  --include dir are the same from API pov)

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zargs = new czmq.Zargs (Number, String)
```

You *must* call the destructor on every Zargs instance:

```
my_zargs.destroy ()
```

Methods:

```
string my_zargs.progname ()
```

Return program name (argv[0])

```
size my_zargs.arguments ()
```

Return number of positional arguments

```
string my_zargs.first ()
```

Return first positional argument or NULL

```
string my_zargs.next ()
```

Return next positional argument or NULL

```
string my_zargs.paramFirst ()
```

Return first named parameter value, or NULL if there are no named
parameters, or value for which zargs_param_empty (arg) returns true.

```
string my_zargs.paramNext ()
```

Return next named parameter value, or NULL if there are no named
parameters, or value for which zargs_param_empty (arg) returns true.

```
string my_zargs.paramName ()
```

Return current parameter name, or NULL if there are no named parameters.

```
string my_zargs.get (String)
```

Return value of named parameter or NULL is it has no value (or was not specified)

```
string my_zargs.getx (String)
```

Return value of one of parameter(s) or NULL is it has no value (or was not specified)

```
boolean my_zargs.has (String)
```

Returns true if named parameter was specified on command line

```
boolean my_zargs.hasx (String)
```

Returns true if named parameter(s) was specified on command line

```
nothing my_zargs.print ()
```

Print an instance of zargs.

```
nothing my_zargs.test (Boolean)
```

Self test of this class.

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
nothing my_zarmour.setMode (Number)
```

Set the mode property.

```
boolean my_zarmour.pad ()
```

Return true if padding is turned on.

```
nothing my_zarmour.setPad (Boolean)
```

Turn padding on or off. Default is on.

```
char my_zarmour.padChar ()
```

Get the padding character.

```
nothing my_zarmour.setPadChar (String)
```

Set the padding character.

```
boolean my_zarmour.lineBreaks ()
```

Return if splitting output into lines is turned on. Default is off.

```
nothing my_zarmour.setLineBreaks (Boolean)
```

Turn splitting output into lines on or off.

```
size my_zarmour.lineLength ()
```

Get the line length used for splitting lines.

```
nothing my_zarmour.setLineLength ()
```

Set the line length used for splitting lines.

```
nothing my_zarmour.print ()
```

Print properties of object

```
nothing my_zarmour.test (Boolean)
```

Self test of this class.

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
nothing my_zcert.setMeta (String, String)
```

Set certificate metadata from formatted string.

```
nothing my_zcert.unsetMeta (String)
```

Unset certificate metadata.

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
nothing my_zcert.apply (Zsock)
```

Apply certificate to socket, i.e. use for CURVE security on socket.
If certificate was loaded from public file, the secret key will be
undefined, and this certificate will not work successfully.

```
zcert my_zcert.dup ()
```

Return copy of certificate; if certificate is NULL or we exhausted
heap memory, returns NULL.

```
boolean my_zcert.eq (Zcert)
```

Return true if two certificates have the same keys

```
nothing my_zcert.print ()
```

Print certificate contents to stdout

```
nothing my_zcert.test (Boolean)
```

Self test of this class

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

```
nothing my_zcertstore.insert (Zcert)
```

Insert certificate into certificate store in memory. Note that this
does not save the certificate to disk. To do that, use zcert_save()
directly on the certificate. Takes ownership of zcert_t object.

```
nothing my_zcertstore.empty ()
```

Empty certificate hashtable. This wrapper exists to be friendly to bindings,
which don't usually have access to struct internals.

```
nothing my_zcertstore.print ()
```

Print list of certificates in store to logging facility

```
zlistx my_zcertstore.certs ()
```

Return a list of all the certificates in the store.
The caller takes ownership of the zlistx_t object and is responsible
for destroying it.  The caller does not take ownership of the zcert_t
objects.

```
nothing my_zcertstore.test (Boolean)
```

Self test of this class

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
nothing my_zchunk.resize ()
```

Resizes chunk max_size as requested; chunk_cur size is set to zero

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
zframe my_zchunk.packx (Zchunk)
```

Transform zchunk into a zframe that can be sent in a message.
Take ownership of the chunk.

```
zchunk my_zchunk.unpack (Zframe)
```

Transform a zframe into a zchunk.

```
string my_zchunk.digest ()
```

Calculate SHA1 digest for chunk, using zdigest class.

```
nothing my_zchunk.print ()
```

Dump message to stderr, for debugging and tracing.
See zchunk_fprint for details

```
nothing my_zchunk.test (Boolean)
```

Self test of this class.

### The Zclock class - millisecond clocks and delays

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zclock = new czmq.Zclock ()
```

Methods:

```
nothing my_zclock.sleep (Number)
```

Sleep for a number of milliseconds

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

```
nothing my_zclock.test (Boolean)
```

Self test of this class.

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
zconfig my_zconfig.dup ()
```

Create copy of zconfig, caller MUST free the value
Create copy of config, as new zconfig object. Returns a fresh zconfig_t
object. If config is null, or memory was exhausted, returns null.

```
string my_zconfig.name ()
```

Return name of config item

```
string my_zconfig.value ()
```

Return value of config item

```
nothing my_zconfig.put (String, String)
```

Insert or update configuration key with value

```
nothing my_zconfig.putf (String, String)
```

Equivalent to zconfig_put, accepting a format specifier and variable
argument list, instead of a single string value.

```
string my_zconfig.get (String, String)
```

Get value for config item into a string value; leading slash is optional
and ignored.

```
nothing my_zconfig.setName (String)
```

Set config item name, name may be NULL

```
nothing my_zconfig.setValue (String)
```

Set new value for config item. The new value may be a string, a printf
format, or NULL. Note that if string may possibly contain '%', or if it
comes from an insecure source, you must use '%s' as the format, followed
by the string.

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
nothing my_zconfig.setComment (String)
```

Add comment to config item before saving to disk. You can add as many
comment lines as you like. If you use a null format, all comments are
deleted.

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

```
nothing my_zconfig.removeSubtree ()
```

Destroy subtree (all children)

```
nothing my_zconfig.remove (Zconfig)
```

Destroy node and subtree (all children)

```
nothing my_zconfig.print ()
```

Print properties of object

```
nothing my_zconfig.test (Boolean)
```

Self test of this class

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
nothing my_zdigest.update (String)
```

Add buffer into digest calculation

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

```
nothing my_zdigest.test (Boolean)
```

Self test of this class.

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
zlist my_zdir.listPaths ()
```

Returns a sorted list of char*; Each entry in the list is a path of a file
or directory contained in self.

```
nothing my_zdir.remove (Boolean)
```

Remove directory, optionally including all files that it contains, at
all levels. If force is false, will only remove the directory if empty.
If force is true, will remove all files and all subdirectories.

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

```
nothing my_zdir.print (Number)
```

Print contents of directory to stdout

```
nothing my_zdir.test (Boolean)
```

Self test of this class.

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
nothing my_zdir_patch.digestSet ()
```

Calculate hash digest for file (create only)

```
string my_zdir_patch.digest ()
```

Return hash digest for patch file

```
nothing my_zdir_patch.test (Boolean)
```

Self test of this class.

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
nothing my_zfile.restat ()
```

Refresh file properties from disk; this is not done automatically
on access methods, otherwise it is not possible to compare directory
snapshots.

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
nothing my_zfile.remove ()
```

Remove the file from disk

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
nothing my_zfile.close ()
```

Close file, if open

```
string my_zfile.digest ()
```

Calculate SHA1 digest for file, using zdigest class.

```
nothing my_zfile.test (Boolean)
```

Self test of this class.

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
string my_zframe.meta (String)
```

Return meta data property for frame
The caller shall not modify or free the returned value, which shall be
owned by the message.

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
nothing my_zframe.setMore (Number)
```

Set frame MORE indicator (1 or 0). Note this is NOT used when sending
frame to socket, you have to specify flag explicitly.

```
number my_zframe.routingId ()
```

Return frame routing ID, if the frame came from a ZMQ_SERVER socket.
Else returns zero.

```
nothing my_zframe.setRoutingId (Number)
```

Set routing ID on frame. This is used if/when the frame is sent to a
ZMQ_SERVER socket.

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

```
nothing my_zframe.reset (String)
```

Set new contents for frame

```
nothing my_zframe.print (String)
```

Send message to zsys log sink (may be stdout, or system facility as
configured by zsys_set_logstream). Prefix shows before frame, if not null.
Long messages are truncated.

```
nothing my_zframe.printN (String)
```

Send message to zsys log sink (may be stdout, or system facility as
configured by zsys_set_logstream). Prefix shows before frame, if not null.
Message length is specified; no truncation unless length is zero.
Backwards compatible with zframe_print when length is zero.

```
nothing my_zframe.test (Boolean)
```

Self test of this class.

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
nothing my_zhash.delete (String)
```

Remove an item specified by key from the hash table. If there was no such
item, this function does nothing.

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
nothing my_zhash.comment (String)
```

Add a comment to hash table before saving to disk. You can add as many
comment lines as you like. These comment lines are discarded when loading
the file. If you use a null format, all comments are deleted.

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

```
nothing my_zhash.autofree ()
```

Set hash for automatic value destruction. Note that this assumes that
values are NULL-terminated strings. Do not use with different types.

```
nothing my_zhash.test (Boolean)
```

Self test of this class.

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
nothing my_zhashx.purge ()
```

Delete all items from the hash table. If the key destructor is
set, calls it on every key. If the item destructor is set, calls
it on every item.

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
nothing my_zhashx.comment (String)
```

Add a comment to hash table before saving to disk. You can add as many
comment lines as you like. These comment lines are discarded when loading
the file. If you use a null format, all comments are deleted.

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

```
nothing my_zhashx.test (Boolean)
```

Self test of this class.

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
nothing my_ziflist.reload ()
```

Reload network interfaces from system

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

```
string my_ziflist.mac ()
```

Return the current interface MAC address as a printable string

```
nothing my_ziflist.print ()
```

Return the list of interfaces.

```
ziflist my_ziflist.newIpv6 ()
```

Get a list of network interfaces currently defined on the system
Includes IPv6 interfaces

```
nothing my_ziflist.reloadIpv6 ()
```

Reload network interfaces from system, including IPv6

```
boolean my_ziflist.isIpv6 ()
```

Return true if the current interface uses IPv6

```
nothing my_ziflist.test (Boolean)
```

Self test of this class.

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
nothing my_zlist.purge ()
```

Purge all items from list

```
size my_zlist.size ()
```

Return number of items in the list

```
nothing my_zlist.autofree ()
```

Set list for automatic item destruction; item values MUST be strings.
By default a list item refers to a value held elsewhere. When you set
this, each time you append or push a list item, zlist will take a copy
of the string value. Then, when you destroy the list, it will free all
item values automatically. If you use any other technique to allocate
list values, you must free them explicitly before destroying the list.
The usual technique is to pop list items and destroy them, until the
list is empty.

```
nothing my_zlist.test (Boolean)
```

Self test of this class.

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
nothing my_zlistx.purge ()
```

Remove all items from the list, and destroy them if the item destructor
is set.

```
nothing my_zlistx.sort ()
```

Sort the list. If an item comparator was set, calls that to compare
items, otherwise compares on item value. The sort is not stable, so may
reorder equal items.

```
zlistx my_zlistx.dup ()
```

Make a copy of the list; items are duplicated if you set a duplicator
for the list, otherwise not. Copying a null reference returns a null
reference.

```
zframe my_zlistx.pack ()
```

Serialize list to a binary frame that can be sent in a message.
The packed format is compatible with the 'strings' type implemented by zproto:

   ; A list of strings
   list            = list-count *longstr
   list-count      = number-4

   ; Strings are always length + text contents
   longstr         = number-4 *VCHAR

   ; Numbers are unsigned integers in network byte order
   number-4        = 4OCTET

```
nothing my_zlistx.test (Boolean)
```

Self test of this class.

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
nothing my_zloop.readerEnd (Zsock)
```

Cancel a socket reader from the reactor. If multiple readers exist for
same socket, cancels ALL of them.

```
nothing my_zloop.readerSetTolerant (Zsock)
```

Configure a registered reader to ignore errors. If you do not set this,
then readers that have errors are removed from the reactor silently.

```
integer my_zloop.timerEnd (Number)
```

Cancel a specific timer identified by a specific timer_id (as returned by
zloop_timer).

```
nothing my_zloop.setTicketDelay ()
```

Set the ticket delay, which applies to all tickets. If you lower the
delay and there are already tickets created, the results are undefined.

```
nothing my_zloop.setMaxTimers ()
```

Set hard limit on number of timers allowed. Setting more than a small
number of timers (10-100) can have a dramatic impact on the performance
of the reactor. For high-volume cases, use ticket timers. If the hard
limit is reached, the reactor stops creating new timers and logs an
error.

```
nothing my_zloop.setVerbose (Boolean)
```

Set verbose tracing of reactor on/off. The default verbose setting is
off (false).

```
nothing my_zloop.setNonstop (Boolean)
```

By default the reactor stops if the process receives a SIGINT or SIGTERM
signal. This makes it impossible to shut-down message based architectures
like zactors. This method lets you switch off break handling. The default
nonstop setting is off (false).

```
integer my_zloop.start ()
```

Start the reactor. Takes control of the thread and returns when the 0MQ
context is terminated or the process is interrupted, or any event handler
returns -1. Event handlers may register new sockets and timers, and
cancel sockets. Returns 0 if interrupted, -1 if canceled by a handler.

```
nothing my_zloop.test (Boolean)
```

Self test of this class.

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
nothing my_zmsg.setRoutingId (Number)
```

Set routing ID on message. This is used if/when the message is sent to a
ZMQ_SERVER socket.

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
nothing my_zmsg.remove (Zframe)
```

Remove specified frame from list, if present. Does not destroy frame.

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
nothing my_zmsg.print ()
```

Send message to zsys log sink (may be stdout, or system facility as
configured by zsys_set_logstream).
Long messages are truncated.

```
nothing my_zmsg.printN ()
```

Send message to zsys log sink (may be stdout, or system facility as
configured by zsys_set_logstream).
Message length is specified; no truncation unless length is zero.
Backwards compatible with zframe_print when length is zero.

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

```
nothing my_zmsg.test (Boolean)
```

Self test of this class.

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
be a libzmq void * socket, a zsock_t instance, a zactor_t instance or a
file handle.

```
nothing my_zpoller.setNonstop (Boolean)
```

By default the poller stops if the process receives a SIGINT or SIGTERM
signal. This makes it impossible to shut-down message based architectures
like zactors. This method lets you switch off break handling. The default
nonstop setting is off (false).

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

```
nothing my_zpoller.test (Boolean)
```

Self test of this class.

### The Zproc class - process configuration and status

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zproc = new czmq.Zproc ()
```

You *must* call the destructor on every Zproc instance:

```
my_zproc.destroy ()
```

Methods:

```
zlist my_zproc.args ()
```

Return command line arguments (the first item is the executable) or
NULL if not set.

```
nothing my_zproc.setArgs (Zlist)
```

Setup the command line arguments, the first item must be an (absolute) filename
to run.

```
nothing my_zproc.setArgsx (String)
```

Setup the command line arguments, the first item must be an (absolute) filename
to run. Variadic function, must be NULL terminated.

```
nothing my_zproc.setEnv (Zhash)
```

Setup the environment variables for the process.

```
integer my_zproc.run ()
```

Starts the process, return just before execve/CreateProcess.

```
integer my_zproc.returncode ()
```

process exit code

```
integer my_zproc.pid ()
```

PID of the process

```
boolean my_zproc.running ()
```

return true if process is running, false if not yet started or finished

```
integer my_zproc.wait (Number)
```

The timeout should be zero or greater, or -1 to wait indefinitely.
wait or poll process status, return return code

```
nothing my_zproc.shutdown (Number)
```

send SIGTERM signal to the subprocess, wait for grace period and
eventually send SIGKILL

```
nothing my_zproc.kill (Number)
```

send a signal to the subprocess

```
nothing my_zproc.setVerbose (Boolean)
```

set verbose mode

```
nothing my_zproc.test (Boolean)
```

Self test of this class.

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
    l = zlistx_t * (DRAFT)
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
    l = zlistx_t ** (creates zlistx) (DRAFT)
    p = void ** (stores pointer)
    m = zmsg_t ** (creates a zmsg with the remaining frames)
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
For types 1, 2, 4 and 8 the caller must allocate the memory itself before
calling zsock_brecv.
For types S, the caller must free the value once finished with it, as
zsock_brecv will allocate the buffer.
For type s, the caller must not free the value as it is stored in a
local cache for performance purposes.
For types c, f, u and m the caller must call the appropriate destructor
depending on the object as zsock_brecv will create new objects.
For type p the caller must coordinate with the sender, as it is just a
pointer value being passed.

```
number my_zsock.routingId ()
```

Return socket routing ID if any. This returns 0 if the socket is not
of type ZMQ_SERVER or if no request was already received on it.

```
nothing my_zsock.setRoutingId (Number)
```

Set routing ID on socket. The socket MUST be of type ZMQ_SERVER.
This will be used when sending messages on the socket via the zsock API.

```
nothing my_zsock.setUnbounded ()
```

Set socket to use unbounded pipes (HWM=0); use this in cases when you are
totally certain the message volume can fit in memory. This method works
across all versions of ZeroMQ. Takes a polymorphic socket reference.

```
integer my_zsock.wait ()
```

Wait on a signal. Use this to coordinate between threads, over pipe
pairs. Blocks until the signal is received. Returns -1 on error, 0 or
greater on success. Accepts a zsock_t or a zactor_t as argument.
Takes a polymorphic socket reference.

```
nothing my_zsock.flush ()
```

If there is a partial message still waiting on the socket, remove and
discard it. This is useful when reading partial messages, to get specific
message types.

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
boolean my_zsock.hasIn ()
```

Check whether the socket has available message to read.

```
integer my_zsock.priority ()
```

Get socket option `priority`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setPriority (Number)
```

Set socket option `priority`.
Available from libzmq 4.3.0.

```
integer my_zsock.reconnectStop ()
```

Get socket option `reconnect_stop`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setReconnectStop (Number)
```

Set socket option `reconnect_stop`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setOnlyFirstSubscribe (Number)
```

Set socket option `only_first_subscribe`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setHelloMsg (Zframe)
```

Set socket option `hello_msg`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setDisconnectMsg (Zframe)
```

Set socket option `disconnect_msg`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setWssTrustSystem (Number)
```

Set socket option `wss_trust_system`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setWssHostname (String)
```

Set socket option `wss_hostname`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setWssTrustPem (String)
```

Set socket option `wss_trust_pem`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setWssCertPem (String)
```

Set socket option `wss_cert_pem`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setWssKeyPem (String)
```

Set socket option `wss_key_pem`.
Available from libzmq 4.3.0.

```
integer my_zsock.outBatchSize ()
```

Get socket option `out_batch_size`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setOutBatchSize (Number)
```

Set socket option `out_batch_size`.
Available from libzmq 4.3.0.

```
integer my_zsock.inBatchSize ()
```

Get socket option `in_batch_size`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setInBatchSize (Number)
```

Set socket option `in_batch_size`.
Available from libzmq 4.3.0.

```
string my_zsock.socksPassword ()
```

Get socket option `socks_password`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setSocksPassword (String)
```

Set socket option `socks_password`.
Available from libzmq 4.3.0.

```
string my_zsock.socksUsername ()
```

Get socket option `socks_username`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setSocksUsername (String)
```

Set socket option `socks_username`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setXpubManualLastValue (Number)
```

Set socket option `xpub_manual_last_value`.
Available from libzmq 4.3.0.

```
integer my_zsock.routerNotify ()
```

Get socket option `router_notify`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setRouterNotify (Number)
```

Set socket option `router_notify`.
Available from libzmq 4.3.0.

```
integer my_zsock.multicastLoop ()
```

Get socket option `multicast_loop`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setMulticastLoop (Number)
```

Set socket option `multicast_loop`.
Available from libzmq 4.3.0.

```
string my_zsock.metadata ()
```

Get socket option `metadata`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setMetadata (String)
```

Set socket option `metadata`.
Available from libzmq 4.3.0.

```
integer my_zsock.loopbackFastpath ()
```

Get socket option `loopback_fastpath`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setLoopbackFastpath (Number)
```

Set socket option `loopback_fastpath`.
Available from libzmq 4.3.0.

```
integer my_zsock.zapEnforceDomain ()
```

Get socket option `zap_enforce_domain`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setZapEnforceDomain (Number)
```

Set socket option `zap_enforce_domain`.
Available from libzmq 4.3.0.

```
integer my_zsock.gssapiPrincipalNametype ()
```

Get socket option `gssapi_principal_nametype`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setGssapiPrincipalNametype (Number)
```

Set socket option `gssapi_principal_nametype`.
Available from libzmq 4.3.0.

```
integer my_zsock.gssapiServicePrincipalNametype ()
```

Get socket option `gssapi_service_principal_nametype`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setGssapiServicePrincipalNametype (Number)
```

Set socket option `gssapi_service_principal_nametype`.
Available from libzmq 4.3.0.

```
string my_zsock.bindtodevice ()
```

Get socket option `bindtodevice`.
Available from libzmq 4.3.0.

```
nothing my_zsock.setBindtodevice (String)
```

Set socket option `bindtodevice`.
Available from libzmq 4.3.0.

```
integer my_zsock.heartbeatIvl ()
```

Get socket option `heartbeat_ivl`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setHeartbeatIvl (Number)
```

Set socket option `heartbeat_ivl`.
Available from libzmq 4.2.0.

```
integer my_zsock.heartbeatTtl ()
```

Get socket option `heartbeat_ttl`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setHeartbeatTtl (Number)
```

Set socket option `heartbeat_ttl`.
Available from libzmq 4.2.0.

```
integer my_zsock.heartbeatTimeout ()
```

Get socket option `heartbeat_timeout`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setHeartbeatTimeout (Number)
```

Set socket option `heartbeat_timeout`.
Available from libzmq 4.2.0.

```
integer my_zsock.useFd ()
```

Get socket option `use_fd`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setUseFd (Number)
```

Set socket option `use_fd`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setXpubManual (Number)
```

Set socket option `xpub_manual`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setXpubWelcomeMsg (String)
```

Set socket option `xpub_welcome_msg`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setStreamNotify (Number)
```

Set socket option `stream_notify`.
Available from libzmq 4.2.0.

```
integer my_zsock.invertMatching ()
```

Get socket option `invert_matching`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setInvertMatching (Number)
```

Set socket option `invert_matching`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setXpubVerboser (Number)
```

Set socket option `xpub_verboser`.
Available from libzmq 4.2.0.

```
integer my_zsock.connectTimeout ()
```

Get socket option `connect_timeout`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setConnectTimeout (Number)
```

Set socket option `connect_timeout`.
Available from libzmq 4.2.0.

```
integer my_zsock.tcpMaxrt ()
```

Get socket option `tcp_maxrt`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setTcpMaxrt (Number)
```

Set socket option `tcp_maxrt`.
Available from libzmq 4.2.0.

```
integer my_zsock.threadSafe ()
```

Get socket option `thread_safe`.
Available from libzmq 4.2.0.

```
integer my_zsock.multicastMaxtpdu ()
```

Get socket option `multicast_maxtpdu`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setMulticastMaxtpdu (Number)
```

Set socket option `multicast_maxtpdu`.
Available from libzmq 4.2.0.

```
integer my_zsock.vmciBufferSize ()
```

Get socket option `vmci_buffer_size`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setVmciBufferSize (Number)
```

Set socket option `vmci_buffer_size`.
Available from libzmq 4.2.0.

```
integer my_zsock.vmciBufferMinSize ()
```

Get socket option `vmci_buffer_min_size`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setVmciBufferMinSize (Number)
```

Set socket option `vmci_buffer_min_size`.
Available from libzmq 4.2.0.

```
integer my_zsock.vmciBufferMaxSize ()
```

Get socket option `vmci_buffer_max_size`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setVmciBufferMaxSize (Number)
```

Set socket option `vmci_buffer_max_size`.
Available from libzmq 4.2.0.

```
integer my_zsock.vmciConnectTimeout ()
```

Get socket option `vmci_connect_timeout`.
Available from libzmq 4.2.0.

```
nothing my_zsock.setVmciConnectTimeout (Number)
```

Set socket option `vmci_connect_timeout`.
Available from libzmq 4.2.0.

```
integer my_zsock.tos ()
```

Get socket option `tos`.
Available from libzmq 4.1.0.

```
nothing my_zsock.setTos (Number)
```

Set socket option `tos`.
Available from libzmq 4.1.0.

```
nothing my_zsock.setRouterHandover (Number)
```

Set socket option `router_handover`.
Available from libzmq 4.1.0.

```
nothing my_zsock.setConnectRid (String)
```

Set socket option `connect_rid`.
Available from libzmq 4.1.0.

```
nothing my_zsock.setConnectRidBin (String)
```

Set socket option `connect_rid` from 32-octet binary
Available from libzmq 4.1.0.

```
integer my_zsock.handshakeIvl ()
```

Get socket option `handshake_ivl`.
Available from libzmq 4.1.0.

```
nothing my_zsock.setHandshakeIvl (Number)
```

Set socket option `handshake_ivl`.
Available from libzmq 4.1.0.

```
string my_zsock.socksProxy ()
```

Get socket option `socks_proxy`.
Available from libzmq 4.1.0.

```
nothing my_zsock.setSocksProxy (String)
```

Set socket option `socks_proxy`.
Available from libzmq 4.1.0.

```
nothing my_zsock.setXpubNodrop (Number)
```

Set socket option `xpub_nodrop`.
Available from libzmq 4.1.0.

```
nothing my_zsock.setRouterMandatory (Number)
```

Set socket option `router_mandatory`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setProbeRouter (Number)
```

Set socket option `probe_router`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setReqRelaxed (Number)
```

Set socket option `req_relaxed`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setReqCorrelate (Number)
```

Set socket option `req_correlate`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setConflate (Number)
```

Set socket option `conflate`.
Available from libzmq 4.0.0.

```
string my_zsock.zapDomain ()
```

Get socket option `zap_domain`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setZapDomain (String)
```

Set socket option `zap_domain`.
Available from libzmq 4.0.0.

```
integer my_zsock.mechanism ()
```

Get socket option `mechanism`.
Available from libzmq 4.0.0.

```
integer my_zsock.plainServer ()
```

Get socket option `plain_server`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setPlainServer (Number)
```

Set socket option `plain_server`.
Available from libzmq 4.0.0.

```
string my_zsock.plainUsername ()
```

Get socket option `plain_username`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setPlainUsername (String)
```

Set socket option `plain_username`.
Available from libzmq 4.0.0.

```
string my_zsock.plainPassword ()
```

Get socket option `plain_password`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setPlainPassword (String)
```

Set socket option `plain_password`.
Available from libzmq 4.0.0.

```
integer my_zsock.curveServer ()
```

Get socket option `curve_server`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setCurveServer (Number)
```

Set socket option `curve_server`.
Available from libzmq 4.0.0.

```
string my_zsock.curvePublickey ()
```

Get socket option `curve_publickey`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setCurvePublickey (String)
```

Set socket option `curve_publickey`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setCurvePublickeyBin (String)
```

Set socket option `curve_publickey` from 32-octet binary
Available from libzmq 4.0.0.

```
string my_zsock.curveSecretkey ()
```

Get socket option `curve_secretkey`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setCurveSecretkey (String)
```

Set socket option `curve_secretkey`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setCurveSecretkeyBin (String)
```

Set socket option `curve_secretkey` from 32-octet binary
Available from libzmq 4.0.0.

```
string my_zsock.curveServerkey ()
```

Get socket option `curve_serverkey`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setCurveServerkey (String)
```

Set socket option `curve_serverkey`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setCurveServerkeyBin (String)
```

Set socket option `curve_serverkey` from 32-octet binary
Available from libzmq 4.0.0.

```
integer my_zsock.gssapiServer ()
```

Get socket option `gssapi_server`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setGssapiServer (Number)
```

Set socket option `gssapi_server`.
Available from libzmq 4.0.0.

```
integer my_zsock.gssapiPlaintext ()
```

Get socket option `gssapi_plaintext`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setGssapiPlaintext (Number)
```

Set socket option `gssapi_plaintext`.
Available from libzmq 4.0.0.

```
string my_zsock.gssapiPrincipal ()
```

Get socket option `gssapi_principal`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setGssapiPrincipal (String)
```

Set socket option `gssapi_principal`.
Available from libzmq 4.0.0.

```
string my_zsock.gssapiServicePrincipal ()
```

Get socket option `gssapi_service_principal`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setGssapiServicePrincipal (String)
```

Set socket option `gssapi_service_principal`.
Available from libzmq 4.0.0.

```
integer my_zsock.ipv6 ()
```

Get socket option `ipv6`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setIpv6 (Number)
```

Set socket option `ipv6`.
Available from libzmq 4.0.0.

```
integer my_zsock.immediate ()
```

Get socket option `immediate`.
Available from libzmq 4.0.0.

```
nothing my_zsock.setImmediate (Number)
```

Set socket option `immediate`.
Available from libzmq 4.0.0.

```
integer my_zsock.sndhwm ()
```

Get socket option `sndhwm`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setSndhwm (Number)
```

Set socket option `sndhwm`.
Available from libzmq 3.0.0.

```
integer my_zsock.rcvhwm ()
```

Get socket option `rcvhwm`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setRcvhwm (Number)
```

Set socket option `rcvhwm`.
Available from libzmq 3.0.0.

```
integer my_zsock.maxmsgsize ()
```

Get socket option `maxmsgsize`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setMaxmsgsize (Number)
```

Set socket option `maxmsgsize`.
Available from libzmq 3.0.0.

```
integer my_zsock.multicastHops ()
```

Get socket option `multicast_hops`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setMulticastHops (Number)
```

Set socket option `multicast_hops`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setXpubVerbose (Number)
```

Set socket option `xpub_verbose`.
Available from libzmq 3.0.0.

```
integer my_zsock.tcpKeepalive ()
```

Get socket option `tcp_keepalive`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setTcpKeepalive (Number)
```

Set socket option `tcp_keepalive`.
Available from libzmq 3.0.0.

```
integer my_zsock.tcpKeepaliveIdle ()
```

Get socket option `tcp_keepalive_idle`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setTcpKeepaliveIdle (Number)
```

Set socket option `tcp_keepalive_idle`.
Available from libzmq 3.0.0.

```
integer my_zsock.tcpKeepaliveCnt ()
```

Get socket option `tcp_keepalive_cnt`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setTcpKeepaliveCnt (Number)
```

Set socket option `tcp_keepalive_cnt`.
Available from libzmq 3.0.0.

```
integer my_zsock.tcpKeepaliveIntvl ()
```

Get socket option `tcp_keepalive_intvl`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setTcpKeepaliveIntvl (Number)
```

Set socket option `tcp_keepalive_intvl`.
Available from libzmq 3.0.0.

```
string my_zsock.tcpAcceptFilter ()
```

Get socket option `tcp_accept_filter`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setTcpAcceptFilter (String)
```

Set socket option `tcp_accept_filter`.
Available from libzmq 3.0.0.

```
string my_zsock.lastEndpoint ()
```

Get socket option `last_endpoint`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setRouterRaw (Number)
```

Set socket option `router_raw`.
Available from libzmq 3.0.0.

```
integer my_zsock.ipv4only ()
```

Get socket option `ipv4only`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setIpv4only (Number)
```

Set socket option `ipv4only`.
Available from libzmq 3.0.0.

```
nothing my_zsock.setDelayAttachOnConnect (Number)
```

Set socket option `delay_attach_on_connect`.
Available from libzmq 3.0.0.

```
integer my_zsock.hwm ()
```

Get socket option `hwm`.
Available from libzmq 2.0.0 to 3.0.0.

```
nothing my_zsock.setHwm (Number)
```

Set socket option `hwm`.
Available from libzmq 2.0.0 to 3.0.0.

```
integer my_zsock.swap ()
```

Get socket option `swap`.
Available from libzmq 2.0.0 to 3.0.0.

```
nothing my_zsock.setSwap (Number)
```

Set socket option `swap`.
Available from libzmq 2.0.0 to 3.0.0.

```
integer my_zsock.affinity ()
```

Get socket option `affinity`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setAffinity (Number)
```

Set socket option `affinity`.
Available from libzmq 2.0.0.

```
string my_zsock.identity ()
```

Get socket option `identity`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setIdentity (String)
```

Set socket option `identity`.
Available from libzmq 2.0.0.

```
integer my_zsock.rate ()
```

Get socket option `rate`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setRate (Number)
```

Set socket option `rate`.
Available from libzmq 2.0.0.

```
integer my_zsock.recoveryIvl ()
```

Get socket option `recovery_ivl`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setRecoveryIvl (Number)
```

Set socket option `recovery_ivl`.
Available from libzmq 2.0.0.

```
integer my_zsock.recoveryIvlMsec ()
```

Get socket option `recovery_ivl_msec`.
Available from libzmq 2.0.0 to 3.0.0.

```
nothing my_zsock.setRecoveryIvlMsec (Number)
```

Set socket option `recovery_ivl_msec`.
Available from libzmq 2.0.0 to 3.0.0.

```
integer my_zsock.mcastLoop ()
```

Get socket option `mcast_loop`.
Available from libzmq 2.0.0 to 3.0.0.

```
nothing my_zsock.setMcastLoop (Number)
```

Set socket option `mcast_loop`.
Available from libzmq 2.0.0 to 3.0.0.

```
integer my_zsock.rcvtimeo ()
```

Get socket option `rcvtimeo`.
Available from libzmq 2.2.0.

```
nothing my_zsock.setRcvtimeo (Number)
```

Set socket option `rcvtimeo`.
Available from libzmq 2.2.0.

```
integer my_zsock.sndtimeo ()
```

Get socket option `sndtimeo`.
Available from libzmq 2.2.0.

```
nothing my_zsock.setSndtimeo (Number)
```

Set socket option `sndtimeo`.
Available from libzmq 2.2.0.

```
integer my_zsock.sndbuf ()
```

Get socket option `sndbuf`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setSndbuf (Number)
```

Set socket option `sndbuf`.
Available from libzmq 2.0.0.

```
integer my_zsock.rcvbuf ()
```

Get socket option `rcvbuf`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setRcvbuf (Number)
```

Set socket option `rcvbuf`.
Available from libzmq 2.0.0.

```
integer my_zsock.linger ()
```

Get socket option `linger`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setLinger (Number)
```

Set socket option `linger`.
Available from libzmq 2.0.0.

```
integer my_zsock.reconnectIvl ()
```

Get socket option `reconnect_ivl`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setReconnectIvl (Number)
```

Set socket option `reconnect_ivl`.
Available from libzmq 2.0.0.

```
integer my_zsock.reconnectIvlMax ()
```

Get socket option `reconnect_ivl_max`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setReconnectIvlMax (Number)
```

Set socket option `reconnect_ivl_max`.
Available from libzmq 2.0.0.

```
integer my_zsock.backlog ()
```

Get socket option `backlog`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setBacklog (Number)
```

Set socket option `backlog`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setSubscribe (String)
```

Set socket option `subscribe`.
Available from libzmq 2.0.0.

```
nothing my_zsock.setUnsubscribe (String)
```

Set socket option `unsubscribe`.
Available from libzmq 2.0.0.

```
integer my_zsock.type ()
```

Get socket option `type`.
Available from libzmq 2.0.0.

```
integer my_zsock.rcvmore ()
```

Get socket option `rcvmore`.
Available from libzmq 2.0.0.

```
integer my_zsock.events ()
```

Get socket option `events`.
Available from libzmq 2.0.0.

```
nothing my_zsock.test (Boolean)
```

Self test of this class.

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
string my_zstr.recvCompress (Zsock)
```

De-compress and receive C string from socket, received as a message
with two frames: size of the uncompressed string, and the string itself.
Caller must free returned string using zstr_free(). Returns NULL if the
context is being terminated or the process was interrupted.

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
integer my_zstr.sendCompress (Zsock, String)
```

Compress and send a C string to a socket, as a message with two frames:
size of the uncompressed string, and the string itself. The string is
sent without trailing null byte; to read this you can use
zstr_recv_compress, or a similar method that de-compresses and adds a
null terminator on the received string.

```
integer my_zstr.sendmCompress (Zsock, String)
```

Compress and send a C string to a socket, as zstr_send_compress(),
with a MORE flag, so that you can send further strings in the same
multi-part message.

```
string my_zstr.str (Zsock)
```

Accepts a void pointer and returns a fresh character string. If source
is null, returns an empty string.

```
nothing my_zstr.free (String)
```

Free a provided string, and nullify the parent pointer. Safe to call on
a null pointer.

```
nothing my_zstr.test (Boolean)
```

Self test of this class.

### The Zsys class -

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zsys = new czmq.Zsys ()
```

Methods:

```
nothing my_zsys.shutdown ()
```

Optionally shut down the CZMQ zsys layer; this normally happens automatically
when the process exits; however this call lets you force a shutdown
earlier, avoiding any potential problems with atexit() ordering, especially
with Windows dlls.

```
string my_zsys.sockname (Number)
```

Return ZMQ socket name for socket type
*** This is for CZMQ internal use only and may change arbitrarily ***

```
zsock my_zsys.createPipe (Zsock)
```

Create a pipe, which consists of two PAIR sockets connected over inproc.
The pipe is configured to use the zsys_pipehwm setting. Returns the
frontend socket successful, NULL if failed.

```
nothing my_zsys.handlerReset ()
```

Reset interrupt handler, call this at exit if needed

```
nothing my_zsys.catchInterrupts ()
```

Set default interrupt handler, so Ctrl-C or SIGTERM will set
zsys_interrupted. Idempotent; safe to call multiple times.
Can be suppressed by ZSYS_SIGHANDLER=false
*** This is for CZMQ internal use only and may change arbitrarily ***

```
boolean my_zsys.isInterrupted ()
```

Check if default interrupt handler of Ctrl-C or SIGTERM was called.
Does not work if ZSYS_SIGHANDLER is false and code does not call
set interrupted on signal.

```
nothing my_zsys.setInterrupted ()
```

Set interrupted flag. This is done by default signal handler, however
this can be handy for language bindings or cases without default
signal handler.

```
boolean my_zsys.fileExists (String)
```

Return 1 if file exists, else zero

```
time my_zsys.fileModified (String)
```

Return file modification time. Returns 0 if the file does not exist.

```
integer my_zsys.fileMode (String)
```

Return file mode; provides at least support for the POSIX S_ISREG(m)
and S_ISDIR(m) macros and the S_IRUSR and S_IWUSR bits, on all boxes.
Returns a mode_t cast to int, or -1 in case of error.

```
integer my_zsys.fileDelete (String)
```

Delete file. Does not complain if the file is absent

```
boolean my_zsys.fileStable (String)
```

Check if file is 'stable'

```
integer my_zsys.dirCreate (String)
```

Create a file path if it doesn't exist. The file path is treated as
printf format.

```
integer my_zsys.dirDelete (String)
```

Remove a file path if empty; the pathname is treated as printf format.

```
integer my_zsys.dirChange (String)
```

Move to a specified working directory. Returns 0 if OK, -1 if this failed.

```
nothing my_zsys.fileModePrivate ()
```

Set private file creation mode; all files created from here will be
readable/writable by the owner only.

```
nothing my_zsys.fileModeDefault ()
```

Reset default file creation mode; all files created from here will use
process file mode defaults.

```
nothing my_zsys.version (Number, Number, Number)
```

Return the CZMQ version for run-time API detection; returns version
number into provided fields, providing reference isn't null in each case.

```
string my_zsys.sprintfHint (Number, String)
```

Format a string using printf formatting, returning a freshly allocated
buffer. If there was insufficient memory, returns NULL. Free the returned
string using zstr_free(). The hinted version allows one to optimize by using
a larger starting buffer size (known to/assumed by the developer) and so
avoid reallocations.

```
string my_zsys.sprintf (String)
```

Format a string using printf formatting, returning a freshly allocated
buffer. If there was insufficient memory, returns NULL. Free the returned
string using zstr_free().

```
nothing my_zsys.socketError (String)
```

Handle an I/O error on some socket operation; will report and die on
fatal errors, and continue silently on "try again" errors.
*** This is for CZMQ internal use only and may change arbitrarily ***

```
string my_zsys.hostname ()
```

Return current host name, for use in public tcp:// endpoints. Caller gets
a freshly allocated string, should free it using zstr_free(). If the host
name is not resolvable, returns NULL.

```
integer my_zsys.daemonize (String)
```

Move the current process into the background. The precise effect depends
on the operating system. On POSIX boxes, moves to a specified working
directory (if specified), closes all file handles, reopens stdin, stdout,
and stderr to the null device, and sets the process to ignore SIGHUP. On
Windows, does nothing. Returns 0 if OK, -1 if there was an error.

```
integer my_zsys.runAs (String, String, String)
```

Drop the process ID into the lockfile, with exclusive lock, and switch
the process to the specified group and/or user. Any of the arguments
may be null, indicating a no-op. Returns 0 on success, -1 on failure.
Note if you combine this with zsys_daemonize, run after, not before
that method, or the lockfile will hold the wrong process ID.

```
boolean my_zsys.hasCurve ()
```

Returns true if the underlying libzmq supports CURVE security.
Uses a heuristic probe according to the version of libzmq being used.

```
nothing my_zsys.setIoThreads ()
```

Configure the number of I/O threads that ZeroMQ will use. A good
rule of thumb is one thread per gigabit of traffic in or out. The
default is 1, sufficient for most applications. If the environment
variable ZSYS_IO_THREADS is defined, that provides the default.
Note that this method is valid only before any socket is created.

```
nothing my_zsys.setThreadSchedPolicy (Number)
```

Configure the scheduling policy of the ZMQ context thread pool.
Not available on Windows. See the sched_setscheduler man page or sched.h
for more information. If the environment variable ZSYS_THREAD_SCHED_POLICY
is defined, that provides the default.
Note that this method is valid only before any socket is created.

```
nothing my_zsys.setThreadPriority (Number)
```

Configure the scheduling priority of the ZMQ context thread pool.
Not available on Windows. See the sched_setscheduler man page or sched.h
for more information. If the environment variable ZSYS_THREAD_PRIORITY is
defined, that provides the default.
Note that this method is valid only before any socket is created.

```
nothing my_zsys.setThreadNamePrefix (Number)
```

Configure the numeric prefix to each thread created for the internal
context's thread pool. This option is only supported on Linux.
If the environment variable ZSYS_THREAD_NAME_PREFIX is defined, that
provides the default.
Note that this method is valid only before any socket is created.

```
integer my_zsys.threadNamePrefix ()
```

Return thread name prefix.

```
nothing my_zsys.setThreadNamePrefixStr (String)
```

Configure the numeric prefix to each thread created for the internal
context's thread pool. This option is only supported on Linux.
If the environment variable ZSYS_THREAD_NAME_PREFIX_STR is defined, that
provides the default.
Note that this method is valid only before any socket is created.

```
string my_zsys.threadNamePrefixStr ()
```

Return thread name prefix.

```
nothing my_zsys.threadAffinityCpuAdd (Number)
```

Adds a specific CPU to the affinity list of the ZMQ context thread pool.
This option is only supported on Linux.
Note that this method is valid only before any socket is created.

```
nothing my_zsys.threadAffinityCpuRemove (Number)
```

Removes a specific CPU to the affinity list of the ZMQ context thread pool.
This option is only supported on Linux.
Note that this method is valid only before any socket is created.

```
nothing my_zsys.setMaxSockets ()
```

Configure the number of sockets that ZeroMQ will allow. The default
is 1024. The actual limit depends on the system, and you can query it
by using zsys_socket_limit (). A value of zero means "maximum".
Note that this method is valid only before any socket is created.

```
size my_zsys.socketLimit ()
```

Return maximum number of ZeroMQ sockets that the system will support.

```
nothing my_zsys.setMaxMsgsz (Number)
```

Configure the maximum allowed size of a message sent.
The default is INT_MAX.

```
integer my_zsys.maxMsgsz ()
```

Return maximum message size.

```
nothing my_zsys.setZeroCopyRecv (Number)
```

Configure whether to use zero copy strategy in libzmq. If the environment
variable ZSYS_ZERO_COPY_RECV is defined, that provides the default.
Otherwise the default is 1.

```
integer my_zsys.zeroCopyRecv ()
```

Return ZMQ_ZERO_COPY_RECV option.

```
nothing my_zsys.setFileStableAgeMsec (Number)
```

Configure the threshold value of filesystem object age per st_mtime
that should elapse until we consider that object "stable" at the
current zclock_time() moment.
The default is S_DEFAULT_ZSYS_FILE_STABLE_AGE_MSEC defined in zsys.c
which generally depends on host OS, with fallback value of 5000.

```
msecs my_zsys.fileStableAgeMsec ()
```

Return current threshold value of file stable age in msec.
This can be used in code that chooses to wait for this timeout
before testing if a filesystem object is "stable" or not.

```
nothing my_zsys.setLinger ()
```

Configure the default linger timeout in msecs for new zsock instances.
You can also set this separately on each zsock_t instance. The default
linger time is zero, i.e. any pending messages will be dropped. If the
environment variable ZSYS_LINGER is defined, that provides the default.
Note that process exit will typically be delayed by the linger time.

```
nothing my_zsys.setSndhwm ()
```

Configure the default outgoing pipe limit (HWM) for new zsock instances.
You can also set this separately on each zsock_t instance. The default
HWM is 1,000, on all versions of ZeroMQ. If the environment variable
ZSYS_SNDHWM is defined, that provides the default. Note that a value of
zero means no limit, i.e. infinite memory consumption.

```
nothing my_zsys.setRcvhwm ()
```

Configure the default incoming pipe limit (HWM) for new zsock instances.
You can also set this separately on each zsock_t instance. The default
HWM is 1,000, on all versions of ZeroMQ. If the environment variable
ZSYS_RCVHWM is defined, that provides the default. Note that a value of
zero means no limit, i.e. infinite memory consumption.

```
nothing my_zsys.setPipehwm ()
```

Configure the default HWM for zactor internal pipes; this is set on both
ends of the pipe, for outgoing messages only (sndhwm). The default HWM is
1,000, on all versions of ZeroMQ. If the environment var ZSYS_ACTORHWM is
defined, that provides the default. Note that a value of zero means no
limit, i.e. infinite memory consumption.

```
size my_zsys.pipehwm ()
```

Return the HWM for zactor internal pipes.

```
nothing my_zsys.setIpv6 (Number)
```

Configure use of IPv6 for new zsock instances. By default sockets accept
and make only IPv4 connections. When you enable IPv6, sockets will accept
and connect to both IPv4 and IPv6 peers. You can override the setting on
each zsock_t instance. The default is IPv4 only (ipv6 set to 0). If the
environment variable ZSYS_IPV6 is defined (as 1 or 0), this provides the
default. Note: has no effect on ZMQ v2.

```
integer my_zsys.ipv6 ()
```

Return use of IPv6 for zsock instances.

```
boolean my_zsys.ipv6Available ()
```

Test if ipv6 is available on the system. Return true if available.
The only way to reliably check is to actually open a socket and
try to bind it. (ported from libzmq)

```
nothing my_zsys.setInterface (String)
```

Set network interface name to use for broadcasts, particularly zbeacon.
This lets the interface be configured for test environments where required.
For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is
the default when there is no specified interface. If the environment
variable ZSYS_INTERFACE is set, use that as the default interface name.
Setting the interface to "*" means "use all available interfaces".

```
string my_zsys.interface ()
```

Return network interface to use for broadcasts, or "" if none was set.

```
nothing my_zsys.setIpv6Address (String)
```

Set IPv6 address to use zbeacon socket, particularly for receiving zbeacon.
This needs to be set IPv6 is enabled as IPv6 can have multiple addresses
on a given interface. If the environment variable ZSYS_IPV6_ADDRESS is set,
use that as the default IPv6 address.

```
string my_zsys.ipv6Address ()
```

Return IPv6 address to use for zbeacon reception, or "" if none was set.

```
nothing my_zsys.setIpv6McastAddress (String)
```

Set IPv6 milticast address to use for sending zbeacon messages. This needs
to be set if IPv6 is enabled. If the environment variable
ZSYS_IPV6_MCAST_ADDRESS is set, use that as the default IPv6 multicast
address.

```
string my_zsys.ipv6McastAddress ()
```

Return IPv6 multicast address to use for sending zbeacon, or "" if none was
set.

```
nothing my_zsys.setIpv4McastAddress (String)
```

Set IPv4 multicast address to use for sending zbeacon messages. By default
IPv4 multicast is NOT used. If the environment variable
ZSYS_IPV4_MCAST_ADDRESS is set, use that as the default IPv4 multicast
address. Calling this function or setting ZSYS_IPV4_MCAST_ADDRESS
will enable IPv4 zbeacon messages.

```
string my_zsys.ipv4McastAddress ()
```

Return IPv4 multicast address to use for sending zbeacon, or NULL if none was
set.

```
nothing my_zsys.setAutoUseFd (Number)
```

Configure the automatic use of pre-allocated FDs when creating new sockets.
If 0 (default), nothing will happen. Else, when a new socket is bound, the
system API will be used to check if an existing pre-allocated FD with a
matching port (if TCP) or path (if IPC) exists, and if it does it will be
set via the ZMQ_USE_FD socket option so that the library will use it
instead of creating a new socket.

```
integer my_zsys.autoUseFd ()
```

Return use of automatic pre-allocated FDs for zsock instances.

```
string my_zsys.zprintf (String, Zhash)
```

Print formatted string. Format is specified by variable names
in Python-like format style

"%(KEY)s=%(VALUE)s", KEY=key, VALUE=value
become
"key=value"

Returns freshly allocated string or NULL in a case of error.
Not enough memory, invalid format specifier, name not in args

```
string my_zsys.zprintfError (String, Zhash)
```

Return error string for given format/args combination.

```
string my_zsys.zplprintf (String, Zconfig)
```

Print formatted string. Format is specified by variable names
in Python-like format style

"%(KEY)s=%(VALUE)s", KEY=key, VALUE=value
become
"key=value"

Returns freshly allocated string or NULL in a case of error.
Not enough memory, invalid format specifier, name not in args

```
string my_zsys.zplprintfError (String, Zconfig)
```

Return error string for given format/args combination.

```
nothing my_zsys.setLogident (String)
```

Set log identity, which is a string that prefixes all log messages sent
by this process. The log identity defaults to the environment variable
ZSYS_LOGIDENT, if that is set.

```
nothing my_zsys.setLogsender (String)
```

Sends log output to a PUB socket bound to the specified endpoint. To
collect such log output, create a SUB socket, subscribe to the traffic
you care about, and connect to the endpoint. Log traffic is sent as a
single string frame, in the same format as when sent to stdout. The
log system supports a single sender; multiple calls to this method will
bind the same sender to multiple endpoints. To disable the sender, call
this method with a null argument.

```
nothing my_zsys.setLogsystem (Boolean)
```

Enable or disable logging to the system facility (syslog on POSIX boxes,
event log on Windows). By default this is disabled.

```
nothing my_zsys.error (String)
```

Log error condition - highest priority

```
nothing my_zsys.warning (String)
```

Log warning condition - high priority

```
nothing my_zsys.notice (String)
```

Log normal, but significant, condition - normal priority

```
nothing my_zsys.info (String)
```

Log informational message - low priority

```
nothing my_zsys.debug (String)
```

Log debug-level message - lowest priority

```
nothing my_zsys.test (Boolean)
```

Self test of this class.

### The Ztimerset class - timer set

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_ztimerset = new czmq.Ztimerset ()
```

You *must* call the destructor on every Ztimerset instance:

```
my_ztimerset.destroy ()
```

Methods:

```
integer my_ztimerset.cancel (Number)
```

Cancel a timer. Returns 0 if OK, -1 on failure.

```
integer my_ztimerset.setInterval (Number)
```

Set timer interval. Returns 0 if OK, -1 on failure.
This method is slow, canceling the timer and adding a new one yield better performance.

```
integer my_ztimerset.reset (Number)
```

Reset timer to start interval counting from current time. Returns 0 if OK, -1 on failure.
This method is slow, canceling the timer and adding a new one yield better performance.

```
integer my_ztimerset.timeout ()
```

Return the time until the next interval.
Should be used as timeout parameter for the zpoller wait method.
The timeout is in msec.

```
integer my_ztimerset.execute ()
```

Invoke callback function of all timers which their interval has elapsed.
Should be call after zpoller wait method.
Returns 0 if OK, -1 on failure.

```
nothing my_ztimerset.test (Boolean)
```

Self test of this class.

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

```
nothing my_ztrie.print ()
```

Print the trie

```
nothing my_ztrie.test (Boolean)
```

Self test of this class.

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
nothing my_zuuid.set (String)
```

Set UUID to new supplied ZUUID_LEN-octet value.

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
nothing my_zuuid.export (String)
```

Store UUID blob in target array

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

```
nothing my_zuuid.test (Boolean)
```

Self test of this class.

### The ZhttpClient class - Http client, allowing multiple requests simultaneously and integrate easily with zpoller.
Use zhttp_request class to create and send the request.
Use zhttp_response class to receive the response.

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zhttp_client = new czmq.ZhttpClient (Boolean)
```

You *must* call the destructor on every ZhttpClient instance:

```
my_zhttp_client.destroy ()
```

Methods:

```
nothing my_zhttp_client.test (Boolean)
```

Self test of this class.

### The ZhttpServer class - Simple http server.
To start handling requests:
1. Create a dealer socket
2. Connect the socket to the server backend address provided in the options.
3. Create a zhttp_request.
4. Call zhttp_request_recv to accept a new request.
5. Call zhttp_response_send to send a response.

You can connect as many dealers as you want.
The Server is using simple dealer socket to route the requests.

NOTE: when using libmicrohttpd << 0.9.34 the application might experience
high CPU usage due to the lack of MHD_suspend_connection and
MHD_resume_connection APIs. It is recommended to use this class only with
libmicrohttpd at least 0.9.34 in production.

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zhttp_server = new czmq.ZhttpServer (ZhttpServerOptions)
```

You *must* call the destructor on every ZhttpServer instance:

```
my_zhttp_server.destroy ()
```

Methods:

```
integer my_zhttp_server.port ()
```

Return the port the server is listening on.

```
nothing my_zhttp_server.test (Boolean)
```

Self test of this class.

### The ZhttpServerOptions class - zhttp server.

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zhttp_server_options = new czmq.ZhttpServerOptions ()
```

You *must* call the destructor on every ZhttpServerOptions instance:

```
my_zhttp_server_options.destroy ()
```

Methods:

```
integer my_zhttp_server_options.port ()
```

Get the server listening port.

```
nothing my_zhttp_server_options.setPort (Number)
```

Set the server listening port

```
string my_zhttp_server_options.backendAddress ()
```

Get the address sockets should connect to in order to receive requests.

```
nothing my_zhttp_server_options.setBackendAddress (String)
```

Set the address sockets should connect to in order to receive requests.

```
nothing my_zhttp_server_options.test (Boolean)
```

Self test of this class.

### The ZhttpRequest class - Http request that can be received from zhttp_server or sent to zhttp_client.
Class can be reused between send & recv calls.
Headers and Content is being destroyed after every send call.

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zhttp_request = new czmq.ZhttpRequest ()
```

You *must* call the destructor on every ZhttpRequest instance:

```
my_zhttp_request.destroy ()
```

Methods:

```
string my_zhttp_request.method ()
```

Get the request method

```
nothing my_zhttp_request.setMethod (String)
```

Set the request method

```
string my_zhttp_request.url ()
```

Get the request url.
When receiving a request from http server this is only the path part of the url.

```
nothing my_zhttp_request.setUrl (String)
```

Set the request url
When sending a request to http client this should be full url.

```
string my_zhttp_request.contentType ()
```

Get the request content type

```
nothing my_zhttp_request.setContentType (String)
```

Set the request content type

```
size my_zhttp_request.contentLength ()
```

Get the content length of the request

```
zhash my_zhttp_request.headers ()
```

Get the headers of the request

```
string my_zhttp_request.content ()
```

Get the content of the request.

```
string my_zhttp_request.getContent ()
```

Get the content of the request.

```
nothing my_zhttp_request.setContent (String)
```

Set the content of the request.
Content must by dynamically allocated string.
Takes ownership of the content.

```
nothing my_zhttp_request.setContentConst (String)
```

Set the content of the request..
The content is assumed to be constant-memory and will therefore not be copied or deallocated in any way.

```
nothing my_zhttp_request.resetContent ()
```

Set the content to NULL

```
nothing my_zhttp_request.setUsername (String)
```

Set the request username

```
nothing my_zhttp_request.setPassword (String)
```

Set the request password

```
boolean my_zhttp_request.match (String, String)
```

Match the path of the request.
Support wildcards with '%s' symbol inside the match string.
Matching wildcards until the next '/', '?' or '\0'.
On successful match the variadic arguments will be filled with the matching strings.
On successful match the method is modifying the url field and break it into substrings.
If you need to use the url, do it before matching or take a copy.

User must not free the variadic arguments as they are part of the url.

To use the percent symbol, just double it, e.g "%%something".

Example:
if (zhttp_request_match (request, "POST", "/send/%s/%s", &name, &id))

```
nothing my_zhttp_request.test (Boolean)
```

Self test of this class.

### The ZhttpResponse class - Http response that can be received from zhttp_client or sent to zhttp_server.
Class can be reused between send & recv calls.
Headers and Content is being destroyed after every send call.

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zhttp_response = new czmq.ZhttpResponse ()
```

You *must* call the destructor on every ZhttpResponse instance:

```
my_zhttp_response.destroy ()
```

Methods:

```
string my_zhttp_response.contentType ()
```

Get the response content type

```
nothing my_zhttp_response.setContentType (String)
```

Set the content type of the response.

```
number my_zhttp_response.statusCode ()
```

Get the status code of the response.

```
nothing my_zhttp_response.setStatusCode (Number)
```

Set the status code of the response.

```
zhash my_zhttp_response.headers ()
```

Get the headers of the response.

```
size my_zhttp_response.contentLength ()
```

Get the content length of the response

```
string my_zhttp_response.content ()
```

Get the content of the response.

```
string my_zhttp_response.getContent ()
```

Get the content of the response.

```
nothing my_zhttp_response.setContent (String)
```

Set the content of the response.
Content must by dynamically allocated string.
Takes ownership of the content.

```
nothing my_zhttp_response.setContentConst (String)
```

Set the content of the response.
The content is assumed to be constant-memory and will therefore not be copied or deallocated in any way.

```
nothing my_zhttp_response.resetContent ()
```

Set the content to NULL

```
nothing my_zhttp_response.test (Boolean)
```

Self test of this class.

### The Zosc class - Create and decode Open Sound Control messages. (OSC)

OSC is a serialisation format (and usually transported over UDP) which is
supported by many applications and appliances. It is a de facto protocol
for networking sound synthesizers, computers, and other multimedia devices
for purposes such as musical performance or show control. It is also often
used for rapid prototyping purposes due to the support by many applications
and frameworks in this field. With ZeroMQ's DGRAM sockets it is possible
to use ZeroMQ to send and receive OSC messages which can be understood by
any device supporting OSC.

Example creating an OSC message:

    zosc_t* conm = zosc_create("/someaddress", "iihfdscF",
                        1, 2, 3, 3.14, 6.283185307179586, "greetings", 'q');

Decoding a message:

    int rc = zosc_retr(oscmsg, "iihfdscF", &intx, &inty, &intz, &floatz,
                        &doublez, &strings, &charq, &someBool);

See the class's test method for more examples how to use the class.

Constructor:

```
var czmq = require ('bindings')('czmq')
var my_zosc = new czmq.Zosc (String)
```

You *must* call the destructor on every Zosc instance:

```
my_zosc.destroy ()
```

Methods:

```
size my_zosc.size ()
```

Return chunk data size

```
buffer my_zosc.data ()
```

Return OSC chunk data. Caller does not own the data!

```
string my_zosc.address ()
```

Return the OSC address string

```
string my_zosc.format ()
```

Return the OSC format of the message.
  i - 32bit integer
  h - 64bit integer
  f - 32bit floating point number (IEEE)
  d - 64bit (double) floating point number
  s - string (NULL terminated)
  t = timetag: an OSC timetag in NTP format (uint64_t)
  S - symbol
  c - char
  m - 4 byte midi packet (8 digits hexadecimal)
  T - TRUE (no value required)
  F - FALSE (no value required)
  N - NIL (no value required)
  I - Impulse (for triggers) or INFINITUM (no value required)
  b - binary blob

```
integer my_zosc.append (String)
```

Append data to the osc message. The format describes the data that
needs to be appended in the message. This essentially relocates all
data!
The format type tags are as follows:
  i - 32bit integer
  h - 64bit integer
  f - 32bit floating point number (IEEE)
  d - 64bit (double) floating point number
  s - string (NULL terminated)
  t = timetag: an OSC timetag in NTP format (uint64_t)
  S - symbol
  c - char
  m - 4 byte midi packet (8 digits hexadecimal)
  T - TRUE (no value required)
  F - FALSE (no value required)
  N - NIL (no value required)
  I - Impulse (for triggers) or INFINITUM (no value required)
  b - binary blob

```
integer my_zosc.retr (String)
```

Retrieve the values provided by the given format. Note that zosc_retr
creates the objects and the caller must destroy them when finished.
The supplied pointers do not need to be initialized. Returns 0 if
successful, or -1 if it failed to retrieve a value in which case the
pointers are not modified. If an argument pointer is NULL is skips the
value. See the format method for a detailed list op type tags for the
format string.

```
zosc my_zosc.dup ()
```

Create copy of the message, as new chunk object. Returns a fresh zosc_t
object, or null if there was not enough heap memory. If chunk is null,
returns null.

```
zframe my_zosc.pack ()
```

Transform zosc into a zframe that can be sent in a message.

```
zframe my_zosc.packx (Zosc)
```

Transform zosc into a zframe that can be sent in a message.
Take ownership of the chunk.

```
zosc my_zosc.unpack (Zframe)
```

Transform a zframe into a zosc.

```
string my_zosc.dump ()
```

Return a string describing the the OSC message. The returned string must be freed by the caller.

```
nothing my_zosc.print ()
```

Dump OSC message to stdout, for debugging and tracing.

```
integer my_zosc.popInt32 (Number)
```

Set the provided 32 bit integer from value at the current cursor position in the message.
If the type tag at the current position does not correspond it will fail and
return -1. Returns 0 on success.

```
integer my_zosc.popInt64 (Number)
```

Set the provided 64 bit integer from the value at the current cursor position in the message.
If the type tag at the current position does not correspond it will fail and
return -1. Returns 0 on success.

```
integer my_zosc.popFloat (Number)
```

Set the provided float from the value at the current cursor position in the message.
If the type tag at the current position does not correspond it will fail and
return -1. Returns 0 on success.

```
integer my_zosc.popDouble (Number)
```

Set the provided double from the value at the current cursor position in the message.
If the type tag at the current position does not correspond it will fail and
return -1. Returns 0 on success.

```
integer my_zosc.popString (String)
```

Set the provided string from the value at the current cursor position in the message.
If the type tag at the current position does not correspond it will fail and
return -1. Returns 0 on success. Caller owns the string!

```
integer my_zosc.popChar (String)
```

Set the provided char from the value at the current cursor position in the message.
If the type tag at the current position does not correspond it will fail and
return -1. Returns 0 on success.

```
integer my_zosc.popBool (Boolean)
```

Set the provided boolean from the type tag in the message. Booleans are not represented
in the data in the message, only in the type tag. If the type tag at the current
position does not correspond it will fail and return -1. Returns 0 on success.

```
integer my_zosc.popMidi (Number)
```

Set the provided 4 bytes (unsigned 32bit int) from the value at the current
cursor position in the message. If the type tag at the current position does
not correspond it will fail and return -1. Returns 0 on success.

```
nothing my_zosc.test (Boolean)
```

Self test of this class.
