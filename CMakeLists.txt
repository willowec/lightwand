# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(light-wand C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()


# include the relevant files
include_directories("src" "inc")
add_executable(${PROJECT_NAME} 
	src/main.c
  src/neopixels.c
  inc/neopixels.h
)

# add the ws2812 library
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/lib/ws2812.pio)

pico_set_program_name(${PROJECT_NAME} "pico-solar-tester")
pico_set_program_version(${PROJECT_NAME} "0.1")

# add libraries
target_link_libraries(${PROJECT_NAME}
  pico_stdlib
  hardware_pio
)

# enable usb communication
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
