#include "pico/stdlib.h"
#include "tusb.h"

int main() {
    stdio_init_all();
    sleep_ms(2000);  // allow USB to enumerate

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    while (true) {
        tud_task();  // allow USB CDC to run

        printf("USB WORKS\n");

        gpio_put(25, 1);
        sleep_ms(500);
        gpio_put(25, 0);
        sleep_ms(500);
    }
}
