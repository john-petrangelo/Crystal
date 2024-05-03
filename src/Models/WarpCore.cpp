#include "../lumos-arduino/Logger.h"
#include "../utils.h"

#include "Gradient.h"
#include "Map.h"
#include "Shift.h"
#include "Solid.h"
#include "WarpCore.h"
#include "Triangle.h"
#include "Sum.h"
#include "Reverse.h"

Color const WarpCore::defaultColor =  Colors::makeColor(95, 95, 255);

WarpCore::WarpCore(float size, float frequency, float dutyCycle, Color color, bool bidirectional)
  : Model("WarpCore"), size(size), frequency(frequency), dutyCycle(dutyCycle), color(color),
    bidirectional(bidirectional)
{
  init();
  lastModeChangeTime = 0.0f; //float(millis()) / 1000.0f;
}

void WarpCore::init() {
  ModelPtr glow = Triangle::make(0.0, 1.0, color);

  // If the size is too big or too small, then we have a warp core breach, light the whole length
  if (size < 0.0 || size >= 1.0) {
    mapModel = glow;
    return;
  }

  // If the frequency is zero then we don't have a functioning Warp Core, just glow
  if (frequency == 0.0) {
    mapModel = glow;
    return;
  }

  // Range of the model to light, from start to end
  float start;
  float end;
  if (frequency > 0.0f) {
    start = 0.0f;
    end = size;
  } else {
    start = 1.0f - size;
    end = 1.0f;
  }

  // Map the glow into the range from start to end
  mapModel = Map::make(start, end, 0.0, 1.0, glow);

  // Calculate how long to display and shift the lights as well as how long to stay dark between cycles
  float const durationTotal = 1 / frequency;
  float const durationLit = durationTotal * dutyCycle;
  durationDark = fabs(durationTotal - durationLit);
  durationIn = durationLit / (1.0f + size);
  durationOut = durationLit - durationIn;

  // Set the initial mode and model
  mode = MODE_IN;
  model = Shift::In::make(-durationIn, mapModel);

  handleBidirectional();
}

void WarpCore::handleBidirectional() {
  if (bidirectional) {
    model = Sum::make({
        Map::make(0.0, 0.5, 0.0, 1.0, Reverse::make(model)),
        Map::make(0.5, 1.0, 0.0, 1.0, model)
    });
  }
}

void WarpCore::set(float newFrequency, float newSize, float newDutyCycle, Color newColor, bool newBidirectional) {
  frequency = newFrequency;
  size = newSize;
  dutyCycle = newDutyCycle;
  color = newColor;
  bidirectional = newBidirectional;

  init();
}

void WarpCore::update(float timeStamp) {
  switch (mode) {
    case MODE_IN:
      if (timeStamp - lastModeChangeTime >= fabs(durationIn)) {
        mode = MODE_OUT;
        model = Shift::Out::make(-durationIn, mapModel);
        handleBidirectional();
        lastModeChangeTime = timeStamp;
      }
      break;
    case MODE_OUT:
      if (timeStamp - lastModeChangeTime >= fabs(durationOut)) {
        mode = MODE_DARK;
        model = Solid::make(BLACK);
        lastModeChangeTime = timeStamp;
      }
      break;
    case MODE_DARK:
      if (timeStamp - lastModeChangeTime >= fabs(durationDark)) {
        mode = MODE_IN;
        model = Shift::In::make(-durationIn, mapModel);;
        handleBidirectional();
        lastModeChangeTime = timeStamp;
      }
      break;
  }

  model->update(timeStamp);
}

Color WarpCore::render(float pos) {
  return model->render(pos);
}

void WarpCore::asJson(JsonObject obj) const {
  Model::asJson(obj);
  obj["size"] = size;
  obj["frequency"] = frequency;
  obj["dutyCycle"] = dutyCycle;
  switch(mode) {
    case MODE_IN:
      obj["mode"] = "IN";
      break;
    case MODE_OUT:
      obj["mode"] = "OUT";
      break;
    case MODE_DARK:
      obj["mode"] = "DARK";
      break;
  }
  obj["durationIn"] = durationIn;
  obj["durationOut"] = durationOut;
  obj["durationDark"] = durationDark;
  model->asJson(obj["model"].to<JsonObject>());
}
