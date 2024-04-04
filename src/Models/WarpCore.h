#pragma once

#include "../lumos-arduino/Colors.h"
#include "Model.h"

class WarpCore;
typedef std::shared_ptr<WarpCore> WarpCorePtr;

class WarpCore : public Model {
  public:
    WarpCore(float size, float speed);

    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

    static WarpCorePtr make(float size, float speed)
      { return std::make_shared<WarpCore>(size, speed); }

  private:
    enum Mode { MODE_IN, MODE_OUT, MODE_BETWEEN };

    Mode mode = MODE_IN;
    float lastModeChangeTime;
    ModelPtr mapModel;

    float size;
    float speed;
    ModelPtr model;

};
