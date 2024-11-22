#pragma once

#include <memory>
#include <ArduinoJson.h>
#include <lumos-arduino/Colors.h>

#include "Models/Model.h"
#include "WS2812Strip.h"

class Renderer {
public:
    virtual ~Renderer() = default;

    void render();
    void reset() { _startTime_ms = to_ms_since_boot(get_absolute_time()); };

    std::shared_ptr<Model> & getModel() { return _model; }
    std::shared_ptr<Model> const & getModel() const { return _model; }
    void setModel(std::shared_ptr<Model> & model) { _model = model; reset(); }

    virtual void setPixel(int i, Color c) = 0;
    virtual void show() = 0;
    virtual int pixelsCount() const = 0;

    virtual uint8_t brightness() const = 0;
    virtual void setBrightness(uint8_t brightness) = 0;

    virtual float gamma() const = 0;
    virtual void setGamma(float gamma) = 0;

    virtual void getStatus(JsonObject obj) const;

private:
    uint64_t _startTime_ms = to_ms_since_boot(get_absolute_time());
    std::shared_ptr<Model> _model;
    float updateDuration = 0.0;
    float renderDuration = 0.0;
    float showDuration = 0.0;
};

class RaspberryPiPico_Renderer : public Renderer {
public:
    explicit RaspberryPiPico_Renderer(int pixelsCount);

    void setPixel(int i, Color c) override;
    void show() override { _strip.show(); }
    int pixelsCount() const override { return _strip.numPixels(); }

    uint8_t brightness() const override { return _strip.brightness(); }
    void setBrightness(uint8_t brightness) override { _strip.setBrightness(brightness); }

    float gamma() const override { return _strip.gamma(); }
    void setGamma(float gamma) override { _strip.setGamma(gamma); }

    void getStatus(JsonObject obj) const override;

private:
    WS2812Strip _strip;
};
