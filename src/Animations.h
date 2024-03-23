#pragma once

#include "Models.h"

/***** COMPOSITES - Animations that are composites of other models *****/
extern std::shared_ptr<Model> makeDarkCrystal();
extern std::shared_ptr<Model> makeCrystal(
  Color upperColor, float upperPeriodSec,
  Color middleColor, float middlePeriodSec,
  Color lowerColor, float lowerPeriodSec);
