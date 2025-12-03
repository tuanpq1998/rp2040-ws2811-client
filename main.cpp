#include <cstdio>          // <-- required for printf
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    sleep_ms(1500);

    while (true) {
        printf("USB WORKS\n");

        gpio_set_dir(25, GPIO_OUT);
        gpio_put(25, 1);
        sleep_ms(500);
        gpio_put(25, 0);
        sleep_ms(500);
    }
}
