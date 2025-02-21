#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>

#include <lumos-arduino/Logger.h>

#include "Logging/ConsoleLogger.h"
#include "Networking/Network.h"
#include "Renderer.h"
#include "Status.h"

#include <lumos-arduino/Models/Crystal.h>

constexpr long logDurationIntervalMS = 60000;

#define BUTTON_PIN 10  // GPIO pin for the button
#define DEBOUNCE_TIME_US 500 * 1000  // debounce time in microseconds

volatile bool button_pressed = false;  // Flag for button press
volatile uint32_t last_press_time_us = 0;  // Last time the button was pressed
int constexpr IS_PRESSED = 0;

/**
 * @brief Interrupt callback function for handling button presses.
 *
 * This function is triggered on an interrupt when a change occurs on the specified GPIO pin.
 * It ensures that the interrupt is processed only for the correct button pin, verifies
 * that the button is actually pressed (to filter out noise), and applies a debounce mechanism
 * to prevent false multiple detections caused by mechanical switch bouncing.
 *
 * @param gpio The GPIO pin that triggered the interrupt.
 * @param events The type of event that triggered the interrupt (e.g., edge fall/rise).
 */
void button_callback(uint const gpio, uint32_t events) {
  // Filter out other GPIO pins
  if (gpio != BUTTON_PIN) {
    return;
  }

  // Filter out "released" events
  if (gpio_get(BUTTON_PIN) != IS_PRESSED) {
    return;
  }

  // Debounce
  uint32_t const now = time_us_32();
  if (now - last_press_time_us < DEBOUNCE_TIME_US) {
    // Too soon, ignore it
    return;
  }

  // Record the time of the press and set the flag
  last_press_time_us = now;
  button_pressed = true;
}

int main() {
  stdio_init_all();
  Logger::set(new ConsoleLogger());

  if (cyw43_arch_init()) {
    logger << "Wi-Fi init failed" << std::endl;
    return -1;
  }

  logger << "Configuring status button" << std::endl;
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN);
  gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);

  logger << "Connecting to network" << std::endl;
  Network::setup();

  // Initialize the renderer
  Renderer *renderer = new RaspberryPiPico_Renderer(64);
  ModelPtr const model = Crystal::make();
  renderer->setModel(model);
  renderer->render();
  auto lastUpdateMS = to_ms_since_boot(get_absolute_time());
  Network::setRenderer(renderer);

  while (true) {
    auto const now_ms = to_ms_since_boot(get_absolute_time());
    renderer->render();

    Network::loop();

    if (button_pressed) {
      button_pressed = false;
      logger << "Status button was pressed" << std::endl;
      logger << getStatus() << std::endl << std::endl;
    }

    if (now_ms - lastUpdateMS >= logDurationIntervalMS) {
      lastUpdateMS = now_ms;
      logger << getStatus() << std::endl << std::endl;
    }
  }
}
