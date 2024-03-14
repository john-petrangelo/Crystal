#pragma once

#include <memory>
#include <NeoPixelBusLg.h>

#include "Model.h"

class Renderer {
private:
    unsigned long _startTime_ms = millis();
    std::shared_ptr<Model> _model;
    float updateDuration = 0.0;
    float renderDuration = 0.0;
    float showDuration = 0.0;

public:
    void render();
    void reset() { _startTime_ms = millis(); };

    std::shared_ptr<Model> & getModel() { return _model; }
    void setModel(std::shared_ptr<Model> & model) { _model = model; reset(); }

    // TODO Get info

    virtual void setPixel(int i, Color c) = 0;
    virtual void show() = 0;
    virtual int pixelsCount() = 0;

    virtual uint8_t getBrightness() = 0;
    virtual void setBrightness(uint8_t brightness) = 0;

    float getUpdateDuration() { return updateDuration; }
    float getRenderDuration() { return renderDuration; }
    float getShowDuration() { return showDuration; }
};

class Esp8266_NeoPixelBus_Renderer : public Renderer {
private:
    // The NeoPixelBusLg class with the UART1 method always uses Pin2.
    NeoPixelBusLg<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod, NeoGammaTableMethod> _strip;

public:
    explicit Esp8266_NeoPixelBus_Renderer(int pixelsCount);

    void setPixel(int i, Color c) override;
    void show() override { _strip.Show(); }
    int pixelsCount() override { return _strip.PixelCount(); }

    uint8_t getBrightness() override { return _strip.GetLuminance(); }
    void setBrightness(uint8_t brightness) override { _strip.SetLuminance(brightness); }
};
