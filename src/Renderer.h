#ifndef RENDERER
#define RENDERER
//#pragma once

#include <memory>

#include "Model.h"
#include "lumos-arduino/lumos-arduino/Colors.h"

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
private:
    unsigned long const startTime_ms; // = millis();
    std::shared_ptr<Model> model;
public:
    void render();
    void reset();
    // TODO Get info

    // TODO Get/set brightness
    // TODO Get model

};

#endif // RENDERER
