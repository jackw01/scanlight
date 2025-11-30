cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

set(PICO_PLATFORM "rp2040")
set(PICO_BOARD "pico")

project(scanlight_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_subdirectory(firmware_sl2)
add_subdirectory(firmware_bsl1)
