#include <math.h>

#include "lumos-arduino/Colors.h"

#include "Animations.h"
#include "Combinations.h"
#include "utils.h"

/***** ROTATE *****/

void Rotate::update(float timeStamp) {
  // New timestamp, calculate the new offset.
  float deltaTime = timeStamp - prevTimeStamp;
  prevTimeStamp = timeStamp;

  // How far should we rotate given the time delta. Handle wrapping to keep
  // offset between 0.0 and 1.0.
  float deltaPos = -deltaTime * speed;
  rotationOffset = fmod(rotationOffset + deltaPos, 1.0);
  if (rotationOffset < 0.0) {
    rotationOffset += 1.0;
  }

  // Update the wrapped model as well.
  model->update(timeStamp);
}

Color Rotate::render(float pos) {
  // If there's no predecessor, then there's nothing to rotate. Bail out.
  if (model == NULL) {
    return RED;
  }

  // Add the offset to the position, then correct for wrap-around
  float rotatedPos = fmod(pos + rotationOffset, 1.0);
  if (rotatedPos < 0.0) {
    rotatedPos += 1.0;
  }

  return model->render(rotatedPos);
}

/***** FLAME *****/

Flame::Flame() : Model("Flame"), lastUpdateMS(-PERIOD_SEC) {
  auto mgm = std::make_shared<MultiGradientModel>("flame-multigradient", 7, BLACK, C1, C2, C3, C2, C1, BLACK);
  model = std::make_shared<MapModel>("flame-map",0.0, 1.0, 0.0, 1.0, mgm);
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

/***** PULSATE *****/

void Pulsate::update(float timeStamp) {
  timeStamp = fmod(timeStamp, periodSecs);
  if (timeStamp < brightenSecs) {
    // We're getting brighter
    dimmness = fmap(timeStamp, 0.0, brightenSecs, brightest, dimmest);
  } else {
    // We're getting dimmer
    dimmness = fmap(timeStamp, brightenSecs, periodSecs, dimmest, brightest);
  }

  // Update the wrapped model as well.
  model->update(timeStamp);
}

Color Pulsate::render(float pos) {
  Color oldColor = model->render(pos);
  Color newColor = Colors::fade(oldColor, dimmness * 100.0);
  return newColor;
}

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
    upperPulsate = std::make_shared<Pulsate>("crystal upper pulsate", 0.3, 1.0, upperPeriodSec/2.0, upperPeriodSec/2.0, upperTriangle);
  }

  auto middleTriangle = std::make_shared<Triangle>("crystal middle color", 0.3, 0.7, middleColor);
  std::shared_ptr<Model> middlePulsate = middleTriangle;
  if (middlePeriodSec <= 10.0) {
    middlePulsate = std::make_shared<Pulsate>("crystal middle pulsate", 0.4, 1.0, middlePeriodSec/2.0, middlePeriodSec/2.0, middleTriangle);
  }

  auto lowerTriangle = std::make_shared<Triangle>("crystal lower color", 0.0, 0.4, lowerColor);
  std::shared_ptr<Model> lowerPulsate = lowerTriangle;
  if (lowerPeriodSec <= 10.0) {
    lowerPulsate = std::make_shared<Pulsate>("crystal lower pulsate", 0.3, 1.0, lowerPeriodSec/2.0, lowerPeriodSec/2.0, lowerTriangle);
  }

  auto sum = std::make_shared<Add>("sum", upperPulsate, middlePulsate);
  sum = std::make_shared<Add>("sum", sum, lowerPulsate);

  return sum;
}
