cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(ws2812_test)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.cpp
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_pio
)

pico_add_extra_outputs(${PROJECT_NAME})
