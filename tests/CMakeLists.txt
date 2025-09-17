project(QuartzTests)

include(CTest)

if((CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    add_compile_options(-Wno-character-conversion -Wno-error=character-conversion)
endif()

# GoogleTest: https://github.com/google/googletest
# Docs: https://google.github.io/googletest/quickstart-cmake.html

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(qz_test_sources
    test_atomic.cpp
    test_allocator.cpp
    test_array.cpp
    test_assert.cpp
    test_types.cpp
)

add_executable(qztest ${qz_test_sources})
set_target_properties(qztest
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        LINKER_LANGUAGE CXX
)
target_link_libraries(qztest PRIVATE quartz)
target_link_libraries(qztest PRIVATE GTest::gtest_main)

# find tests and make available to IDEs
include(GoogleTest)
gtest_discover_tests(qztest)
