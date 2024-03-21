#pragma once
#include <memory>

#include "lumos-arduino/defs.h"
#include "lumos-arduino/Colors.h"
#include "lumos-arduino/Logger.h"

#include "Model.h"

/***** ADD *****/

/*
 * Add two models together, channel by channel, constraining each channel to 1.0
 * 
 * Requires two input models.
 * Position and time independent.
 */
class Add : public Model {
  public:
    Add(char const *name, std::shared_ptr<Model> a, std::shared_ptr<Model> b)
      : Model(name), modelA(a), modelB(b) { }
    void update(float timeStamp) override { modelA->update(timeStamp); modelB->update(timeStamp); }
    Color render(float pos) override;

    void asJson(JsonObject obj) const override;

  private:
    ModelPtr modelA;
    ModelPtr modelB;
};

/***** WINDOW *****/

/*
 * TODO WindowModel description
 */
class WindowModel : public Model {
  public:
    WindowModel(char const *name, float rangeMin, float rangeMax,
        ModelPtr insideModel, ModelPtr outsideModel)
      : Model(name), rangeMin(rangeMin), rangeMax(rangeMax),
        insideModel(std::move(insideModel)), outsideModel(std::move(outsideModel)) { }
    void update(float timeStamp) override { insideModel->update(timeStamp); outsideModel->update(timeStamp); }
    Color render(float pos) override;

    void asJson(JsonObject obj) const override;

private:
    std::shared_ptr<Model> insideModel;
    std::shared_ptr<Model> outsideModel;
    float rangeMin, rangeMax;
};
