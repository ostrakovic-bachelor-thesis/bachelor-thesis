project("BachelorThesis_bsp")
cmake_minimum_required(VERSION 3.0)

include_directories($ENV{GOOGLETEST_HOME}/googletest/include $ENV{GOOGLETEST_HOME}/googlemock/include)
include_directories(inc)
link_directories($ENV{GOOGLETEST_HOME}/build/lib)
add_compile_options(-std=c++14 -Wall)

set(BachelorThesis_bsp_component_cpp_source)

set(BachelorThesis_bsp_test_cpp_sources)

add_executable(test ${BachelorThesis_bsp_component_cpp_sources} ${BachelorThesis_bsp_test_cpp_sources})

target_link_libraries(test libgtest.a)
target_link_libraries(test libgmock.a)
target_link_libraries(test libgtest_main.a)
target_link_libraries(test pthread)