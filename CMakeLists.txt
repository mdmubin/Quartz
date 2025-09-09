cmake_minimum_required(VERSION 3.28)

project(Quartz LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(qz_header_files
    include/quartz.hpp
)
set(qz_source_files
)

add_library(quartz ${qz_header_files} ${qz_source_files})
set_target_properties(quartz
    PROPERTIES
        CXX_STANDARD 20
        LINKER_LANGUAGE CXX
)

target_include_directories(quartz PUBLIC include)
target_include_directories(quartz PRIVATE source)
