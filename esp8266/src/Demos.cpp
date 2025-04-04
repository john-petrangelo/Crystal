#include <memory>

#include "lumos-arduino/Models/Gradient.h"
#include "lumos-arduino/Models/Map.h"
#include "lumos-arduino/Models/Pulsate.h"
#include "lumos-arduino/Models/Reverse.h"
#include "lumos-arduino/Models/Rotate.h"
#include "lumos-arduino/Models/Sum.h"
#include "lumos-arduino/Models/Triangle.h"
#include "lumos-arduino/Models/Window.h"

std::shared_ptr<Model> makeDemo1() {
  auto grad_left = Gradient::make(RED, YELLOW);
  auto grad_right = Gradient::make(BLUE, GREEN);

  auto rot_left = std::make_shared<Rotate>(-4.0, grad_left);
  auto rot_right = std::make_shared<Rotate>(1.0, grad_right);

  auto pulsator = std::make_shared<Pulsate>(0.2, 1.0, 2.5, 2.5, rot_left);


  auto map_left = Map::make(0.0, 0.2, 0.0, 1.0, pulsator);
  auto map_right = Map::make(0.2, 1.0, 0.0, 1.0, rot_right);

  auto window = std::make_shared<Window>(0.0, 0.2, map_left, map_right);
  auto rot_window = std::make_shared<Rotate>(-0.5, window);

  return rot_window;
}

std::shared_ptr<Model> makeDemo2() {
  auto gradient = Gradient::make(BLUE, RED);
  auto rot_grad = std::make_shared<Rotate>(2.0, gradient);
  auto rev_grad = std::make_shared<Reverse>(rot_grad);

  auto map_left = Map::make(0.0, 0.5, 0.0, 1.0, rot_grad);
  auto map_right = Map::make(0.5, 1.0, 0.0, 1.0, rev_grad);

  auto window = std::make_shared<Window>(0.0, 0.5, map_left, map_right);
  auto pulsator = std::make_shared<Pulsate>(0.2, 1.0, 2.5, 2.5, window);

  return  pulsator;
}

std::shared_ptr<Model> makeDemo3() {
  auto gradientRed = Gradient::make(BLACK, RED);
  auto gradientBlue = Gradient::make(BLUE, BLACK);
  auto triangle = std::make_shared<Triangle>(0.2, 0.4, GREEN);
  auto sum = Sum::make({gradientRed, gradientBlue, triangle});

  return  sum;
}
