project("BachelorThesis_utility")
cmake_minimum_required(VERSION 3.0)

include_directories($ENV{GOOGLETEST_HOME}/googletest/include $ENV{GOOGLETEST_HOME}/googlemock/include)
include_directories(inc)
link_directories($ENV{GOOGLETEST_HOME}/build/lib)
add_compile_options(-std=c++14 -Wall -DUNIT_TEST_DRIVER)

set(BachelorThesis_utility_component_cpp_sources
    src/MemoryAccess.cpp
    src/MemoryUtility.cpp)

set(BachelorThesis_utility_test_cpp_sources
    test/MemoryAccessTest.cpp
    test/MemoryUtilityTest.cpp)

add_executable(test ${BachelorThesis_utility_component_cpp_sources} ${BachelorThesis_utility_test_cpp_sources})

target_link_libraries(test libgtest.a)
target_link_libraries(test libgmock.a)
target_link_libraries(test libgtest_main.a)
target_link_libraries(test pthread)