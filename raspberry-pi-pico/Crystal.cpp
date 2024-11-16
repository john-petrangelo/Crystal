#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>

#include <lumos-arduino/Logger.h>

#include "Logging/ConsoleLogger.h"
#include "Network.h"
#include "Renderer.h"
#include "WS2812Strip.h"
#include "Utils.h"

int main() {
  stdio_init_all();
  Logger::set(new ConsoleLogger());

  // Initialize the renderer
  // Renderer const *renderer = new RaspberryPiPico_Renderer(8);

  if (cyw43_arch_init()) {
    logger << "Wi-Fi init failed" << std::endl;
    return -1;
  }

  logger << "Connecting to network" << std::endl;
  Network::setup();

  auto strip = WS2812Strip(22, 64);

  while (true) {
    absolute_time_t start_time = get_absolute_time();
    uint32_t const msSinceBoot = to_ms_since_boot(start_time);
    logger << msToString(msSinceBoot) << " Blonk" << std::endl;

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    for (int i = 0; i < 8; i++) {
      strip.setPixel(0 + 8*i, BLUE);
      strip.setPixel(1 + 8*i, GREEN);
      strip.setPixel(2 + 8*i, CYAN);
      strip.setPixel(3 + 8*i, RED);
      strip.setPixel(4 + 8*i, PURPLE);
      strip.setPixel(5 + 8*i, YELLOW);
      strip.setPixel(6 + 8*i, WHITE);
      strip.setPixel(7 + 8*i, ORANGE);
    }

    strip.show();
    sleep_ms(700);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    for (int i = 0; i < 64; i++) {
      strip.setPixel(i, BLACK);
    }
    strip.show();
    sleep_ms(300);

    Network::loop();
  }
}
