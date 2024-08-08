#include <iomanip>
#include <sstream>

#include "System.h"

#include "pico/cyw43_arch.h"

void System::setup() {
  // TODO still need setup?
//  Serial.begin(115200);
//  Serial.println("Serial started");
}

std::string System::getStatus() {
  std::ostringstream oss;

  oss << "timeSinceBoot: " << getTime() << std::endl;

//  obj["freeHeapBytes"] = EspClass::getFreeHeap();
//  obj["heapFragmentation"] = EspClass::getHeapFragmentation();
//  obj["maxFreeBlockSize"] = EspClass::getMaxFreeBlockSize();
//  obj["sketchUsedBytes"] = EspClass::getSketchSize();
//  obj["sketchFreeBytes"] = EspClass::getFreeSketchSpace();
//  obj["lastResetReason"] = EspClass::getResetReason();
//  obj["cpuFreqMHz"] = EspClass::getCpuFreqMHz();
//  obj["flashChipSize"] = EspClass::getFlashChipSize();
//  obj["realFlashChipSize"] = EspClass::getFlashChipRealSize();

return oss.str();
}

std::string System::getTime() {
  absolute_time_t start_time = get_absolute_time();
  uint32_t msSinceBoot = to_ms_since_boot(start_time);

  auto days = msSinceBoot / (24 * 60 * 60 * 1000);
  msSinceBoot %= 24 * 60 * 60 * 1000;
  auto hours = msSinceBoot / (1000 * 60 * 60);
  msSinceBoot %= 1000 * 60 * 60;
  auto minutes = msSinceBoot / (1000 * 60);
  msSinceBoot %= 1000 * 60;
  auto seconds = msSinceBoot / 1000;
  auto milliSeconds = msSinceBoot % 1000;

  std::ostringstream oss;
  oss << days << "d " << hours << "h " << minutes << "m "
      << seconds << "." << std::setfill('0') << std::setw(3) << milliSeconds << "s";

  return oss.str();
}
