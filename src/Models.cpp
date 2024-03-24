#include "Models.h"

/***** GAUGE *****/

void GaugeModel::setValue(float valueIn) {
  value = valueIn;
  fullColorPixels = int(value * pixelsCount);
  remainder = value - fullColorPixels / pixelsCount;
  colorRatio = remainder * pixelsCount;
  dimmedColor = Colors::fade(color, (int)(100.0 * colorRatio));
}

Color GaugeModel::render(float pos) {
  // Figure out which is the last pixel we should light.
  float posPixel = int(pos * pixelsCount);

  // If this isn't the last pixel, just return the full color.
  if (posPixel < fullColorPixels) {
    return color;
  }

  // If this pixel is beyond the final lit pixel, paint it black.
  if (posPixel > fullColorPixels) {
    return BLACK;
  }

  // It's the last lit pixel, dim the color proportionally to the remainder.
  return dimmedColor;
}
