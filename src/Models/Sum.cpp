#include "Sum.h"

Color Sum::render(float pos) {
  Color const colorA = modelA->render(pos);
  Color const colorB = modelB->render(pos);

  Color const newColor = Colors::add(colorA, colorB);
  return newColor;
}

void Sum::asJson(JsonObject obj) const {
  Model::asJson(obj);
  JsonArray models = obj["models"].to<JsonArray>();
  modelA->asJson(models.add<JsonObject>());
  modelB->asJson(models.add<JsonObject>());
}
