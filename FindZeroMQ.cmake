if(UNIX)
    include(FindPkgConfig)
    PKG_CHECK_MODULES(PC_ZEROMQ "libzmq")
endif()

find_path(
    ZEROMQ_INCLUDE_DIRS
    NAMES zmq.h
    HINTS ${PC_ZEROMQ_INCLUDE_DIRS}
)

#
# find debug libraries
#
find_library(
    ZEROMQ_LIBRARIES_DEBUG
    NAMES zmq libzmq-mt-gd-4_1_0.lib
    HINTS ${PC_ZEROMQ_LIBRARY_DIRS}
)

#
# find release libraries
#
find_library(
    ZEROMQ_LIBRARIES_RELEASE
    NAMES zmq libzmq-mt-4_1_0.lib
    HINTS ${PC_ZEROMQ_LIBRARY_DIRS}
)

if(NOT ZEROMQ_LIBRARIES_DEBUG   MATCHES "NOTFOUND" AND
   NOT ZEROMQ_LIBRARIES_RELEASE MATCHES "NOTFOUND")
elseif(NOT ZEROMQ_LIBRARIES_DEBUG   MATCHES "NOTFOUND")
    set(ZEROMQ_LIBRARIES ${ZEROMQ_LIBRARIES_DEBUG})
elseif(NOT ZEROMQ_LIBRARIES_RELEASE MATCHES "NOTFOUND")
    set(ZEROMQ_LIBRARIES ${ZEROMQ_LIBRARIES_RELEASE})
endif()

message("PC_ZEROMQ_INCLUDE_DIRS   - ${PC_ZEROMQ_INCLUDE_DIRS}")
message("PC_ZEROMQ_LIBRARY_DIRS   - ${PC_ZEROMQ_LIBRARY_DIRS}")
message("ZEROMQ_LIBRARIES_DEBUG   - ${ZEROMQ_LIBRARIES_DEBUG}")
message("ZEROMQ_LIBRARIES_RELEASE - ${ZEROMQ_LIBRARIES_RELEASE}")
message("ZEROMQ_LIBRARIES         - ${ZEROMQ_LIBRARIES}")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZEROMQ DEFAULT_MSG ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIRS)
mark_as_advanced(ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIRS)
