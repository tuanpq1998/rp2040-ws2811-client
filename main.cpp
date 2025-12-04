#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define LED_PIN 21
#define LED_COUNT 30

int main() {
    stdio_init_all();
    sleep_ms(2000);

    printf("Init PIO...\n");

    PIO pio = pio0;
    uint sm = 0;

    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, LED_PIN, 800000);

    printf("PIO ready\n");

    while (1) {
        uint32_t color = 0x00FF0000; // red GRB
        pio_sm_put_blocking(pio, sm, color << 8);
        sleep_ms(500);

        pio_sm_put_blocking(pio, sm, 0x00000000);
        sleep_ms(500);
    }
}
