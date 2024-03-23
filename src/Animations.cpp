#include "Animations.h"
#include "Combinations.h"
#include "Models/Pulsate.h"

/***** COMPOSITES *****/

std::shared_ptr<Model> makeDarkCrystal() {
  return makeCrystal(0xff00d0, 5.0, 0xff00d0, 8.0, 0xff00d0, 7.0);
}

std::shared_ptr<Model> makeCrystal(
    Color upperColor, float upperPeriodSec,
    Color middleColor, float middlePeriodSec,
    Color lowerColor, float lowerPeriodSec) {

  auto upperTriangle = std::make_shared<Triangle>("crystal upper color", 0.6, 1.0, upperColor);
  std::shared_ptr<Model> upperPulsate = upperTriangle;
  if (upperPeriodSec <= 10.0) {
    upperPulsate = std::make_shared<Pulsate>(0.3, 1.0, upperPeriodSec/2.0, upperPeriodSec/2.0, upperTriangle);
  }

  auto middleTriangle = std::make_shared<Triangle>("crystal middle color", 0.3, 0.7, middleColor);
  std::shared_ptr<Model> middlePulsate = middleTriangle;
  if (middlePeriodSec <= 10.0) {
    middlePulsate = std::make_shared<Pulsate>(0.4, 1.0, middlePeriodSec/2.0, middlePeriodSec/2.0, middleTriangle);
  }

  auto lowerTriangle = std::make_shared<Triangle>("crystal lower color", 0.0, 0.4, lowerColor);
  std::shared_ptr<Model> lowerPulsate = lowerTriangle;
  if (lowerPeriodSec <= 10.0) {
    lowerPulsate = std::make_shared<Pulsate>(0.3, 1.0, lowerPeriodSec/2.0, lowerPeriodSec/2.0, lowerTriangle);
  }

  auto sum = std::make_shared<Add>("sum", upperPulsate, middlePulsate);
  sum = std::make_shared<Add>("sum", sum, lowerPulsate);

  return sum;
}
