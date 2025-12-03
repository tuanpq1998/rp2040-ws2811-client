
#pragma once

#include <array>
#include <cstdint>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"   // the PIO program from the SDK

enum WS2812ColorMapping {
    WS2812_RGB,
    WS2812_GRB
};

template <uint NUM_LEDS, WS2812ColorMapping MAPPING>
class WS2812Client {
public:
    WS2812Client(PIO pioInstance, uint dataPin, bool isRgbw = false)
        : pio(pioInstance),
          pin(dataPin),
          rgbw(isRgbw)
    {
        initPio();
        clear();
        show();
    }

    void setPixel(uint index, uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) {
        if (index >= NUM_LEDS) return;

        uint32_t value = 0;
        if (rgbw) {
            // simple RGBW packing: G,R,B,W or R,G,B,W depending on mapping
            switch (MAPPING) {
                case WS2812_RGB:
                    value =
                        (uint32_t(r) << 24) |
                        (uint32_t(g) << 16) |
                        (uint32_t(b) <<  8) |
                        (uint32_t(w) <<  0);
                    break;
                case WS2812_GRB:
                    value =
                        (uint32_t(g) << 24) |
                        (uint32_t(r) << 16) |
                        (uint32_t(b) <<  8) |
                        (uint32_t(w) <<  0);
                    break;
            }
        } else {
            // simple RGB / GRB packing
            switch (MAPPING) {
                case WS2812_RGB:
                    value =
                        (uint32_t(r) << 16) |
                        (uint32_t(g) <<  8) |
                        (uint32_t(b) <<  0);
                    break;
                case WS2812_GRB:
                    value =
                        (uint32_t(g) << 16) |
                        (uint32_t(r) <<  8) |
                        (uint32_t(b) <<  0);
                    break;
            }
        }

        ledState[index] = value;
    }

    void clear() {
        for (auto &v : ledState) {
            v = 0;
        }
    }

    void show() {
        for (uint i = 0; i < NUM_LEDS; ++i) {
            putPixel(ledState[i]);
        }
        // reset time: at least 50 µs for WS2812
        sleep_us(60);
    }

    uint getNumLeds() const {
        return NUM_LEDS;
    }

private:
    PIO  pio;
    uint sm;
    uint offset;
    uint pin;
    bool rgbw;

    std::array<uint32_t, NUM_LEDS> ledState{};

    void initPio() {
        static int loaded_offset = -1;

        if (loaded_offset < 0) {
            loaded_offset = pio_add_program(pio, &ws2812_program);
        }

        offset = loaded_offset;
        sm = pio_claim_unused_sm(pio, true);

        ws2812_program_init(pio, sm, offset, pin, 800000, rgbw);
    }

    inline void putPixel(uint32_t value) {
    if (!rgbw)
        value <<= 8;  // WS2812 (24-bit)
    pio_sm_put_blocking(pio, sm, value);
}
};
