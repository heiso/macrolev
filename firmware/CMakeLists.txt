cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(macrolev)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

##############################
# Main
##############################

add_executable(main)

target_sources(main PUBLIC
  src/main.c
  src/usb_descriptors.c
  src/rotary_encoder.c
  src/void_switches.c
  src/DRV2605L.c
)

target_include_directories(main PUBLIC src)

# pull in common dependencies
target_link_libraries(main PUBLIC pico_stdlib hardware_pio hardware_i2c hardware_adc hardware_dma tinyusb_device tinyusb_board)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)

add_custom_command(TARGET main POST_BUILD COMMAND cp main.uf2 /Volumes/RPI-RP2/NEW.uf2)

##############################
# Rotary test
##############################

add_executable(test_rotary)

target_sources(test_rotary PUBLIC tests/rotary.c src/rotary_encoder.c)

target_include_directories(main PUBLIC src)

# pull in common dependencies
target_link_libraries(test_rotary PUBLIC pico_stdlib hardware_pio)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_rotary 1)
pico_enable_stdio_uart(test_rotary 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(test_rotary)

add_custom_command(TARGET test_rotary POST_BUILD COMMAND cp test_rotary.uf2 /Volumes/RPI-RP2/NEW.uf2)

##############################
# Usb test
##############################

add_executable(test_usb)

target_sources(test_usb PUBLIC tests/usb.c)

# pull in common dependencies
target_link_libraries(test_usb PUBLIC pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_usb 1)
pico_enable_stdio_uart(test_usb 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(test_usb)

add_custom_command(TARGET test_usb POST_BUILD COMMAND cp test_usb.uf2 /Volumes/RPI-RP2/NEW.uf2)

##############################
# Void switches test
##############################

add_executable(test_void_switches)

target_sources(test_void_switches PUBLIC tests/void_switches.c src/void_switches.c)

# pull in common dependencies
target_link_libraries(test_void_switches PUBLIC pico_stdlib hardware_adc)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_void_switches 1)
pico_enable_stdio_uart(test_void_switches 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(test_void_switches)

add_custom_command(TARGET test_void_switches POST_BUILD COMMAND cp test_void_switches.uf2 /Volumes/RPI-RP2/NEW.uf2)

##############################
# drv2605L test
##############################

add_executable(test_drv2605L)

target_sources(test_drv2605L PUBLIC tests/drv2605L.c src/DRV2605L.c)

# pull in common dependencies
target_link_libraries(test_drv2605L PUBLIC pico_stdlib hardware_i2c)

# enable usb output, disable uart output
pico_enable_stdio_usb(test_drv2605L 1)
pico_enable_stdio_uart(test_drv2605L 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(test_drv2605L)

add_custom_command(TARGET test_drv2605L POST_BUILD COMMAND cp test_drv2605L.uf2 /Volumes/RPI-RP2/NEW.uf2)