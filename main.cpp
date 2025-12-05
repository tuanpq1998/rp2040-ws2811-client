#include <stdio.h>
#include <stdlib.h>
#include <cmath>

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
#define WS2812_PIN 17 // for leopard fan
#endif

#define WS2812_PIN_2 18 // for aio fan

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

float GAMMAS = [1.1f, 1.0f];

float R_SCALES = [1.00f, 0.9];
float G_SCALES = [1.5f, 1.375f];
float B_SCALES = [1.2f, 1.2f];

uint8_t gamma_table[2][256];

void build_gamma_table(int index) {
    for (int i = 0; i < 256; i++) {
        float x = powf(i / 255.0f, GAMMAS[index]);
        gamma_table[index][i] = (uint8_t)(x * 255.0f + 0.5f);
    }
}

inline uint8_t clamp8(int v) {
    return (v < 0) ? 0 : (v > 255 ? 255 : v);
}

inline void correct_color_fast(uint8_t r, uint8_t g, uint8_t b,
                             uint8_t &r2, uint8_t &g2, uint8_t &b2, uint index)
{
    // Lookup gamma
    int rf = gamma_table[index][r];
    int gf = gamma_table[index][g];
    int bf = gamma_table[index][b];

    // Per-channel scale
    rf = (int)(rf * R_SCALES[index]);
    gf = (int)(gf * G_SCALES[index]);
    bf = (int)(bf * B_SCALES[index]);

    // Clamp
    r2 = clamp8(rf);
    g2 = clamp8(gf);
    b2 = clamp8(bf);
}

int main() {

    stdio_init_all();
    for (uint i = 0; i < GAMMAS.length; i++) {
        build_gamma_table(i);
    }

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);

    int sm0 = pio_claim_unused_sm(pio, true);
    int sm1 = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm0, offset, WS2812_PIN, 800000, IS_RGBW);
    ws2812_program_init(pio, sm1, offset, WS2812_PIN_2, 800000, IS_RGBW);

    auto ws2811 = WS2811Client<NUM_LEDS_TO_EMULATE, GRB>();

    while (1) {

        const auto leds = ws2811.getLEDsAtomic();
        printf("input size: %d", leds.size()  );
        for (auto it = leds.begin(); it != leds.end(); it++) {
            printf("[%7u] LED %u: ", time_us_32() / 1000, std::distance(leds.begin(), it));
            print_led_state(*it);

            for (uint i = 0; i < NUM_PIXELS; ++i) {
                uint index = i % NUM_LEDS_TO_EMULATE;

                uint8_t r2, g2, b2;
                correct_color_fast(leds[index].colors.r, leds[index].colors.g, leds[index].colors.b, r2, g2, b2, 0);
                
                uint8_t r3, g3, b3;
                correct_color_fast(leds[index].colors.r, leds[index].colors.g, leds[index].colors.b, r3, g3, b3, 1);
                
                put_pixel(pio, sm0, urgb_u32(r2, g2, b2));
                put_pixel(pio, sm1, urgb_u32(r3, g3, b3));
            }
        }
 
        sleep_ms(30);

    }
}

