
add_executable(part3_ssd1306
        main.cpp
        pico-modbus/display/mono_vlsb.cpp
        pico-modbus/display/mono_vlsb.h
        pico-modbus/display/ssd1306.cpp
        pico-modbus/display/ssd1306.h
        pico-modbus/display/i2c_device.h
        pico-modbus/i2c/PicoI2CBus.cpp
        pico-modbus/i2c/PicoI2CBus.h
        pico-modbus/i2c/PicoI2CDevice.cpp
        pico-modbus/i2c/PicoI2CDevice.h
        pico-modbus/display/framebuf.cpp
        pico-modbus/display/framebuf.h
        OLEDDisplay.cpp
)

pico_add_extra_outputs(part3_ssd1306)

target_link_libraries(part3_ssd1306
        pico_stdlib
        hardware_i2c
        hardware_spi
        hardware_gpio
        hardware_pwm
)
target_include_directories(part3_ssd1306 PRIVATE
        pico-modbus
        pico-modbus/display/fonts
        pico-modbus/display
        pico-modbus/i2c

        ${CMAKE_CURRENT_LIST_DIR}/include
)

pico_enable_stdio_usb(part3_ssd1306 0)
pico_enable_stdio_uart(part3_ssd1306 1)