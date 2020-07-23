# additional target to perform clang-format run, requires clang-format

# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES
     RELATIVE ${CMAKE_CURRENT_BINARY_DIR}
     ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*.cpp
     ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.hpp
     ${PROJECT_SOURCE_DIR}/tests/*.c ${PROJECT_SOURCE_DIR}/tests/*.cc ${PROJECT_SOURCE_DIR}/tests/*.cpp
     ${PROJECT_SOURCE_DIR}/tests/*.h ${PROJECT_SOURCE_DIR}/tests/*.hpp
     ${PROJECT_SOURCE_DIR}/perf/*.c ${PROJECT_SOURCE_DIR}/perf/*.cc ${PROJECT_SOURCE_DIR}/perf/*.cpp
     ${PROJECT_SOURCE_DIR}/perf/*.h ${PROJECT_SOURCE_DIR}/perf/*.hpp
     ${PROJECT_SOURCE_DIR}/tools/*.c ${PROJECT_SOURCE_DIR}/tools/*.cc ${PROJECT_SOURCE_DIR}/tools/*.cpp
     ${PROJECT_SOURCE_DIR}/tools/*.h ${PROJECT_SOURCE_DIR}/tools/*.hpp
     ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/*.hpp
    )

if("${CLANG_FORMAT}" STREQUAL "")
  set(CLANG_FORMAT "clang-format")
endif()

if (NOT TARGET clang-format)
    add_custom_target(
        clang-format
        COMMAND ${CLANG_FORMAT} -style=file -i ${ALL_SOURCE_FILES}
    )
endif()

function(JOIN VALUES GLUE OUTPUT)
  string (REPLACE ";" "${GLUE}" _TMP_STR "${VALUES}")
  set (${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endfunction()

configure_file(builds/cmake/clang-format-check.sh.in clang-format-check.sh @ONLY)

if (NOT TARGET clang-format-check)
    add_custom_target(
        clang-format-check
        COMMAND chmod +x clang-format-check.sh
        COMMAND ./clang-format-check.sh
        COMMENT "Checking correct formatting according to .clang-format file using ${CLANG_FORMAT}"
    )
endif()

if (NOT TARGET clang-format-check-CI)
    add_custom_target(
        clang-format-check-CI
        COMMAND chmod +x clang-format-check.sh
        COMMAND ./clang-format-check.sh --CI
        COMMENT "Checking correct formatting according to .clang-format file using ${CLANG_FORMAT}"
    )
endif()


if (NOT TARGET clang-format-diff)
    add_custom_target(
        clang-format-diff
        COMMAND ${CLANG_FORMAT} -style=file -i ${ALL_SOURCE_FILES}
        COMMAND git diff ${ALL_SOURCE_FILES}
        COMMENT "Formatting with clang-format (using ${CLANG_FORMAT}) and showing differences with latest commit"
    )
endif()

