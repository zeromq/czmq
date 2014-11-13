#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 2.8)
project(czmq)
enable_language(C)
enable_testing()

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

########################################################################
# determine version
########################################################################
foreach(which MAJOR MINOR PATCH)
    file(STRINGS "${SOURCE_DIR}/include/czmq.h" CZMQ_VERSION_STRING REGEX "#define CZMQ_VERSION_${which}")
    string(REGEX MATCH "#define CZMQ_VERSION_${which} ([0-9_]+)" CZMQ_REGEX_MATCH "${CZMQ_VERSION_STRING}")
    if (NOT CZMQ_REGEX_MATCH)
        message(FATAL_ERROR "failed to parse CZMQ_VERSION_${which} from czmq.h")
    endif()
    set(CZMQ_${which}_VERSION ${CMAKE_MATCH_1})
endforeach(which)

set(CZMQ_VERSION ${CZMQ_MAJOR_VERSION}.${CZMQ_MINOR_VERSION}.${CZMQ_PATCH_VERSION})

########################################################################
# platform.h
########################################################################
include(CheckIncludeFile)
CHECK_INCLUDE_FILE("linux/wireless.h" HAVE_LINUX_WIRELESS_H)
CHECK_INCLUDE_FILE("net/if_media.h" HAVE_NET_IF_MEDIA_H)

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS("getifaddrs" HAVE_GETIFADDRS)
CHECK_FUNCTION_EXISTS("freeifaddrs" HAVE_FREEIFADDRS)

include(CheckIncludeFiles)
check_include_files("sys/socket.h;net/if.h" HAVE_NET_IF_H)
if (NOT HAVE_NET_IF_H)
    CHECK_INCLUDE_FILE("net/if.h" HAVE_NET_IF_H)
endif()

file(WRITE ${BINARY_DIR}/platform.h.in "
#cmakedefine HAVE_LINUX_WIRELESS_H
#cmakedefine HAVE_NET_IF_H
#cmakedefine HAVE_NET_IF_MEDIA_H
#cmakedefine HAVE_GETIFADDRS
#cmakedefine HAVE_FREEIFADDRS
")

configure_file(${BINARY_DIR}/platform.h.in ${BINARY_DIR}/platform.h)

#The MSVC C compiler is too out of date,
#so the sources have to be compiled as c++
if (MSVC)
    enable_language(CXX)
    file(GLOB sources ${SOURCE_DIR}/src/*.c)
    set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)
    set(MORE_LIBRARIES ws2_32 Rpcrt4 Iphlpapi)
endif()

# required libraries for mingw
if (MINGW)
    set(MORE_LIBRARIES -lws2_32 -lrpcrt4 -liphlpapi)
endif()

########################################################################
# ZeroMQ depedency
########################################################################
list(APPEND CMAKE_MODULE_PATH ${SOURCE_DIR})
find_package(ZeroMQ REQUIRED)

########################################################################
# includes
########################################################################
set (czmq_headers
    include/czmq.h
    include/czmq_prelude.h
    include/zactor.h
    include/zauth.h
    include/zarmour.h
    include/zbeacon.h
    include/zcert.h
    include/zcertstore.h
    include/zchunk.h
    include/zclock.h
    include/zconfig.h
    include/zdigest.h
    include/zdir.h
    include/zdir_patch.h
    include/zfile.h
    include/zframe.h
    include/zgossip.h
    include/zhashx.h
    include/ziflist.h
    include/zlistx.h
    include/zloop.h
    include/zmonitor.h
    include/zmsg.h
    include/zpoller.h
    include/zproxy.h
    include/zrex.h
    include/zsock.h
    include/zsock_option.h
    include/zstr.h
    include/zsys.h
    include/zuuid.h
    src/zgossip_msg.h
    include/zauth_v2.h
    include/zbeacon_v2.h
    include/zctx.h
    include/zhash.h
    include/zlist.h
    include/zmonitor_v2.h
    include/zmutex.h
    include/zproxy_v2.h
    include/zsocket.h
    include/zsockopt.h
    include/zthread.h
    src/zgossip_engine.inc
    src/zhash_primes.inc
    src/zclass_example.xml
)
source_group ("Header Files" FILES ${czmq_headers})
install(FILES ${czmq_headers} DESTINATION include)

########################################################################
# library
########################################################################
include_directories(${BINARY_DIR})
include_directories(${SOURCE_DIR}/include)
include_directories(${ZEROMQ_INCLUDE_DIRS})
set (czmq_sources
    src/zactor.c
    src/zauth.c
    src/zarmour.c
    src/zbeacon.c
    src/zcert.c
    src/zcertstore.c
    src/zchunk.c
    src/zclock.c
    src/zconfig.c
    src/zdigest.c
    src/zdir.c
    src/zdir_patch.c
    src/zfile.c
    src/zframe.c
    src/zgossip.c
    src/zhashx.c
    src/ziflist.c
    src/zlistx.c
    src/zloop.c
    src/zmonitor.c
    src/zmsg.c
    src/zpoller.c
    src/zproxy.c
    src/zrex.c
    src/zsock.c
    src/zsock_option.c
    src/zstr.c
    src/zsys.c
    src/zuuid.c
    src/zgossip_msg.c
    src/zauth_v2.c
    src/zbeacon_v2.c
    src/zctx.c
    src/zhash.c
    src/zlist.c
    src/zmonitor_v2.c
    src/zmutex.c
    src/zproxy_v2.c
    src/zsocket.c
    src/zsockopt.c
    src/zthread.c
)
source_group ("Source Files" FILES ${czmq_sources})
add_library(czmq SHARED ${czmq_sources})
set_target_properties(czmq PROPERTIES DEFINE_SYMBOL "LIBCZMQ_EXPORTS")
target_link_libraries(czmq ${ZEROMQ_LIBRARIES} ${MORE_LIBRARIES})

install(TARGETS czmq
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# pkgconfig
########################################################################
set(VERSION "${CZMQ_VERSION}")
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${prefix}/lib${LIB_SUFFIX}")
set(includedir "\${prefix}/include")
configure_file(
    ${SOURCE_DIR}/src/libczmq.pc.in
    ${BINARY_DIR}/libczmq.pc
@ONLY)

install(
    FILES ${BINARY_DIR}/libczmq.pc
    DESTINATION lib${LIB_SUFFIX}/pkgconfig
)

########################################################################
# tests
########################################################################
add_executable(czmq_selftest ${SOURCE_DIR}/src/czmq_selftest.c)
target_link_libraries(czmq_selftest czmq ${ZEROMQ_LIBRARIES})
add_test(czmq_selftest czmq_selftest)

########################################################################
# summary
########################################################################
message(STATUS "version: ${CZMQ_VERSION}")
message(STATUS "install: ${CMAKE_INSTALL_PREFIX}")

#################################################################
#   GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY    #
#   Please read the README.txt file in the model directory.     #
#################################################################
