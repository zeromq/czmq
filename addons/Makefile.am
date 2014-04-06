EXTRA_DIST = makecert.c 

AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = makecert

makecert_LDADD = $(top_builddir)/src/libczmq.la
makecert_SOURCES = makecert.c

