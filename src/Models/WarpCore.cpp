#include <vector>

#include "../lumos-arduino/Logger.h"
#include "../utils.h"

#include "Map.h"
#include "Shift.h"
//#include "Solid.h"
//#include "Sum.h"
#include "Triangle.h"
#include "WarpCore.h"
#include "Gradient.h"

//WarpCore::WarpCore(float size, float speed, int count) :
//    Model("WarpCore"), size(size), speed(speed), count(count) {
//  Color c = Colors::makeColor(95, 95, 255);
//  ModelPtr solid = Solid::make(c);
//  if (size * (float)count >= 1.0) {
//    // Overflow, warp core breach, just always return the solid color
//    model = solid;
//    return;
//  }
//
//  std::vector<ModelPtr> maps;
//  float const offset = 1.0f / (float)count;
//  for (int i = 0; i < count; ++i) {
//    float start = (float)i * offset;
//    float end = start + size;
//    maps.push_back(Map::make(start, end, 0.0, 1.0, solid));
//  }
//  auto sum = Sum::make(maps.begin(), maps.end());
//
//  model = Rotate::make(speed, sum);
//}

WarpCore::WarpCore(float size, float speed, int count) :
        Model("WarpCore"), size(size), speed(speed), count(count) {
  auto c = Colors::makeColor(95, 95, 255);
//  auto triangle = Triangle::make(0.0, 1.0, c);
  auto triangle = Gradient::make(RED, VIOLET, INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED);
  model = Shift::make(1.0, speed, triangle);
}

Color WarpCore::render(float pos) {
  return model->render(pos);
}

void WarpCore::asJson(JsonObject obj) const {
  Model::asJson(obj);
  obj["size"] = size;
  obj["speed"] = speed;
  obj["count"] = count;
  model->asJson(obj["model"].to<JsonObject>());
}
