#pragma once

#include "../lumos-arduino/Colors.h"
#include "Model.h"

class WarpCore;
typedef std::shared_ptr<WarpCore> WarpCorePtr;

class WarpCore : public Model {
  public:
    WarpCore(float size, float speed, int count, int slidein);

    void update(float timeStamp) override { model->update(timeStamp); }
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

    static WarpCorePtr make(float size, float speed, int count, int slidein)
      { return std::make_shared<WarpCore>(size, speed, count, slidein); }

  private:
    float size;
    float speed;
    int count;
    ModelPtr model;
};
