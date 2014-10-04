if ENABLE_LABS
lib_LTLIBRARIES = libczmq_labs.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libczmq_labs.pc

include_HEADERS = \
    zpubsub.h \
	zpubsub_filter.h \
	zpubsub_option.h 

libczmq_labs_la_SOURCES = \
    zpubsub.c \
	zpubsub_filter.c \
	zpubsub_option.c 


AM_CFLAGS = -g
AM_CPPFLAGS = -I$(top_srcdir)/include
#bin_PROGRAMS = czmq_labs_selftest
#czmq_labs_selftest_LDADD = libczmq_labs.la
#czmq_labs_selftest_SOURCES = czmq_labs_selftest.c

if ON_MINGW
libczmq_labs_la_LDFLAGS = -no-undefined -avoid-version -version-info @LTVER@
else
libczmq_labs_la_LDFLAGS = -version-info @LTVER@
endif

#TESTS = czmq_labs_selftest
endif
