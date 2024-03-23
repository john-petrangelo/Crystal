#include <cmath>

#include "Gradient.h"

MultiGradientModel::MultiGradientModel(int count, ...) : Model("MultiGradient"), count(count) {
  if (count < 2 || count > MAX_COLORS) {
    count = 0;
    return;
  }
  
  // Declare a va_list macro and initialize it with va_start.
  // Copy all the colors from varargs to array.
  va_list argList;
  va_start(argList, count);
  for (int i = 0; i < count; i++) {
    colors[i] = va_arg(argList, Color);
  }
  va_end(argList);
}

Color MultiGradientModel::render(float pos) {
  float colorPos = pos * float(count - 1);

  // Get the two colors flanking the mapped position
  int lower = floor(colorPos);
  int upper = ceil(colorPos);

  // Linearly interpolate from the lower color to the upper color. If same, quick return.
  if (upper == lower) {
    return colors[lower];
  }

  float ratio = (colorPos - float(lower)) / float(upper - lower);
  return Colors::blend(colors[lower], colors[upper], int(100.0f * ratio));
}
