project("BachelorThesis_driver")
cmake_minimum_required(VERSION 3.0)

include_directories($ENV{GOOGLETEST_HOME}/googletest/include $ENV{GOOGLETEST_HOME}/googlemock/include)
include_directories(inc)
include_directories(test)
include_directories(sdk)
include_directories(../utility/inc)
link_directories($ENV{GOOGLETEST_HOME}/build/lib)
add_compile_options(-std=c++14 -Wall -DUNIT_TEST_DRIVER)

set(BachelorThesis_utility_component_cpp_sources
    ../utility/src/MemoryAccess.cpp
    ../utility/src/MemoryUtility.cpp)

set(BachelorThesis_driver_component_cpp_sources
    src/GPIO.cpp
    src/RCC.cpp
    src/USART.cpp)

set(BachelorThesis_driver_test_cpp_sources
    test/DriverTest.cpp
    test/GPIOTest.cpp
    test/RCCTest.cpp
    test/USARTTest.cpp)

add_executable(test ${BachelorThesis_utility_component_cpp_sources} 
                    ${BachelorThesis_driver_component_cpp_sources} 
                    ${BachelorThesis_driver_test_cpp_sources})

target_link_libraries(test libgtest.a)
target_link_libraries(test libgmock.a)
target_link_libraries(test libgtest_main.a)
target_link_libraries(test pthread)