#pragma once

#include "Model.h"

class Triangle : public Model {
public: Triangle(float rangeMin, float rangeMax, Color color)
            : Model("Triangle"), rangeMin(rangeMin), rangeMax(rangeMax), color(color) { }
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

private:
    float const rangeMin;
    float const rangeMax;
    Color const color;
};
