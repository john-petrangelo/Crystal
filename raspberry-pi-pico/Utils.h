#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

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
 * Return a random float between fmin and fmax.
 */
float frand(float fmin, float fmax);

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

/**
 * Returns the sign of the given value as an integer.
 * - If the value is negative, returns -1.
 * - If the value is positive, returns 1.
 * - If the value is zero, returns 0.
 */
template <typename T>
inline T sign(T val) {
  return (T(0) < val) - (val < T(0));
}

std::string msToString(uint32_t timeMS);
std::string_view trim(std::string_view const& str);
std::vector<std::string> split(std::string_view str, char delimiter);

// Computes a hash value for a string in a case-insensitive manner.
// This is achieved by converting the string to lowercase before applying the standard hash.
// Useful for creating case-insensitive unordered sets.
struct CaseInsensitiveHash {
  size_t operator()(const std::string &key) const {
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return std::hash<std::string>()(lowerKey);
  }
};

// CaseInsensitiveEqual: Compares two strings for equality in a case-insensitive manner.
// This is achieved by converting each character to lowercase during comparison.
// Useful for creating case-insensitive unordered sets.
struct CaseInsensitiveEqual {
  bool operator()(const std::string &lhs, const std::string &rhs) const {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                      [](unsigned char c1, unsigned char c2) {
                          return std::tolower(c1) == std::tolower(c2);
                      });
  }
};
