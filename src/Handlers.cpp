#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "lumos-arduino/Colors.h"

#include "Animations.h"
#include "Demos.h"
#include "Handlers.h"
#include "Network.h"
#include "Status.h"
#include "utils.h"

void handleRoot() {
    auto startMS = millis();
    File file = LittleFS.open("/index.html", "r");
    auto openedMS = millis();
    size_t sent = server.streamFile(file, "text/html");
    auto streamedMS = millis();
    file.close();
    auto closedMS = millis();
    Logger::logf("handleRoot sentBytes=%d open=%dms stream=%dms close=%dms total=%dms\n",
      sent, openedMS - startMS, streamedMS - openedMS, closedMS - streamedMS, closedMS - startMS);
}

void handleCSS() {
    auto startMS = millis();
    File file = LittleFS.open("/crystal.css", "r");
    auto openedMS = millis();
    size_t sent = server.streamFile(file, "text/css");
    auto streamedMS = millis();
    file.close();
    auto closedMS = millis();
    Logger::logf("handleCSS sentBytes=%d open=%dms stream=%dms close=%dms total=%dms\n",
      sent, openedMS - startMS, streamedMS - openedMS, closedMS - streamedMS, closedMS - startMS);
}

void handleJS() {
    auto startMS = millis();
    File file = LittleFS.open("/crystal.js", "r");
    auto openedMS = millis();
    size_t sent = server.streamFile(file, "text/javascript");
    auto streamedMS = millis();
    file.close();
    auto closedMS = millis();
    Logger::logf("handleJS sentBytes=%d open=%dms stream=%dms close=%dms total=%dms\n",
      sent, openedMS - startMS, streamedMS - openedMS, closedMS - streamedMS, closedMS - startMS);
}

void handleStatus() {
  String output = getStatus();
  server.send(200, "application/json", output);
}

void handleGetBrightness() {
  // Create the response.
  JsonDocument doc;
  doc["value"] = getNetworkRenderer()->getBrightness();
  String output;
  serializeJsonPretty(doc, output);

  server.send(200, "application/json", output);
}

void handleSetBrightness() {
  if(!server.hasArg("value")) {
    server.send(400, "text/plain", "Value parameter missing\n");
    return;
  }

  String valueStr = server.arg("value");
  uint8_t value = strtol(valueStr.c_str(), nullptr, 10);

  getNetworkRenderer()->setBrightness(value);

  server.send(200, "text/plain");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri();
  message += "\nMethod: ";
  if (server.method() == HTTP_GET) {
    message += "GET";
  } else {
    message += "POST";
  }
  message += String("\nArguments: ") + String(server.args()) + "\n";
  for (int i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleCrystal() {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    Logger::logf("handleCrystal failed to parse JSON: %s\n", error.c_str());
    server.send(400, "text/plain", error.c_str());
    return;
  }

  Color upperColor = strtol(doc["upper"]["color"], nullptr, 16);
  Color middleColor = strtol(doc["middle"]["color"], nullptr, 16);
  Color lowerColor = strtol(doc["lower"]["color"], nullptr, 16);
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
  getNetworkRenderer()->setModel(model);

  server.send(200, "text/plain");
}

void handleFlame() {
  std::shared_ptr<Model> model = std::make_shared<Flame>();
  getNetworkRenderer()->setModel(model);
  server.send(200, "text/plain");
}

void handleRainbow() {
  JsonDocument doc;
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

  auto model = getNetworkRenderer()->getModel();
  if (strcmp(model->getName(), "rainbow-rotate") == 0) {
    auto rainbowModel = static_cast<Rotate*>(model.get());
    if (rainbowModel != nullptr) {
      rainbowModel->setSpeed(speed);
      rainbowModel->setModel(gm);
      server.send(200, "text/plain");
      return;
    }
  }

  std::shared_ptr<Model> rm = std::make_shared<Rotate>("rainbow-rotate", speed, gm);
  getNetworkRenderer()->setModel(rm);

  server.send(200, "text/plain");
}

void handleSolid() {
  if(!server.hasArg("color")) {
    server.send(400, "text/plain", "Color parameter missing\n");
    return;
  }

  String colorStr = server.arg("color");
  Color color = strtol(colorStr.c_str(), nullptr, 16);
  std::shared_ptr<Model> model = std::make_shared<SolidModel>("net solid model", color);
  getNetworkRenderer()->setModel(model);

  server.send(200, "text/plain");
}


void handleDemo1() {
  auto model = makeDemo1();
  getNetworkRenderer()->setModel(model);
  server.send(200, "text/plain");
}

void handleDemo2() {
  auto model = makeDemo2();
  getNetworkRenderer()->setModel(model);
  server.send(200, "text/plain");
}

void handleDemo3() {
  auto model = makeDemo3();
  getNetworkRenderer()->setModel(model);
  server.send(200, "text/plain");
}
