include(FindPkgConfig)

if(UNIX)
    PKG_CHECK_MODULES(PC_ZEROMQ "libzmq")
endif()

find_path(
    ZEROMQ_INCLUDE_DIRS
    NAMES zmq.h
    HINTS ${PC_ZEROMQ_INCLUDE_DIRS}
)

if(CMAKE_BUILD_TYPE MATCHES "Release")
    #
    # find release libraries
    #
    find_library(
        ZEROMQ_LIBRARIES_RELEASE
        NAMES zmq libzmq-mt-4_1_0.lib
        HINTS ${PC_ZEROMQ_LIBRARY_DIRS}
    )

    set(ZEROMQ_LIBRARIES ${ZEROMQ_LIBRARIES_RELEASE})
else()
    #
    # find debug libraries
    #
    find_library(
        ZEROMQ_LIBRARIES_DEBUG
        NAMES zmq libzmq-mt-gd-4_1_0.lib
        HINTS ${PC_ZEROMQ_LIBRARY_DIRS}
    )

    set(ZEROMQ_LIBRARIES ${ZEROMQ_LIBRARIES_DEBUG})
endif()


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZEROMQ DEFAULT_MSG ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIRS)
mark_as_advanced(ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIRS)
