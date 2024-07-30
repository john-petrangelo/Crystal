#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/timer.h"

#include "../secrets.h"
#include "HttpServer.h"


int main() {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed");
    return -1;
  }

  // Connect to an access point in station mode
  cyw43_arch_enable_sta_mode();

  printf("Connecting to WiFi...\n");
  int connect_error = cyw43_arch_wifi_connect_timeout_ms(
          SECRET_SSID, SECRET_PASSWORD,
          CYW43_AUTH_WPA2_AES_PSK, 30000);
  if (connect_error) {
    printf("failed to connect: error=%d\n", connect_error);
    return 1;
  } else {
    printf("Connected.\n");

    extern cyw43_t cyw43_state;
    auto ip_addr = cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;
    printf("IP Address: %lu.%lu.%lu.%lu\n", ip_addr & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr >> 16) & 0xFF, ip_addr >> 24);
  }

  printf("Starting HTTP server\n");
  HTTPServer server;
  server.init();

  while (true) {
    printf("Setting pin to 1\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(500);

    printf("Setting pin to 0\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(500);

    // Call poll to give the network a change run each iteration
    cyw43_arch_poll();
  }

}
