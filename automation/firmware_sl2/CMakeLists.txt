set(OUTPUT_NAME "sl2_controller")

add_executable(${OUTPUT_NAME})

add_compile_options(
)

add_link_options(
)

pico_enable_stdio_usb(${OUTPUT_NAME} 1)
pico_enable_stdio_uart(${OUTPUT_NAME} 0)

target_compile_definitions(${OUTPUT_NAME} PRIVATE PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

target_sources(${OUTPUT_NAME}
    PRIVATE main.c
)

target_link_libraries(${OUTPUT_NAME}
    pico_stdlib
    pico_multicore
    hardware_pwm
    hardware_adc
    hardware_clocks
    hardware_gpio
)

pico_add_extra_outputs(${OUTPUT_NAME})
