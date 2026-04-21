//
// Created by 张悦 on 14.4.2026.
//

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <cstdio>
#include "private.h"
#include "MQTTConnection.h"
#include "JsonParser.h"

//helper functions
//https://stackoverflow.com/questions/58860733/writing-a-json-parser-for-c

#define SW2_PIN 7
#define TOPIC_LED "suzy/LED"
#define NUM_LEDS 3

struct Leds {
    const char *led_name;
    uint gpio;
};

static constexpr Leds leds[] = {
    {"D1", 20},
    {"D2", 21},
    {"D3", 22},
};

// helper function to parse LED name
int find_gpio_by_led_name(const std::string& target_name) {
    for (int i = 0; i < NUM_LEDS; i++) {
        if (target_name ==leds[i].led_name) {
            return leds[i].gpio;
        }
    }
    return -1; // not found
}

void handle_led_command(const char *payload) {
    JsonParser::Reader reader;
    JsonParser::Value root;
    std::string raw_data(payload);

    if (!JsonParser::Reader::parse(raw_data, root)) {
        printf("Error: Payload is not valid JSON!\n");
        return;
    }

    if (!root.is_member("name") || !root.is_member("state")) {
        printf("Error: JSON missing 'name' or 'state' keys!\n");
        return;
    }

    std::string led_name = root["name"].as_string();
    std::string state_cmd = root["state"].as_string();

    int gpio_pin = find_gpio_by_led_name(led_name);
    if (gpio_pin == -1) {
        printf("Unknown LED name: %s\n", led_name.c_str());
        return;
    }

    if (state_cmd == "ON") {
        gpio_put(gpio_pin, 1);
    } else if (state_cmd == "OFF") {
        gpio_put(gpio_pin, 0);
    } else if (state_cmd == "TOGG") {
        gpio_put(gpio_pin, !gpio_get(gpio_pin));
    } else {
        printf("Unknown state command: %s\n", state_cmd.c_str());
    }
}

int main() {
    stdio_init_all();
    gpio_init(SW2_PIN);
    gpio_set_dir(SW2_PIN, GPIO_IN);
    gpio_pull_up(SW2_PIN);

    for (int i = 0; i < NUM_LEDS; i++) {
        gpio_init(leds[i].gpio);
        gpio_set_dir(leds[i].gpio, GPIO_OUT);
        gpio_put(leds[i].gpio, 0);
    }

    MQTTConnection mqtt(WIFI_SSID, WIFI_PASS);
    mqtt.connect();

    if (mqtt.mqtt_is_connected()) {
        mqtt.publish("suzy/test","Hi from Pico");
    }

    if (!mqtt.mqtt_is_connected()) {
        printf("MQTT connection failed\n");
        return 1;
    }

    MQTTConnection::set_command_handler(handle_led_command);
    mqtt.subscribe(TOPIC_LED);
    bool sw2_pressed = true;

    while (true) {
        mqtt.client_yield();
        bool current_sw2_state = gpio_get(SW2_PIN);
        if (sw2_pressed && !current_sw2_state) {
            mqtt.publish("suzy/button","SW2 pressed on Pico W");
        }
        sw2_pressed = current_sw2_state;
        sleep_ms(10);
    }
}