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
    ../include/zgossip.h \
    ../include/zinterface.h \
    ../include/zlist.h \
    ../include/zloop.h \
    ../include/zmonitor.h \
    ../include/zmsg.h \
    ../include/zpoller.h \
    ../include/zproxy.h \
    ../include/zrex.h \
    ../include/zsock.h \
    ../include/zsock_option.h \
    ../include/zstr.h \
    ../include/zsys.h \
    ../include/zuuid.h \
    zgossip_msg.h \
    ../include/zauth_v2.h \
    ../include/zctx.h \
    ../include/zmonitor_v2.h \
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
    zgossip.c \
    zinterface.c \
    zlist.c \
    zloop.c \
    zmonitor.c \
    zmsg.c \
    zpoller.c \
    zproxy.c \
    zrex.c \
    zsock.c \
    zsock_option.c \
    zstr.c \
    zsys.c \
    zuuid.c \
    zgossip_msg.c \
    zauth_v2.c \
    zctx.c \
    zmonitor_v2.c \
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

# Produce generated models; do this manually in src directory
code:
	gsl -q sockopts.xml
	gsl -q zgossip.xml
	gsl -q zgossip_msg.xml

#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################
