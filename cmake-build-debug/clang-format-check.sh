#!/bin/sh

# Use clang-format tool to verify this codebase conforms to our style standards

FAILED=0
IFS=";"
FILES="../src/czmq_private_selftest.c;../src/czmq_selftest.c;../src/test_randof.c;../src/test_zgossip.c;../src/zactor.c;../src/zargs.c;../src/zarmour.c;../src/zauth.c;../src/zbeacon.c;../src/zcert.c;../src/zcertstore.c;../src/zchunk.c;../src/zclock.c;../src/zconfig.c;../src/zdigest.c;../src/zdir.c;../src/zdir_patch.c;../src/zfile.c;../src/zframe.c;../src/zgossip.c;../src/zgossip_msg.c;../src/zhash.c;../src/zhashx.c;../src/ziflist.c;../src/zlist.c;../src/zlistx.c;../src/zloop.c;../src/zmakecert.c;../src/zmonitor.c;../src/zmsg.c;../src/zpoller.c;../src/zproc.c;../src/zproxy.c;../src/zrex.c;../src/zsock.c;../src/zsp.c;../src/zstr.c;../src/zsys.c;../src/ztimerset.c;../src/ztrie.c;../src/zuuid.c;../src/czmq_classes.h;../src/foreign/sha1/sha1.h;../src/foreign/slre/slre.h;../src/platform.h;../src/zgossip_msg.h;../include/czmq.h;../include/czmq_library.h;../include/czmq_prelude.h;../include/test_zgossip.h;../include/zactor.h;../include/zargs.h;../include/zarmour.h;../include/zauth.h;../include/zbeacon.h;../include/zcert.h;../include/zcertstore.h;../include/zchunk.h;../include/zclock.h;../include/zconfig.h;../include/zdigest.h;../include/zdir.h;../include/zdir_patch.h;../include/zfile.h;../include/zframe.h;../include/zgossip.h;../include/zhash.h;../include/zhashx.h;../include/ziflist.h;../include/zlist.h;../include/zlistx.h;../include/zloop.h;../include/zmonitor.h;../include/zmsg.h;../include/zpoller.h;../include/zproc.h;../include/zproxy.h;../include/zrex.h;../include/zsock.h;../include/zstr.h;../include/zsys.h;../include/ztimerset.h;../include/ztrie.h;../include/zuuid.h"
IDS=$(echo -en "\n\b")
for FILE in $FILES
do
    clang-format -style=file -output-replacements-xml "$FILE" | grep "<replacement " >/dev/null && \
    {
      echo "$FILE is not correctly formatted" >&2
      FAILED=1
    }
done
if [ "$FAILED" -eq "1" ] ; then exit 1 ; fi
