#include <functional>

#include "lumos-arduino/lumos-arduino/Colors.h"

#include "Renderer.h"

void Renderer::loop(int numPixels, std::function<void(int, Color)> setPixel) {
  // If there's no model then there's nothing to do
  if (model == NULL) {
    return;
  }

  // Update the current state of the model to match the current time.
  unsigned long const nowMS = millis();
  float const timeStamp = (nowMS - startTimeMS) / 1000.0;
  model->update(timeStamp);

  // Now apply the model to each LED position.
  for (int i = 0; i < numPixels; i++) {
    // Get the color from the model
    float const pos = ((float)i) / (numPixels - 1);
    Color const color = model->render(pos);

    // Apply gamma correction
    uint16_t const red = Colors::getRed(color);
    uint16_t const green = Colors::getGreen(color);
    uint16_t const blue = Colors::getBlue(color);
    Color const correctedColor = Colors::makeColor(
      (uint8_t)(red*red/255), (uint8_t)(green*green/255), (uint8_t)(blue*blue/255));

    // Set the pixel on the light strip
    setPixel(i, correctedColor);
  }
}

// Give the ModelRunner a new model to run. The old model will be deleted.
void Renderer::setModel(std::shared_ptr<Model> newModel) {
  model = newModel;
  // Reset time
  // startTimeMS = millis();
}

// TODO Doc
void Renderer2::render() {
    ; // TODO
}

// TODO Doc
void Renderer2::reset() {
    ; // TODO
}
