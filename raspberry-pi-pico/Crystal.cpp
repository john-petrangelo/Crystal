#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>

#include <lumos-arduino/Logger.h>

#include "Logging/ConsoleLogger.h"
#include "Network.h"
#include "Renderer.h"
#include "Utils.h"

int main() {
  stdio_init_all();
  Logger::set(new ConsoleLogger());

  // Initialize the renderer
  Renderer const *renderer = new RaspberryPiPico_Renderer(8);

  if (cyw43_arch_init()) {
    logger << "Wi-Fi init failed" << std::endl;
    return -1;
  }

  logger << "Connecting to network" << std::endl;
  Network::setup();

  while (true) {
    absolute_time_t start_time = get_absolute_time();
    uint32_t const msSinceBoot = to_ms_since_boot(start_time);
    logger << msToString(msSinceBoot) << " Blonk" << std::endl;

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(700);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(300);

    Network::loop();
  }
}
