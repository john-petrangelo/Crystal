#include <cmath>

#include "Models.h"
#include "utils.h"

/***** TRIANGLE *****/

Color Triangle::render(float pos) {
  if (pos < rangeMin || pos > rangeMax) {
    return BLACK;
  }

  float midPoint = (rangeMin + rangeMax) / 2;

  if (pos <= midPoint) {
    // Rising side of triangle
    int ratio = 100 * fmap(pos, rangeMin, midPoint, 0.0, 1.0);
    return Colors::blend(BLACK, color, ratio);
  } else {
    // Falling side of triangle
    int ratio = 100 * fmap(pos, midPoint, rangeMax, 1.0, 0.0);
    return Colors::blend(BLACK, color, ratio);
  }
}

/***** GAUGE *****/

void GaugeModel::setValue(float valueIn) {
  value = valueIn;
  fullColorPixels = int(value * pixelsCount);
  remainder = value - fullColorPixels / pixelsCount;
  colorRatio = remainder * pixelsCount;
  dimmedColor = Colors::fade(color, (int)(100.0 * colorRatio));
//  Logger::logf("gauge value=%f fullColorPixels=%f remainder=%f colorRatio=%f dc=0x%06X\n",
//               value, fullColorPixels, remainder, colorRatio, dimmedColor);
}

Color GaugeModel::render(float pos) {
  // Figure out which is the last pixel we should light.
  float posPixel = int(pos * pixelsCount);
  //  Logger::logf("gauge pos=%f posPixel=%f", pos, posPixel);

  // If this isn't the last pixel, just return the full color.
  if (posPixel < fullColorPixels) {
  //    Logger::logf(" returning full color\n");
    return color;
  }

  // If this pixel is beyond the final lit pixel, paint it black.
  if (posPixel > fullColorPixels) {
  //    Logger::logf(" returning BLACK\n");
    return BLACK;
  }

  // It's the last lit pixel, dim the color proportionally to the remainder.
  //  Logger::logf(" returning dimmed color\n");
  return dimmedColor;
}
