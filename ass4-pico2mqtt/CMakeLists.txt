
add_executable(ass4-pico2mqtt
        main.cpp
        MQTTConnection.cpp
        JsonParser.cpp
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
        pico-modbus/mqtt/IPStack.cpp
        pico-modbus/mqtt/Countdown.cpp
        pico-modbus/paho.mqtt.embedded-c/MQTTPacket/src/MQTTConnectClient.c
        pico-modbus/paho.mqtt.embedded-c/MQTTPacket/src/MQTTDeserializePublish.c
        pico-modbus/paho.mqtt.embedded-c/MQTTPacket/src/MQTTFormat.c
        pico-modbus/paho.mqtt.embedded-c/MQTTPacket/src/MQTTPacket.c
        pico-modbus/paho.mqtt.embedded-c/MQTTPacket/src/MQTTSerializePublish.c
        pico-modbus/paho.mqtt.embedded-c/MQTTPacket/src/MQTTSubscribeClient.c
        pico-modbus/paho.mqtt.embedded-c/MQTTPacket/src/MQTTUnsubscribeClient.c
)

pico_add_extra_outputs(ass4-pico2mqtt)

target_link_libraries(ass4-pico2mqtt
        pico_stdlib
        hardware_i2c
        hardware_spi
        hardware_gpio
        hardware_pwm
        pico_cyw43_arch_lwip_threadsafe_background

)
target_include_directories(ass4-pico2mqtt PRIVATE
        pico-modbus
        pico-modbus/display/fonts
        pico-modbus/display
        pico-modbus/i2c
        pico-modbus/mqtt
        pico-modbus/paho.mqtt.embedded-c/MQTTClient/src
        pico-modbus/paho.mqtt.embedded-c/MQTTPacket/src

        ${CMAKE_CURRENT_LIST_DIR}/include
)

pico_enable_stdio_usb(ass4-pico2mqtt 0)
pico_enable_stdio_uart(ass4-pico2mqtt 1)