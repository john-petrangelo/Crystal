#include "Combinations.h"

/***** WINDOW *****/

Color WindowModel::render(float pos) {
  if ((rangeMin <= pos) && (pos <= rangeMax)) {
    return insideModel->render(pos);
  }

  // The pos is outside the range
  return outsideModel->render(pos);
}

void WindowModel::asJson(JsonObject obj) const {
  Model::asJson(obj);
  insideModel->asJson(obj["insideModel"].to<JsonObject>());
  outsideModel->asJson(obj["outsideModel"].to<JsonObject>());
  obj["rangeMin"] = rangeMin;
  obj["rangeMax"] = rangeMax;
}
