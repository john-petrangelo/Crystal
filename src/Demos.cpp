#include <memory>

#include "Models/Gradient.h"
#include "Models/Map.h"
#include "Models/Pulsate.h"
#include "Models/Reverse.h"
#include "Models/Rotate.h"
#include "Models/Sum.h"
#include "Models/Triangle.h"
#include "Models/Window.h"

std::shared_ptr<Model> makeDemo1() {
  auto grad_left = Gradient::make(RED, YELLOW);
  auto grad_right = Gradient::make(BLUE, GREEN);

  auto rot_left = std::make_shared<Rotate>(-4.0, grad_left);
  auto rot_right = std::make_shared<Rotate>(1.0, grad_right);

  auto pulsator = std::make_shared<Pulsate>(0.2, 1.0, 2.5, 2.5, rot_left);


  auto map_left = std::make_shared<Map>(0.0, 0.2, 0.0, 1.0, pulsator);
  auto map_right = std::make_shared<Map>(0.2, 1.0, 0.0, 1.0, rot_right);

  auto window = std::make_shared<Window>(0.0, 0.2, map_left, map_right);
  auto rot_window = std::make_shared<Rotate>(-0.5, window);

  return rot_window;
}

std::shared_ptr<Model> makeDemo2() {
  auto gradient = Gradient::make(BLUE, RED);
  auto rot_grad = std::make_shared<Rotate>(2.0, gradient);
  auto rev_grad = std::make_shared<Reverse>(rot_grad);

  auto map_left = std::make_shared<Map>(0.0, 0.5, 0.0, 1.0, rot_grad);
  auto map_right = std::make_shared<Map>(0.5, 1.0, 0.0, 1.0, rev_grad);

  auto window = std::make_shared<Window>(0.0, 0.5, map_left, map_right);
  auto pulsator = std::make_shared<Pulsate>(0.2, 1.0, 2.5, 2.5, window);

  return  pulsator;
}

std::shared_ptr<Model> makeDemo3() {
  auto gradientRed = Gradient::make(BLACK, RED);
  auto gradientBlue = Gradient::make(BLUE, BLACK);
  auto sum = std::make_shared<Sum>(gradientRed, gradientBlue);

  auto triangle = std::make_shared<Triangle>(0.2, 0.4, GREEN);
  auto sum2 = std::make_shared<Sum>(sum, triangle);

  return  sum2;
}
