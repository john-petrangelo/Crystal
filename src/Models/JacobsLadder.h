#pragma once

#include "../lumos-arduino/Colors.h"
#include "Shift.h"

class JacobsLadder;
typedef std::shared_ptr<JacobsLadder> JacobsLadderPtr;

class JacobsLadder : public Model {
  public:
    JacobsLadder(float size, float frequency, Color color);

    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;
    void set(float newFrequency, float newSize, Color color);

    static JacobsLadderPtr make(float size, float frequency, Color color)
      { return std::make_shared<JacobsLadder>(size, frequency, color); }

    static Color const defaultColor;

private:
    void init();

    float lastResetTime;

    ModelPtr mapModel;
    ShiftPtr model;

    float size;
    float frequency;
    Color color;

    float duration{};
};
