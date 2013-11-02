#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################

cmake_minimum_required (VERSION 2.8)

project (czmq)

set (SOURCES_files_Source_Files
    src/zauth.c
    src/zbeacon.c
    src/zcert.c
    src/zcertstore.c
    src/zchunk.c
    src/zclock.c
    src/zconfig.c
    src/zctx.c
    src/zdir.c
    src/zfile.c
    src/zframe.c
    src/zhash.c
    src/zlist.c
    src/zloop.c
    src/zmonitor.c
    src/zmsg.c
    src/zmutex.c
    src/zpoller.c
    src/zproxy.c
    src/zrex.c
    src/zsocket.c
    src/zsockopt.c
    src/zstr.c
    src/zsys.c
    src/zthread.c
    src/ztree.c
)
source_group ("Source Files" FILES ${SOURCES_files_Source_Files})

set (SOURCES_Source_Files
  ${SOURCES_files_Source_Files}
)

set (SOURCES_files_Header_Files
    include/czmq.h
    include/czmq_prelude.h
    include/zauth.h
    include/zbeacon.h
    include/zcert.h
    include/zcertstore.h
    include/zchunk.h
    include/zclock.h
    include/zconfig.h
    include/zctx.h
    include/zdir.h
    include/zfile.h
    include/zframe.h
    include/zhash.h
    include/zlist.h
    include/zloop.h
    include/zmonitor.h
    include/zmsg.h
    include/zmutex.h
    include/zpoller.h
    include/zproxy.h
    include/zrex.h
    include/zsocket.h
    include/zsockopt.h
    include/zstr.h
    include/zsys.h
    include/zthread.h
    include/ztree.h
)
source_group ("Header Files" FILES ${SOURCES_files_Header_Files})

set (SOURCES_Header_Files
  ${SOURCES_files_Header_Files}
)

set (SOURCES_
  ${SOURCES_Source_Files}
  ${SOURCES_Header_Files}
)

set (WITH_LIBZMQ .)

include_directories (
  ../libzmq/include
  include
  ${CMAKE_INSTALL_PREFIX}/include
  ${WITH_LIBZMQ}
)

set_source_files_properties (
    ${SOURCES_files_Source_Files} PROPERTIES LANGUAGE CXX
)

add_library (
    czmq
    ${SOURCES_}
)

install (
    TARGETS czmq
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install (
    FILES ${SOURCES_files_Header_Files} DESTINATION include
)

#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################
