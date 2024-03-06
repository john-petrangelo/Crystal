#ifndef __COMBINE__
#define __COMBINE__

#include <memory>

#include "lumos-arduino/defs.h"
#include "lumos-arduino/Colors.h"
#include "lumos-arduino/Logger.h"

#include "Model.h"

/***** ADD *****/

/*
 * Add two models together, channel by channel, constraining each channel to 1.0
 * 
 * Requires two input models.
 * Position and time independent.
 */
class Add : public Model {
  public:
    Add(char const *name, std::shared_ptr<Model> a, std::shared_ptr<Model> b)
      : Model(name), modelA(a), modelB(b) { }
    void update(float timeStamp) { modelA->update(timeStamp); modelB->update(timeStamp); }
    Color render(float pos);

  private:
    std::shared_ptr<Model> modelA;
    std::shared_ptr<Model> modelB;
};

/***** WINDOW *****/

/*
 * TODO WindowModel description
 */
class WindowModel : public Model {
  public:
    WindowModel(char const *name, float rangeMin, float rangeMax,
        std::shared_ptr<Model> insideModel, std::shared_ptr<Model> outsideModel)
      : Model(name), rangeMin(rangeMin), rangeMax(rangeMax),
        insideModel(insideModel), outsideModel(outsideModel) { }
    void update(float timeStamp) { insideModel->update(timeStamp); outsideModel->update(timeStamp); }
    Color render(float pos);

  private:
    std::shared_ptr<Model> insideModel;
    std::shared_ptr<Model> outsideModel;
    float rangeMin, rangeMax;
};

#endif // __COMBINE__
