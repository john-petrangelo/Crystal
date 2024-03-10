#include <memory>
#include <string>

#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

//#include "src/utils.h"

String getStatus() {
  StaticJsonDocument<512> doc;
  auto now = millis();
  auto days = now / (24*60*60*1000);
  now %= 24*60*60*1000;
  auto hours = now / (1000*60*60);
  now %= 1000*60*60;
  auto mins = now / (1000*60);
  now %= 1000*60;
  auto secs = now / 1000;
  auto millisecs = now % 1000;
  std::string xtime = std::to_string(days) + "d " +
                      std::to_string(hours) + "h " +
                      std::to_string(mins) + "m " +
                      std::to_string(secs) + "." + std::to_string(millisecs) + "s";
  doc["time"] = xtime;

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

  JsonObject fs = doc.createNestedObject("filesystem");
  FSInfo fsInfo;
  bool gotInfo = LittleFS.info(fsInfo);
  if (gotInfo) {
    fs["totalBytes"] = fsInfo.totalBytes;
    fs["usedBytes"] = fsInfo.usedBytes;
    fs["blockSize"] = fsInfo.blockSize;
    fs["pageSize"] = fsInfo.pageSize;
    fs["maxOpenFiles"] = fsInfo.maxOpenFiles;
    fs["maxPathLength"] = fsInfo.maxPathLength;
  }

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
