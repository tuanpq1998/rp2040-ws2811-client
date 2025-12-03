#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "ws2811.hpp"
#include "ws2812.hpp"

constexpr uint WS2812_NUM_LEDS = 30;
constexpr uint WS2812_DATA_PIN = 21;

void core1_entry() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(200);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(200);
    }
}

static void print_led_state(const RGBLED led) {
    printf("%3u\t%3u\t%3u\n", led.colors.r, led.colors.g, led.colors.b);
}

int main()
{
    stdio_init_all();
    puts("Hello, world!");

    // multicore_launch_core1(core1_entry);

    // auto ws2811 = WS2811Client<NUM_LEDS_TO_EMULATE, GRB>();

    WS2812Client<WS2812_NUM_LEDS, WS2812_GRB> strip(pio0, WS2812_DATA_PIN, false);

    while (true) {
        // const auto leds = ws2811.getLEDsAtomic();
        // for (auto it = leds.begin(); it != leds.end(); it++) {
        //     printf("[%7u] LED %u: ", time_us_32() / 1000, std::distance(leds.begin(), it));
        //     print_led_state(*it);

            // for (uint i = 0; i < strip.getNumLeds(); ++i) {
            //     // strip.clear();
            //     uint index = i > NUM_LEDS_TO_EMULATE ? (NUM_LEDS_TO_EMULATE - 1)  : i;
            //     strip.setPixel(i, leds[i].colors.r, leds[i].colors.g, leds[i].colors.b);
            // }
        // }

        for (uint i = 0; i < strip.getNumLeds(); ++i) {
            strip.clear();
            strip.setPixel(i, 255, 0, 0);  // red
            strip.show();
            sleep_ms(100);
        }

        // strip.show();
        // sleep_ms(500);
        // tight_loop_contents();
    }
}
