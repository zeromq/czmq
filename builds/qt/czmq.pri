DEFINES += CZMQ_STATIC CZMQ_BUILD_DRAFT_API

SOURCES += \
    $$PWD/../../src/zactor.c \
    $$PWD/../../src/zargs.c \
    $$PWD/../../src/zarmour.c \
    $$PWD/../../src/zauth.c \
    $$PWD/../../src/zbeacon.c \
    $$PWD/../../src/zcert.c \
    $$PWD/../../src/zcertstore.c \
    $$PWD/../../src/zchunk.c \
    $$PWD/../../src/zclock.c \
    $$PWD/../../src/zconfig.c \
    $$PWD/../../src/zdigest.c \
    $$PWD/../../src/zdir.c \
    $$PWD/../../src/zdir_patch.c \
    $$PWD/../../src/zfile.c \
    $$PWD/../../src/zframe.c \
    $$PWD/../../src/zgossip.c \
    $$PWD/../../src/zgossip_msg.c \
    $$PWD/../../src/zhash.c \
    $$PWD/../../src/zhashx.c \
    $$PWD/../../src/zhttp_client.c \
    $$PWD/../../src/zhttp_request.c \
    $$PWD/../../src/zhttp_response.c \
    $$PWD/../../src/zhttp_server.c \
    $$PWD/../../src/zhttp_server_options.c \
    $$PWD/../../src/ziflist.c \
    $$PWD/../../src/zlist.c \
    $$PWD/../../src/zlistx.c \
    $$PWD/../../src/zloop.c \
    $$PWD/../../src/zmonitor.c \
    $$PWD/../../src/zmsg.c \
    $$PWD/../../src/zpoller.c \
    $$PWD/../../src/zproc.c \
    $$PWD/../../src/zproxy.c \
    $$PWD/../../src/zrex.c \
    $$PWD/../../src/zsock.c \
    $$PWD/../../src/zstr.c \
    $$PWD/../../src/zsys.c \
    $$PWD/../../src/ztimerset.c \
    $$PWD/../../src/ztrie.c \
    $$PWD/../../src/zuuid.c

HEADERS += \
    $$PWD/../../src/czmq_classes.h \
    $$PWD/../../src/foreign/sha1/sha1.h \
    $$PWD/../../src/foreign/slre/slre.h \
    $$PWD/../../src/zgossip_msg.h

INCLUDEPATH += \
    $$PWD/../../include

unix {
INCLUDEPATH += \
    $$PWD/macos
}

win32 {
    LIBS += -L$$C:/Windows/System32 -lrpcrt4
}
