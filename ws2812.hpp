#pragma once

#include <array>
#include <cstdint>
#include <cstdio>

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
            // For RGBW we pack into 32 bits; map order depending on mapping
            switch (MAPPING) {
                case WS2812_RGB:
                    // R G B W -> store as R in MSB down to W LSB
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
            // 24-bit packing (we will left-align later when sending)
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
        for (auto &v : ledState) v = 0;
    }

    void show() {
        for (uint i = 0; i < NUM_LEDS; ++i) {
            putPixel(ledState[i]);
        }
        // reset time: at least 50 µs for WS2812
        sleep_us(60);
    }

    uint getNumLeds() const { return NUM_LEDS; }

private:
    PIO  pio;
    uint sm;
    uint offset;
    uint pin;
    bool rgbw;

    std::array<uint32_t, NUM_LEDS> ledState{};

    // Static storage to ensure we load the program only once per PIO instance
    static int32_t get_or_load_offset(PIO pio_inst) {
        // index by pio pointer (pio0/pio1). Use small static table.
        // pio0 address will be (uintptr_t)pio0 etc. We'll map to 0/1.
        static int32_t offsets[2] = { -1, -1 };
        int idx = (pio_inst == pio0) ? 0 : 1;
        if (offsets[idx] < 0) {
            offsets[idx] = pio_add_program(pio_inst, &ws2812_program);
        }
        return offsets[idx];
    }

    void initPio() {
        // Acquire offset (load program once per PIO)
        offset = get_or_load_offset(pio);

        // Claim an unused state machine for this PIO
        sm = pio_claim_unused_sm(pio, true);
        if (sm == (uint)-1) {
            printf("WS2812: ERROR - failed to claim PIO SM on pio %p\n", (void*)pio);
            return;
        }

        // initialize the program with the correct arguments
        // ws2812_program_init signature in SDK: (PIO pio, uint sm, uint offset, uint pin, float freq, bool is_rgbw)
        ws2812_program_init(pio, sm, offset, pin, 800000.0f, rgbw);

        printf("WS2812: init pio=%p sm=%u offset=%d pin=%u rgbw=%d\n", (void*)pio, sm, offset, pin, rgbw ? 1 : 0);
    }

    inline void putPixel(uint32_t value) {
        // If RGB (24-bit) we left-align to 32-bit FIFO expected format in the example:
        // SDK example uses (value << 8) for 24-bit data so top 24 bits are used.
        uint32_t send = value;
        if (!rgbw) {
            send = (value << 8u); // left-align 24-bit into 32-bit word
        }
        // For rgbw (32-bit) send as-is.
        pio_sm_put_blocking(pio, sm, send);
    }
};
