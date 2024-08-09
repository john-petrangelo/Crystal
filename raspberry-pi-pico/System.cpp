#include <hardware/clocks.h>
#include <malloc.h>
#include <pico/cyw43_arch.h>

#include <ArduinoJson.h>

#include "System.h"
#include "Utils.h"

void System::setup() {
  // TODO still need setup?
//  Serial.begin(115200);
//  Serial.println("Serial started");
}

  void System::getStatus(JsonObject obj) {
    obj["timeSinceBoot: "] = getTime();
    obj["cpuFreqMhz: "] = getCpuFreqMHz();

    auto memory = obj["memory"].to<JsonObject>();
    memory["heapTotal"] = getHeapTotal();
    memory["heapAvailable"] = getHeapAvailable();
    memory["heapHighWater"] = getHeapHighWater();
    memory["heapAllocated"] = getHeapAllocated();
    memory["heapFreed"] = getHeapFreed();
}

std::string System::getTime() {
  absolute_time_t start_time = get_absolute_time();
  uint32_t msSinceBoot = to_ms_since_boot(start_time);

  return msToString(msSinceBoot);
}

uint32_t System::getHeapTotal() {
  extern char __StackLimit;
  extern char __bss_end__;
  return &__StackLimit - &__bss_end__;
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
