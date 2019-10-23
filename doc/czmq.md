czmq(7)
=======


NAME
----
czmq - high-level C binding for ZeroMQ


SYNOPSIS
--------
----
#include <czmq.h>

cc ['flags'] 'files' -lzmq -lczmq ['libraries']
----


DESCRIPTION
-----------

Classes
~~~~~~~

These classes provide the main socket and message API:

* linkczmq:zsock[3] - working with ZeroMQ sockets (high-level)
* linkczmq:zstr[3] - sending and receiving strings
* linkczmq:zmsg[3] - working with multipart messages
* linkczmq:zframe[3] - working with single message frames
* linkczmq:zactor[3] - Actor class (socket + thread)
* linkczmq:zloop[3] - event-driven reactor
* linkczmq:zpoller[3] - trivial socket poller class
* linkczmq:zproxy[3] - proxy actor (like zmq_proxy_steerable)
* linkczmq:zmonitor[3] - monitor events on ZeroMQ sockets

These classes support authentication and encryption:

* linkczmq:zauth[3] - authentication actor for ZeroMQ servers
* linkczmq:zcert[3] - work with CURVE security certificates
* linkczmq:zcertstore[3] - work with CURVE security certificate stores

These classes provide generic containers:

* linkczmq:zhash[3] - simple generic hash container
* linkczmq:zhashx[3] - extended generic hash container
* linkczmq:zlist[3] - simple generic list container
* linkczmq:zlistx[3] - extended generic list container

These classes wrap-up non-portable functionality:

* linkczmq:zbeacon[3] - LAN discovery and presence
* linkczmq:zclock[3] - millisecond clocks and delays
* linkczmq:zdir[3] - work with file-system directories
* linkczmq:zdir_patch[3] - work with directory differences
* linkczmq:zfile[3] - work with file-system files
* linkczmq:zsys[3] - system-level methods
* linkczmq:zuuid[3] - UUID support class
* linkczmq:ziflist[3] - list available network interfaces

And these utility classes add value:

* linkczmq:zchunk[3] - work with memory chunks
* linkczmq:zconfig[3] - work with textual config files
* linkczmq:zrex[3] - work with regular expressions
* linkczmq:zgossip[3] - decentralized configuration management

Scope and Goals
~~~~~~~~~~~~~~~

CZMQ has these goals:

* To wrap the ØMQ core API in semantics that are natural and lead to shorter, more readable applications.
* To hide the differences between versions of ØMQ.
* To provide a space for development of more sophisticated API semantics.

Ownership and License
~~~~~~~~~~~~~~~~~~~~~

CZMQ is maintained by the ZeroMQ community at github.com/zeromq. Its other authors and contributors are listed in the AUTHORS file.

The contributors are listed in AUTHORS. This project uses the MPL v2 license, see LICENSE.

Contributing
~~~~~~~~~~~~

To submit an issue use the issue tracker at http://github.com/zeromq/czmq/issues. All discussion happens on the  zeromq-dev list or #zeromq IRC channel at irc.freenode.net.

The proper way to submit patches is to clone this repository, make your changes, and use git to create a patch or a pull request. See http://www.zeromq.org/docs:contributing. All contributors are listed in AUTHORS.

All classes are maintained by a single person, who is the responsible editor for that class and who is named in the header as such. This is usually the originator of the class. When several people collaborate on a class, one single person is always the lead maintainer and the one to blame when it breaks.

The general rule is, if you contribute code to CZMQ you must be willing to maintain it as long as there are users of it. Code with no active maintainer will in general be deprecated and/or removed.

USING CZMQ
----------

Building and Installing
~~~~~~~~~~~~~~~~~~~~~~~

CZMQ uses autotools for packaging. To build from git (all example commands are for Linux):
----
    git clone git://github.com/zeromq/czmq.git
    cd czmq
    sh autogen.sh
    ./configure
    make all
    sudo make install
    sudo ldconfig
----

You will need the pkg-config, libtool, and autoreconf packages. Set the LD_LIBRARY_PATH to /usr/local/libs unless you install elsewhere.

After building, you can run the CZMQ selftests:
----
    cd src
    ./czmq_selftest
----

Linking with an Application
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Include `czmq.h` in your application and link with CZMQ. Here is a typical gcc link command:
----
    gcc -lczmq -lzmq myapp.c -o myapp
----

You should read `czmq.h`. This file includes `zmq.h` and the system header files that typical ØMQ applications will need. The provided 'c' shell script lets you write simple portable build scripts:
----
    c -lczmq -lzmq -l myapp
----

The Class Model
~~~~~~~~~~~~~~~

CZMQ consists of classes, each class consisting of a .h and a .c. Classes may depend on other classes.

`czmq.h` includes all classes header files, all the time. For the user, CZMQ forms one single package. All classes start by including `czmq.h`. All applications that use CZMQ start by including `czmq.h`. `czmq.h` also defines a limited number of small, useful macros and typedefs that have proven useful for writing clearer C code.

All classes (with some exceptions) are based on a flat C class system and follow these rules (where 'zclass' is the class name):

* Class typedef: `zclass_t`
* Constructor: `zclass_new`
* Destructor: `zclass_destroy`
* Property methods: `zclass_property_set`, `zclass_property`
* Class structures are private (defined in the .c source but not the .h)
* Properties are accessed only via methods named as described above.
* In the class source code the object is always called `self`.
* The constructor may take arbitrary arguments, and returns NULL on failure, or a new object.
* The destructor takes a pointer to an object reference and nullifies it.

Return values for methods are:

* For methods that return an object reference, either the reference, or NULL on failure.
* For methods that signal success/failure, a return value of 0 means success, -1 failure.

Private/static functions in a class are named `s_functionname` and are not exported via the header file.

All classes (with some exceptions) have a test method called `zclass_test`.

Design Ideology
---------------

The Problem with C
~~~~~~~~~~~~~~~~~~

C has the significant advantage of being a small language that, if we take a little care with formatting and naming, can be easily interchanged between developers. Every C developer will use much the same 90% of the language. Larger languages like C++ provide powerful abstractions like STL containers but at the cost of interchange. 

The huge problem with C is that any realistic application needs packages of functionality to bring the language up to the levels we expect for the 21st century. Much can be done by using external libraries but every additional library is a dependency that makes the resulting applications harder to build and port. While C itself is a highly portable language (and can be made more so by careful use of the preprocessor), most C libraries consider themselves part of the operating system, and as such do not attempt to be portable.

The answer to this, as we learned from building enterprise-level C applications at iMatix from 1995-2005, is to create our own fully portable, high-quality libraries of pre-packaged functionality, in C. Doing this right solves both the requirements of richness of the language, and of portability of the final applications.

A Simple Class Model
~~~~~~~~~~~~~~~~~~~~

C has no standard API style. It is one thing to write a useful component, but something else to provide an API that is consistent and obvious across many components. We learned from building OpenAMQ (http://www.openamq.org), a messaging client and server of 0.5M LoC, that a consistent model for extending C makes life for the application developer much easier.

The general model is that of a class (the source package) that provides objects (in fact C structures). The application creates objects and then works with them. When done, the application destroys the object. In C, we tend to use the same name for the object as the class, when we can, and it looks like this (to take a fictitious CZMQ class):
----
    zregexp_t *regexp = zregexp_new (regexp_string);
    if (!regexp)
        printf ("E: invalid regular expression: %s\n", regexp_string);
    else
    if (zregexp_match (regexp, input_buffer))
        printf ("I: successful match for %s\n", input buffer);
    zregexp_destroy (&amp;regexp);
----

As far as the C program is concerned, the object is a reference to a structure (not a void pointer). We pass the object reference to all methods, since this is still C. We could do weird stuff like put method addresses into the structure so that we can emulate a C++ syntax but it's not worthwhile. The goal is not to emulate an OO system, it's simply to gain consistency. The constructor returns an object reference, or NULL if it fails. The destructor nullifies the class pointer, and is idempotent.

What we aim at here is the simplest possible consistent syntax.

No model is fully consistent, and classes can define their own rules if it helps make a better result. For example:

* Some classes may not be opaque. For example, we have cases of generated serialization classes that encode and decode structures to/from binary buffers. It feels clumsy to have to use methods to access the properties of these classes.

* While every class has a new method that is the formal constructor, some methods may also act as constructors. For example, a "dup" method might take one object and return a second object.

* While every class has a destroy method that is the formal destructor, some methods may also act as destructors. For example, a method that sends an object may also destroy the object (so that ownership of any buffers can passed to background threads). Such methods take the same "pointer to a reference" argument as the destroy method.

Naming Style
~~~~~~~~~~~~

CZMQ aims for short, consistent names, following the theory that names we use most often should be shortest. Classes get one-word names, unless they are part of a family of classes in which case they may be two words, the first being the family name. Methods, similarly, get one-word names and we aim for consistency across classes (so a method that does something semantically similar in two classes will get the same name in both). So the canonical name for any method is:
----
    zclassname_methodname
----

And the reader can easily parse this without needing special syntax to separate the class name from the method name.

Containers
~~~~~~~~~~

After a long experiment with containers, we've decided that we need exactly two containers:

* A singly-linked list.
* A hash table using text keys.

These are zlist and zhash, respectively. Both store void pointers, with no attempt to manage the details of contained objects. You can use these containers to create lists of lists, hashes of lists, hashes of hashes, etc.

We assume that at some point we'll need to switch to a doubly-linked list.

Portability
~~~~~~~~~~~

Creating a portable C application can be rewarding in terms of maintaining a single code base across many platforms, and keeping (expensive) system-specific knowledge separate from application developers. In most projects (like ØMQ core), there is no portability layer and application code does conditional compilation for all mixes of platforms. This leads to quite messy code.

czmq acts as a portability layer, similar to but thinner than libraries like the [Apache Portable Runtime](http://apr.apache.org) (APR).

These are the places a C application is subject to arbitrary system differences:

* Different compilers may offer slightly different variants of the C language, often lacking specific types or using neat non-portable names. Windows is a big culprit here. We solve this by 'patching' the language in czmq_prelude.h, e.g. defining int64_t on Windows.
* System header files are inconsistent, i.e. you need to include different files depending on the OS type and version. We solve this by pulling in all necessary header files in czmq_prelude.h. This is a proven brute-force approach that increases recompilation times but eliminates a major source of pain.
* System libraries are inconsistent, i.e. you need to link with different libraries depending on the OS type and version. We solve this with an external compilation tool, 'C', which detects the OS type and version (at runtime) and builds the necessary link commands.
* System functions are inconsistent, i.e. you need to call different functions depending, again, on OS type and version. We solve this by building small abstract classes that handle specific areas of functionality, and doing conditional compilation in these.

An example of the last:
----
    #if (defined (__UNIX__))
        pid = GetCurrentProcessId();
    #elif (defined (__WINDOWS__))
        pid = getpid ();
    #else
        pid = 0;
    #endif
----

CZMQ uses the GNU autotools system, so non-portable code can use the macros this defines. It can also use macros defined by the czmq_prelude.h header file.

Technical Aspects
~~~~~~~~~~~~~~~~~

* *Thread safety*: the use of opaque structures is thread safe, though ØMQ applications should not share state between threads in any case.
* *Name spaces*: we prefix class names with `z`, which ensures that all exported functions are globally safe.
* *Library versioning*: we don't make any attempt to version the library at this stage. Classes are in our experience highly stable once they are built and tested, the only changes typically being added methods.
* *Performance*: for critical path processing, you may want to avoid creating and destroying classes. However on modern Linux systems the heap allocator is very fast. Individual classes can choose whether or not to nullify their data on allocation.
* *Self-testing*: every class has a `selftest` method that runs through the methods of the class. In theory, calling all selftest functions of all classes does a full unit test of the library. The `czmq_selftest` application does this.
* *Memory management*: CZMQ classes do not use any special memory management techiques to detect leaks. We've done this in the past but it makes the code relatively complex. Instead, we do memory leak testing using tools like valgrind.

Under the Hood
--------------

Adding a New Class
~~~~~~~~~~~~~~~~~~

If you define a new CZMQ class `myclass` you need to:

* Write the `zmyclass.c` and `zmyclass.h` source files, in `src` and `include` respectively.
* Add`#include <zmyclass.h>` to `include/czmq.h`.
* Add the myclass header and test call to `src/czmq_selftest.c`.
* Add a reference documentation to 'doc/zmyclass.txt'.
* Add myclass to 'src/Makefile.am` and `doc/Makefile.am`.

The `bin/newclass.sh` shell script will automate these steps for you.

Coding Style
~~~~~~~~~~~~

In general the zctx class defines the style for the whole library. The overriding rules for coding style are consistency, clarity, and ease of maintenance. We use the C99 standard for syntax including principally:

* The // comment style.
* Variables definitions placed in or before the code that uses them.

So while ANSI C code might say:
----
    zblob_t *file_buffer;       /*  Buffer for our file */
    ... (100 lines of code)
    file_buffer = zblob_new ();
    ...
----

The style in CZMQ would be:
----
    zblob_t *file_buffer = zblob_new ();
----

Assertions
~~~~~~~~~~

We use assertions heavily to catch bad argument values. The CZMQ classes do not attempt to validate arguments and report errors; bad arguments are treated as fatal application programming errors.

We also use assertions heavily on calls to system functions that are never supposed to fail, where failure is to be treated as a fatal non-recoverable error (e.g. running out of memory).

Assertion code should always take this form:
----
    int rc = some_function (arguments);
    assert (rc == 0);
----

Rather than the side-effect form:
----
    assert (some_function (arguments) == 0);
----

Since assertions may be removed by an optimizing compiler.

Documentation
~~~~~~~~~~~~~

Man pages are generated from the class header and source files via the doc/mkman tool, and similar functionality in the gitdown tool (http://github.com/imatix/gitdown). The header file for a class must wrap its interface as follows (example is from include/zclock.h):
----
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
----

The source file for a class must provide documentation as follows:
----
    /*  
    @header
    ...short explanation of class...
    @discuss
    ...longer discussion of how it works...
    @end
    */
----

The source file for a class then provides the self test example as follows:
----
    //  @selftest
    int64_t start = zclock_time ();
    zclock_sleep (10);
    assert ((zclock_time () - start) >= 10);
    //  @end
----

The template for man pages is in doc/mkman.

Development
~~~~~~~~~~~

CZMQ is developed through a test-driven process that guarantees no memory violations or leaks in the code:

* Modify a class or method.
* Update the test method for that class.
* Run the 'selftest' script, which uses the Valgrind memcheck tool.
* Repeat until perfect.

Porting CZMQ 
~~~~~~~~~~~~

When you try CZMQ on an OS that it's not been used on (ever, or for a while), you will hit code that does not compile. In some cases the patches are trivial, in other cases (usually when porting to Windows), the work needed to build equivalent functionality may be quite heavy. In any case, the benefit is that once ported, the functionality is available to all applications.

Before attempting to patch code for portability, please read the `czmq_prelude.h` header file. There are several typical types of changes you may need to make to get functionality working on a specific operating system:

* Defining typedefs which are missing on that specific compiler: do this in czmq_prelude.h.
* Defining macros that rename exotic library functions to more conventional names: do this in czmq_prelude.h.
* Reimplementing specific methods to use a non-standard API: this is typically needed on Windows. Do this in the relevant class, using #ifdefs to properly differentiate code for different platforms.

The canonical 'standard operating system' for all CZMQ code is Linux, gcc, POSIX.
