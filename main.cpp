#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#include "ws2811.hpp"

#define IS_RGBW false
#define NUM_PIXELS 30

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
#define WS2812_PIN 17
#endif

#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            ((uint32_t) (w) << 24) |
            (uint32_t) (b);
}

static void print_led_state(const RGBLED led) {
    printf("%3u\t%3u\t%3u\n", led.colors.r, led.colors.g, led.colors.b);
}


int main() {

    stdio_init_all();

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    auto ws2811 = WS2811Client<NUM_LEDS_TO_EMULATE, GRB>();

    while (1) {

        const auto leds = ws2811.getLEDsAtomic();
        printf("input size: %d", leds.size()  );
        for (auto it = leds.begin(); it != leds.end(); it++) {
            printf("[%7u] LED %u: ", time_us_32() / 1000, std::distance(leds.begin(), it));
            print_led_state(*it);

            for (uint i = 0; i < NUM_PIXELS; ++i) {
                uint index = i % NUM_LEDS_TO_EMULATE;
                
                // modify color: G + 8
                uint16_t g_tmp = leds[index].colors.g + 8;
                if (g_tmp > 255) g_tmp = 255;
                else if (g_tmp == 8) g_tmp = 0;

                put_pixel(pio, sm, urgb_u32(leds[index].colors.r, static_cast<uint8_t>(g_tmp), leds[index].colors.b));
            }
        }
 
        sleep_ms(30);

    }
}

