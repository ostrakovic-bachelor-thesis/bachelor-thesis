project("BachelorThesis_module")
cmake_minimum_required(VERSION 3.0)

set(GOOGLETEST_HOME ../googletest/)

include_directories(${GOOGLETEST_HOME}/googletest/include ${GOOGLETEST_HOME}/googlemock/include)
include_directories(interface)
include_directories(inc)
include_directories(test)
include_directories(mock)
include_directories(../bsp/inc)
include_directories(../bsp/mock)
include_directories(../driver/sdk)
include_directories(../driver/inc)
include_directories(../driver/mock)
include_directories(../utility/interface)
include_directories(../utility/inc)
link_directories(${GOOGLETEST_HOME}/build/lib)
add_compile_options(-std=c++14 -Wall -DUNIT_TEST)

set(BachelorThesis_utility_component_cpp_sources
    ../utility/src/MemoryAccess.cpp)

set(BachelorThesis_driver_component_cpp_sources
    ../driver/src/GPIO.cpp
    ../driver/src/ClockControl.cpp
    ../driver/src/ResetControl.cpp
    ../driver/src/USART.cpp
    ../driver/src/DMA2D.cpp
    ../driver/src/SysTick.cpp)

set(BachelorThesis_bsp_component_cpp_sources
    ../bsp/src/FT3267.cpp)

set(BachelorThesis_module_component_cpp_sources
    src/GUIRectangleBase.cpp
    src/GUIRectangle.cpp
    src/GUIImage.cpp
    src/GUIContainer.cpp
    src/GUISceneBase.cpp
    src/USARTLogger.cpp
    src/TouchEvent.cpp
    src/FT3267TouchDevice.cpp)

set(BachelorThesis_module_test_cpp_sources
    test/GPIOManagerTest.cpp
    test/IFrameBufferTest.cpp
    test/FrameBufferTest.cpp
    test/GUICommonTest.cpp
    test/GUIRectangleBaseTest.cpp
    test/GUIRectangleTest.cpp
    test/GUIImageTest.cpp
    test/GUIContainerTest.cpp
    #test/GUISceneBaseTest.cpp
    #test/GUISceneTest.cpp
    test/TouchEventTest.cpp
    test/FT3267TouchDeviceTest.cpp
    test/GUITouchControllerTest.cpp
    test/USARTLoggerTest.cpp)

add_executable(test ${BachelorThesis_utility_component_cpp_sources}
                    ${BachelorThesis_driver_component_cpp_sources}
                    ${BachelorThesis_bsp_component_cpp_sources}
                    ${BachelorThesis_module_component_cpp_sources}
                    ${BachelorThesis_module_test_cpp_sources})

target_link_libraries(test libgtestd.a)
target_link_libraries(test libgmockd.a)
target_link_libraries(test libgtest_maind.a)
target_link_libraries(test pthread)