project("BachelorThesis_utility")
cmake_minimum_required(VERSION 3.0)

set(GOOGLETEST_HOME ../googletest/)

include_directories(${GOOGLETEST_HOME}/googletest/include ${GOOGLETEST_HOME}/googlemock/include)
include_directories(inc)
link_directories(${GOOGLETEST_HOME}/build/lib)
add_compile_options(-std=c++14 -Wall -DUNIT_TEST)

set(BachelorThesis_utility_component_cpp_sources
    src/MemoryAccess.cpp)

set(BachelorThesis_utility_test_cpp_sources
    test/MemoryAccessTest.cpp
    test/MemoryUtilityTest.cpp
    test/RegisterUtilityTest.cpp)

add_executable(test ${BachelorThesis_utility_component_cpp_sources} ${BachelorThesis_utility_test_cpp_sources})

target_link_libraries(test libgtestd.a)
target_link_libraries(test libgmockd.a)
target_link_libraries(test libgtest_maind.a)
target_link_libraries(test pthread)