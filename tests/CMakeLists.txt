project(QuartzTests)

include(CTest)

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
    test_types.cpp
)

add_executable(qztest ${qz_test_sources})
target_link_libraries(qztest PRIVATE quartz)
target_link_libraries(qztest PRIVATE googletest)

# find tests and make available to IDEs
include(GoogleTest)
gtest_discover_tests(qztest)
