#pragma once

#include <memory>

#include "lumos-arduino/Colors.h"
#include "Models.h"

/*
 * Pulsate
 * 
 * Adjust the brightness of the underlying model between two percentages over a given period.
 * For example, the brightness may vary from 20% to 100% and back down to 20% over 3 seconds.
 * 
 * Constructors:
 *   Pulse(dimmest, brightest, dimSecs, brightenSecs, model) - varies between dimmest and brightest,
 *      taking dimSecs to dim and brightenSecs to brighten
 *
 * Future constructors:
 *   Pulse(period, model) - varies 0%-100% over the period in seconds, period is divided evenly between up and down
 *   Pulse(dimmest, brightest, period, model) - varies between dimmest and brightest over the period
 *    
 * Requires underlying model
 * Position independent, time dependent
 */
class Pulsate : public Model {
  public:
    Pulsate(char const *name, float dimmest, float brightest, float dimSecs, float brightenSecs,
        std::shared_ptr<Model> model)
      : Model(name), model(model), dimmest(dimmest), brightest(brightest),
        dimSecs(dimSecs), brightenSecs(brightenSecs), periodSecs(dimSecs + brightenSecs) {}
    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

  private:
    float dimness = 0.0;

    float const dimmest;
    float const brightest;
    float const dimSecs;
    float const brightenSecs;
    float const periodSecs;
    std::shared_ptr<Model> model;
};

/***** COMPOSITES - Animations that are composites of other models *****/
extern std::shared_ptr<Model> makeDarkCrystal();
extern std::shared_ptr<Model> makeCrystal(
  Color upperColor, float upperPeriodSec,
  Color middleColor, float middlePeriodSec,
  Color lowerColor, float lowerPeriodSec);
