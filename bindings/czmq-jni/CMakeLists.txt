cmake_minimum_required (VERSION 2.8)

project(czmqjni CXX)
enable_language(C)

find_package(JNI REQUIRED)
find_library(ZMQ_LIBRARY NAMES zmq libzmq REQUIRED)
find_library(CZMQ_LIBRARY NAMES czmq REQUIRED)

include_directories(
  ${JAVA_INCLUDE_PATH}
  ${JAVA_INCLUDE_PATH2}
  src/native/include)

set (czmqjni_sources
   src/main/c/zctx.c
   src/main/c/zframe.c
   src/main/c/zmsg.c
   src/main/c/zsock.c
)

add_library(czmqjni SHARED ${czmqjni_sources})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -O2")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

target_link_libraries(czmqjni ${ZMQ_LIBRARY} ${CZMQ_LIBRARY})
