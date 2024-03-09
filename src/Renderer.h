#pragma once

#include <memory>
#include <NeoPixelBus.h>

#include "Model.h"

class Renderer {
private:
    unsigned long _startTime_ms = millis();
    std::shared_ptr<Model> _model;

    virtual void setPixel(int i, Color c) = 0;
    virtual void show() = 0;
    virtual int pixelsCount() = 0;

public:
    void render();
    void reset() { _startTime_ms = millis(); };

    // TODO Get info
    // TODO Get/set brightness

    std::shared_ptr<Model> & getModel() { return _model; }
    void setModel(std::shared_ptr<Model> & model) { _model = model; reset(); }
};

class Esp8266_NeoPixelBus_Renderer : public Renderer {
private:
    NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod> _strip;
    Esp8266_NeoPixelBus_Renderer(int pixelsPin, int pixelsCount);
    void setPixel(int i, Color c) override;
    void show() override;
    int pixelsCount() override { return _strip.PixelCount(); }
};
