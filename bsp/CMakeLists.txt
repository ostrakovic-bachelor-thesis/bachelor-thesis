project("BachelorThesis_bsp")
cmake_minimum_required(VERSION 3.0)

set(GOOGLETEST_HOME ../googletest/)

include_directories(${GOOGLETEST_HOME}/googletest/include ${GOOGLETEST_HOME}/googlemock/include)
include_directories(inc)
include_directories(mock)
include_directories(test)
include_directories(../utility/inc)
include_directories(../driver/sdk)
include_directories(../driver/inc)
include_directories(../driver/mock)
link_directories(${GOOGLETEST_HOME}/build/lib)
add_compile_options(-std=c++14 -Wall -DUNIT_TEST)

set(BachelorThesis_utility_component_cpp_sources
    ../utility/src/MemoryAccess.cpp)

set(BachelorThesis_driver_component_cpp_sources
    ../driver/src/GPIO.cpp
    ../driver/src/ClockControl.cpp
    ../driver/src/ResetControl.cpp
    ../driver/src/USART.cpp
    ../driver/src/SysTick.cpp
    ../driver/src/DMA2D.cpp
    ../driver/src/LTDC.cpp
    ../driver/src/I2C.cpp
    ../driver/src/PowerControl.cpp
    ../driver/src/SystemConfig.cpp
    ../driver/src/EXTI.cpp
    ../driver/src/LTDC.cpp
    ../driver/src/DSIHost.cpp
    ../driver/src/InterruptController.cpp
    ../driver/src/DriverManager.cpp)

set(BachelorThesis_bsp_component_cpp_sources
    src/MFXSTM32L152.cpp
    src/RaydiumRM67160.cpp)

set(BachelorThesis_bsp_test_cpp_sources
    #test/MFXSTM32L152Test.cpp
    test/RaydiumRM67160Test.cpp)

add_executable(test ${BachelorThesis_utility_component_cpp_sources}
                    ${BachelorThesis_driver_component_cpp_sources}
                    ${BachelorThesis_bsp_component_cpp_sources}
                    ${BachelorThesis_bsp_test_cpp_sources})

target_link_libraries(test libgtestd.a)
target_link_libraries(test libgmockd.a)
target_link_libraries(test libgtest_maind.a)
target_link_libraries(test pthread)