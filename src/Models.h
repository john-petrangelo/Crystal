#pragma once

#include "Models/Model.h"

/***** REVERSE *****/

class ReverseModel : public Model {
  public:
    ReverseModel(std::shared_ptr<Model> model) : Model("ReverseModel"), model(model) { }
    void update(float timeStamp) override { model->update(timeStamp); }
    Color render(float pos) override { return model->render(1.0 - pos); }

  private:
    std::shared_ptr<Model> model;
};

/***** TRIANGLE *****/

class Triangle : public Model {
public: Triangle(char const *name, float rangeMin, float rangeMax, Color color)
            : Model(name), rangeMin(rangeMin), rangeMax(rangeMax), color(color) { }
    Color render(float pos) override;

private:
    float const rangeMin;
    float const rangeMax;
    Color const color;
};

/***** GAUGE *****/

class GaugeModel : public Model {
public: GaugeModel(char const *name, uint16_t pixelsCount, Color color, float value = 0.0)
            : Model(name), pixelsCount(pixelsCount), color(color) { setValue(value); }
    Color render(float pos) override;
    float getValue() { return value; }
    void setValue(float value);

private:
    float const pixelsCount;
    Color const color;
    float value = 0.0;
    float fullColorPixels = 0.0;
    float remainder = 0.0;
    float colorRatio = 0.0;
    Color dimmedColor = BLACK;
};

/***** IDEAS *****/

/*
 * Dim
 * Adjust the brightness of the color down by the provided percent
 * 
 * Constructors:
 *   Dim(dimPercent, model) - dims all colors of the underlying model by dimPercent, expressed as 0.0-1.0
 *
 * Requires input model
 * Position and time independent
 */

/*
 * Brighten
 * Increases the brightness of the color up by the provided percent, no R, G, or B will exceed 255
 * 
 * Constructors:
 *   Brighten(brightenPercent, model) - brightens all colors of the underlying model by brightenPercent, expressed as 0.0-1.0
 * 
 * Requires input model
 * Position and time independent
 */

/*
 * Firefly
 * A firely (small light band? needs definition) flits around in a specified range with specified speed parameters (TBD)
 */

/*
 * Matrix
 * Green spots flow from one end of the strip to the other.
 * Can experiment with varying rates, sizes, brightnesses, hues.
 * 
 * Position and time dependend.
 */

/*
 * Blend
 * Blend two models together. Details TBD, but options include LERP, add, etc.
 * 
 * Requires two input models.
 * Position and time independent.
 */

/*
 * Blur
 * Performs some sort of convolution around a position to blur the colors.
 * 
 * Requires input model.
 * Position and time independent.
 */

/*
 * Lava lamp
 * Simulate a lava lamp.
 * 
 * Direction - up/down
 * Color
 */

/*
 * Warp core
 * Simulate a Star Trek warp core.
 * 
 * Direction - up/down?
 * Color?
 * Speed?
 */

/*
 * Jacob's ladder
 * Simulate the rising electrical arc of a Jacob's ladder.
 * 
 * Color?
 * Speed?
 */

/*
 * Static
 * Simulate static/random noise.
 *
 * Color or BW
 * Speed? (how often the static updates)
 */

/*
 * Knight Rider
 * Strobe light back and forth
 *
 * Color?
 * Speed?
 * Velocity shape? (e.g. linear, faster on edges, faster in middle, etc.)
 */
