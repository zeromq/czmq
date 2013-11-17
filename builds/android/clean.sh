#! /bin/bash
#   Restores the tree after a build.sh. Does no harm if used more
#   than once. Run from the builds/android directory.
rm -rf ../../src/jni
rm -rf ../../src/obj
rm -rf ../../src/libs
rm -f libczmq.so

