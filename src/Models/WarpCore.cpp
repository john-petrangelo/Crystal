#include "../lumos-arduino/Logger.h"
#include "../utils.h"

#include "Gradient.h"
#include "Map.h"
#include "Shift.h"
#include "Solid.h"
#include "WarpCore.h"
#include "Triangle.h"

WarpCore::WarpCore(float size, float frequency, float dutyCycle)
  : Model("WarpCore"), size(size), frequency(frequency), dutyCycle(dutyCycle)
{
  ModelPtr glow = Triangle::make(0.0, 1.0, coreColor);

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
  float const start = 0.5f - size/2.0f;
  float const end = 0.5f + size/2.0f;

  // Map the glow into the range from start to end
  mapModel = Map::make(start, end, 0.0, 1.0, glow);

  // Calculate how long to display and shift the lights as well as how long to stay dark between cycles
  float const durationTotal = 1 / frequency;
  float const durationLit = durationTotal * dutyCycle;
  durationDark = fabs(durationTotal - durationLit);
  durationIn = durationLit / (1.0f + size);
  durationOut = durationLit - durationIn;
  Logger::logf("WarpCore::WarpCore     frequency=%-f\n", frequency);
  Logger::logf("WarpCore::WarpCore     dutyCycle=%-f\n", dutyCycle);
  Logger::logf("WarpCore::WarpCore durationTotal=%-f\n", durationTotal);
  Logger::logf("WarpCore::WarpCore   durationLit=%-f\n", durationLit);
  Logger::logf("WarpCore::WarpCore  durationDark=%-f\n", durationDark);
  Logger::logf("WarpCore::WarpCore    durationIn=%-f\n", durationIn);
  Logger::logf("WarpCore::WarpCore   durationOut=%-f\n", durationOut);

  // Set the initial mode and model
  mode = MODE_IN;
  model = Shift::In::make(-durationIn, mapModel);
  lastModeChangeTime = 0.0f;
}

void WarpCore::update(float timeStamp) {
  switch (mode) {
    case MODE_IN:
//      Logger::logf("WarpCore::update case MODE_IN\n");
      if (timeStamp - lastModeChangeTime >= fabs(durationIn)) {
        Logger::logf("WarpCore::update mode->OUT\n");
        mode = MODE_OUT;
        model = Shift::Out::make(-durationOut, mapModel);
        lastModeChangeTime = timeStamp;
      }
      break;
    case MODE_OUT:
      Logger::logf("WarpCore::update case MODE_OUT\n");
      if (timeStamp - lastModeChangeTime >= fabs(durationOut)) {
        Logger::logf("WarpCore::update mode->DARK\n");
        mode = MODE_DARK;
        model = Solid::make(BLACK);
        lastModeChangeTime = timeStamp;
      }
      break;
    case MODE_DARK:
      Logger::logf("WarpCore::update case MODE_DARK\n");
      if (timeStamp - lastModeChangeTime >= fabs(durationDark)) {
        Logger::logf("WarpCore::update mode->IN\n");
        mode = MODE_IN;
        model = Shift::In::make(-durationIn, mapModel);;
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
