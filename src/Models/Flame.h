#pragma once

#include "../lumos-arduino/Colors.h"
#include "Model.h"
#include "Map.h"

class Flame : public Model {
  public:
    Flame();

    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

  private:
    Color const C1 = Colors::blend(RED, YELLOW, 50);
    Color const C2 = Colors::blend(RED, YELLOW, 70);
    Color const C3 = Colors::blend(RED, YELLOW, 90);

    std::shared_ptr<Map> model{};

    float lastUpdateMS;
    float const PERIOD_SEC = 0.110;
};
