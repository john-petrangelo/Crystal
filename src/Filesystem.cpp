#include <ArduinoJson.h>
#include <LittleFS.h>

#include "Filesystem.h"
#include "lumos-arduino/Logger.h"

void Filesystem::setup() {
  if (!LittleFS.begin()) {
    Logger::logMsgLn("Failed to start LittleFS");
  } else {
    Logger::logMsgLn("Started LittleFS");
  }
}

void Filesystem::getStatus(JsonObject obj) {
  FSInfo fsInfo{};
  bool gotInfo = LittleFS.info(fsInfo);
  if (gotInfo) {
    obj["totalBytes"] = fsInfo.totalBytes;
    obj["usedBytes"] = fsInfo.usedBytes;
    obj["blockSize"] = fsInfo.blockSize;
    obj["pageSize"] = fsInfo.pageSize;
    obj["maxOpenFiles"] = fsInfo.maxOpenFiles;
    obj["maxPathLength"] = fsInfo.maxPathLength;
  }
}

void Filesystem::end() {
  LittleFS.end();
  Logger::logMsgLn("Ended LittleFS");
}
