#include "Renderer.h"

// TODO Doc
void Renderer::render() {
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
    for (auto i = 0; i < pixelsCount(); ++i) {
        float pos = (float)i / float(pixelsCount() - 1);
        auto color = _model->render(pos);
        setPixel(i, color);
    }

    // Write the colors to the LED strip
    show();
}

// TODO Doc
Esp8266_NeoPixelBus_Renderer::Esp8266_NeoPixelBus_Renderer(int pixelsCount) :
        Renderer(),
        _strip(pixelsCount) {
    _strip.Begin();
    _strip.Show();  // Initialize all pixels to 'off'
}

void Esp8266_NeoPixelBus_Renderer::setPixel(int i, Color c) {
    RgbColor rgbColor(Colors::getRed(c), Colors::getGreen(c), Colors::getBlue(c));
    _strip.SetPixelColor(i, rgbColor);
}
