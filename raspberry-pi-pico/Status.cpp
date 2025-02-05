#include <ArduinoJson.h>

#include "Networking/Network.h"
#include "Status.h"
#include "System.h"

std::string getStatus() {
  JsonDocument doc;

  System::getStatus(doc["system"].to<JsonObject>());
  Network::getRenderer()->getStatus(doc["renderer"].to<JsonObject>());
  Network::getStatus(doc["network"].to<JsonObject>());

  std::string output;
  serializeJsonPretty(doc, output);
  output += "\n";

  return output;
}
