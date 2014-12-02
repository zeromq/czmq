ACLOCAL_AMFLAGS = -I config

AM_CPPFLAGS = \
	${zmq_CFLAGS} \
	-I$(srcdir)/include

SUBDIRS = doc

DIST_SUBDIRS = doc

lib_LTLIBRARIES =

bin_PROGRAMS =

check_PROGRAMS =

EXTRA_DIST = \
	version.sh \
	builds/android/Android.mk \
	builds/android/Application.mk \
	builds/android/build.sh \
	builds/android/clean.sh \
	builds/mingw32/Makefile.mingw32 \
	builds/mingw32/platform.h \
	examples/security/grasslands.c \
	examples/security/strawhouse.c \
	examples/security/woodhouse.c \
	examples/security/stonehouse.c \
	examples/security/ironhouse.c \
	examples/security/ironhouse2.c \
	examples/security/README.txt \
	examples/security/LICENSE \
	foreign/slre/readme.txt \
	foreign/slre/slre.h \
	foreign/slre/slre.c \
	foreign/sha1/sha1.c \
	foreign/sha1/sha1.h \
	packaging/dist/gitlog2changelog.py \
	packaging/nuget/package.config \
	packaging/nuget/package.targets \
	packaging/nuget/package.bat \
	packaging/nuget/package.gsl \
	packaging/nuget/package.xml \
	packaging/nuget/package.nuspec \
	builds/msvc/build/buildbase.bat \
	builds/msvc/build/buildall.bat \
	builds/msvc/vs2012/czmq.sln \
	builds/msvc/vs2012/czmq_selftest/czmq_selftest.props \
	builds/msvc/vs2012/czmq_selftest/czmq_selftest.vcxproj \
	builds/msvc/vs2012/czmq/czmq.props \
	builds/msvc/vs2012/czmq/czmq.vcxproj \
	builds/msvc/vs2012/czmq/czmq.vcxproj.filters \
	builds/msvc/vs2012/.gitignore \
	builds/msvc/vs2012/libzmq.import.props \
	builds/msvc/vs2012/libzmq.import.xml \
	builds/msvc/vs2012/czmq.import.props \
	builds/msvc/vs2012/czmq.import.xml \
	builds/msvc/vs2012/libsodium.import.props \
	builds/msvc/vs2012/libsodium.import.xml \
	builds/msvc/vs2008/README.txt \
	builds/msvc/vs2008/czmq.sln \
	builds/msvc/vs2008/czmq_selftest/czmq_selftest.vcproj \
	builds/msvc/vs2008/czmq/czmq.vcproj \
	builds/msvc/.gitignore \
	builds/msvc/vs2010/czmq.sln \
	builds/msvc/vs2010/czmq_selftest/czmq_selftest.props \
	builds/msvc/vs2010/czmq_selftest/czmq_selftest.vcxproj \
	builds/msvc/vs2010/czmq/czmq.props \
	builds/msvc/vs2010/czmq/czmq.vcxproj \
	builds/msvc/vs2010/czmq/czmq.vcxproj.filters \
	builds/msvc/vs2010/.gitignore \
	builds/msvc/vs2010/libzmq.import.props \
	builds/msvc/vs2010/libzmq.import.xml \
	builds/msvc/vs2010/czmq.import.props \
	builds/msvc/vs2010/czmq.import.xml \
	builds/msvc/vs2010/libsodium.import.props \
	builds/msvc/vs2010/libsodium.import.xml \
	builds/msvc/platform.h \
	builds/msvc/vs2013/czmq.sln \
	builds/msvc/vs2013/czmq_selftest/czmq_selftest.props \
	builds/msvc/vs2013/czmq_selftest/czmq_selftest.vcxproj \
	builds/msvc/vs2013/czmq/czmq.props \
	builds/msvc/vs2013/czmq/czmq.vcxproj \
	builds/msvc/vs2013/czmq/czmq.vcxproj.filters \
	builds/msvc/vs2013/libzmq.import.props \
	builds/msvc/vs2013/libzmq.import.xml \
	builds/msvc/vs2013/czmq.import.props \
	builds/msvc/vs2013/czmq.import.xml \
	builds/msvc/vs2013/libsodium.import.props \
	builds/msvc/vs2013/libsodium.import.xml \
	builds/msvc/resource.h \
	builds/msvc/czmq.rc \
	builds/msvc/properties/ReleaseLIB.props \
	builds/msvc/properties/Debug.props \
	builds/msvc/properties/LIB.props \
	builds/msvc/properties/ReleaseDLL.props \
	builds/msvc/properties/LTCG.props \
	builds/msvc/properties/Link.props \
	builds/msvc/properties/DebugLTCG.props \
	builds/msvc/properties/ReleaseLTCG.props \
	builds/msvc/properties/x64.props \
	builds/msvc/properties/Messages.props \
	builds/msvc/properties/DebugDEXE.props \
	builds/msvc/properties/Release.props \
	builds/msvc/properties/Win32.props \
	builds/msvc/properties/DLL.props \
	builds/msvc/properties/ReleaseSEXE.props \
	builds/msvc/properties/DebugSEXE.props \
	builds/msvc/properties/EXE.props \
	builds/msvc/properties/ReleaseDEXE.props \
	builds/msvc/properties/DebugLIB.props \
	builds/msvc/properties/Common.props \
	builds/msvc/properties/DebugDLL.props \
	builds/msvc/properties/ReleaseLEXE.props \
	builds/msvc/properties/DebugLEXE.props \
	builds/msvc/properties/Output.props

include $(srcdir)/model/Makemodule.am

include $(srcdir)/src/Makemodule.am

include $(srcdir)/addons/Makemodule.am

# workaround the dist generated files that are also part of the distribution
#distcleancheck_listfiles = \
#	find . -type f -exec sh -c 'test -f $(srcdir)/{} || echo {}' ';'
# Note that distcleancheck is disabled for now, while waiting for a proper
# solution, that do not break older unix systems
distcleancheck:
	@:

# Automatically generate the ChangeLog from Git logs:
MAINTAINERCLEAN_FILES = ChangeLog
# Force ChangeLog regeneration upon make dist (due to nonexistant 'dummy-stamp'),
# in case it has already been generated previously
#
# Older boundary of the ChangeLog commits range
# It can be a tag ('v2.2.0'), a commit hash, a date, ...
# See gitrevisions for more information on specifying ranges
GIT_START_POINT=v2.2.0
dummy-stamp:
ChangeLog: packaging/dist/gitlog2changelog.py dummy-stamp
	$(top_srcdir)/packaging/dist/gitlog2changelog.py $(GIT_START_POINT) || \
		echo "gitlog2changelog.py failed to generate the ChangeLog. See https://github.com/zeromq/czmq/commits/master" > $@
