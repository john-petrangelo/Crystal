#pragma once

#include <vector>

#include "hardware/pio.h"

#include <lumos-arduino/Colors.h>

/**
 * WS2812Strip controls an LED strip using the WS2812 protocol.
 * This class initializes the PIO, maintains pixel state, and handles
 * cleanup when the strip is no longer needed.
 */
class WS2812Strip {
public:
    explicit WS2812Strip(uint pin, uint numPixels);
    ~WS2812Strip();

    void setPixel(int const i, Color const c) { _pixels[i] = c; }
    int numPixels() const { return _pixels.size(); }

    // Write the buffered pixels to the physical light strip
    void show() const;

    // uint8_t brightness() const;
    // void setBrightness(uint8_t brightness);

private:
    uint const _pin;
    PIO const _pio;
    uint const _sm; // state machine
    uint _offset;

    std::vector<Color> _pixels;
};
