#include "Renderer.h"

// TODO Doc
Esp8266_NeoPixelBus_Renderer::Esp8266_NeoPixelBus_Renderer(int pixelsPin, int pixelsCount) :
        Renderer(),
        _pixelsPin(pixelsPin),
        _pixelsCount(pixelsCount),
        _strip(pixelsCount, pixelsPin) {
    _strip.Begin();
    _strip.Show();  // Initialize all pixels to 'off'
}

// TODO Doc
void Esp8266_NeoPixelBus_Renderer::render() {
    // If there is no model, we're done.
    if (_model == nullptr) {
        return;
    }

    // Update the current state of the model to match the current time
    unsigned long absoluteNow_ms = millis();
    unsigned long relativeNow_ms = absoluteNow_ms - _startTime_ms;
    auto timeStamp = float(float(relativeNow_ms) / 1000.0);
    _model->update(timeStamp);

    // Set the color of each pixel
    for (auto i = 0; i < _pixelsCount; ++i) {
        float pos = (float)i / float(_pixelsCount - 1);
        auto color = _model->render(pos);
        RgbColor pixelColor(Colors::getRed(color), Colors::getGreen(color), Colors::getBlue(color));
        _strip.SetPixelColor(i, pixelColor);
    }

    // Write the colors to the LED strip
    _strip.Show();
}
