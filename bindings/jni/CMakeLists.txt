cmake_minimum_required (VERSION 2.8)

project (czmqjni CXX)
enable_language (C)

find_package (JNI REQUIRED)
find_library (ZMQ_LIBRARY NAMES zmq libzmq REQUIRED)
find_library (CZMQ_LIBRARY NAMES czmq REQUIRED)

include_directories (${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} src/native/include)

set (czmqjni_sources
    src/main/c/org_zeromq_czmq_Zarmour.c
    src/main/c/org_zeromq_czmq_Zconfig.c
    src/main/c/org_zeromq_czmq_Zdir.c
    src/main/c/org_zeromq_czmq_ZdirPatch.c
    src/main/c/org_zeromq_czmq_Zfile.c
    src/main/c/org_zeromq_czmq_Zframe.c
    src/main/c/org_zeromq_czmq_Zhash.c
    src/main/c/org_zeromq_czmq_Zhashx.c
    src/main/c/org_zeromq_czmq_Ziflist.c
    src/main/c/org_zeromq_czmq_Zlist.c
    src/main/c/org_zeromq_czmq_Zloop.c
    src/main/c/org_zeromq_czmq_Zmsg.c
    src/main/c/org_zeromq_czmq_Zpoller.c
    src/main/c/org_zeromq_czmq_Zsock.c
    src/main/c/org_zeromq_czmq_Zstr.c
    src/main/c/org_zeromq_czmq_Ztrie.c
    src/main/c/org_zeromq_czmq_Zuuid.c
)

add_library (czmqjni SHARED ${czmqjni_sources})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -O2")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

target_link_libraries (czmqjni ${ZMQ_LIBRARY} ${CZMQ_LIBRARY})
