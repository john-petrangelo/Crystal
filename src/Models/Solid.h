#pragma once

#include "Model.h"

/*
 * Set a solid color pattern, invariant with time.
 */
class Solid : public Model {
  public:
    Solid(char const *name, Color color) : Model("Solid"), color(color) {}
    Color render(float pos) override { return color; }
    void asJson(JsonObject obj) const override { colorAsJson(obj["color"].to<JsonObject>(), color); };

  private:
    Color color;
};
