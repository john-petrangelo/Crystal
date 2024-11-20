#include <iomanip>
#include <cmath>

#include <pico/time.h>

#include "lumos-arduino/Colors.h"

#include "ws2812.pio.h"
#include "WS2812Strip.h"

// TODO Doc
WS2812Strip::WS2812Strip(uint const pin, uint const numPixels) : _pin(pin), _pixels(numPixels, BLACK)
{
    _offset = pio_add_program(_pio, &ws2812_program);
    ws2812_program_init(_pio, _sm, _offset, _pin, 800000, false);

    setGamma(DEFAULT_GAMMA_VALUE);
}

WS2812Strip::~WS2812Strip() {
    // Before going away, turn off all the lights on the physical strip
    std::fill(_pixels.begin(), _pixels.end(), BLACK);
    show();

    // Disable the state machine
    pio_sm_set_enabled(_pio, _sm, false);

    // Remove the program form the PIO
    pio_remove_program(_pio, &ws2812_program, _offset);

    // Reset the state machine
    pio_sm_restart(_pio, _sm);
    pio_sm_clear_fifos(_pio, _sm);
}

void WS2812Strip::setGamma(float const newGammaValue) {
    _gammaValue = newGammaValue;

    for (int i = 0; i < 256; ++i) {
        // Compute gamma-corrected value
        float const normalized = i / 255.0f;
        float const corrected = powf(normalized, _gammaValue) * 255.0f;

        // Round to the nearest integer and store in the table
        _gamma_table[i] = std::round(corrected);
    }
}

Color WS2812Strip::gammaCorrect(Color const &pixel) const {
    return Colors::makeColor(
        _gamma_table[Colors::getRed(pixel)],
        _gamma_table[Colors::getGreen(pixel)],
        _gamma_table[Colors::getBlue(pixel)]);
}

Color WS2812Strip::toGRB(Color const &pixel) {
    return Colors::getGreen(pixel) << 16 |
           Colors::getRed(pixel) << 8 |
           Colors::getBlue(pixel);
}

void WS2812Strip::show() const {
    for (Color const& pixel : _pixels) {
        // Gamma corrects the color
        auto const gamma_pixel = gammaCorrect(pixel);

        // Re-arrange to GRB order
        auto const grbPixel = toGRB(gamma_pixel);

        pio_sm_put_blocking(_pio, _sm, grbPixel << 8u);
    }
}
