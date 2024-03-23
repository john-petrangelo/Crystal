#pragma once

#include "Models/Model.h"

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
