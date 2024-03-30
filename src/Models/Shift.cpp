#include "../utils.h"
#include "../lumos-arduino/Logger.h"

#include "Shift.h"

Shift::Shift(float distance, float speed, ModelPtr model) :
        Model("Shift"), distance(distance), speed(speed), model(std::move(model)),
        shiftOffset(1.0f), startTime(float(millis()) / 1000.0f), updateTime(0)
{
  if (speed == 0.0f) {
    endTime = startTime;
  } else {
    endTime = startTime + 1.0f / speed;
  }
}

void Shift::update(float timeStamp) {
  updateTime = startTime + timeStamp;

  // If speed is stopped, then only update the wrapped model.
  if (updateTime >= endTime) {
    // We've already reached the end, render as completely shifted
    shiftOffset = 0.0;
  } else {
    // How far along are we in the shift window?
    shiftOffset = 1.0f - fmap(updateTime, startTime, endTime, 0.0f, 1.0f);
  }

  // Update the wrapped model as well.
  model->update(timeStamp);

  Logger::logf("Shift - t=%f offset=%f\n", timeStamp, shiftOffset);
}

Color Shift::render(float pos) {
  // If there's no predecessor, then there's nothing to shift. Bail out.
  if (model == nullptr) {
    return BLACK;
  }

  // Add the offset to the position
  float shiftedPos = pos + shiftOffset;

  if (shiftedPos > 1.0) {
    return BLACK;
  }

  // Render the predecessor model at the adjusted position
  return model->render(shiftedPos);
}

void Shift::asJson(JsonObject obj) const {
  Model::asJson(obj);
  obj["speed"] = speed;
  obj["distance"] = distance;
  obj["shiftOffset"] = shiftOffset;
  obj["startTime"] = startTime;
  obj["endTime"] = endTime;
  obj["updateTime"] = updateTime;
  model->asJson(obj["model"].to<JsonObject>());
}
