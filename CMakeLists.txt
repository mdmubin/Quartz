cmake_minimum_required(VERSION 3.28)

project(Quartz LANGUAGES CXX VERSION 0.1.0)

set(QZ_MAIN_PROJECT 0)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(QZ_MAIN_PROJECT 1)
endif ()

option(QZ_BUILD_TESTS "Option for building test subproject." ${QZ_MAIN_PROJECT})
option(QZ_BUILD_DOCS "Option for building project documentations." ${QZ_MAIN_PROJECT})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(QZ_HEADER_FILES
    include/quartz/allocator.hpp
    include/quartz/array.hpp
    include/quartz/assert.hpp
    include/quartz/atomic.hpp
    include/quartz/atomic_operations.hpp
    include/quartz/macros.hpp
    include/quartz/types.hpp
    include/quartz/utilities.hpp
    include/quartz.hpp
)
set(QZ_SOURCE_FILES
    source/assert.cpp
)

add_library(quartz ${QZ_HEADER_FILES} ${QZ_SOURCE_FILES})
set_target_properties(quartz
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        LINKER_LANGUAGE CXX
)

target_include_directories(quartz PUBLIC include)
target_include_directories(quartz PRIVATE source)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(quartz PRIVATE atomic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT WIN32) # cannot link to libatomic for windows builds. Use windows intrinsics instead.
    target_link_libraries(quartz PRIVATE atomic)
endif ()

if (QZ_BUILD_TESTS)
    add_subdirectory(tests)
endif ()

if(QZ_BUILD_DOCS)
    add_subdirectory(docs)
endif()
