#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define LED_PIN 21      // the WS2812 data pin
#define IS_RGBW false  // you are using RGB, not RGBW

// Helper: GRB format
static inline uint32_t grb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

int main() {
    stdio_init_all();

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, LED_PIN, IS_RGBW);

    while (1) {
        // Yellow = R 255, G 255, B 0 → GRB = FF FF 00
        uint32_t yellow = grb(255, 255, 0);

        pio_sm_put_blocking(pio, sm, yellow); // turn LED yellow
        sleep_ms(500);

        pio_sm_put_blocking(pio, sm, 0x00000000); // turn LED off
        sleep_ms(500);
    }
}
