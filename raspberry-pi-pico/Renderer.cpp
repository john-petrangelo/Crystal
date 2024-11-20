#include "Network.h"
#include "Renderer.h"

#include <hardware/pio.h>
#include "ws2812.pio.h"

// TODO Doc
void Renderer::render() {
    // If there is no model, we're done.
    if (_model == nullptr) {
        return;
    }

  // Get the current time in milliseconds, then convert to decimal seconds
  auto const absoluteNow_ms = to_ms_since_boot(get_absolute_time());
  auto const relativeNow_ms = absoluteNow_ms - _startTime_ms;
  auto const now_sec = static_cast<float>(relativeNow_ms) / 1000.0f;

  // Update the current state of the model to match the current time
  auto const before_update_ms = to_ms_since_boot(get_absolute_time());
  _model->update(now_sec);
  updateDuration = static_cast<float>((to_ms_since_boot(get_absolute_time())) - before_update_ms) / 1000.0f;

  // Set the color of each pixel
  auto const before_render_ms = to_ms_since_boot(get_absolute_time());
  for (auto i = 0; i < pixelsCount(); ++i) {
      float const pos = static_cast<float>(i) / static_cast<float>(pixelsCount() - 1);
      auto const color = _model->render(pos);
      setPixel(i, color);
  }
  renderDuration = static_cast<float>((to_ms_since_boot(get_absolute_time())) - before_render_ms) / 1000.0f;

  // Write the colors to the LED strip
  auto const before_show_ms = to_ms_since_boot(get_absolute_time());
  show();
  showDuration = static_cast<float>((to_ms_since_boot(get_absolute_time())) - before_show_ms) / 1000.0f;
}

void Renderer::getStatus(JsonObject obj) const {
  obj["updateDuration"] = updateDuration;
  obj["renderDuration"] = renderDuration;
  obj["showDuration"] = showDuration;

  getModel()->asJson(obj["model"].to<JsonObject>());
}

// TODO Doc
RaspberryPiPico_Renderer::RaspberryPiPico_Renderer(int pixelsCount) :
        Renderer(), _strip(22, pixelsCount)
{
    PIO const pio = pio0;
    constexpr uint sm = 0;
    uint const offset = pio_add_program(pio, &ws2812_program);
    int constexpr PICO_DEFAULT_WS2812_PIN = 22;
    ws2812_program_init(pio, sm, offset, PICO_DEFAULT_WS2812_PIN, 80000, false);

    _strip.show();  // Initialize all pixels to 'off'
}

void RaspberryPiPico_Renderer::setPixel(int i, Color c) {
    _strip.setPixel(i, c);
}

void RaspberryPiPico_Renderer::getStatus(JsonObject obj) const {
    obj["type"] = "RaspberryPiPico_Renderer";
    obj["pixelsCount"] = pixelsCount();
    obj["brightness"] = getBrightness();
    obj["gamma"] = _strip.gamma();
    Renderer::getStatus(obj);
}
