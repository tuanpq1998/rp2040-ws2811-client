#include <stdio.h>
#include "pico/stdlib.h"
#include "ws2812.hpp"

constexpr uint WS2812_NUM_LEDS = 10;
constexpr uint WS2812_DATA_PIN = 21;

int main() {
    stdio_init_all();
    sleep_ms(2000);  // give USB time to come up
    printf("Starting WS2812 test\n");

    WS2812Client<WS2812_NUM_LEDS, WS2812_GRB> strip(pio0, WS2812_DATA_PIN, false);

    // quick sanity: set first LED green for 500ms then clear
    strip.clear();
    strip.setPixel(0, 0, 255, 0);
    strip.show();
    sleep_ms(500);
    strip.clear();
    strip.show();

    while (true) {
        for (uint i = 0; i < strip.getNumLeds(); ++i) {
            strip.clear();
            strip.setPixel(i, 255, 0, 0);
            strip.show();
            sleep_ms(100);
        }
    }
}
