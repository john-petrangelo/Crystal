#pragma once

#include "Model.h"

/*
 * Set a gradient color pattern from one color to another color.
 */
class GradientModel : public Model {
  public:
    GradientModel(Color a, Color b) : Model("Gradient"), a(a), b(b) { }
    Color render(float pos) override { return Colors::blend(a, b, int(100.0f * pos)); }

  private:
    Color a, b;
};

/*
 * Set a gradient color pattern. The number of defined color points is variable.
 */
class MultiGradientModel : public Model {
  public:
    explicit MultiGradientModel(int count, ...);
    Color render(float pos) override;

  private:
    static int const MAX_COLORS = 10;
    int count;
    Color colors[MAX_COLORS]{};
};
