#include "lumos-arduino/Colors.h"

#include "Combinations.h"
#include "utils.h"

/***** ADD *****/

Color Add::render(float pos) {
  Color const colorA = modelA->render(pos);
  Color const colorB = modelB->render(pos);

  Color const newColor = Colors::add(colorA, colorB);
  return newColor;
}

void Add::asJson(JsonObject obj) const {
  Model::asJson(obj);
  JsonArray models = obj["models"].to<JsonArray>();
  modelA->asJson(models.add<JsonObject>());
  modelB->asJson(models.add<JsonObject>());
}

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
