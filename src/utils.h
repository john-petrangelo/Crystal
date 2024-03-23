#include <Arduino.h>

/**
 * Re-maps a number from one range to another. That is, a value of fromLow would get mapped to toLow,
 * a value of fromHigh to toHigh, values in-between to values in-between, etc.
 *
 * This is taken directly from the Arduino map function described here:
 * https://www.arduino.cc/reference/en/language/functions/math/map/
 * 
 * ...except it has been re-applied to floats instead of longs.
 */
inline float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * Return a random number between fmin and fmax.
 */
inline float frand(double fmin, double fmax) {
  return fmin + random(1UL << 31) * (fmax - fmin) / (1UL << 31);
}

/**
 * Returns an interpolation between two inputs (v0, v1) for a parameter (t)
 * 
 * This is taken directly from the sample code on the Wikipedia page:
 * https://en.wikipedia.org/wiki/Linear_interpolation#Programming_language_support
 *
 * Imprecise method, which does not guarantee v = v1 when t = 1, due to floating-point arithmetic error. This method is monotonic
 * This form may be used when the hardware has a native fused multiply-add instruction.
 *
 */
inline float lerp(float v0, float v1, float t) {
  return v0 + t * (v1 - v0);
}
