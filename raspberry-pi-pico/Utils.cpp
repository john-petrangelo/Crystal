#include <iomanip>
#include <sstream>

#include "Utils.h"

inline float frand(float fmin, float fmax) {
  // Scale the result of rand() to the range [fmin, fmax]
  float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  return fmin + random * (fmax - fmin);
}

std::string msToString(uint32_t timeMS) {
  auto days = timeMS / (24 * 60 * 60 * 1000);
  timeMS %= 24 * 60 * 60 * 1000;
  auto hours = timeMS / (1000 * 60 * 60);
  timeMS %= 1000 * 60 * 60;
  auto minutes = timeMS / (1000 * 60);
  timeMS %= 1000 * 60;
  auto seconds = timeMS / 1000;
  auto milliSeconds = timeMS % 1000;

  std::ostringstream oss;
  oss << days << "d " << hours << "h " << minutes << "m "
      << seconds << "." << std::setfill('0') << std::setw(3) << milliSeconds << "s";

  return oss.str();
}
