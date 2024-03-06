#pragma once

#include <memory>
#include <NeoPixelBus.h>

#include "Model.h"
//#include "lumos-arduino/lumos-arduino/Colors.h"

//class Renderer {
//  private:
//    std::shared_ptr<Model> model;
//
//    unsigned long startTimeMS;
//
//  public:
//    Renderer(std::shared_ptr<Model> model) : model(model), startTimeMS(millis()) { }
//    Renderer() : model(NULL) { }
//    void loop(int numPixels, std::function<void(int, Color)> setPixel);
//
//    auto getModel() { return model; }
//    void setModel(std::shared_ptr<Model> model);
//};

class Renderer {
protected:
    unsigned long _startTime_ms = millis();
    std::shared_ptr<Model> _model;
public:
    virtual void render() = 0;
    virtual void reset() { _startTime_ms = millis(); };
    // TODO Get info

    // TODO Get/set brightness
    // TODO Get model
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
