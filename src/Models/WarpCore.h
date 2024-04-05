#pragma once

#include "../lumos-arduino/Colors.h"
#include "Model.h"

class WarpCore;
typedef std::shared_ptr<WarpCore> WarpCorePtr;

class WarpCore : public Model {
  public:
    WarpCore(float size, float frequency, float dutyCycle);

    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

    static WarpCorePtr make(float size, float frequency, float dutyCycle)
      { return std::make_shared<WarpCore>(size, frequency, dutyCycle); }

  private:
    enum Mode { MODE_IN, MODE_OUT, MODE_DARK };

    Color const coreColor =  Colors::makeColor(95, 95, 255);

    Mode mode = MODE_IN;
    float lastModeChangeTime;

    ModelPtr mapModel;
    ModelPtr model;

    float size;
    float frequency;
    float dutyCycle;
    float durationDark;
    float durationIn;
    float durationOut;
};
