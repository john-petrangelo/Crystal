#include "Gradient.h"

Color Gradient::render(float pos) {
  float colorPos = pos * float(colors.size() - 1);

  // Get the two colors flanking the mapped position
  int lower = floor(colorPos);
  int upper = ceil(colorPos);

  // Linearly interpolate from the lower color to the upper color. If same, quick return.
  if (upper == lower) {
    return colors[lower];
  }

  float ratio = (colorPos - float(lower)) / float(upper - lower);
  return Colors::blend(colors[lower], colors[upper], ratio);
}

void Gradient::asJson(JsonObject obj) const {
  Model::asJson(obj);
  JsonArray colorArray = obj["colors"].to<JsonArray>();
  for (Color color : colors) {
    colorAsJson(colorArray.add<JsonObject>(), color);
  }
}
