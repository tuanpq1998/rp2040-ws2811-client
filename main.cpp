#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 150

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
#define WS2812_PIN 21
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

int main() {

    stdio_init_all();

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    while (1) {

        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, urgb_u32(0xff, 0, 0));
            // pio_sm_put_blocking(pio, sm,  make_color(255, 0, 0));
        }

        sleep_ms(5000);
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, urgb_u32(0, 0xff, 0));
        }
        
        sleep_ms(5000);
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, urgb_u32(0, 0, 0xff));
        }

        sleep_ms(5000);
    }
}

