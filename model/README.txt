# Project files

Since 15 October 2013, we generate the project files automatically from a
single XML file that looks like this:

    <project name = "czmq">
        <header name = "czmq" />
        <header name = "czmq_prelude" />
        <class name = "zauth" />
        <class name = "zbeacon" />
        ...
        <class name = "ztree" />
    </project>

The goal was to reduce the effort to add new classes and ensure they're
properly built on all systems. The downside is that it makes it harder to
fix the project files for any given system. However, you can cheat...

## Option 0 - Adding a New Class

1. Add your new class to the src and include directories, edit czmq.h and
   czmq_selftest.c, and make a pull request.

2. Someone else will update the project files.

## Option 1 - Hacking the Project Files

1. Take any given project file, make whatever changes you need to, and
   send the changed project file as a pull request.

2. A maintainer will then take your changes and backport them to the code
   generation templates.

## Option 2 - Doing It Properly

1. Grab http://github.com/imatix/gsl, build this tool. It is the tool that
   we use to do the code generation itself. GSL takes XML models like our
   project file, and chucks them at templates written in its own scripting
   language.

3. To add a new class, edit projects.xml and then run ./generate.

2. To add a new platform, read the various build-xxx.gsl scripts in this
   directory. These are very simple GSL programs that vomit out makefiles
   and project files.

3. Then add a new script using one of the existing ones as inspiration, and
   a hand-written makefile/whatever as raw material.

4. Add your new script to projects.gsl. Test, and make a pull request with
   the results.

## Current Support

Currently we generate these project-related files:

    CMakeLists.txt

    builds/android/Android.mk

    builds/mingw32/Makefile.mingw32

    builds/msvc/vs2008/czmq/czmq.vcproj

    builds/msvc/vs2010/czmq/czmq.vcxproj
    builds/msvc/vs2010/czmq/czmq.vcxproj.filters

    builds/msvc/vs2012/czmq/czmq.vcxproj
    builds/msvc/vs2012/czmq/czmq.vcxproj.filters

    builds/msvc/vs2013/czmq/czmq.vcxproj
    builds/msvc/vs2013/czmq_selftest/czmq_selftest.vcxproj

    src/Makefile.am
    doc/Makefile.am

And also the socket options class zsockopts using the same approach.
