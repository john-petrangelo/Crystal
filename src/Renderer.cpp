#include <functional>

#include "Renderer.h"

//void Renderer::loop(int numPixels, std::function<void(int, Color)> setPixel) {
//  // If there's no model then there's nothing to do
//  if (model == NULL) {
//    return;
//  }
//
//  // Update the current state of the model to match the current time.
//  unsigned long const nowMS = millis();
//  float const timeStamp = (nowMS - startTimeMS) / 1000.0;
//  model->update(timeStamp);
//
//  // Now apply the model to each LED position.
//  for (int i = 0; i < numPixels; i++) {
//    // Get the color from the model
//    float const pos = ((float)i) / (numPixels - 1);
//    Color const color = model->render(pos);
//
//    // Apply gamma correction
//    uint16_t const red = Colors::getRed(color);
//    uint16_t const green = Colors::getGreen(color);
//    uint16_t const blue = Colors::getBlue(color);
//    Color const correctedColor = Colors::makeColor(
//      (uint8_t)(red*red/255), (uint8_t)(green*green/255), (uint8_t)(blue*blue/255));
//
//    // Set the pixel on the light strip
//    setPixel(i, correctedColor);
//  }
//}

// TODO Doc
Esp8266_NeoPixelBus_Renderer::Esp8266_NeoPixelBus_Renderer(int pixelsPin, int pixelsCount) :
        Renderer(),
        _pixelsPin(pixelsPin),
        _pixelsCount(pixelsCount),
        _strip(pixelsCount, pixelsPin) {
    _strip.Begin();
    _strip.SetBrightness(255);
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
    for (auto i = 0; i < numPixels; ++i) {
        // TODO Do I need to cast to force pos to be a float?
        float pos = i / (numPixels - 1);
        auto color = _model->render(pos);
        RgbColor pixelColor(Colors::getRed(color), Colors::getGreen(color), Colors::getBlue(color));
        _strip.SetPixelColor(i, pixelColor);
    }

    // Write the colors to the LED strip
    _strip.Show()
}
