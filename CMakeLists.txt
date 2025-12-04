cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(rp2040-ws2811-client)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.cpp ws2811.cpp
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2811.pio)


target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_pio
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME} pico_stdlib pico_multicore pico_mem_ops hardware_pio hardware_dma)


pico_add_extra_outputs(${PROJECT_NAME})
