#include <LittleFS.h>

#include "Filesystem.h"
#include "lumos-arduino/Logger.h"

void Filesystem::setup() {
  if (!LittleFS.begin()) {
    logger << "Failed to start LittleFS" << std::endl;
  } else {
    logger << "Started LittleFS" << std::endl;
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

  JsonArray files = obj["files"].to<JsonArray>();
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    JsonObject file = files.add<JsonObject>();
    file["name"] = dir.fileName();
    file["size"] = dir.fileSize();
    if (dir.isDirectory()) {
      file["type"] = "directory";
    } else if (dir.isFile()) {
      file["type"] = "file";
    } else {
      file["type"] = "unknown";
    }
  }
}

void Filesystem::end() {
  LittleFS.end();
  logger << "Ended LittleFS" << std::endl;
}
