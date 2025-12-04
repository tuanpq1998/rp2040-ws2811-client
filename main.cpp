// main.cpp — minimal USB + blink test (C++)
#include <cstdio>
#include "pico/stdlib.h"
#include "tusb.h"


int main() {
    stdio_init_all();
    sleep_ms(2000);     // allow time for USB CDC to connect

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    while (true) {
        tud_task();     // IMPORTANT! allow USB CDC to process

        printf("USB WORKS\n");

        gpio_put(25, 1);
        sleep_ms(500);
        gpio_put(25, 0);
        sleep_ms(500);
    }
}