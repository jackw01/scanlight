add_subdirectory(vendor/rp2040-button)

add_executable(bsl1_controller)
add_executable(sl4_controller)

pico_enable_stdio_usb(bsl1_controller 1)
pico_enable_stdio_uart(bsl1_controller 0)
pico_enable_stdio_usb(sl4_controller 1)
pico_enable_stdio_uart(sl4_controller 0)

target_compile_definitions(bsl1_controller PRIVATE PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1 HW_VERSION_BSL1)
target_compile_definitions(sl4_controller PRIVATE PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1 HW_VERSION_SL4)

target_sources(bsl1_controller
    PRIVATE main.c protocol.c
)

target_sources(sl4_controller
    PRIVATE main.c protocol.c
)

target_link_libraries(bsl1_controller
    pico_stdlib
    pico_bootrom
    pico_multicore
    hardware_pwm
    hardware_adc
    hardware_clocks
    hardware_gpio
    hardware_flash
    hardware_sync
    button
)

target_link_libraries(sl4_controller
    pico_stdlib
    pico_bootrom
    pico_multicore
    hardware_pwm
    hardware_adc
    hardware_clocks
    hardware_gpio
    hardware_flash
    hardware_sync
    button
)

pico_add_extra_outputs(bsl1_controller)
pico_add_extra_outputs(sl4_controller)
