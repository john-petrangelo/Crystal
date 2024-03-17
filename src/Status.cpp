#include <ArduinoJson.h>
#include <LittleFS.h>

#include "Filesystem.h"
#include "Network.h"
#include "Status.h"

String getStatus() {
  JsonDocument doc;
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
  snprintf(buffer, sizeof(buffer),"%lud %luh %lum %lu.%0.3lus", days, hours, mins, secs, milliSecs);
  doc["time"] = buffer;

  Network::getRenderer()->getStatus(doc["renderer"].to<JsonObject>());

  JsonObject system = doc["system"].to<JsonObject>();
  system["freeHeapBytes"] = EspClass::getFreeHeap();
  system["heapFragmentation"] = EspClass::getHeapFragmentation();
  system["maxFreeBlockSize"] = EspClass::getMaxFreeBlockSize();
  system["sketchUsedBytes"] = EspClass::getSketchSize();
  system["sketchFreeBytes"] = EspClass::getFreeSketchSpace();
  system["lastResetReason"] = EspClass::getResetReason();
  system["cpuFreqMHz"] = EspClass::getCpuFreqMHz();
  system["flashChipSize"] = EspClass::getFlashChipSize();
  system["realFlashChipSize"] = EspClass::getFlashChipRealSize();

  Filesystem::getStatus(doc["filesystem"].to<JsonObject>());
  Network::getStatus(doc["network"].to<JsonObject>());

  String output;
  serializeJsonPretty(doc, output);
  output += "\n";

  return output;
}
