#include <ArduinoJson.h>
#include <LittleFS.h>

#include "Filesystem.h"
#include "Network.h"
#include "Status.h"
#include "System.h"

String getStatus() {
  JsonDocument doc;

  System::getStatus(doc["system"].to<JsonObject>());
  Network::getRenderer()->getStatus(doc["renderer"].to<JsonObject>());
  Filesystem::getStatus(doc["filesystem"].to<JsonObject>());
  Network::getStatus(doc["network"].to<JsonObject>());

  String output;
  serializeJsonPretty(doc, output);
  output += "\n";

  return output;
}
