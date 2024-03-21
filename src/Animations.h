#pragma once

#include <memory>

#include "lumos-arduino/Colors.h"
#include "Model.h"

class Flame : public Model {
  public:
    Flame();

    void update(float timeStamp) override;
    Color render(float pos) override;
    void asJson(JsonObject obj) const override;

  private:
    Color const C1 = Colors::blend(RED, YELLOW, 50);
    Color const C2 = Colors::blend(RED, YELLOW, 70);
    Color const C3 = Colors::blend(RED, YELLOW, 90);

    std::shared_ptr<MapModel> model;

    float lastUpdateMS;
    float const PERIOD_SEC = 0.110;
};

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

// An animation that rotates or shifts lights to the left or right.
// Wraps around so that once a color reaches the end, then it wraps around.
// The speed of rotation is given as a frequncy expressed in Hz. A frequency
// of zero is stopped. Positive speed rotates up, negative speed rotates down.
class Rotate : public Model {
  public:
    Rotate(char const *name, float speed, std::shared_ptr<Model> model) 
      : Model(name), speed(speed), model(model) {}
    virtual void update(float timeStamp);
    virtual Color render(float pos);
    virtual void asJson(JsonObject obj) const override;
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setModel(std::shared_ptr<Model> newModel) { model = newModel; }

private:
    float speed;
    float rotationOffset = 0.0;
    float prevTimeStamp = 0.0;
    std::shared_ptr<Model> model;
};

/***** COMPOSITES - Animations that are composites of other models *****/
extern std::shared_ptr<Model> makeDarkCrystal();
extern std::shared_ptr<Model> makeCrystal(
  Color upperColor, float upperPeriodSec,
  Color middleColor, float middlePeriodSec,
  Color lowerColor, float lowerPeriodSec);
