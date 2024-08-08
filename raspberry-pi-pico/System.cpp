#include <hardware/clocks.h>
#include <iomanip>
#include <malloc.h>
#include <pico/cyw43_arch.h>
#include <sstream>

#include "System.h"

void System::setup() {
  // TODO still need setup?
//  Serial.begin(115200);
//  Serial.println("Serial started");
}

std::string System::getStatus() {
  std::ostringstream oss;

  oss << "timeSinceBoot: " << getTime() << std::endl;

  oss << "heapTotal: " << getHeapTotal() << std::endl;
  oss << "heapAvailable: " << getHeapAvailable() << std::endl;
  oss << "heapHighWater: " << getHeapHighWater() << std::endl;
  oss << "heapAllocated: " << getHeapAllocated() << std::endl;
  oss << "heapFreed: " << getHeapFreed() << std::endl;

  oss << "cpuFreqMhz: " << getCpuFreqMHz() << std::endl;

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

uint32_t System::getHeapTotal() {
  extern char __StackLimit;
  extern char __bss_end__;
  return &__StackLimit  - &__bss_end__;
}

uint32_t System::getHeapAvailable() {
  struct mallinfo m = mallinfo();
  return getHeapTotal() - m.uordblks;
}

uint32_t System::getHeapHighWater() {
  return mallinfo().arena;
}

uint32_t System::getHeapAllocated() {
  return mallinfo().uordblks;
}

uint32_t System::getHeapFreed() {
  return mallinfo().fordblks;
}

uint32_t System::getCpuFreqMHz() {
  // Convert frequency from Hz to MHz
  return clock_get_hz(clk_sys) / 1000000;
}
