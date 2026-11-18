cmake_minimum_required(VERSION 3.13)

set(PICOTOOL_FORCE_FETCH_FROM_GIT TRUE)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(PICO_PLATFORM "rp2040")
set(PICO_BOARD "pico")

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(scanlight_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_subdirectory(firmware_bsl1)
