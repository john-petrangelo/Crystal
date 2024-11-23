#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>

#include <lumos-arduino/Logger.h>

#include "Logging/ConsoleLogger.h"
#include "Network.h"
#include "Renderer.h"
#include "Status.h"

#include "Models/Crystal.h"
#include "Models/Solid.h"

constexpr long logDurationIntervalMS = 60000;

int main() {
  stdio_init_all();
  Logger::set(new ConsoleLogger());

  if (cyw43_arch_init()) {
    logger << "Wi-Fi init failed" << std::endl;
    return -1;
  }

  logger << "Connecting to network" << std::endl;
  Network::setup();

  // Initialize the renderer
  Renderer *renderer = new RaspberryPiPico_Renderer(64);
  ModelPtr model = Crystal::make();
  renderer->setModel(model);
  renderer->render();
  auto lastUpdateMS = to_ms_since_boot(get_absolute_time());
  Network::setRenderer(renderer);

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
