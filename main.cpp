#include <stdio.h>
#include "pico/stdlib.h"
#include "ws2812.hpp"

constexpr uint WS2812_NUM_LEDS = 30;
constexpr uint WS2812_DATA_PIN = 21;

int main() {
    stdio_init_all();
    sleep_ms(3000);  // Give USB enough time to enumerate

    printf("USB OK. Initializing strip...\n");

    WS2812Client<WS2812_NUM_LEDS, WS2812_GRB> strip(pio0, WS2812_DATA_PIN, false);

    printf("Strip initialized\n");

    // quick sanity: set first LED green for 500ms then clear
    strip.clear();
    strip.setPixel(0, 0, 255, 0);
    strip.show();
    sleep_ms(500);
    strip.clear();
    strip.show();

    while (true) {
        strip.clear();
        strip.setPixel(0, 255, 0, 0);  // RED
        strip.setPixel(1, 0, 255, 0);  // GREEN
        strip.setPixel(2, 0, 0, 255);  // BLUE
        strip.show();
        sleep_ms(10);
        printf("Loop\n");
    }

}
