project("BachelorThesis_app" C CXX ASM)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(BachelorThesis_driver_component_cpp_sources
    ../driver/src/GPIO.cpp
    ../driver/src/ClockControl.cpp
    ../driver/src/ResetControl.cpp
    ../driver/src/USART.cpp
    ../driver/src/SysTick.cpp
    ../driver/src/InterruptController.cpp
    ../driver/src/DMA2D.cpp
    ../driver/src/I2C.cpp
    ../driver/src/PowerControl.cpp
    ../driver/src/EXTI.cpp
    ../driver/src/SystemConfig.cpp
    ../driver/src/LTDC.cpp
    ../driver/src/DSIHost.cpp
    ../driver/src/FlashController.cpp
    ../driver/src/DriverManager.cpp)

set(BachelorThesis_bsp_component_cpp_sources
    ../bsp/src/MFXSTM32L152.cpp
    ../bsp/src/RaydiumRM67160.cpp
    ../bsp/src/FT3267.cpp)

set(BachelorThesis_module_component_cpp_sources
    ../module/src/GUIRectangleBase.cpp
    ../module/src/GUIRectangle.cpp
    ../module/src/GUIImage.cpp
    ../module/src/GUISceneBase.cpp
    ../module/src/GUIContainer.cpp
    ../module/src/USARTLogger.cpp
    ../module/src/GUITouchEvent.cpp
    ../module/src/GUITouchController.cpp
    ../module/src/FT3267TouchDevice.cpp)

set(BachelorThesis_app_component_cpp_sources
    config/DSIHostConfig.cpp
    config/FT3267Config.cpp
    config/GPIOConfig.cpp
    config/I2CConfig.cpp
    config/LTDCConfig.cpp
    config/MFXSTM32L152Config.cpp
    config/RaydiumRM67160Config.cpp
    config/EXTIConfig.cpp
    config/ClockControlConfig.cpp
    config/SysTickConfig.cpp
    config/USARTConfig.cpp
    src/InterruptDispatcher.cpp
    src/GUIObjectDescription.cpp
    src/Startup.cpp
    src/AppFrameBuffer.cpp)

add_executable(app ${BachelorThesis_driver_component_cpp_sources}
                   ${BachelorThesis_bsp_component_cpp_sources}
                   ${BachelorThesis_module_component_cpp_sources}
                   ${BachelorThesis_app_component_cpp_sources}
                   src/startup_stm32l4r9xx.s
                   src/system_stm32l4xx.c
                   src/syscalls.c
                   src/main.c
                   bitmaps/brightness.c
                   bitmaps/untzLogo.c
                   bitmaps/playButton.c)

target_include_directories(app PRIVATE
                           inc
                           config
                           ../utility/interface
                           ../utility/inc
                           ../driver/inc
                           ../driver/sdk
                           ../bsp/inc
                           ../module/interface
                           ../module/inc)

target_compile_definitions(app PRIVATE
                           -DSTM32L4R9xx)

set_property(TARGET app PROPERTY CXX_STANDARD 14)

target_compile_options(app PRIVATE
                       -mcpu=cortex-m4
                       -march=armv7e-m
                       -mlittle-endian
                       -mthumb
                       -mfpu=fpv4-sp-d16
                       -mfloat-abi=hard
                       -Wfatal-errors
                       -Wall
                       -g
                       -O0)

target_link_options(app PRIVATE
                    -T${CMAKE_SOURCE_DIR}/linker/STM32L4R9AIIx_FLASH.ld
                    -mcpu=cortex-m4
                    -march=armv7e-m
                    -mlittle-endian
                    -mthumb
                    -mfpu=fpv4-sp-d16
                    -mfloat-abi=hard
                    -specs=nano.specs
                    -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
                    -Wl,--gc-sections
                    -Wl,-static
                    -Wl,-Map=app.map,--cref)

# Print executable size
add_custom_command(TARGET app
                   POST_BUILD
                   COMMAND arm-none-eabi-size app)

# Create hex file
add_custom_command(TARGET app
                   POST_BUILD
                   COMMAND arm-none-eabi-objcopy -O ihex app app.hex
                   COMMAND arm-none-eabi-objcopy -O binary app app.bin)