#ifndef __MODEL__
#define __MODEL__

#include <memory>

#include "lumos-arduino/defs.h"
#include "lumos-arduino/Colors.h"
#include "lumos-arduino/Logger.h"

class Model {
  public:
    Model(char const *name) : name(name) { }
    virtual ~Model() = default;

    // Updates the model to the current timestamp. Models that change with time (animations)
    // will need to implement this function. A default implementation is provided for static
    // models that do not change with time.
    virtual void update(float timeStamp) {}

    // Returns the color that should be displayed at the specified pos at the current time.
    virtual Color render(float pos) = 0;

    // Returns the name of this model, provided in the constructor.
    char const * getName() { return name; }

  private:
    char const *name;
};

/***** SOLID *****/

/*
 * Set a solid color pattern.
 */
class SolidModel : public Model {
  public:
    SolidModel(char const *name, Color color) : Model(name), color(color) {}
    Color render(float pos) override { return color; }

  private:
    Color color;
};

/***** GRADIENT *****/

/*
 * Set a gradient color pattern from one color to another color.
 */
class GradientModel : public Model {
  public:
    GradientModel(char const *name, Color a, Color b) : Model(name), a(a), b(b) { }
    Color render(float pos) override { return Colors::blend(a, b, 100 * pos); }

  private:
    Color a, b;
};

/***** MULTIGRADIENT *****/

/*
 * Set a gradient color pattern. The number of defined color points is variable.
 */
class MultiGradientModel : public Model {
  public:
    MultiGradientModel(char const *name, int count, ...);
    Color render(float pos) override;

  private:
    static int const MAX_COLORS = 10;
    int count;
    Color colors[MAX_COLORS];
};

/***** MAP *****/

/*
 * TODO MapModel description
 */
class MapModel : public Model {
  public:
    MapModel(char const *name, float inRangeMin, float inRangeMax, float outRangeMin, float outRangeMax,
        std::shared_ptr<Model> model)
      : Model(name), inRangeMin(inRangeMin), inRangeMax(inRangeMax),
        outRangeMin(outRangeMin), outRangeMax(outRangeMax), model(model) { }
    void update(float timeStamp) override { model->update(timeStamp); }
    Color render(float pos) override;

    void setInRange(float inRangeMin, float inRangeMax) {
      this->inRangeMin = inRangeMin;
      this->inRangeMax = inRangeMax;
    }

  private:
    std::shared_ptr<Model> model;
    float inRangeMin, inRangeMax, outRangeMin, outRangeMax;
};

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

class Gauge : public Model {
public: Gauge(char const *name, uint16_t pixelsCount, Color color, float value = 0.0)
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


#endif // __MODEL__
