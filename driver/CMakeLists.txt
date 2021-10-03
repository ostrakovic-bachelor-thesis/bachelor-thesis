project("BachelorThesis_driver")
cmake_minimum_required(VERSION 3.0)

set(GOOGLETEST_HOME ../googletest/)

include_directories(${GOOGLETEST_HOME}/googletest/include ${GOOGLETEST_HOME}/googlemock/include)
include_directories(inc)
include_directories(test)
include_directories(mock)
include_directories(sdk)
include_directories(../utility/inc)
link_directories(${GOOGLETEST_HOME}/build/lib)
add_compile_options(-std=c++14 -Wall -DUNIT_TEST)

set(BachelorThesis_utility_component_cpp_sources
    ../utility/src/MemoryAccess.cpp)

set(BachelorThesis_driver_component_cpp_sources
    src/GPIO.cpp
    src/ClockControl.cpp
    src/ResetControl.cpp
    src/USART.cpp
    src/SysTick.cpp
    src/DMA2D.cpp
    src/LTDC.cpp
    src/I2C.cpp
    src/InterruptController.cpp
    src/DriverManager.cpp)

set(BachelorThesis_driver_test_cpp_sources
    test/DriverTest.cpp
    test/GPIOTest.cpp
    test/ClockControlTest.cpp
    test/ResetControlTest.cpp
    test/USARTTest.cpp
    test/SysTickTest.cpp
    test/InterruptControllerTest.cpp
    test/DMA2DTest.cpp
    test/LTDCTest.cpp
    test/I2CTest.cpp
    test/DriverManagerTest.cpp)

add_executable(test ${BachelorThesis_utility_component_cpp_sources}
                    ${BachelorThesis_driver_component_cpp_sources}
                    ${BachelorThesis_driver_test_cpp_sources})

target_link_libraries(test libgtestd.a)
target_link_libraries(test libgmockd.a)
target_link_libraries(test libgtest_maind.a)
target_link_libraries(test pthread)