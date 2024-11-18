#include <iomanip>

#include <pico/time.h>

#include "lumos-arduino/Colors.h"
// #include "lumos-arduino/Logger.h"

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

uint32_t WS2812Strip::toGRB(Color const &pixel) {
    return Colors::getGreen(pixel) << 16 |
           Colors::getRed(pixel) << 8 |
           Colors::getBlue(pixel);
}

void WS2812Strip::show() const {
    // int i = 0;
    // for (Color const& pixel : _pixels) {
    //     auto const grbPixel = toGRB(pixel);
    //     logger << "show pixel[" << i << "]=(0x";
    //     logger << std::hex << std::setfill('0')
    //         << std::setw(2) << static_cast<int>(Colors::getRed(pixel))
    //         << std::setw(2) << static_cast<int>(Colors::getGreen(pixel))
    //         << std::setw(2) << static_cast<int>(Colors::getBlue(pixel)) << ") grbPixel=(0x"
    //         << std::setw(2) << static_cast<int>(Colors::getGreen(grbPixel))
    //         << std::setw(2) << static_cast<int>(Colors::getRed(grbPixel))
    //         << std::setw(2) << static_cast<int>(Colors::getBlue(grbPixel)) << ")"
    //         << std::endl;
    //     i++;
    // }

    for (Color const& pixel : _pixels) {
        // Pixels must be written in GRB order
        auto const grbPixel = toGRB(pixel);

        pio_sm_put_blocking(_pio, _sm, grbPixel << 8u);
    }
}
