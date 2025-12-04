#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define LED_PIN 21      // the WS2812 data pin
#define IS_RGBW false  // you are using RGB, not RGBW
#define LED_COUNT 30

static inline uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

int main() {
    stdio_init_all();

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, LED_PIN, IS_RGBW);

    while (1) {

        for (int i = 0; i < LED_COUNT; i++) {
            pio_sm_put_blocking(pio, sm, rgb(41, 24, 17));
        }

        sleep_ms(500);
    }
}
