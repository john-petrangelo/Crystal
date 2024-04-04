#pragma once

#include <utility>

#include "Model.h"

class Shift;
typedef std::shared_ptr<Shift> ShiftPtr;

// An animation that shifts lights to the left or right. Unlike Rotate, it does not
// wrap around once a color reaches the end.
// The speed of shift is given as a frequency expressed in Hz. A frequency
// of zero is stopped. Positive speed shifts up, negative speed shifts down.
class Shift : public Model {
  public:
    enum INOUT {
        SHIFT_IN,
        SHIFT_OUT
    };

    Shift(INOUT inout, float speed, ModelPtr model) :
            Model("Shift"), inout(inout), speed(speed), model(std::move(model)),
            shiftOffset(1.0f), startTime(0.0f), endTime(0.0f), updateTime(0.0f) {}
    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setModel(std::shared_ptr<Model> newModel) { model = std::move(newModel); }

    static ShiftPtr make(INOUT inout, float speed, ModelPtr model) {
      return std::make_shared<Shift>(inout, speed, std::move(model));
    }

    class In {
    public:
        static ShiftPtr make(float speed, ModelPtr model) {
          return std::make_shared<Shift>(SHIFT_IN, speed, std::move(model));
        }
    };

    class Out {
    public:
        static ShiftPtr make(float speed, ModelPtr model) {
          return std::make_shared<Shift>(SHIFT_OUT, speed, std::move(model));
        }
    };

  private:
    INOUT inout;
    float speed;
    float startTime;
    float endTime;
    float updateTime;
    float shiftOffset;
    std::shared_ptr<Model> model;
};
