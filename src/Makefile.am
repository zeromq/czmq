lib_LTLIBRARIES = libzapi.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libzapi.pc

include_HEADERS = \
    ../include/zapi.h \
    ../include/zapi_prelude.h \
    ../include/zctx.h \
    ../include/zframe.h \
    ../include/zloop.h \
    ../include/zmsg.h \
    ../include/zstr.h

libzapi_la_SOURCES = \
    zctx.c \
    zframe.c \
    zloop.c \
    zmsg.c \
    zstr.c 

EXTRA_libzapi_la_SOURCES = import/cJSON/cJSON.c

INCLUDES = -I$(top_srcdir)/include
bin_PROGRAMS = zapi_selftest
zapi_selftest_LDADD = libzapi.la
zapi_selftest_SOURCES = zapi_selftest.c

if ON_MINGW
libzapi_la_LDFLAGS = -no-undefined -avoid-version -version-info @LTVER@
else
libzapi_la_LDFLAGS = -version-info @LTVER@
endif

TESTS = zapi_selftest


