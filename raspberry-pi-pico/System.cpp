#include <hardware/clocks.h>
#include <malloc.h>
#include <pico/cyw43_arch.h>
#include <sstream>

#include <ArduinoJson.h>

#include "System.h"
#include "Utils.h"

void System::setup() {
  // TODO still need setup?
//  Serial.begin(115200);
//  Serial.println("Serial started");
}

//std::string System::getStatus() {
//  std::ostringstream oss;
//
//  oss << "timeSinceBoot: " << getTime() << std::endl;
//
//  oss << "heapTotal: " << getHeapTotal() << std::endl;
//  oss << "heapAvailable: " << getHeapAvailable() << std::endl;
//  oss << "heapHighWater: " << getHeapHighWater() << std::endl;
//  oss << "heapAllocated: " << getHeapAllocated() << std::endl;
//  oss << "heapFreed: " << getHeapFreed() << std::endl;
//
//  oss << "cpuFreqMhz: " << getCpuFreqMHz() << std::endl;
//
//return oss.str();

  void System::getStatus(JsonObject obj) {
    obj["timeSinceBoot: "] = getTime();

    obj["heapTotal: "] = getHeapTotal();
    obj["heapAvailable: "] = getHeapAvailable();
    obj["heapHighWater: "] = getHeapHighWater();
    obj["heapAllocated: "] = getHeapAllocated();
    obj["heapFreed: "] = getHeapFreed();

    obj["cpuFreqMhz: "] = getCpuFreqMHz();
}

std::string System::getTime() {
  absolute_time_t start_time = get_absolute_time();
  uint32_t msSinceBoot = to_ms_since_boot(start_time);

  return msToString(msSinceBoot);
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
