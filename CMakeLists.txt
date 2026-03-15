cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD pico_w)

include(lib/pico-sdk/pico_sdk_init.cmake)

project(ece5610-project C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    src/main.c
    src/imu.c
    src/motor.c
)

target_include_directories(ece5610-project PRIVATE inc)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_cyw43_arch_none
    hardware_gpio
    hardware_i2c
    hardware_adc
)