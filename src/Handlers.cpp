#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "lumos-arduino/Colors.h"
#include "lumos-arduino/Logger.h"

#include "Demos.h"
#include "Handlers.h"
#include "Network.h"
#include "Status.h"
#include "utils.h"
#include "Models/Crystal.h"
#include "Models/Flame.h"
#include "Models/Gradient.h"
#include "Models/Rotate.h"
#include "Models/Solid.h"
#include "Models/WarpCore.h"

template<typename T>
static T getJsonValue(JsonVariant obj, const char* paramName, T defaultValue) {
  auto value = obj[paramName];
  if (value.isNull() || !value.is<T>()) {
    return defaultValue;
  }

  return value.as<T>();
}

static Color getJsonColor(JsonVariant obj, const char* paramName, Color defaultValue = BLACK){
  auto value = obj[paramName];
  if (value.isNull() || !value.is<const char *>()) {
    return defaultValue;
  }

  auto colorStr = value.as<char const *>();
  return strtol(colorStr, nullptr, 16);
}

static bool getArgAsLong(const char* paramName, long& paramValue) {
  if (!Network::getServer().hasArg(paramName)) {
    Network::getServer().send(400, "text/plain", String(paramName) + " parameter missing\n");
    return false; // Error occurred
  }
  String paramStr = Network::getServer().arg(paramName);
  paramValue = strtol(paramStr.c_str(), nullptr, 10);
  Logger::logf("getArgAsLong str=%s value=%ld\n", paramStr.c_str(), paramValue);
  return true; // No error
}

static bool getArgAsColor(const char* paramName, Color& paramValue) {
  if (!Network::getServer().hasArg(paramName)) {
    Network::getServer().send(400, "text/plain", String(paramName) + " parameter missing\n");
    return false; // Error occurred
  }
  String paramStr = Network::getServer().arg(paramName);
  paramValue = strtol(paramStr.c_str(), nullptr, 16);
  return true; // No error
}

static bool getArgAsFloat(const char* paramName, float& paramValue) {
  if (!Network::getServer().hasArg(paramName)) {
    Network::getServer().send(400, "text/plain", String(paramName) + " parameter missing\n");
    return false; // Error occurred
  }
  String paramStr = Network::getServer().arg(paramName);
  paramValue = strtof(paramStr.c_str(), nullptr);
  return true; // No error
}

void handleRoot() {
    auto startMS = millis();
    File file = LittleFS.open("/index.html", "r");
    auto openedMS = millis();
    size_t sent = Network::getServer().streamFile(file, "text/html");
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
    size_t sent = Network::getServer().streamFile(file, "text/css");
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
    size_t sent = Network::getServer().streamFile(file, "text/javascript");
    auto streamedMS = millis();
    file.close();
    auto closedMS = millis();
    Logger::logf("handleJS sentBytes=%d open=%dms stream=%dms close=%dms total=%dms\n",
      sent, openedMS - startMS, streamedMS - openedMS, closedMS - streamedMS, closedMS - startMS);
}

void handleStatus() {
  Network::getServer().send(200, "application/json", getStatus());
}

void handleGetBrightness() {
  // Create the response.
  JsonDocument doc;
  doc["value"] = Network::getRenderer()->getBrightness();
  String output;
  serializeJsonPretty(doc, output);

  Network::getServer().send(200, "application/json", output);
}

void handleSetBrightness() {
  long value;
  Logger::logf("handleSetBrightness\n");
  if (!getArgAsLong("value", value)) return;
  Logger::logf("handleSetBrightness value=%ld\n", value);
  if (value < 0 || value > 255) {
    Network::getServer().send(400, "text/plain", "invalid value, expected 0-255\n");
    return;
  }

  // Success
  Network::getRenderer()->setBrightness(value);
  Network::getServer().send(200, "text/plain");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + Network::getServer().uri();
  message += "\nMethod: ";
  if (Network::getServer().method() == HTTP_GET) {
    message += "GET";
  } else {
    message += "POST";
  }
  message += String("\nArguments: ") + String(Network::getServer().args()) + "\n";
  for (int i = 0; i < Network::getServer().args(); i++) {
    message += " " + Network::getServer().argName(i) + ": " + Network::getServer().arg(i) + "\n";
  }
  Network::getServer().send(404, "text/plain", message);
}

void handleCrystal() {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, Network::getServer().arg("plain"));
  if (error) {
    Logger::logf("handleCrystal failed to parse JSON: %s\n", error.c_str());
    Network::getServer().send(400, "text/plain", error.c_str());
    return;
  }

  Color upperColor = getJsonColor(doc, "upper_color", 0xff00d0);
  Color middleColor = getJsonColor(doc, "middle_color", 0xff00d0);
  Color lowerColor = getJsonColor(doc, "lower_color", 0xff00d0);
  float upperSpeed = getJsonValue(doc, "upper_speed", 0.5f);
  float middleSpeed = getJsonValue(doc, "middle_speed", 0.2f);
  float lowerSpeed = getJsonValue(doc, "lower_speed", 0.3f);

  float upperPeriodSec = fmap(upperSpeed, 0.0, 1.0, 11.0, 1.0);
  float middlePeriodSec = fmap(middleSpeed, 0.0, 1.0, 11.0, 1.0);
  float lowerPeriodSec = fmap(lowerSpeed, 0.0, 1.0, 11.0, 1.0);

  ModelPtr model = std::make_shared<Crystal>(
    upperColor, upperPeriodSec,
    middleColor, middlePeriodSec,
    lowerColor, lowerPeriodSec);
  Network::getRenderer()->setModel(model);

  Network::getServer().send(200, "text/plain");
}

void handleFlame() {
  ModelPtr model = std::make_shared<Flame>();
  Network::getRenderer()->setModel(model);
  Network::getServer().send(200, "text/plain");
}

void handleRainbow() {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, Network::getServer().arg("plain"));
  if (error) {
    Logger::logf("handleRainbow failed to parse JSON: %s\n", error.c_str());
    Network::getServer().send(400, "text/plain", error.c_str());
    return;
  }

  String mode = doc["mode"];
  float speed = doc["speed"];

  ModelPtr gm = nullptr;
  if (mode == "tv") {
    gm = Gradient::make(0x0000BF, 0xBF0000, 0xBF00C0, 0x00BF00, 0x00BFBE, 0xBFBF00, 0xBFBFBF, 0x666666);
  } else if (mode == "50s") {
    gm = Gradient::make(0xB26463, 0x79C874, 0x8AC5BD, 0x9A942E, 0x937535, 0xB26463);
  } else if (mode == "60s") {
    gm = Gradient::make(0xFCD53A, 0xBFC847, 0xF7790B, 0xDC4E80, 0x7BD0EC, 0xFCD53A);
  } else if (mode == "70s") {
    gm = Gradient::make(0xE59F23, 0x925E18, 0x758C32, 0xCE985A, 0xE15623, 0xE59F23);
  } else if (mode == "80s") {
    gm = Gradient::make(0xFE167A, 0x003BB1, 0x00C8AB, 0xFFC800, 0x8000E2, 0xFE167A);
  } else if (mode == "90s") {
    gm = Gradient::make(0xB03A50, 0x7F5F14, 0x065B44, 0x4B183C, 0x6B0A29, 0xB03A50);
  } else if (mode == "00s") {
    gm = Gradient::make(0x79AED6, 0x8F8C84, 0xC4A051, 0x69322C, 0x3D4D4C, 0x79AED6);
  } else if (mode == "10s") {
    gm = Gradient::make(0xF0A2A3, 0x62656D, 0x8E6E29, 0x725A47, 0x4F7018, 0xF0A2A3);
  } else {
    // We want to get here for "classic". We'll also return a "classic" rainbow for any unexpected mode values.
    gm = Gradient::make(RED, VIOLET, INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED);
  }

  auto model = Network::getRenderer()->getModel();
  if (strcmp(model->getName(), "RotatingRainbow") == 0) {
    // Just update the current rainbow model
    auto rainbowModel = static_cast<Rotate*>(model.get());
    rainbowModel->setSpeed(speed);
    rainbowModel->setModel(gm);
    Network::getServer().send(200, "text/plain");
  } else {
    // Create a new model
    ModelPtr rm = std::make_shared<Rotate>("RotatingRainbow", speed, gm);
    Network::getRenderer()->setModel(rm);

    Network::getServer().send(200, "text/plain");
  }
}

void handleWarpCore() {
  float frequency;
  float size;
  float dutyCycle;

  if (!getArgAsFloat("frequency", frequency)) return;
  if (!getArgAsFloat("size", size)) return;
  if (!getArgAsFloat("dutyCycle", dutyCycle)) return;

  ModelPtr model = WarpCore::make(size, frequency, dutyCycle);
  Network::getRenderer()->setModel(model);
  Network::getServer().send(200, "text/plain");
}

void handleSolid() {
  Color color;
  if (!getArgAsColor("color", color)) return;

  ModelPtr model = std::make_shared<Solid>(color);
  Network::getRenderer()->setModel(model);

  Network::getServer().send(200, "text/plain");
}

void handleDemo1() {
  auto model = makeDemo1();
  Network::getRenderer()->setModel(model);
  Network::getServer().send(200, "text/plain");
}

void handleDemo2() {
  auto model = makeDemo2();
  Network::getRenderer()->setModel(model);
  Network::getServer().send(200, "text/plain");
}

void handleDemo3() {
  auto model = makeDemo3();
  Network::getRenderer()->setModel(model);
  Network::getServer().send(200, "text/plain");
}
