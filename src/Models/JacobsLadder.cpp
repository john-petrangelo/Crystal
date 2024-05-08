#include "../lumos-arduino/Logger.h"

#include "Gradient.h"
#include "JacobsLadder.h"
#include "Map.h"
#include "Shift.h"
#include "Triangle.h"

Color const JacobsLadder::defaultColor =  WHITE;

JacobsLadder::JacobsLadder(float size, float frequency, Color color)
    : Model("JacobsLadder"), size(size), frequency(frequency), color(color) {
  init();
  lastResetTime = 0.0f;
}

void JacobsLadder::init() {
  ModelPtr glow = Triangle::make(0.0, 1.0, color);

  // If the size is too big or too small, then we have a warp core breach, light the whole length
  if (size < 0.0 || size >= 1.0) {
    size = 1.0;
  }

  // Can't handle exactly zero frequency, so pretend by setting it really, really low
  if (frequency <= 0.0) {
    frequency = 0.000001;
    return;
  }

  // Map the glow into the middle with the specified size
  mapModel = Map::make(0.0f, size, 0.0, 1.0, glow);

  // Calculate how long to display and shift the lights
  duration = 1 / frequency;

  // Set the initial mode and model
  model = Shift::Out::make(duration, mapModel);
}

void JacobsLadder::set(float newFrequency, float newSize, Color newColor) {
  frequency = newFrequency;
  size = newSize;
  color = newColor;

  init();
}

void JacobsLadder::update(float timeStamp) {
  if (timeStamp - lastResetTime >= fabs(duration)) {
    lastResetTime = timeStamp;
    model->reset();
  }

  model->update(timeStamp);
}

Color JacobsLadder::render(float pos) {
  Color c = model->render(pos);
  uint8_t red = Colors::fade(Colors::getRed(c), pos);
  uint8_t green = Colors::fade(Colors::getRed(c), pos);
  uint8_t blue = Colors::fade(Colors::getRed(c), pos);

  return Colors::makeColor(red, green, blue);
}

void JacobsLadder::asJson(JsonObject obj) const {
  Model::asJson(obj);
  obj["size"] = size;
  obj["frequency"] = frequency;
  obj["duration"] = duration;
  colorAsJson(obj["color"].to<JsonObject>(), color);
  model->asJson(obj["model"].to<JsonObject>());
}
