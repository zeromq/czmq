.set GIT=http://github.com/zeromq/zfl/win32

# Building ZFL on Windows

## Contents

.toc

## Microsoft Visual C/C++

This directory contains projects and solution to build ZFL as a static library.

Traps for the unwary contributor:

* To compile the C99 constructs in the ZFL codebase, you must use the /TP switch. You may not be able to build ZFL with older versions of MSVC, we've tested VisualStudio 2008.

* MSVC is much stricter about type conversions than gcc. Particularly when casting from void * to another pointer, explicit casts are necessary. Also for signed/unsigned character usage.
