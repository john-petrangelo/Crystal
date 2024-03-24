#pragma once

#include <utility>

#include "Model.h"

class Reverse : public Model {
  public:
    explicit Reverse(ModelPtr model) : Model("Reverse"), model(std::move(model)) { }
    void update(float timeStamp) override { model->update(timeStamp); }
    Color render(float pos) override { return model->render(1.0f - pos); }

    void asJson(JsonObject obj) const override {
      Model::asJson(obj);
      model->asJson(obj["model"].to<JsonObject>());
    }

private:
    ModelPtr model;
};
