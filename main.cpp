#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "ws2811.hpp"

#include "hardware/pio.h"
#include "ws2812.pio.h"   // from pico-examples
#include "ws2811.hpp"     // for RGBLED

static PIO ws2812_pio = pio1;
static uint ws2812_sm;
static const uint WS2812_PIN = 16;   // choose your real output pin

void ws2812_tx_init() {
    uint offset = pio_add_program(ws2812_pio, &ws2812_program);
    ws2812_sm = pio_claim_unused_sm(ws2812_pio, true);

    ws2812_program_init(ws2812_pio, ws2812_sm, offset, WS2812_PIN, 800000, false);
    // last arg is is_rgbw; false for normal RGB. [web:14][web:126]
}

void send_out_leds(const RGBLED *out_leds, uint num_leds) {
    for (uint i = 0; i < num_leds; ++i) {
        const RGBLED &led = out_leds[i];

        // Convert to 24-bit GRB as WS2812 expects on the wire
        uint32_t grb =
            ((uint32_t)led.colors.g << 16) |
            ((uint32_t)led.colors.r <<  8) |
            ((uint32_t)led.colors.b <<  0);

        // Block until there is space in TX FIFO, then push one pixel
        pio_sm_put_blocking(ws2812_pio, ws2812_sm, grb);
    }
}


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
    ws2812_tx_init();

    RGBLED out_leds[NUM_LEDS];

    while (true) {
    // 1) capture one frame
    const auto leds_in = ws2811.getLEDsAtomic();

    // 2) copy + adjust G
    for (uint i = 0; i < NUM_LEDS; ++i) {
        RGBLED led = leds_in[i];
        uint16_t g = led.colors.g + 8;
        if (g > 255) g = 255;
        led.colors.g = (uint8_t)g;
        out_leds[i] = led;
    }

    // 3) transmit adjusted frame
    send_out_leds(out_leds, NUM_LEDS);
}

}
