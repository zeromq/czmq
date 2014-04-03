#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################
lib_LTLIBRARIES = libczmq.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libczmq.pc

include_HEADERS = \
    ../include/czmq.h \
    ../include/czmq_prelude.h \
    ../include/zauth.h \
    ../include/zbeacon.h \
    ../include/zcert.h \
    ../include/zcertstore.h \
    ../include/zchunk.h \
    ../include/zclock.h \
    ../include/zconfig.h \
    ../include/zctx.h \
    ../include/zdir.h \
    ../include/zdir_patch.h \
    ../include/zdigest.h \
    ../include/zfile.h \
    ../include/zframe.h \
    ../include/zhash.h \
    ../include/zlist.h \
    ../include/zlog.h \
    ../include/zloop.h \
    ../include/zmonitor.h \
    ../include/zmsg.h \
    ../include/zmutex.h \
    ../include/zpoller.h \
    ../include/zproxy.h \
    ../include/zrex.h \
    ../include/zsocket.h \
    ../include/zsockopt.h \
    ../include/zstr.h \
    ../include/zsys.h \
    ../include/zthread.h \
    ../include/zuuid.h

libczmq_la_SOURCES = \
    platform.h \
    zauth.c \
    zbeacon.c \
    zcert.c \
    zcertstore.c \
    zchunk.c \
    zclock.c \
    zconfig.c \
    zctx.c \
    zdir.c \
    zdir_patch.c \
    zdigest.c \
    zfile.c \
    zframe.c \
    zhash.c \
    zlist.c \
    zlog.c \
    zloop.c \
    zmonitor.c \
    zmsg.c \
    zmutex.c \
    zpoller.c \
    zproxy.c \
    zrex.c \
    zsocket.c \
    zsockopt.c \
    zstr.c \
    zsys.c \
    zthread.c \
    zuuid.c

AM_CFLAGS = -g
AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = czmq_selftest
czmq_selftest_LDADD = libczmq.la
czmq_selftest_SOURCES = czmq_selftest.c

if ON_MINGW
libczmq_la_LDFLAGS = -no-undefined -avoid-version -version-info @LTVER@
else
libczmq_la_LDFLAGS = -version-info @LTVER@
endif

TESTS = czmq_selftest

#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################
