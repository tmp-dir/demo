cmake_minimum_required(VERSION 3.10)
project(YieldCurve CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# against spec, but i didnt want to do assert() testing.
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)


add_subdirectory(src)

add_executable(YieldCurve_Tests test/test_main.cpp)
add_subdirectory(test)

target_link_libraries(YieldCurve_Tests
    PRIVATE
        YieldCurve_Lib
        gtest_main
        gtest
)

include(GoogleTest)
gtest_discover_tests(YieldCurve_Tests)
