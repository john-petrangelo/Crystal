#include "../lumos-arduino/Logger.h"
#include "../utils.h"

#include "Gradient.h"
#include "Map.h"
#include "Shift.h"
#include "Solid.h"
#include "WarpCore.h"
#include "Triangle.h"

WarpCore::WarpCore(float size, float speed) : Model("WarpCore"), size(size), speed(speed) {
  Color c = Colors::makeColor(95, 95, 255);
  ModelPtr glow = Triangle::make(0.0, 1.0, c);
  if (size < 0.0 || size >= 1.0) {
    // Overflow, warp core breach, just always return the glow color
    mapModel = glow;
    return;
  }

  float start = 0.5f - size/2.0f;
  float end = 0.5f + size/2.0f;

  mapModel = Map::make(start, end, 0.0, 1.0, glow);

  mode = MODE_IN;
  model = Shift::In::make(speed, mapModel);
  lastModeChangeTime = 0.0f;
}

void WarpCore::update(float timeStamp) {
  float modeDuration = 1.0f / fabs(speed);
  if (mode == MODE_BETWEEN) {
    modeDuration /= 2;
  }

  if (timeStamp - lastModeChangeTime >= modeDuration) {
    switch (mode) {
      case MODE_IN:
        mode = MODE_OUT;
        model = Shift::Out::make(speed, mapModel);
        break;
      case MODE_OUT:
        mode = MODE_BETWEEN;
        model = Solid::make(BLACK);
        break;
      case MODE_BETWEEN:
        mode = MODE_IN;
        model = Shift::In::make(speed, mapModel);
        break;
    }

    lastModeChangeTime = timeStamp;
  }

  model->update(timeStamp);
}

Color WarpCore::render(float pos) {
  return model->render(pos);
}

void WarpCore::asJson(JsonObject obj) const {
  Model::asJson(obj);
  obj["size"] = size;
  obj["speed"] = speed;
  model->asJson(obj["model"].to<JsonObject>());
}
