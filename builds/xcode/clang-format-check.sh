#!/bin/sh

# Use clang-format tool to verify this codebase conforms to our style standards

FAILED=0
IFS=";"
FILES="../../include/czmq.h;../../include/czmq_library.h;../../include/czmq_prelude.h;../../include/test_zgossip.h;../../include/zactor.h;../../include/zargs.h;../../include/zarmour.h;../../include/zauth.h;../../include/zbeacon.h;../../include/zcert.h;../../include/zcertstore.h;../../include/zchunk.h;../../include/zclock.h;../../include/zconfig.h;../../include/zdigest.h;../../include/zdir.h;../../include/zdir_patch.h;../../include/zfile.h;../../include/zframe.h;../../include/zgossip.h;../../include/zhash.h;../../include/zhashx.h;../../include/zhttp_client.h;../../include/zhttp_request.h;../../include/zhttp_response.h;../../include/zhttp_server.h;../../include/zhttp_server_options.h;../../include/ziflist.h;../../include/zlist.h;../../include/zlistx.h;../../include/zloop.h;../../include/zmonitor.h;../../include/zmsg.h;../../include/zpoller.h;../../include/zproc.h;../../include/zproxy.h;../../include/zrex.h;../../include/zsock.h;../../include/zstr.h;../../include/zsys.h;../../include/ztimerset.h;../../include/ztrie.h;../../include/zuuid.h;../../src/czmq_classes.h;../../src/czmq_private_selftest.c;../../src/czmq_selftest.c;../../src/foreign/sha1/sha1.h;../../src/foreign/slre/slre.h;../../src/test_randof.c;../../src/test_zgossip.c;../../src/zactor.c;../../src/zargs.c;../../src/zarmour.c;../../src/zauth.c;../../src/zbeacon.c;../../src/zcert.c;../../src/zcertstore.c;../../src/zchunk.c;../../src/zclock.c;../../src/zconfig.c;../../src/zdigest.c;../../src/zdir.c;../../src/zdir_patch.c;../../src/zfile.c;../../src/zframe.c;../../src/zgossip.c;../../src/zgossip_msg.c;../../src/zgossip_msg.h;../../src/zhash.c;../../src/zhashx.c;../../src/zhttp_client.c;../../src/zhttp_private_selftest.c;../../src/zhttp_request.c;../../src/zhttp_response.c;../../src/zhttp_selftest.c;../../src/zhttp_server.c;../../src/zhttp_server_options.c;../../src/ziflist.c;../../src/zlist.c;../../src/zlistx.c;../../src/zloop.c;../../src/zmakecert.c;../../src/zmonitor.c;../../src/zmsg.c;../../src/zpoller.c;../../src/zproc.c;../../src/zproxy.c;../../src/zrex.c;../../src/zsock.c;../../src/zsp.c;../../src/zstr.c;../../src/zsys.c;../../src/ztimerset.c;../../src/ztrie.c;../../src/zuuid.c"
IDS=$(echo -en "\n\b")
[ -n "$MAKE" ] || MAKE=make
for FILE in $FILES
do
    clang-format -style=file -output-replacements-xml "$FILE" | grep "<replacement " >/dev/null && \
    {
      echo "$FILE is not correctly formatted" >&2
      FAILED=1
    }
done
if [ "$FAILED" -eq "1" ] ; then
    if [ "$1" = "--CI" ] ; then
        echo "Style mismatches were found by clang-format; detailing below:" >&2
        ${MAKE} clang-format-diff
        if test x"${CI_REQUIRE_GOOD_CLANG_FORMAT}" = xtrue ; then
            echo "FAILED : Style checks have failed and CI_REQUIRE_GOOD_CLANG_FORMAT==true" >&2
            exit 1
        fi
        echo "WARNING : Style checks have failed, but the result is not-fatal because CI_REQUIRE_GOOD_CLANG_FORMAT!=true" >&2
        exit 0
    fi
    exit 1
fi
