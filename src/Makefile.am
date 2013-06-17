lib_LTLIBRARIES = libczmq.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libczmq.pc

include_HEADERS = \
    ../include/czmq.h \
    ../include/czmq_prelude.h \
    ../include/zbeacon.h \
    ../include/zclock.h \
    ../include/zconfig.h \
    ../include/zctx.h \
    ../include/zfile.h \
    ../include/zframe.h \
    ../include/zhash.h \
    ../include/zlist.h \
    ../include/zloop.h \
    ../include/zmsg.h \
    ../include/zmutex.h \
    ../include/zsocket.h \
    ../include/zsockopt.h \
    ../include/zstr.h \
    ../include/zsys.h \
    ../include/zthread.h \
    ../include/ztree.h

libczmq_la_SOURCES = \
    zbeacon.c \
    zclock.c \
    zconfig.c \
    zctx.c \
    zfile.c \
    zframe.c \
    zhash.c \
    zlist.c \
    zloop.c \
    zmsg.c \
    zmutex.c \
    zsocket.c \
    zsockopt.c \
    zstr.c \
    zsys.c \
    zthread.c \
    ztree.c

AM_CFLAGS = -g

INCLUDES = -I$(top_srcdir)/include
bin_PROGRAMS = czmq_selftest
czmq_selftest_LDADD = libczmq.la
czmq_selftest_SOURCES = czmq_selftest.c

if ON_MINGW
libczmq_la_LDFLAGS = -no-undefined -avoid-version -version-info @LTVER@
else
libczmq_la_LDFLAGS = -version-info @LTVER@
endif

TESTS = czmq_selftest


