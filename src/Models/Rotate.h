#pragma once

#include <utility>

#include "Model.h"

// An animation that rotates or shifts lights to the left or right.
// Wraps around so that once a color reaches the end, then it wraps around.
// The speed of rotation is given as a frequency expressed in Hz. A frequency
// of zero is stopped. Positive speed rotates up, negative speed rotates down.
class Rotate : public Model {
  public:
    Rotate(float speed, std::shared_ptr<Model> model) : Model("Rotate"), speed(speed), model(std::move(model)) {}
    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setModel(std::shared_ptr<Model> newModel) { model = std::move(newModel); }

private:
    float speed;
    float rotationOffset = 0.0;
    float prevTimeStamp = 0.0;
    std::shared_ptr<Model> model;
};
