#include "Network.h"
#include "Renderer.h"

// TODO Doc
void Renderer::render() {
    // If there is no model, we're done.
    if (_model == nullptr) {
        return;
    }

    // Get the current time in milliseconds, then convert to decimal seconds
    auto absoluteNow_ms = millis();
    auto relativeNow_ms = absoluteNow_ms - _startTime_ms;
    auto now_sec = float(relativeNow_ms) / 1000.0f;

    // Update the current state of the model to match the current time
    auto before_update_ms = millis();
    _model->update(now_sec);
    updateDuration = float((millis()) - before_update_ms) / 1000.0f;

    // Set the color of each pixel
    auto before_render_ms = millis();
    for (auto i = 0; i < pixelsCount(); ++i) {
        float pos = (float)i / float(pixelsCount() - 1);
        auto color = _model->render(pos);
        setPixel(i, color);
    }
  renderDuration = float((millis()) - before_render_ms) / 1000.0f;

    // Write the colors to the LED strip
    auto before_show_ms = millis();
    show();
  showDuration = float((millis()) - before_show_ms) / 1000.0f;
}

void Renderer::getStatus(JsonObject obj) const {
  obj["updateDuration"] = Network::getRenderer()->getUpdateDuration();
  obj["renderDuration"] = Network::getRenderer()->getRenderDuration();
  obj["showDuration"] = Network::getRenderer()->getShowDuration();
  obj["model"] = _model->getName();
}

// TODO Doc
Esp8266_NeoPixelBus_Renderer::Esp8266_NeoPixelBus_Renderer(int pixelsCount) :
        Renderer(),
        _strip(pixelsCount) {
    _strip.Begin();
    _strip.Show();  // Initialize all pixels to 'off'
}

void Esp8266_NeoPixelBus_Renderer::setPixel(int i, Color c) {
    RgbColor rgbColor(Colors::getRed(c), Colors::getGreen(c), Colors::getBlue(c));
    _strip.SetPixelColor(i, rgbColor);
}

void Esp8266_NeoPixelBus_Renderer::getStatus(JsonObject obj) const {
  obj["type"] = "Esp8266_NeoPixelBus_Renderer";
  obj["pixelsCount"] = pixelsCount();
  obj["brightness"] = getBrightness();
  Renderer::getStatus(obj);
}

