#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>

#include <lumos-arduino/Logger.h>

#include "Logging/ConsoleLogger.h"
#include "Network.h"
#include "Renderer.h"
#include "Status.h"
#include "WS2812Strip.h"

#include "Models/Crystal.h"
#include "Models/Gradient.h"
#include "Models/Map.h"
#include "Models/Rotate.h"
#include "Models/Solid.h"
#include "Models/Sum.h"

constexpr long logDurationIntervalMS = 10000;

int main() {
  stdio_init_all();
  Logger::set(new ConsoleLogger());

  // Initialize the renderer
  Renderer *renderer = new RaspberryPiPico_Renderer(64);
  renderer->render();

  ModelPtr m1 = Gradient::make(RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET, RED);
  ModelPtr model = std::make_shared<Rotate>("RotatingRainbow", 0.3, m1);

  renderer->setModel(model);

  if (cyw43_arch_init()) {
    logger << "Wi-Fi init failed" << std::endl;
    return -1;
  }

  logger << "Connecting to network" << std::endl;
  Network::setup();
  Network::setRenderer(renderer);

  auto strip = WS2812Strip(22, 64);

  auto lastUpdateMS = to_ms_since_boot(get_absolute_time());

  while (true) {
    auto const now_ms = to_ms_since_boot(get_absolute_time());
    renderer->render();
    Network::loop();

    if (now_ms - lastUpdateMS >= logDurationIntervalMS) {
      lastUpdateMS = now_ms;
      Logger::log(getStatus().c_str());
      Logger::log("\n");
    }
  }
}
