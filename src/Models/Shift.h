#pragma once

#include <utility>

#include "Model.h"

class Shift;
typedef std::shared_ptr<Shift> ShiftPtr;

// An animation that shifts lights to the left or right. Unlike Rotate, does not
// wrap around once a color reaches the end.
// The speed of shift is given as a frequency expressed in Hz. A frequency
// of zero is stopped. Positive speed shifts up, negative speed shifts down.
class Shift : public Model {
  public:
    Shift(float distance, float speed, ModelPtr model);
    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setModel(std::shared_ptr<Model> newModel) { model = std::move(newModel); }

    static ShiftPtr make(float distance, float speed, const ModelPtr& model) {
      return std::make_shared<Shift>(distance, speed, model);
    }

private:
    float distance;
    float speed;
    float startTime;
    float endTime;
    float updateTime;
    float shiftOffset;
    std::shared_ptr<Model> model;
};
