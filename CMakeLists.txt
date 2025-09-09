cmake_minimum_required(VERSION 3.28)

project(Quartz LANGUAGES CXX)

set(qz_main_project 0)
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(qz_main_project 1)
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(QZ_BUILD_TESTS "Option for building test subproject." ${qz_main_project})

set(qz_header_files
    include/quartz/types.hpp
    include/quartz.hpp
)
set(qz_source_files
)

add_library(quartz ${qz_header_files} ${qz_source_files})
set_target_properties(quartz
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        LINKER_LANGUAGE CXX
)

target_include_directories(quartz PUBLIC include)
target_include_directories(quartz PRIVATE source)

if (${QZ_BUILD_TESTS})
    add_subdirectory(tests)
endif ()
