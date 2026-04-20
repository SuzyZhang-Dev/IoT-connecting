add_executable(part1_blinkyD2
        main.cpp
)

pico_add_extra_outputs(part1_blinkyD2)

target_link_libraries(part1_blinkyD2
        pico_stdlib
)

pico_enable_stdio_usb(part1_blinkyD2 0)
pico_enable_stdio_uart(part1_blinkyD2 1)