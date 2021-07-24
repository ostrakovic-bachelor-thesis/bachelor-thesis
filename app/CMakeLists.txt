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
    ../driver/src/DriverManager.cpp)

set(BachelorThesis_app_component_cpp_sources
    src/InterruptDispatcher.cpp
    src/Startup.cpp)

add_executable(app ${BachelorThesis_driver_component_cpp_sources}
                   ${BachelorThesis_app_component_cpp_sources}
                   src/startup_stm32l4r9xx.s
                   src/system_stm32l4xx.c
                   src/syscalls.c
                   src/sysclock.c
                   src/main.c)
                  
target_include_directories(app PRIVATE
                           inc
                           ../driver/inc
                           ../driver/sdk
                           ../bsp/inc
                           ../module/inc
                           ../utility/inc) 

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

# TODO remove later
#ASFLAGS  = -g -O2 -Wfatal-errors
#ASFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -march=armv7e-m
#ASFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 

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