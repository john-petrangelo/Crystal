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

// Configuration information about the NeoPixel strip we are using.
int const PIXELS_COUNT = 24;
Renderer *renderer = nullptr;

long const logDurationIntervalMS = 5000;

void setup() {
  Serial.begin(115200);
  Serial.println("Serial started");

  Serial.println("Creating renderer");
  renderer = new Esp8266_NeoPixelBus_Renderer(PIXELS_COUNT);

#ifdef ENABLE_GDB_STUB
  gdbstub_init();
#endif

  Filesystem::setup();
  setupNetwork(renderer);

  std::shared_ptr<Model> model = makeDarkCrystal();
  renderer->setModel(model);
}

void loop() {
  static auto lastUpdateMS = millis();

  auto beforeMS = millis();

  // Check for network activity.
  loopNetwork();
  auto afterNetworkMS = millis();

  renderer->render();
  auto afterRenderMS = millis();
  yield();
  auto afterYieldMS = millis();

  loopLogger();
  auto afterAllMS = millis();

  if (beforeMS - lastUpdateMS >= logDurationIntervalMS) {
    lastUpdateMS = beforeMS;
    Logger::logf("%0.3f Completed loop, total %dms(network %dms, render %dms, yield %dms, logger %dms), free heap %d bytes\n",
                 (float)afterAllMS / 1000.0,
                 afterAllMS - beforeMS,
                 afterNetworkMS - beforeMS,
                 afterRenderMS - afterNetworkMS,
                 afterYieldMS - afterRenderMS,
                 afterAllMS - afterYieldMS,
                 EspClass::getFreeHeap());

    Logger::logMsgLn(getStatus().c_str());
    WiFi.printDiag(Serial);
    Serial.println("");
  }
}
