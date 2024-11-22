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

/**
 * Adjusts the brightness of a single pixel by scaling its red, green, and blue components.
 *
 * The brightness adjustment is performed by multiplying each color channel by the
 * brightness scale factor (`_brightness + 1`) and then shifting the result by 8 bits
 * (equivalent to dividing by 256). This ensures the adjusted brightness is proportionally
 * applied to each channel while keeping the output within the 8-bit color range.
 *
 * @param pixel The original color of the pixel to be adjusted.
 * @return A new Color object with brightness-adjusted red, green, and blue components.
 *
 * Note: `_brightness` is assumed to be an 8-bit value (0–255), where 0 represents no scaling
 * and 255 represents full brightness scaling.
 */
Color WS2812Strip::adjustBrightness(Color const &pixel) const {
    auto const scale = _brightness + 1;

    return Colors::makeColor(
        Colors::getRed(pixel) * scale >> 8,
        Colors::getGreen(pixel) * scale >> 8,
        Colors::getBlue(pixel) * scale >> 8);
}

Color WS2812Strip::toGRB(Color const &pixel) {
    return Colors::getGreen(pixel) << 16 |
           Colors::getRed(pixel) << 8 |
           Colors::getBlue(pixel);
}

void WS2812Strip::show() const {
    for (Color pixel : _pixels) {
        // Gamma-correct the color
        pixel = gammaCorrect(pixel);

        // Adjust brightness
        pixel = adjustBrightness(pixel);

        // Re-arrange to GRB order
        pixel = toGRB(pixel);

        pio_sm_put_blocking(_pio, _sm, pixel << 8u);
    }
}
