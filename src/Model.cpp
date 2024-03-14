#include <math.h>

#include "Model.h"
#include "utils.h"

/***** MULTIGRADIENT *****/

MultiGradientModel::MultiGradientModel(char const *name, int count, ...) : Model(name), count(count) {
  if (count < 2 || count > MAX_COLORS) {
    count = 0;
    return;
  }
  
  // Declare a va_list macro and initialize it with va_start.
  // Copy all of the colors from varargs to array.
  va_list argList;
  va_start(argList, count);
  for (int i = 0; i < count; i++) {
    colors[i] = va_arg(argList, Color);
  }
  va_end(argList);
}

Color MultiGradientModel::render(float pos) {
  float colorPos = pos * (count - 1);

  // Get the two colors flanking the mapped position
  int lower = floor(colorPos);
  int upper = ceil(colorPos);

  // Linearly interpolate from the lower color to the upper color. If same, quick return.
  if (upper == lower) {
    return colors[lower];
  }

  float ratio = (colorPos - lower) / (upper - lower);
  return Colors::blend(colors[lower], colors[upper], 100 * ratio);
}

/***** MAP *****/

Color MapModel::render(float pos) {
  if ((inRangeMin <= pos) && (pos <= inRangeMax)) {
    float outPos = fmap(pos, inRangeMin, inRangeMax, outRangeMin, outRangeMax);
    return model->render(outPos);
  }

  // Everything outside the "in range" will be BLACK
  return BLACK;
}

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
