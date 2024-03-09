#pragma once

#include <memory>
#include <NeoPixelBus.h>

#include "Model.h"

class Renderer {
protected:
    unsigned long _startTime_ms = millis();
    std::shared_ptr<Model> _model;
public:
    virtual void render() = 0;
    virtual void reset() { _startTime_ms = millis(); };

    // TODO Get info
    // TODO Get/set brightness

    std::shared_ptr<Model> & getModel() { return _model; }
    void setModel(std::shared_ptr<Model> & model) { _model = model; reset(); }
};

class Esp8266_NeoPixelBus_Renderer : public Renderer {
private:
    int _pixelsPin;
    int _pixelsCount;
    NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod> _strip;
public:
    Esp8266_NeoPixelBus_Renderer(int pixelsPin, int pixelsCount);
    void render() override;
};
