.set GIT=http://github.com/zeromq/czmq/win32

# Building czmq on Windows

## Contents

.toc

## Microsoft Visual C/C++

This directory contains projects and solution to build czmq as a static library.

Traps for the unwary contributor:

* To compile the C99 constructs in the czmq codebase, you must use the /TP switch. You may not be able to build czmq with older versions of MSVC, we've tested VisualStudio 2008.

* MSVC is much stricter about type conversions than gcc. Particularly when casting from void * to another pointer, explicit casts are necessary. Also for signed/unsigned character usage.
