#include <cmath>

#include "../lumos-arduino/Colors.h"

#include "Flame.h"
#include "../Models.h"
#include "../utils.h"

Flame::Flame() : Model("Flame") {
  auto mgm = std::make_shared<MultiGradientModel>("flame-multigradient", 7, BLACK, C1, C2, C3, C2, C1, BLACK);
  model = std::make_shared<MapModel>("flame-map",0.0, 1.0, 0.0, 1.0, mgm);
  lastUpdateMS = -PERIOD_SEC;
}

void Flame::update(float timeStamp) {
  if ((timeStamp - lastUpdateMS) > PERIOD_SEC) {
    lastUpdateMS = timeStamp;

    float const lower = frand(0, 0.2);
    float const upper = frand(0.8, 1.0);

    model->setInRange(lower, upper);
    model->update(timeStamp);
  }
}

Color Flame::render(float pos) {
  return model->render(pos);
}

void Flame::asJson(JsonObject obj) const {
  Model::asJson(obj);
  model->asJson(obj["model"].to<JsonObject>());
  colorAsJson(obj["Color1"].to<JsonObject>(), C1);
  colorAsJson(obj["Color2"].to<JsonObject>(), C2);
  colorAsJson(obj["Color3"].to<JsonObject>(), C3);
}
