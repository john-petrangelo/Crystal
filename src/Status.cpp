#include <ArduinoJson.h>
#include <LittleFS.h>

#include "Filesystem.h"
#include "Network.h"
#include "Status.h"

static void systemGetStatus(JsonObject obj) {
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

String getStatus() {
  JsonDocument doc;

  systemGetStatus(doc["system"].to<JsonObject>());
  Network::getRenderer()->getStatus(doc["renderer"].to<JsonObject>());
  Filesystem::getStatus(doc["filesystem"].to<JsonObject>());
  Network::getStatus(doc["network"].to<JsonObject>());

  String output;
  serializeJsonPretty(doc, output);
  output += "\n";

  return output;
}
