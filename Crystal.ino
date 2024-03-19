#ifdef ENABLE_GDB_STUB
#include <GDBStub.h>
#endif

#include <Arduino.h>
#include <NeoPixelBus.h>

#include "src/lumos-arduino/Colors.h"
#include "src/lumos-arduino/Logger.h"

#include "src/Animations.h"
#include "src/Filesystem.h"
#include "src/Model.h"
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

  Serial.println("Creating renderer");
  renderer = new Esp8266_NeoPixelBus_Renderer(PIXELS_COUNT);

  Filesystem::setup();
  Network::setup(renderer);

  std::shared_ptr<Model> model = makeDarkCrystal();
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
