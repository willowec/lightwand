# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(light-wand C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# include the relevant files
include_directories("src" "inc")

## Primary build target
add_executable(${PROJECT_NAME} 
	src/main.c
  inc/neopixels.h
  src/neopixels.c
  inc/ADXL343.h
  src/ADXL343.c 
  inc/alphabet.h 
)
# add the ws2812 library
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/lib/ws2812.pio)

pico_set_program_name(${PROJECT_NAME} "light-wand")
pico_set_program_version(${PROJECT_NAME} "0.5")

# add libraries
target_link_libraries(${PROJECT_NAME}
  pico_stdlib
  hardware_pio
  hardware_i2c
  pico_multicore
  pico_rand
)

# enable usb communication
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})

## Accelerometer data collection build target
add_executable(accelerometer_data
  src/accel_data_main.c
  inc/ADXL343.h
  src/ADXL343.c 
)

pico_set_program_name(accelerometer_data "light-wand-accel-data-acq")
pico_set_program_version(accelerometer_data "1.0")

# add libraries
target_link_libraries(accelerometer_data
  pico_stdlib
  hardware_pio
  hardware_i2c
)

# enable usb communication
pico_enable_stdio_usb(accelerometer_data 1)
pico_enable_stdio_uart(accelerometer_data 0)
pico_add_extra_outputs(accelerometer_data)