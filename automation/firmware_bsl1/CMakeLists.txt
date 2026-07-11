add_subdirectory(vendor/rp2040-button)

add_executable(bsl1_controller)
add_executable(sl4a_controller)
add_executable(sl4b_controller)
add_executable(sl2_controller)

pico_enable_stdio_usb(bsl1_controller 1)
pico_enable_stdio_uart(bsl1_controller 0)
pico_enable_stdio_usb(sl4a_controller 1)
pico_enable_stdio_uart(sl4a_controller 0)
pico_enable_stdio_usb(sl4b_controller 1)
pico_enable_stdio_uart(sl4b_controller 0)
pico_enable_stdio_usb(sl2_controller 1)
pico_enable_stdio_uart(sl2_controller 0)

target_compile_definitions(bsl1_controller PRIVATE PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1 HW_VERSION_BSL1)
target_compile_definitions(sl4a_controller PRIVATE PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1 HW_VERSION_SL4)
target_compile_definitions(sl4b_controller PRIVATE PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1 HW_VERSION_SL4 HW_SUPPORTS_FOCUS)
target_compile_definitions(sl2_controller PRIVATE PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1 HW_VERSION_SL2)

target_sources(bsl1_controller
    PRIVATE main.c protocol.c
)

target_sources(sl4a_controller
    PRIVATE main.c protocol.c
)

target_sources(sl4b_controller
    PRIVATE main.c protocol.c
)

target_sources(sl2_controller
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

target_link_libraries(sl4a_controller
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

target_link_libraries(sl4b_controller
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

target_link_libraries(sl2_controller
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
pico_add_extra_outputs(sl4a_controller)
pico_add_extra_outputs(sl4b_controller)
pico_add_extra_outputs(sl2_controller)
