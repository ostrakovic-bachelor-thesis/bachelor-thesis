project("BachelorThesis_module")
cmake_minimum_required(VERSION 3.0)

include_directories($ENV{GOOGLETEST_HOME}/googletest/include $ENV{GOOGLETEST_HOME}/googlemock/include)
include_directories(inc)
include_directories(test)
include_directories(../driver/inc)
link_directories($ENV{GOOGLETEST_HOME}/build/lib)
add_compile_options(-std=c++14 -Wall)

set(BachelorThesis_driver_component_cpp_sources
    ../driver/src/GPIO.cpp
    ../driver/src/ClockControl.cpp
    ../driver/src/ResetControl.cpp
    ../driver/src/USART.cpp)

set(BachelorThesis_module_component_cpp_sources)

set(BachelorThesis_module_test_cpp_sources)

add_executable(test ${BachelorThesis_driver_component_cpp_sources}
                    ${BachelorThesis_module_component_cpp_sources}
                    ${BachelorThesis_module_test_cpp_sources})

target_link_libraries(test libgtest.a)
target_link_libraries(test libgmock.a)
target_link_libraries(test libgtest_main.a)
target_link_libraries(test pthread)