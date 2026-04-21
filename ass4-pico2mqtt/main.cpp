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

struct LedCommand {
    const char *led_name;
    uint gpio;
    const char *state_cmd;
};

static constexpr LedCommand leds[] = {
    {"D1", 20, nullptr},
    {"D2", 21, nullptr},
    {"D3", 22, nullptr},
};

// Parse "D1;ON"
//
static LedCommand parse_command(const char *payload) {
    for (int i = 0; i < NUM_LEDS; i++) {
        size_t name_len = strlen(leds[i].led_name);
        if (strncmp(payload, leds[i].led_name, name_len) == 0 &&
            payload[name_len] == ';') {
            return {leds[i].led_name,
                leds[i].gpio,
                payload + name_len + 1};
        }
    }
    // no correct LED found or invalid cmd
    return {nullptr, 0, nullptr};
}

void handle_led_command(const char *payload) {
    LedCommand cmd = parse_command(payload);
    if (cmd.led_name == nullptr) {
        printf("Unknown LED in payload: %s\n", payload);
        return;
    }
    if (strcmp(cmd.state_cmd, "ON") == 0) {
        gpio_put(cmd.gpio, 1);
    } else if (strcmp(cmd.state_cmd, "OFF") == 0) {
        gpio_put(cmd.gpio, 0);
    } else if (strcmp(cmd.state_cmd, "TOGG") == 0) {
        gpio_put(cmd.gpio, !gpio_get(cmd.gpio));
    } else {
        printf("Unknown command: %s\n", cmd.state_cmd);
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
    mqtt.connect_tcp();
    mqtt.connect_mqtt();

    if (mqtt.mqtt_is_connected_()) {
        mqtt.publish("Hi from Pico", "suzy/test");
    }

    if (!mqtt.mqtt_is_connected_()) {
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
            mqtt.publish("SW2 pressed on Pico W", "suzy/button");
        }
        sw2_pressed = current_sw2_state;
        sleep_ms(10);
    }
}