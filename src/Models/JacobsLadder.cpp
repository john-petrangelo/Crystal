#include "../lumos-arduino/Logger.h"
#include "../utils.h"

#include "Gradient.h"
#include "JacobsLadder.h"
#include "Map.h"
#include "Shift.h"
#include "Triangle.h"

Color const JacobsLadder::defaultColor =  WHITE;

JacobsLadder::JacobsLadder(float size, float frequency, Color color, float jitterSize, float jitterPeriod)
    : Model("JacobsLadder"), size(size), frequency(frequency), color(color),
      jitterSize(jitterSize), jitterPeriod(jitterPeriod) {
  init();
}

void JacobsLadder::init() {
  Logger::logf("JL:init color=0x%06X\n", color);
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

void JacobsLadder::set(float newFrequency, float newSize, Color newColor, float newJitterSize, float newJitterPeriod) {
  Logger::logf("JL:init color=0x%06X newColor=0x%06X\n", color, newColor);
  frequency = newFrequency;
  size = newSize;
  color = newColor;
  jitterSize = newJitterSize;
  jitterPeriod = newJitterPeriod;

  init();
}

void JacobsLadder::update(float timeStamp) {
  if (timeStamp - lastResetTime >= fabs(duration)) {
    lastResetTime = timeStamp;
    model->reset();
  }

  model->update(timeStamp);

  if (timeStamp - lastJitterTime >= jitterPeriod) {
    jitter = frand(0.0, jitterSize);
    lastJitterTime = timeStamp;
  }
}

Color JacobsLadder::render(float pos) {
  pos = constrain(pos + jitter, 0.0, 1.0);
  Color c = model->render(pos);
  uint8_t red = Colors::fade(Colors::getRed(c), pos);
  uint8_t green = Colors::fade(Colors::getGreen(c), pos);
  uint8_t blue = Colors::fade(Colors::getBlue(c), pos);

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
