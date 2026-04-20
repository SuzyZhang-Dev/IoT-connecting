add_executable(part2_multiblinky
        main.cpp
)

pico_add_extra_outputs(part2_multiblinky)

target_link_libraries(part2_multiblinky
        pico_stdlib
        hardware_pwm
        hardware_gpio
)

pico_enable_stdio_usb(part2_multiblinky 0)
pico_enable_stdio_uart(part2_multiblinky 1)