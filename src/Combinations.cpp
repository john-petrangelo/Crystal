#include <math.h>

#include "lumos-arduino/Colors.h"
#include "lumos-arduino/Logger.h"

#include "Combinations.h"
#include "utils.h"

/***** ADD *****/

Color Add::render(float pos) {
  Color const colorA = modelA->render(pos);
  Color const colorB = modelB->render(pos);

  Color const newColor = Colors::add(colorA, colorB);
  return newColor;
}

/***** WINDOW *****/

Color WindowModel::render(float pos) {
  if ((rangeMin <= pos) && (pos <= rangeMax)) {
    return insideModel->render(pos);
  }

  // The pos is outside the range
  return outsideModel->render(pos);
}
