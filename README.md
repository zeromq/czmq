
<A name="toc1-3" title="ZFL - ØMQ Function Library" />
# ZFL - ØMQ Function Library

<A name="toc2-6" title="Contents" />
## Contents


**<a href="#toc2-11">Overview</a>**
&emsp;<a href="#toc3-14">Scope and Goals</a>
&emsp;<a href="#toc3-23">Ownership and License</a>
&emsp;<a href="#toc3-30">Contributing</a>

**<a href="#toc2-41">Using ZFL</a>**
&emsp;<a href="#toc3-44">Building and Installing</a>
&emsp;<a href="#toc3-65">Linking with an Application</a>
&emsp;<a href="#toc3-76">The Class Model</a>
&emsp;<a href="#toc3-109">ZFL Classes</a>
&emsp;<a href="#toc3-126">Predefined Macros</a>
&emsp;<a href="#toc3-167">Error Handling</a>

**<a href="#toc2-172">Design Ideology</a>**
&emsp;<a href="#toc3-175">The Problem with C</a>
&emsp;<a href="#toc3-184">A Simple Class Model</a>
&emsp;<a href="#toc3-211">Naming Style</a>
&emsp;<a href="#toc3-220">Containers</a>
&emsp;<a href="#toc3-232">Inheritance</a>
&emsp;<a href="#toc3-241">Portability</a>
&emsp;<a href="#toc3-267">Technical Aspects</a>

**<a href="#toc2-283">Under the Hood</a>**
&emsp;<a href="#toc3-286">Adding a New Class</a>
&emsp;<a href="#toc3-299">Coding Style</a>
&emsp;<a href="#toc3-318">Assertions</a>
&emsp;<a href="#toc3-336">Documentation</a>
&emsp;<a href="#toc3-341">Porting ZFL</a>
&emsp;<a href="#toc3-354">Memory Leak Testing</a>
&emsp;<a href="#toc3-367">This Document</a>

<A name="toc2-11" title="Overview" />
## Overview

<A name="toc3-14" title="Scope and Goals" />
### Scope and Goals

ZFL is the ØMQ Function Library, a thin portability & function library for ZeroMQ applications in C/C++. It is written as clear readable C classes, portable to all ØMQ platforms, and licensed under the LGPL.

The main goal is to allow the construction of industrial-scale ØMQ services and devices that integrate properly with the operating system, while staying 100% portable. ZFL acts as a primary abstraction layer on top of the ØMQ API, mainly for C applications but also for other languages. ZFL is inspired by the iMatix Standard Function Library (SFL), and borrows liberally from it. See http://legacy.imatix.com/html/sfl/.

ZFL is meant to be lightweight, consistent, class-based, minimalistic, highly efficient, and aimed at making it faster and easier to develop realistic, secure, and portable ØMQ devices and applications.

<A name="toc3-23" title="Ownership and License" />
### Ownership and License

ZFL is maintained by Pieter Hintjens and Martin Hurton (code) and Mikko Koppanen (build system). Its other authors and contributors are listed in the AUTHORS file. It is held by the ZeroMQ organization at github.com.

The authors of ZFL grant you use of this software under the terms of the GNU Lesser General Public License (LGPL). For details see the files `COPYING` and `COPYING.LESSER` in this directory.

<A name="toc3-30" title="Contributing" />
### Contributing

To submit an issue use the [issue tracker](http://github.com/zeromq/zfl/issues). All discussion happens on the [zeromq-dev](zeromq-dev@lists.zeromq.org) list or #zeromq IRC channel at irc.freenode.net.

The proper way to submit patches is to clone this repository, make your changes, and use git to create a patch. See http://www.zeromq.org/docs:contributing. All contributors are listed in AUTHORS.

All classes are maintained by a single person, who is the responsible editor for that class and who is named in the header as such. This is usually the originator of the class. When several people collaborate on a class, one single person is always the lead maintainer and the one to blame when it breaks.

The general rule is, if you contribute code to ZFL you must be willing to maintain it as long as there are users of it. Code with no active maintainer will in general be deprecated and/or removed.

<A name="toc2-41" title="Using ZFL" />
## Using ZFL

<A name="toc3-44" title="Building and Installing" />
### Building and Installing

ZFL uses autotools for packaging. To build from git (all example commands are for Linux):

    git clone git://github.com/zeromq/zfl.git
    cd zfl
    sh autogen.sh
    ./configure
    make all
    sudo make install
    sudo ldconfig

You will need the libtool and autotools packages. On FreeBSD, you may need to specify the default directories for configure:

    ./configure --with-zeromq=/usr/local

After building, you can run the ZFL selftests:

    make check

<A name="toc3-65" title="Linking with an Application" />
### Linking with an Application

Include `zfl.h` in your application and link with libzfl. Here is a typical gcc link command:

    gcc -lzfl -lzmq myapp.c -o myapp

You should read `zfl.h`. This file includes `zmq.h` and the system header files that typical ØMQ applications will need. The provided 'c' shell script lets you write simple portable build scripts:

    c -lzfl -lzmq -l myapp

<A name="toc3-76" title="The Class Model" />
### The Class Model

ZFL consists of classes, each class consisting of a .h and a .c. Classes may depend on other classes.

`zfl.h` includes all classes header files, all the time. For the user, ZFL forms one single package. All classes start by including `zfl.h`. All applications that use ZFL start by including `zfl.h`. `zfl.h` also defines a limited number of small, useful macros and typedefs that have proven useful for writing clearer C code.

The canonical example for ZFL style is the zfl_base class, which defines the template for all other classes. The nomenclature for all classes is consistent. We use zfl_base as an example:

* Source files: zfl_base.c, zfl_base.h
* Methods: zfl_base_test, zfl_base_print, ...

All classes are based on a flat C class system and follow these rules:

* Class typedef: `zfl_base_t`
* Constructor: `zfl_base_new`
* Destructor: `zfl_base_destroy`
* Property methods: `zfl_base_property_set`, `zfl_base_property`
* Class structures are private (defined in the .c source but not the .h)
* Properties are accessed only via methods named as described above.
* In the class source code the object is always called `self`.
* The constructor may take arbitrary arguments, and returns NULL on failure, or a new object.
* The destructor takes a pointer to an object reference and nullifies it.

Return values for methods are:

* For methods that return an object reference, either the reference, or NULL on failure.
* For methods that signal success/failure, a return value of 0 means sucess, -1 failure.

Private/static functions in a class are named `s_functionname` and are not exported via the header file.

All classes have a test method called `zfl_classname_test`.

<A name="toc3-109" title="ZFL Classes" />
### ZFL Classes

These are the existing ZFL classes:

* zfl_base - base class for ZFL
* zfl_blob - binary long object
* zfl_clock - system timer & date functions
* zfl_config - work with configuration files
* zfl_device - configure a device or device socket
* zfl_hash - expandable hash table container
* zfl_list - singly-linked list container
* zfl_msg - multipart ØMQ message
* zfl_rpcd - server side reliable RPC
* zfl_rpc - client side reliable RPC
* zfl_thread - work with operating system threads

<A name="toc3-126" title="Predefined Macros" />
### Predefined Macros

The file zfl_prelude.h defines a number of macros including these:

    #define FOREVER             for (;;)            //  FOREVER { ... }
    #define until(expr)         while (!(expr))     //  do { ... } until (expr)
    #define streq(s1,s2)        (!strcmp ((s1), (s2)))
    #define strneq(s1,s2)       (strcmp ((s1), (s2)))
    #define strused(s)          (*(s) != 0)
    #define strnull(s)          (*(s) == 0)
    #define strclr(s)           (*(s) = 0)
    #define strlast(s)          ((s) [strlen (s) - 1])
    #define strterm(s)          ((s) [strlen (s)])

    #define tblsize(x)          (sizeof (x) / sizeof ((x) [0]))
    #define tbllast(x)          (x [tblsize (x) - 1])

    #define randomof(num)       (int) (((float) num) * rand () / (RAND_MAX + 1.0))
    #define randomize()         srand ((uint) apr_time_usec (apr_time_now ()))

    #if (!defined (MIN))
    #   define MIN(a,b)         (((a) < (b))? (a): (b))
    #   define MAX(a,b)         (((a) > (b))? (a): (b))
    #endif

    //- Assertion that pointer value is as expect -------------------------------

    #define assert_eq(value,const) \
    if ((value) != (const)) {\
        printf ("Assertion failed, expected=%d actual=%d", (const), (value));\
        assert ((value) == (const));\
    }

    //- Boolean operators and constants -----------------------------------------

    #if (!defined (TRUE))
    #    define TRUE        1               //  ANSI standard
    #    define FALSE       0
    #endif

<A name="toc3-167" title="Error Handling" />
### Error Handling

Functions that create or search objects return object references success and NULL on failure.  Functions that perform work return 0 on success and -1 on failure.

<A name="toc2-172" title="Design Ideology" />
## Design Ideology

<A name="toc3-175" title="The Problem with C" />
### The Problem with C

C has the significant advantage of being a small language that, if we take a little care with formatting and naming, can be easily interchanged between developers. Every C developer will use much the same 90% of the language. Larger languages like C++ provide powerful abstractions like STL containers but at the cost of interchange. Every C++ developer will use a different 20% of the language.

The huge problem with C is that any realistic application needs packages of functionality to bring the language up to the levels we expect for the 21st century. Much can be done by using external libraries but every additional library is a dependency that makes the resulting applications harder to build and port. While C itself is a highly portable language (and can be made more so by careful use of the preprocessor), most C libraries consider themselves part of the operating system, and as such do not attempt to be portable.

The answer to this, as we learned from building enterprise-level C applications at iMatix from 1995-2005, is to create our own fully portable, high-quality libraries of pre-packaged functionality, in C. Doing this right solves both the requirements of richness of the language, and of portability of the final applications.

<A name="toc3-184" title="A Simple Class Model" />
### A Simple Class Model

C has no standard API style. It is one thing to write a useful component, but something else to provide an API that is consistent and obvious across many components. We learned from building [OpenAMQ](http://www.openamq.org), a messaging client and server of 0.5M LoC, that a consistent model for extending C makes life for the application developer much easier.

The general model is that of a class (the source package) that provides objects (in fact C structures). The application creates objects and then works with them. When done, the application destroys the object. In C, we tend to use the same name for the object as the class, when we can, and it looks like this (to take a fictitious ZFL class):

    zfl_regexp_t *regexp = zfl_regexp_new (regexp_string);
    if (!regexp)
        printf ("E: invalid regular expression: %s\n", regexp_string);
    else
    if (zfl_regexp_match (regexp, input_buffer))
        printf ("I: successful match for %s\n", input buffer);
    zfl_regexp_destroy (&regexp);

As far as the C program is concerned, the object is a reference to a structure (not a void pointer). We pass the object reference to all methods, since this is still C. We could do weird stuff like put method addresses into the structure so that we can emulate a C++ syntax but it's not worthwhile. The goal is not to emulate an OO system, it's simply to gain consistency. The constructor returns an object reference, or NULL if it fails. The destructor nullifies the class pointer, and is idempotent.

What we aim at here is the simplest possible consistent syntax.

No model is fully consistent, and classes can define their own rules if it helps make a better result. For example:

* Some classes may not be opaque. For example, we have cases of generated serialization classes that encode and decode structures to/from binary buffers. It feels clumsy to have to use methods to access the properties of these classes.

* While every class has a new method that is the formal constructor, some methods may also act as constructors. For example, a "dup" method might take one object and return a second object.

* While every class has a destroy method that is the formal destructor, some methods may also act as destructors. For example, a method that sends an object may also destroy the object (so that ownership of any buffers can passed to background threads). Such methods take the same "pointer to a reference" argument as the destroy method.

<A name="toc3-211" title="Naming Style" />
### Naming Style

ZFL aims for short, consistent names, following the theory that names we use most often should be shortest. Classes get one-word names, unless they are part of a family of classes in which case they may be two words, the first being the family name. Methods, similarly, get one-word names and we aim for consistency across classes (so a method that does something semantically similar in two classes will get the same name in both). So the canonical name for any method is:

    zfl_classname_methodname

And the reader can easily parse this without needing special syntax to separate the class name from the method name.

<A name="toc3-220" title="Containers" />
### Containers

After a long experiment with containers, we've decided that we need exactly two containers:

* A singly-linked list.
* A hash table using text keys.

These are zfl_list and zfl_hash, respectively. Both store void pointers, with no attempt to manage the details of contained objects. You can use these containers to create lists of lists, hashes of lists, hashes of hashes, etc.

We assume that at some point we'll need to switch to a doubly-linked list.

<A name="toc3-232" title="Inheritance" />
### Inheritance

ZFL provides two ways to do inheritance from base classes to higher level classes. First, by code copying. You may laugh but it works. The zfl_base class defines a basic syntactic structure. If we decide to change some of the ground rules shared by all classes, we modify the zfl_base class and then we manually make the same modifications in all other ZFL classes. Obviously as the number of classes in ZFL grows this becomes progressively harder, which is good: we don't want the basics to change more than they need to.

The second way is by straight encapsulation. For example if I want to make a specialized container that has some intelligence about the objects it contains, I can take the list or hash class, wrap that in a new class and add the necessary code on top. There is no attempt, nor need, to export methods or properties automatically. If I want this, I do it by hand.

Writing such code by hand may seem laborious but when we work with ruthlessly consistent style and semantics, it is easy, safe, and often the shortest path from problem to solution.

<A name="toc3-241" title="Portability" />
### Portability

Creating a portable C application can be rewarding in terms of maintaining a single code base across many platforms, and keeping (expensive) system-specific knowledge separate from application developers. In most projects (like ØMQ core), there is no portability layer and application code does conditional compilation for all mixes of platforms. This leads to quite messy code.

ZFL is explicitly meant to become a portability layer, similar to but thinner than libraries like the [Apache Portable Runtime](http://apr.apache.org) (APR).

These are the places a C application is subject to arbitrary system differences:

* Different compilers may offer slightly different variants of the C language, often lacking specific types or using neat non-portable names. Windows is a big culprit here. We solve this by 'patching' the language in zfl_prelude.h, e.g. defining int64_t on Windows.
* System header files are inconsistent, i.e. you need to include different files depending on the OS type and version. We solve this by pulling in all necessary header files in zfl_prelude.h. This is a proven brute-force approach that increases recompilation times but eliminates a major source of pain.
* System libraries are inconsistent, i.e. you need to link with different libraries depending on the OS type and version. We solve this with an external compilation tool, 'C', which detects the OS type and version (at runtime) and builds the necessary link commands.
* System functions are inconsistent, i.e. you need to call different functions depending, again, on OS type and version. We solve this by building small abstract classes that handle specific areas of functionality, and doing conditional compilation in these.

An example of the last:

    #if (defined (__UNIX__))
        pid = GetCurrentProcessId();
    #elif (defined (__WINDOWS__))
        pid = getpid ();
    #else
        pid = 0;
    #endif

ZFL uses the GNU autotools system, so non-portable code can use the macros this defines. It can also use macros defined by the zfl_prelude.h header file.

<A name="toc3-267" title="Technical Aspects" />
### Technical Aspects

* *Thread safety*: the use of opaque structures is thread safe, though ØMQ applications should not share state between threads in any case.
* *Name spaces*: we prefix class names with `zfl_`, which ensures that all exported functions are globally safe.
* *Library versioning*: we don't make any attempt to version the library at this stage. Classes are in our experience highly stable once they are built and tested, the only changes typically being added methods.
* *Performance*: for critical path processing, you may want to avoid creating and destroying classes. However on modern Linux systems the heap allocator is very fast. Individual classes can choose whether or not to nullify their data on allocation.
* *Self-testing*: every class has a `selftest` method that runs through the methods of the class. In theory, calling all selftest functions of all classes does a full unit test of the library. The `zfl_selftest` application does this.
* *Portability*: the ZFL library is aimed at becoming a portability layer (like Apache APR or the older iMatix SFL) but that depends on it actually being ported. See section on 'Porting ZFL' below.
* *Memory management*: ZFL classes do not use any special memory management techiques to detect leaks. We've done this in the past but it makes the code relatively complex. Instead, we do memory leak testing using tools like mtrace:

    gcc -g -o zfl_selftest zfl*.c -lzmq
    export MALLOC_TRACE=mtrace.txt
    zfl_selftest
    mtrace zfl_selftest mtrace.txt

<A name="toc2-283" title="Under the Hood" />
## Under the Hood

<A name="toc3-286" title="Adding a New Class" />
### Adding a New Class

If you define a new ZFL class `myclass` you need to:

* Write the `zfl_myclass.c` and `zfl_myclass.h` source files, in `zfl/src` and `zfl/include` respectively.
* Add`#include <zfl_myclass.h>` to `zfl/include/zfl.h`.
* Add the myclass header and test call to `src/zfl_selftest.c`.
* Add a reference documentation to 'doc/zfl_myclass.txt'.
* Add myclass to 'src/Makefile.am` and `doc/Makefile.am`.

The `bin/newclass.sh` shell script will automate these steps for you.

<A name="toc3-299" title="Coding Style" />
### Coding Style

In general the zfl_base class defines the style for the whole library. The overriding rules for coding style are consistency, clarity, and ease of maintenance. We use the C99 standard for syntax including principally:

* The // comment style.
* Variables definitions placed in or before the code that uses them.

So while ANSI C code might say:

    zfl_blob_t *file_buffer;       /*  Buffer for our file */
    ... (100 lines of code)
    file_buffer = zfl_blob_new ();
    ...

The style in ZFL would be:

    zfl_blob_t *file_buffer = zfl_blob_new ();

<A name="toc3-318" title="Assertions" />
### Assertions

We use assertions heavily to catch bad argument values. The ZFL classes do not attempt to validate arguments and report errors; bad arguments are treated as fatal application programming errors.

We also use assertions heavily on calls to system functions that are never supposed to fail, where failure is to be treated as a fatal non-recoverable error (e.g. running out of memory).

Assertion code should always take this form:

    int rc = some_function (arguments);
    assert (rc == 0);

Rather than the side-effect form:

    assert (some_function (arguments) == 0);

Since assertions may be removed by an optimizing compiler.

<A name="toc3-336" title="Documentation" />
### Documentation

Each ZFL class has its own man page in the doc directory. The contents of the man page are cut/paste from the class header and source files. Please maintain the doc file as you add new methods or modify the class test method (which for most classes acts as the EXAMPLE code).

<A name="toc3-341" title="Porting ZFL" />
### Porting ZFL

When you try ZFL on an OS that it's not been used on (ever, or for a while), you will hit code that does not compile. In some cases the patches are trivial, in other cases (usually when porting to Windows), the work needed to build equivalent functionality may be quite heavy. In any case, the benefit is that once ported, the functionality is available to all applications.

Before attempting to patch code for portability, please read the `zfl_prelude.h` header file. There are several typical types of changes you may need to make to get functionality working on a specific operating system:

* Defining typedefs which are missing on that specific compiler: do this in zfl_prelude.h.
* Defining macros that rename exotic library functions to more conventional names: do this in zfl_prelude.h.
* Reimplementing specific methods to use a non-standard API: this is typically needed on Windows. Do this in the relevant class, using #ifdefs to properly differentiate code for different platforms.

The canonical 'standard operating system' for all ZFL code is Linux, gcc, POSIX.

<A name="toc3-354" title="Memory Leak Testing" />
### Memory Leak Testing

To test against memory leaks we use the mtrace tool under Linux. The zfl_selftest.c program calls MALLOC_TRACE, which zfl_prelude.h sets to mtrace() under Linux. This is how we build and run the selftests with mtrace:

    #  Run selftests and check memory
    gcc -g -o zfl_selftest zfl*.c -lzmq
    export MALLOC_TRACE=mtrace.txt
    zfl_selftest -v
    mtrace zfl_selftest mtrace.txt

Note that mtrace is not threadsafe and will not work consistently in multithreaded applications or test cases. All test cases should therefore be single-threaded.

<A name="toc3-367" title="This Document" />
### This Document

This document is originally at README.txt and is built using [gitdown](http://github.com/imatix/gitdown).
