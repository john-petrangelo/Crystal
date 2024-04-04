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
    enum SHIFT_MODE {
        SHIFT_IN,
        SHIFT_OUT
    };

    /**
     * @brief Constructor for Shift.
     *
     * @param shiftMode Specifies that the model shifts from the outside in or from the inside out
     * @param speed The speed of the movement:
     *              - Positive values indicate low-to-high speed
     *              - Negative values indicate high-to-low speed
     * @param model The model to shift in or out
     */
     Shift(SHIFT_MODE shiftMode, float speed, ModelPtr model) :
             Model("Shift"), shiftMode(shiftMode), speed(speed), model(std::move(model)),
             shiftOffset(1.0f), startTime(0.0f), endTime(0.0f), updateTime(0.0f) {}
    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setModel(std::shared_ptr<Model> newModel) { model = std::move(newModel); }

    static ShiftPtr make(SHIFT_MODE shiftMode, float speed, ModelPtr model) {
      return std::make_shared<Shift>(shiftMode, speed, std::move(model));
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
    SHIFT_MODE shiftMode;
    float speed;
    float startTime;
    float endTime;
    float updateTime;
    float shiftOffset;
    std::shared_ptr<Model> model;
};
