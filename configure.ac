#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.61)
#
# The version number is extracted from include/czmq.h using
# the version.sh script. Hence, it should be updated there.
# The version in git should reflect the *next* version planned.
#
AC_INIT([czmq],[m4_esyscmd([./version.sh czmq])],[zeromq-dev@lists.zeromq.org])

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)
AC_CONFIG_HEADERS([src/platform.h])
AM_INIT_AUTOMAKE([subdir-objects tar-ustar foreign])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# This defines PACKAGE_VERSION_... in src/platform.h
PV_MAJOR=`echo $PACKAGE_VERSION | cut -d . -f 1`
PV_MINOR=`echo $PACKAGE_VERSION | cut -d . -f 2`
PV_PATCH=`echo $PACKAGE_VERSION | cut -d . -f 3`
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_MAJOR],[$PV_MAJOR],
    [CZMQ major version])
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_MINOR],[$PV_MINOR],
    [CZMQ minor version])
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_PATCH],[$PV_PATCH],
    [CZMQ patchlevel])
# This lets us use PACKAGE_VERSION in Makefiles
AC_SUBST(PACKAGE_VERSION)

#
# Libtool -version-info (ABI version)
#
# Currently 2.0.1 ("stable"). Don't change this unless you
# know exactly what you're doing and have read and understand
# http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
#
# libczmq -version-info
LTVER="2:0:1"
AC_SUBST(LTVER)

# Capture c flags
CZMQ_ORIG_CFLAGS="${CFLAGS:-none}"

# Checks for programs
AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_PROG_SED
AC_PROG_AWK
PKG_PROG_PKG_CONFIG

# Check for makecert intent
AC_ARG_WITH([makecert],
    AS_HELP_STRING([--with-makecert], 
        [Include the program makecert in compilation and installation.]),
    [with_makecert=$withval],
    [with_makecert=yes])

AM_CONDITIONAL([WITH_MAKECERT], [test x$with_makecert != xno])
AM_COND_IF([WITH_MAKECERT], [AC_MSG_NOTICE([WITH_MAKECERT defined])])

# Checks for libraries
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([uuid], [uuid_generate])

# Code coverage
AC_ARG_WITH(gcov, [AS_HELP_STRING([--with-gcov=yes/no],
                  [With GCC Code Coverage reporting.])],
                  [CZMQ_GCOV="$withval"])

if test "x${CZMQ_GCOV}" == "xyes"; then
    CFLAGS="-O0 -g -fprofile-arcs -ftest-coverage"

    if test "x${CZMQ_ORIG_CFLAGS}" != "xnone"; then
        CFLAGS="${CFLAGS} ${CZMQ_ORIG_CFLAGS}"
    fi
fi

# Check for libzmq
PKG_CHECK_MODULES([zmq], [libzmq])

# Platform specific checks
czmq_on_mingw32="no"
czmq_on_android="no"

# Host speciffic checks
AC_CANONICAL_HOST

# Determine whether or not documentation should be built and installed.
czmq_build_doc="yes"
czmq_install_man="yes"

# Check for asciidoc and xmlto and don't build the docs if these are not installed.
AC_CHECK_PROG(czmq_have_asciidoc, asciidoc, yes, no)
AC_CHECK_PROG(czmq_have_xmlto, xmlto, yes, no)
if test "x$czmq_have_asciidoc" = "xno" -o "x$czmq_have_xmlto" = "xno"; then
    czmq_build_doc="no"
    # Tarballs built with 'make dist' ship with prebuilt documentation.
    if ! test -f doc/czmq.7; then
        czmq_install_man="no"
        AC_MSG_WARN([You are building an unreleased version of CZMQ and asciidoc or xmlto are not installed.])
        AC_MSG_WARN([Documentation will not be built and manual pages will not be installed.])
    fi
fi
AC_MSG_CHECKING([whether to build documentation])
AC_MSG_RESULT([$czmq_build_doc])
AC_MSG_CHECKING([whether to install manpages])
AC_MSG_RESULT([$czmq_install_man])

# Set some default features required by libczmq code.
CPPFLAGS="-D_REENTRANT -D_THREAD_SAFE $CPPFLAGS"

# OS-specific tests
case "${host_os}" in
    *linux*)
        # Define on Linux to enable all library features
        CPPFLAGS="-D_GNU_SOURCE -DLINUX $CPPFLAGS"
        AC_DEFINE(CZMQ_HAVE_LINUX, 1, [Have Linux OS])
        
        case "${host_os}" in
            *android*)
                AC_DEFINE(CZMQ_HAVE_ANDROID, 1, [Have Android OS])
                czmq_on_android="yes"
            ;;
        esac
        ;;
    *solaris*)
        # Define on Solaris to enable all library features
        CPPFLAGS="-D_PTHREADS $CPPFLAGS"
        AC_DEFINE(CZMQ_HAVE_SOLARIS, 1, [Have Solaris OS])
        CFLAGS="${CFLAGS} -lsocket -lssp"
        ;;
    *freebsd*)
        # Define on FreeBSD to enable all library features
        CPPFLAGS="-D__BSD_VISIBLE $CPPFLAGS"
        AC_DEFINE(CZMQ_HAVE_FREEBSD, 1, [Have FreeBSD OS])
        ;;
    *darwin*)
        # Define on Darwin to enable all library features
        CPPFLAGS="-D_DARWIN_C_SOURCE $CPPFLAGS"
        AC_DEFINE(CZMQ_HAVE_OSX, 1, [Have DarwinOSX OS])
        ;;
    *netbsd*)
        # Define on NetBSD to enable all library features
        CPPFLAGS="-D_NETBSD_SOURCE $CPPFLAGS"
        AC_DEFINE(CZMQ_HAVE_NETBSD, 1, [Have NetBSD OS])
        ;;
    *openbsd*)
        # Define on OpenBSD to enable all library features
        CPPFLAGS="-D_BSD_SOURCE $CPPFLAGS"
        AC_DEFINE(CZMQ_HAVE_OPENBSD, 1, [Have OpenBSD OS])
        ;;
    *nto-qnx*)
        AC_DEFINE(CZMQ_HAVE_QNXNTO, 1, [Have QNX Neutrino OS])
        ;;
    *aix*)
        AC_DEFINE(CZMQ_HAVE_AIX, 1, [Have AIX OS])
        ;;
    *hpux*)
        # Define on HP-UX to enable all library features
        CPPFLAGS="-D_POSIX_C_SOURCE=200112L"
        AC_DEFINE(CZMQ_HAVE_HPUX, 1, [Have HPUX OS])
        ;;
    *mingw32*)
        AC_DEFINE(CZMQ_HAVE_WINDOWS, 1, [Have Windows OS])
        AC_DEFINE(CZMQ_HAVE_MINGW32, 1, [Have MinGW32])
        AC_CHECK_HEADERS(windows.h)
        czmq_on_mingw32="yes"
        czmq_install_man="no"
        ;;
    *cygwin*)
        # Define on Cygwin to enable all library features
        CPPFLAGS="-D_GNU_SOURCE $CPPFLAGS"
        AC_DEFINE(CZMQ_HAVE_CYGWIN, 1, [Have Cygwin])
        ;;
    *)
        AC_MSG_ERROR([unsupported system: ${host_os}.])
        ;;
esac

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(errno.h arpa/inet.h netinet/tcp.h netinet/in.h stddef.h \
                 stdlib.h string.h sys/socket.h sys/time.h unistd.h \
                 limits.h ifaddrs.h)
AC_CHECK_HEADERS([net/if.h net/if_media.h linux/wireless.h], [], [],
[
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#include <stdint.h>
])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_TYPE_UINT32_T
AC_C_VOLATILE

# These options are GNU compiler specific.
if test "x$GCC" = "xyes"; then
    CPPFLAGS="-pedantic -Werror -Wall -Wc++-compat ${CPPFLAGS}"
fi

AM_CONDITIONAL(ON_MINGW, test "x$czmq_on_mingw32" = "xyes")
AM_CONDITIONAL(ON_ANDROID, test "x$czmq_on_android" = "xyes")
AM_CONDITIONAL(INSTALL_MAN, test "x$czmq_install_man" = "xyes")
AM_CONDITIONAL(BUILD_DOC, test "x$czmq_build_doc" = "xyes")

# PKG_CHECK_MODULES doesn't add to LIBS on android
if test "x$czmq_on_android" = "xyes"; then
    LIBS="-lzmq $LIBS"
fi

# Checks for library functions.
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(perror gettimeofday memset getifaddrs)

# Set pkgconfigdir
AC_ARG_WITH([pkgconfigdir], AS_HELP_STRING([--with-pkgconfigdir=PATH],
    [Path to the pkgconfig directory [[LIBDIR/pkgconfig]]]),
    [pkgconfigdir="$withval"], [pkgconfigdir='${libdir}/pkgconfig'])
AC_SUBST([pkgconfigdir])

# Specify output files
AC_CONFIG_FILES([Makefile doc/Makefile src/libczmq.pc])
AC_OUTPUT
