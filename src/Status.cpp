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

  getNetworkRenderer()->getStatus(doc.createNestedObject("renderer"));

  JsonObject system = doc.createNestedObject("system");
  system["freeHeapBytes"] = ESP.getFreeHeap();
  system["heapFragmentation"] = ESP.getHeapFragmentation();
  system["maxFreeBlockSize"] = ESP.getMaxFreeBlockSize();
  system["sketchUsedBytes"] = ESP.getSketchSize();
  system["sketchFreeBytes"] = ESP.getFreeSketchSpace();
  system["lastResetReason"] = ESP.getResetReason();
  system["cpuFreqMHz"] = ESP.getCpuFreqMHz();
  system["flashChipSize"] = ESP.getFlashChipSize();
  system["realFlashChipSize"] = ESP.getFlashChipRealSize();

  Filesystem::getStatus(doc.createNestedObject("filesystem"));

  JsonObject network = doc.createNestedObject("network");
  network["hostname"] = hostname + ".local";
  network["wifiMACAddress"] = WiFi.macAddress();
  network["ipAddress"] = WiFi.localIP().toString();
  network["mode"] = "unknown";
  switch(WiFi.getMode()) {
    case WIFI_OFF:
      network["mode"] = "WIFI_OFF";
      break;
    case WIFI_STA:
      network["mode"] = "WIFI_STA";
      break;
    case WIFI_AP:
      network["mode"] = "WIFI_AP";
      break;
    case WIFI_AP_STA:
      network["mode"] = "WIFI_AP_STA";
      break;
    default:
      network["mode"] = String(WiFi.getMode());
      break;
  }
  network["phyMode"] = "unknown";
  switch(WiFi.getPhyMode()) {
    case WIFI_PHY_MODE_11B:
      network["phyMode"] = "WIFI_PHY_MODE_11B";
      break;
    case WIFI_PHY_MODE_11G:
      network["phyMode"] = "WIFI_PHY_MODE_11G";
      break;
    case WIFI_PHY_MODE_11N:
      network["phyMode"] = "WIFI_PHY_MODE_11N";
      break;
    default:
      network["phyMode"] = String(WiFi.getPhyMode());
      break;
  }
  network["softAPssid"] = hostname;
  network["softAPStationNum"] = WiFi.softAPgetStationNum();
  network["softAPIP"] = WiFi.softAPIP();
  network["softAPmacAddress"] = WiFi.softAPmacAddress();

  String output;
  serializeJsonPretty(doc, output);
  output += "\n";

  return output;
}
