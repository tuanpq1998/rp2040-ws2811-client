#include <stdio.h>
#include "pico/stdlib.h"
#include "ws2812.hpp"

constexpr uint WS2812_NUM_LEDS = 10;
constexpr uint WS2812_DATA_PIN = 21;

int main() {
    stdio_init_all();
    sleep_ms(2000);

    while (true) {
        printf("USB TEST\n");
        sleep_ms(1000);
    }
}
