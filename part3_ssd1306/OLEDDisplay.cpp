//
// Created by Yue on 1.3.2026.
//

#include "OLEDDisplay.h"
#include "Untitled.h"
#include "mono_vlsb.h"


// I2C1, SDA=GPIO14, SCL=GPIO15, 400kHz
// SSD1306 default I2C address: 0x3C
OLEDDisplay::OLEDDisplay() :
    i2c_bus(std::make_shared<PicoI2CBus>(1, 14, 15, 400000)),
    i2c_device(std::make_shared<PicoI2CDevice>(i2c_bus, 0x3C)),
    display(i2c_device){clear();}


void OLEDDisplay::clear() {
    display.fill(0);
    display.show();
}

void OLEDDisplay::show_time_image(uint32_t seconds) {
    display.fill(0);
    display.text("Time: " + std::to_string(seconds) + "s", 0, 0);
    //binary_data from image editor
    mono_vlsb img(binary_data, 39, 39);
    display.blit(img, 0, 16);
    display.show();
}

