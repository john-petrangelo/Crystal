#include <ESP8266WebServer.h>

#ifdef ENABLE_GDB_STUB
#include <GDBStub.h>
#endif

#include <LittleFS.h>

//#include <Adafruit_NeoPixel.h>
#include <NeoPixelBus.h>

#include "src/lumos-arduino/Colors.h"
#include "src/lumos-arduino/Logger.h"

#include "src/Animations.h"
#include "src/Model.h"
#include "src/Renderer.h"

// Configuration information about the NeoPixel strip we are using.
int const PIXELS_COUNT = 24;
int8_t const PIXELS_PIN = 2;
Esp8266_NeoPixelBus_Renderer *renderer = nullptr;

String hostname = "crystal";
ESP8266WebServer server(80);

Color pixels[PIXELS_COUNT];

long const logDurationIntervalMS = 5000;

void setup() {
  Serial.begin(115200);
  Serial.println("Serial started");

#ifdef ENABLE_GDB_STUB
  gdbstub_init();
#endif

  if (!LittleFS.begin()) {
    Serial.println("Failed to start LittleFS");
  }
  Serial.println("Started LittleFS");

  setupNetwork();
  Serial.println("Network setup");

  Serial.println("Creating renderer");
  renderer = new Esp8266_NeoPixelBus_Renderer(PIXELS_COUNT);

  std::shared_ptr<Model> model = makeDarkCrystal();
  renderer->setModel(model);
}

void loop() {
  static int lastUpdateMS = millis();

  long beforeMS = millis();

  // Check for network activity.
  loopNetwork();
  long afterNetworkMS = millis();

  // Update and render the LEDs
  renderer->render();
  long afterRenderMS = millis();
  yield();
  long afterYieldMS = millis();

  loopLogger();
  long afterAllMS = millis();

  if (beforeMS - lastUpdateMS >= logDurationIntervalMS) {
    lastUpdateMS = beforeMS;
    Logger::logf("%0.3f Completed loop, total %dms(network %dms, render %dms, yield %dms, logger %dms), free heap %d bytes\n",
                 afterAllMS / 1000.0,
                 afterAllMS - beforeMS,
                 afterNetworkMS - beforeMS,
                 afterRenderMS - afterNetworkMS,
                 afterYieldMS - afterRenderMS,
                 afterAllMS - afterYieldMS,
                 ESP.getFreeHeap());

    Logger::logMsg(getStatus().c_str());
    WiFi.printDiag(Serial);
    Serial.println("");
  }
}
