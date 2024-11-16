#include <iomanip>

#include "lumos-arduino/Colors.h"
#include <lumos-arduino/Logger.h>

#include "ws2812.pio.h"

#include "WS2812Strip.h"

// TODO Doc
WS2812Strip::WS2812Strip(uint const pin, uint const numPixels) :
    _pin(pin), _pio(pio0), _sm(0), _pixels(numPixels, BLACK)
{
    _offset = pio_add_program(_pio, &ws2812_program);
    ws2812_program_init(_pio, _sm, _offset, _pin, 800000, false);

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

#define R_SHIFT 16
#define G_SHIFT  8
#define B_SHIFT  0

#define R_MASK 0x00FF0000
#define G_MASK 0x0000FF00
#define B_MASK 0x000000FF

uint8_t getRed(Color color) {
    return (color & R_MASK) >> R_SHIFT;
}

uint8_t getGreen(Color color) {
    return (color & G_MASK) >> G_SHIFT;
}

uint8_t getBlue(Color color) {
    return (color & B_MASK) >> B_SHIFT;
}

void WS2812Strip::show() const {
    for (Color const& pixel : _pixels) {
        // Pixels must be written in GRB order
        uint32_t grbPixel =
            getGreen(pixel) << 16 |
            getRed(pixel) << 8 |
            getBlue(pixel);

        pio_sm_put_blocking(_pio, _sm, grbPixel << 8u);
    }
}


// class Colors
// static uint8_t getRed(Color color);
// static uint8_t getGreen(Color color);
// static uint8_t getBlue(Color color);
