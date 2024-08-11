#include <Arduino.h>
#include <NeoPixelBus.h>

#include <lumos-arduino/Logger.h>

#include "src/ArduinoStreamLogger.h"
#include "src/Filesystem.h"
#include "src/Models/Crystal.h"
#include "src/Network.h"
#include "src/Renderer.h"
#include "src/Status.h"
#include "src/System.h"

// Configuration information about the NeoPixel strip we are using.
int const PIXELS_COUNT = 24;
Renderer *renderer = nullptr;

long const logDurationIntervalMS = 10000;

void setup() {
  System::setup();
  Logger::set(new ArduinoStreamLogger(&Serial));

  Serial.println("Creating renderer");
  renderer = new Esp8266_NeoPixelBus_Renderer(PIXELS_COUNT);

  Filesystem::setup();
  Network::setup(renderer);

  ModelPtr model = std::make_shared<Crystal>();
  renderer->setModel(model);
}

void loop() {
  static auto lastUpdateMS = millis();

  auto now_ms = millis();

  Network::loop();
  renderer->render();

  yield();

  if (now_ms - lastUpdateMS >= logDurationIntervalMS) {
    lastUpdateMS = now_ms;
    Logger::logMsgLn(getStatus().c_str());
  }
}
