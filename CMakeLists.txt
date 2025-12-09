cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(rp2040-ws2811-client C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Debug: xem đường dẫn
message(STATUS "PICO_EXTRAS_PATH = ${PICO_EXTRAS_PATH}")

# Thêm pico-extras như subproject
add_subdirectory(${PICO_EXTRAS_PATH} pico-extras)

add_executable(${PROJECT_NAME}
    main.cpp
    ws2811.cpp
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2811.pio)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_multicore
    pico_mem_ops
    hardware_pio
    hardware_dma
    pico_sleep        # từ pico-extras
)

# NẾU vẫn không tìm được pico/sleep.h thì chỉ thêm include của pico_sleep, không thêm SDK
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PICO_EXTRAS_PATH}/src/rp2_common/pico_sleep/include
)

pico_add_extra_outputs(${PROJECT_NAME})
