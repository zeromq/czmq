# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.czmq.Debug:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.dylib:\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.dylib


PostBuild.czmq-static.Debug:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.a:
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.a


PostBuild.czmq_objects.Debug:
PostBuild.czmq_selftest.Debug:
PostBuild.czmq.Debug: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/czmq_selftest
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/czmq_selftest:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/czmq_selftest


PostBuild.test_randof.Debug:
PostBuild.czmq.Debug: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/test_randof
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/test_randof:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/test_randof


PostBuild.zmakecert.Debug:
PostBuild.czmq.Debug: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/zmakecert
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/zmakecert:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/zmakecert


PostBuild.zsp.Debug:
PostBuild.czmq.Debug: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/zsp
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/zsp:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/zsp


PostBuild.czmq.Release:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.dylib:\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.dylib


PostBuild.czmq-static.Release:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.a:
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.a


PostBuild.czmq_objects.Release:
PostBuild.czmq_selftest.Release:
PostBuild.czmq.Release: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/czmq_selftest
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/czmq_selftest:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/czmq_selftest


PostBuild.test_randof.Release:
PostBuild.czmq.Release: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/test_randof
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/test_randof:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/test_randof


PostBuild.zmakecert.Release:
PostBuild.czmq.Release: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/zmakecert
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/zmakecert:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/zmakecert


PostBuild.zsp.Release:
PostBuild.czmq.Release: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/zsp
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/zsp:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/zsp


PostBuild.czmq.MinSizeRel:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.dylib:\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.dylib


PostBuild.czmq-static.MinSizeRel:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.a:
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.a


PostBuild.czmq_objects.MinSizeRel:
PostBuild.czmq_selftest.MinSizeRel:
PostBuild.czmq.MinSizeRel: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/czmq_selftest
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/czmq_selftest:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/czmq_selftest


PostBuild.test_randof.MinSizeRel:
PostBuild.czmq.MinSizeRel: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/test_randof
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/test_randof:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/test_randof


PostBuild.zmakecert.MinSizeRel:
PostBuild.czmq.MinSizeRel: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/zmakecert
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/zmakecert:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/zmakecert


PostBuild.zsp.MinSizeRel:
PostBuild.czmq.MinSizeRel: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/zsp
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/zsp:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/zsp


PostBuild.czmq.RelWithDebInfo:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.dylib:\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.dylib


PostBuild.czmq-static.RelWithDebInfo:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.a:
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.a


PostBuild.czmq_objects.RelWithDebInfo:
PostBuild.czmq_selftest.RelWithDebInfo:
PostBuild.czmq.RelWithDebInfo: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/czmq_selftest
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/czmq_selftest:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/czmq_selftest


PostBuild.test_randof.RelWithDebInfo:
PostBuild.czmq.RelWithDebInfo: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/test_randof
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/test_randof:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/test_randof


PostBuild.zmakecert.RelWithDebInfo:
PostBuild.czmq.RelWithDebInfo: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/zmakecert
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/zmakecert:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/zmakecert


PostBuild.zsp.RelWithDebInfo:
PostBuild.czmq.RelWithDebInfo: /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/zsp
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/zsp:\
	/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.4.2.1.dylib\
	/usr/local/lib/libzmq.dylib\
	/usr/lib/libcurl.dylib
	/bin/rm -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/zsp




# For each target create a dummy ruleso the target does not have to exist
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Debug/libczmq.4.2.1.dylib:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/MinSizeRel/libczmq.4.2.1.dylib:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/RelWithDebInfo/libczmq.4.2.1.dylib:
/Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/Release/libczmq.4.2.1.dylib:
/usr/lib/libcurl.dylib:
/usr/local/lib/libzmq.dylib:
