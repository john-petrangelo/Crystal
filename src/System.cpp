#include <ArduinoJson.h>

#include "System.h"

#ifdef ENABLE_GDB_STUB
#include <GDBStub.h>
#endif

void System::setup() {
  Serial.begin(115200);
  Serial.println("Serial started");

#ifdef ENABLE_GDB_STUB
  gdbstub_init();
#endif
}

void System::getStatus(JsonObject obj) {
  getTime(obj);

  obj["freeHeapBytes"] = EspClass::getFreeHeap();
  obj["heapFragmentation"] = EspClass::getHeapFragmentation();
  obj["maxFreeBlockSize"] = EspClass::getMaxFreeBlockSize();
  obj["sketchUsedBytes"] = EspClass::getSketchSize();
  obj["sketchFreeBytes"] = EspClass::getFreeSketchSpace();
  obj["lastResetReason"] = EspClass::getResetReason();
  obj["cpuFreqMHz"] = EspClass::getCpuFreqMHz();
  obj["flashChipSize"] = EspClass::getFlashChipSize();
  obj["realFlashChipSize"] = EspClass::getFlashChipRealSize();
}

void System::getTime(JsonObject obj) {
  auto now = millis();
  auto days = now / (24*60*60*1000);
  now %= 24*60*60*1000;
  auto hours = now / (1000*60*60);
  now %= 1000*60*60;
  auto mins = now / (1000*60);
  now %= 1000*60;
  auto secs = now / 1000;
  auto milliSecs = now % 1000;

  char buffer[32];
  sprintf(buffer,"%lud %luh %lum %lu.%0.3lus", days, hours, mins, secs, milliSecs);
  obj["time"] = String(buffer);
}
