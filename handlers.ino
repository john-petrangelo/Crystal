#include <memory>

#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#include "src/lumos-arduino/Colors.h"
#include "src/lumos-arduino/Patterns.h"

#include "src/Animations.h"
#include "src/Combinations.h"
#include "src/Demos.h"
#include "src/Renderer.h"
#include "src/utils.h"

void handleRoot() {
    long startMS = millis();
    File file = LittleFS.open("/index.html", "r");
    long openedMS = millis();
    size_t sent = server.streamFile(file, "text/html");
    long streamedMS = millis();
    file.close();
    long closedMS = millis();
    Logger::logf("handleRoot sentBytes=%d open=%dms stream=%dms close=%dms total=%dms\n",
      sent, openedMS - startMS, streamedMS - openedMS, closedMS - streamedMS, closedMS - startMS);
}

void handleCSS() {
    long startMS = millis();
    File file = LittleFS.open("/crystal.css", "r");
    long openedMS = millis();
    size_t sent = server.streamFile(file, "text/css");
    long streamedMS = millis();
    file.close();
    long closedMS = millis();
    Logger::logf("handleCSS sentBytes=%d open=%dms stream=%dms close=%dms total=%dms\n",
      sent, openedMS - startMS, streamedMS - openedMS, closedMS - streamedMS, closedMS - startMS);
}

void handleJS() {
    long startMS = millis();
    File file = LittleFS.open("/crystal.js", "r");
    long openedMS = millis();
    size_t sent = server.streamFile(file, "text/javascript");
    long streamedMS = millis();
    file.close();
    long closedMS = millis();
    Logger::logf("handleJS sentBytes=%d open=%dms stream=%dms close=%dms total=%dms\n",
      sent, openedMS - startMS, streamedMS - openedMS, closedMS - streamedMS, closedMS - startMS);
}

void handleStatus() {
  // StaticJsonDocument<512> doc;
  // doc["time"] = millis() / 1000.0;

  // JsonObject memory = doc.createNestedObject("memory");
  // memory["freeHeapBytes"] = ESP.getFreeHeap();

  // JsonObject fs = doc.createNestedObject("filesystem");
  // FSInfo fsInfo;
  // bool gotInfo = LittleFS.info(fsInfo);
  // if (gotInfo) {
  //   fs["totalBytes"] = fsInfo.totalBytes;
  //   fs["usedBytes"] = fsInfo.usedBytes;
  //   fs["blockSize"] = fsInfo.blockSize;
  //   fs["pageSize"] = fsInfo.pageSize;
  //   fs["maxOpenFiles"] = fsInfo.maxOpenFiles;
  //   fs["maxPathLength"] = fsInfo.maxPathLength;
  // }

  // JsonObject network = doc.createNestedObject("network");
  // network["hostname"] = hostname + ".local";
  // network["wifiMACAddress"] = WiFi.macAddress();
  // network["ipAddress"] = WiFi.localIP().toString();
  // network["mode"] = "unknown";
  // switch(WiFi.getMode()) {
  //   case WIFI_OFF:
  //     network["mode"] = "WIFI_OFF";
  //     break;
  //   case WIFI_STA:
  //     network["mode"] = "WIFI_STA";
  //     break;
  //   case WIFI_AP:
  //     network["mode"] = "WIFI_AP";
  //     break;
  //   case WIFI_AP_STA:
  //     network["mode"] = "WIFI_AP_STA";
  //     break;
  //   default:
  //     network["mode"] = String(WiFi.getMode());
  //     break;
  // }
  // network["phyMode"] = "unknown";
  // switch(WiFi.getPhyMode()) {
  //   case WIFI_PHY_MODE_11B:
  //     network["phyMode"] = "WIFI_PHY_MODE_11B";
  //     break;
  //   case WIFI_PHY_MODE_11G:
  //     network["phyMode"] = "WIFI_PHY_MODE_11G";
  //     break;
  //   case WIFI_PHY_MODE_11N:
  //     network["phyMode"] = "WIFI_PHY_MODE_11N";
  //     break;
  //   default:
  //     network["phyMode"] = String(WiFi.getPhyMode());
  //     break;
  // }
  // network["softAPssid"] = hostname;
  // network["softAPStationNum"] = WiFi.softAPgetStationNum();
  // network["softAPIP"] = WiFi.softAPIP();
  // network["softAPmacAddress"] = WiFi.softAPmacAddress();

  // String output;
  // serializeJsonPretty(doc, output);
  String output = getStatus();

  server.send(200, "text/json", output);
  Logger::logf("handleStatus %s", output.c_str());
}

String getStatus() {
  StaticJsonDocument<512> doc;
  doc["time"] = millis() / 1000.0;

  JsonObject memory = doc.createNestedObject("memory");
  memory["freeHeapBytes"] = ESP.getFreeHeap();

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

  return output;
}

void handleGetBrightness() {
//  uint8_t value = strip.getBrightness();
  uint8_t value = 255;

  // Un-apply gamma correction
  uint8_t uncorrectedValue = sqrt(value * 255);

  // Create the response.
  StaticJsonDocument<200> doc;
  doc["value"] = uncorrectedValue;
  String output;
  serializeJsonPretty(doc, output);

  server.send(200, "text/json", output);
}

void handleSetBrightness() {
  if(!server.hasArg("value")) {
    server.send(400, "text/plain", "Value parameter missing\n");
    return;
  }

  String valueStr = server.arg("value");
  uint8_t value = strtol(valueStr.c_str(), 0, 10);

  // Apply gamma correction
  uint8_t correctedValue = value * value / 255;

//  strip.setBrightness(correctedValue);

  server.send(200, "text/plain", "");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri();
  message += "\nMethod: "+ (server.method() == HTTP_GET) ? "GET" : "POST";
  message += String("\nArguments: ") + server.args() + "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleCrystal() {
  StaticJsonDocument<384> doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    Logger::logf("handleCrystal failed to parse JSON: %s\n", error.c_str());
    server.send(400, "text/plain", error.c_str());
    return;
  }

  Color upperColor = strtol(doc["upper"]["color"], 0, 16);
  Color middleColor = strtol(doc["middle"]["color"], 0, 16);
  Color lowerColor = strtol(doc["lower"]["color"], 0, 16);
  float upperSpeed = doc["upper"]["speed"];
  float middleSpeed = doc["middle"]["speed"];
  float lowerSpeed = doc["lower"]["speed"];

  float upperPeriodSec = fmap(upperSpeed, 0.0, 1.0, 11.0, 1.0);
  float middlePeriodSec = fmap(middleSpeed, 0.0, 1.0, 11.0, 1.0);
  float lowerPeriodSec = fmap(lowerSpeed, 0.0, 1.0, 11.0, 1.0);

  auto model = makeCrystal(
    upperColor, upperPeriodSec,
    middleColor, middlePeriodSec,
    lowerColor, lowerPeriodSec);
  renderer->setModel(model);

  server.send(200, "text/plain", "");
}

void handleFlame() {
  std::shared_ptr<Model> model = std::make_shared<Flame>();
  renderer->setModel(model);
  server.send(200, "text/plain", "");
}

void handleRainbow() {
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    Logger::logf("handleRainbow failed to parse JSON: %s\n", error.c_str());
    server.send(400, "text/plain", error.c_str());
    return;
  }

  String mode = doc["mode"];
  float speed = doc["speed"];

  std::shared_ptr<Model> gm = nullptr;
  if (mode == "classic") {
    gm = std::make_shared<MultiGradientModel>("vivid-gradient",
      8, RED, VIOLET, INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED);
  } else if (mode == "tv") {
    gm = std::make_shared<MultiGradientModel>("tv-gradient",
      8, 0x0000BF, 0xBF0000, 0xBF00C0, 0x00BF00, 0x00BFBE, 0xBFBF00, 0xBFBFBF, 0x666666);
  } else if (mode == "50s") {
    gm = std::make_shared<MultiGradientModel>("50s-gradient",
      6, 0xB26463, 0x79C874, 0x8AC5BD, 0x9A942E, 0x937535, 0xB26463);
  } else if (mode == "60s") {
    gm = std::make_shared<MultiGradientModel>("60s-gradient",
      6, 0xFCD53A, 0xBFC847, 0xF7790B, 0xDC4E80, 0x7BD0EC, 0xFCD53A);
  } else if (mode == "70s") {
    gm = std::make_shared<MultiGradientModel>("70s-gradient",
      6, 0xE59F23, 0x925E18, 0x758C32, 0xCE985A, 0xE15623, 0xE59F23);
  } else if (mode == "80s") {
    gm = std::make_shared<MultiGradientModel>("80s-gradient",
      6, 0xFE167A, 0x003BB1, 0x00C8AB, 0xFFC800, 0x8000E2, 0xFE167A);
  } else if (mode == "90s") {
    gm = std::make_shared<MultiGradientModel>("90s-gradient",
      6, 0xB03A50, 0x7F5F14, 0x065B44, 0x4B183C, 0x6B0A29, 0xB03A50);
  } else if (mode == "00s") {
    gm = std::make_shared<MultiGradientModel>("00s-gradient",
      6, 0x79AED6, 0x8F8C84, 0xC4A051, 0x69322C, 0x3D4D4C, 0x79AED6);
  } else if (mode == "10s") {
    gm = std::make_shared<MultiGradientModel>("10s-gradient",
      6, 0xF0A2A3, 0x62656D, 0x8E6E29, 0x725A47, 0x4F7018, 0xF0A2A3);
  } else {
    // We shouldn't get here, but assume "classic" as a default in case we do.
    gm = std::make_shared<MultiGradientModel>("rainbow-gradient",
      8, RED, VIOLET, INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED);
  }

  auto model = renderer->getModel();
  if (model->getName() == "rainbow-rotate") {
    auto rainbowModel = static_cast<Rotate*>(model.get());
    if (rainbowModel != NULL) {
      rainbowModel->setSpeed(speed);
      rainbowModel->setModel(gm);
      server.send(200, "text/plain", "");
      return;
    }
  }

  std::shared_ptr<Model> rm = std::make_shared<Rotate>("rainbow-rotate", speed, gm);
  renderer->setModel(rm);

  server.send(200, "text/plain", "");
}

void handleSolid() {
  if(!server.hasArg("color")) {
    server.send(400, "text/plain", "Color parameter missing\n");
    return;
  }

  String colorStr = server.arg("color");
  Color color = strtol(colorStr.c_str(), 0, 16);
  std::shared_ptr<Model> model = std::make_shared<SolidModel>("net solid model", color);
  renderer->setModel(model);

  server.send(200, "text/plain", "");
}


void handleDemo1() {
  auto model = makeDemo1();
  renderer->setModel(model);
  server.send(200, "text/plain", "");
}

void handleDemo2() {
  auto model = makeDemo2();
  renderer->setModel(model);
  server.send(200, "text/plain", "");
}

void handleDemo3() {
  auto model = makeDemo3();
  renderer->setModel(model);
  server.send(200, "text/plain", "");
}
