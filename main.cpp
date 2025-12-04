#include <stdio.h>
#include "pico/stdlib.h"
#include "ws2812.hpp"

constexpr uint WS2812_NUM_LEDS = 10;
constexpr uint WS2812_DATA_PIN = 21;

int main() {
    stdio_init_all();
    sleep_ms(3000);  // Give USB enough time to enumerate

    printf("USB OK. Initializing strip...\n");

    WS2812Client<WS2812_NUM_LEDS, WS2812_GRB> strip(pio0, WS2812_DATA_PIN, false);

    printf("Strip initialized\n");

}
