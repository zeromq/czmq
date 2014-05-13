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
    ../include/zactor.h \
    ../include/zauth.h \
    ../include/zbeacon.h \
    ../include/zcert.h \
    ../include/zcertstore.h \
    ../include/zchunk.h \
    ../include/zclock.h \
    ../include/zconfig.h \
    ../include/zdigest.h \
    ../include/zdir.h \
    ../include/zdir_patch.h \
    ../include/zfile.h \
    ../include/zframe.h \
    ../include/zhash.h \
    ../include/zlist.h \
    ../include/zlog.h \
    ../include/zloop.h \
    ../include/zmsg.h \
    ../include/zpoller.h \
    ../include/zproxy.h \
    ../include/zrex.h \
    ../include/zsock.h \
    ../include/zsock_monitor.h \
    ../include/zsock_option.h \
    ../include/zstr.h \
    ../include/zsys.h \
    ../include/zuuid.h \
    ../include/zctx.h \
    ../include/zmonitor.h \
    ../include/zmutex.h \
    ../include/zsocket.h \
    ../include/zsockopt.h \
    ../include/zthread.h

libczmq_la_SOURCES = \
    platform.h \
    zactor.c \
    zauth.c \
    zbeacon.c \
    zcert.c \
    zcertstore.c \
    zchunk.c \
    zclock.c \
    zconfig.c \
    zdigest.c \
    zdir.c \
    zdir_patch.c \
    zfile.c \
    zframe.c \
    zhash.c \
    zlist.c \
    zlog.c \
    zloop.c \
    zmsg.c \
    zpoller.c \
    zproxy.c \
    zrex.c \
    zsock.c \
    zsock_monitor.c \
    zsock_option.c \
    zstr.c \
    zsys.c \
    zuuid.c \
    zctx.c \
    zmonitor.c \
    zmutex.c \
    zsocket.c \
    zsockopt.c \
    zthread.c

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
