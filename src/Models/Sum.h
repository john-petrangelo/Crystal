#pragma once

#include <vector>

#include "Model.h"

/*
 * Sum models together, channel by channel, constraining each channel to maximum brightness
 * 
 * Position and time independent.
 */
class Sum;
typedef std::shared_ptr<Sum> SumPtr;

class Sum : public Model {
  public:
    Sum(std::initializer_list<ModelPtr> modelArgs) : Model("Sum"), models(modelArgs) {}
    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

    static SumPtr make(ModelPtr m1, ModelPtr m2) {
      return std::make_shared<Sum>(std::initializer_list<ModelPtr>{
        std::move(m1), std::move(m2)});
    }
    static SumPtr make(ModelPtr m1, ModelPtr m2, ModelPtr m3) {
      return std::make_shared<Sum>(std::initializer_list<ModelPtr>{
        std::move(m1), std::move(m2), std::move(m3)});
    }
    static SumPtr make(ModelPtr m1, ModelPtr m2, ModelPtr m3, ModelPtr m4) {
      return std::make_shared<Sum>(std::initializer_list<ModelPtr>{
        std::move(m1), std::move(m2), std::move(m3), std::move(m4)});
    }

private:
    std::vector<ModelPtr> models;
};
