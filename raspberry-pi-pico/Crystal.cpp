#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "Network.h"
#include "Utils.h"

int main() {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed");
    return -1;
  }

  printf("Connecting to network...\n");
  Network::setup();

  while (true) {
    absolute_time_t start_time = get_absolute_time();
    uint32_t msSinceBoot = to_ms_since_boot(start_time);
    printf("%s Blink\n", msToString(msSinceBoot).c_str());

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(700);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(300);

    Network::loop();
  }
}
