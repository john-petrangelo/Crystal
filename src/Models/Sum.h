#pragma once

#include "Model.h"

/*
 * Sum models together, channel by channel, constraining each channel to maximum brightness
 * 
 * Position and time independent.
 */
class Sum : public Model {
  public:
    Sum(ModelPtr a, ModelPtr b)
      : Model("Sum"), modelA(std::move(a)), modelB(std::move(b)) { }
    void update(float timeStamp) override { modelA->update(timeStamp); modelB->update(timeStamp); }
    Color render(float pos) override;

    void asJson(JsonObject obj) const override;

  private:
    ModelPtr modelA;
    ModelPtr modelB;
};
