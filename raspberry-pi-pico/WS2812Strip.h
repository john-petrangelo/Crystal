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

    float gamma() const { return _gammaValue; }
    void setGamma(float const newGammaValue);

    // uint8_t brightness() const { return brightness; }
    // void setBrightness(uint8_t newBrightness);

private:
    Color gammaCorrect(Color const &pixel) const;
    static Color toGRB(Color const &pixel);

    uint const _pin;
    PIO const _pio = pio0;
    uint const _sm = 0; // state machine
    uint _offset;

    float const DEFAULT_GAMMA_VALUE = 2.8;
    float _gammaValue = -1.0;
    uint8_t _gamma_table[256] = {};

    std::vector<Color> _pixels;
};
