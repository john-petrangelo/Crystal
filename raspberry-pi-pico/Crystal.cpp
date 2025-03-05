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
#define DEBOUNCE_TIME_US 100 * 1000  // debounce time in microseconds

volatile bool button_event_pending = false;  // Flag for button press
volatile uint32_t last_press_time_us = 0;  // Last time the button was pressed
int constexpr IS_PRESSED = 0;
bool button_held = false;

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

  button_event_pending = true;
}

/**
 * @brief Handles debounced button press detection.
 *
 * This function checks for button press events and applies debounce logic to
 * prevent false triggers caused by mechanical bouncing. It ensures that a
 * button press is only registered once per press-release cycle.
 *
 * - If a button event is pending, it proceeds with checking the button state.
 * - If the button is pressed and wasn't previously held, it verifies that enough
 *   time has passed since the last press (debounce check) before registering the press.
 * - If the button was previously held but is now released, it resets the tracking
 *   to allow detecting the next press.
 *
 * This approach ensures that:
 * - Multiple detections don't occur while holding the button.
 * - Bounces after release don't trigger additional presses.
 * - The button must be fully released before another press is detected.
 */
void checkButtonPress() {
  if (!button_event_pending) {
    return;
  }
  button_event_pending = false;

  auto const now_us = time_us_32();
  bool const is_pressed = gpio_get(BUTTON_PIN) == IS_PRESSED;

  if (is_pressed && !button_held) {
    // The button is pressed, and it wasn't pressed before (avoiding duplicate presses)
    if (now_us - last_press_time_us > DEBOUNCE_TIME_US) {
      last_press_time_us = now_us;
      button_held = true;  // Mark as held to track release later
      logger << "Status button was pressed" << std::endl;
      // logger << getStatus() << std::endl << std::endl;
    }
  } else if (!is_pressed && button_held) {
    // The button was held, but now it's released, reset tracking
    button_held = false;
  }
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
  gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &button_callback);

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

    checkButtonPress();

    // Send a period message so we know the system hasn't locked up
    if (now_ms - lastUpdateMS >= logDurationIntervalMS) {
      lastUpdateMS = now_ms;
      // logger << getStatus() << std::endl << std::endl;
      logger << "Tick" << std::endl;
    }
  }
}
