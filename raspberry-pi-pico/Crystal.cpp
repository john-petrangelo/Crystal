#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/timer.h"

int main() {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed");
    return -1;
  }

  // Connect to an access point in station mode
  cyw43_arch_enable_sta_mode();


  while (true) {
    printf("Setting pin to 1\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(500);

    printf("Setting pin to 0\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(500);
  }
}
