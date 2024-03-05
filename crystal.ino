#include <ESP8266WebServer.h>

#ifdef ENABLE_GDB_STUB
#include <GDBStub.h>
#endif

#include <LittleFS.h>

//#include <Adafruit_NeoPixel.h>
#include <NeoPixelBus.h>

#include "src/lumos-arduino/lumos-arduino/Colors.h"
#include "src/lumos-arduino/lumos-arduino/Logger.h"

#include "src/Animations.h"
#include "src/Model.h"
#include "src/Renderer.h"

// Configuration information about the NeoPixel strip we are using.
int const PIXELS_COUNT = 24;
int8_t const PIXELS_PIN = 2;
//neoPixelType const STRIP_FLAGS = NEO_GRB + NEO_KHZ800;
// Adafruit_NeoPixel strip(PIXELS_COUNT, PIXELS_PIN, STRIP_FLAGS);
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod> strip(PIXELS_COUNT, PIXELS_PIN);

String hostname = "crystal";
ESP8266WebServer server(80);


Color pixels[PIXELS_COUNT];

long const logDurationIntervalMS = 10000;
Renderer renderer;

void setup() {
  Serial.begin(115200);
  Serial.println("");

#ifdef ENABLE_GDB_STUB
  gdbstub_init();
#endif

  if (!LittleFS.begin()) {
    Serial.println("Failed to start LittleFS");
  }

  setupNetwork();

  strip.Begin();
//  strip.setBrightness(255);
  strip.Show();  // Initialize all pixels to 'off'

  renderer.setModel(makeDarkCrystal());
}

void loop() {
  static int lastUpdateMS = millis();

  long beforeMS = millis();

  // Check for network activity.
  loopNetwork();
  long afterNetworkMS = millis();

  // Animate the LEDs.
  renderer.loop(strip.PixelCount(), [&strip](int i, Color color) {
    RgbColor newColor(Colors::getRed(color), Colors::getGreen(color), Colors::getBlue(color));
    strip.SetPixelColor(i, newColor);
  });
//  renderer.loop(strip.PixelCount(), [&strip](int i, Color color) { strip.SetPixelColor(i, color); });
  long afterRenderMS = millis();
  yield();
  long afterYieldMS = millis();
  strip.Show();
  long afterShowMS = millis();

  loopLogger();
  long afterAllMS = millis();

  if (beforeMS - lastUpdateMS >= logDurationIntervalMS) {
    lastUpdateMS = beforeMS;
    Logger::logf("%0.3f Completed loop, total %dms(network %dms, render %dms, yield %dms, show %dms, logger %dms), free heap %d bytes\n",
                 afterAllMS / 1000.0,
                 afterAllMS - beforeMS,
                 afterNetworkMS - beforeMS,
                 afterRenderMS - afterNetworkMS,
                 afterYieldMS - afterRenderMS,
                 afterShowMS - afterYieldMS,
                 afterAllMS - afterShowMS,
                 ESP.getFreeHeap());

    Logger::logMsg(getStatus().c_str());
    WiFi.printDiag(Serial);
    Serial.println("");
  }
}
