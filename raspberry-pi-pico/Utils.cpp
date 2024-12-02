#include <iomanip>
#include <sstream>

#include "Utils.h"

float frand(float const fmin, float const fmax) {
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

std::string_view trim(const std::string_view &str) {
  size_t const start = str.find_first_not_of(" \t\r\n");
  size_t const end = str.find_last_not_of(" \t\r\n");
  return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::vector<std::string> split(std::string_view str, char delimiter) {
  std::vector<std::string> result;
  size_t start = 0;
  size_t end;

  while ((end = str.find(delimiter, start)) != std::string_view::npos) {
    result.emplace_back(str.substr(start, end - start));
    start = end + 1; // Move past the delimiter
  }

  // Add the last token
  result.emplace_back(str.substr(start));
  return result;
}
