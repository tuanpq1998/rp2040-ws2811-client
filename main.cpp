#include "pico/stdlib.h"
int main() {
    stdio_init_all();
    sleep_ms(1500);
    for (int i=0;; ++i) {
        printf("USB WORKS %d\n", i);
        gpio_set_dir(25, GPIO_OUT);
        gpio_put(25, i & 1);
        sleep_ms(1000);
    }
}
