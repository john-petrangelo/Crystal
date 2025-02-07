#include <ArduinoJson.h>

#include "lumos-arduino/Colors.h"
#include "lumos-arduino/Logger.h"
#include "lumos-arduino/Models/ModelUtils.h"
#include "lumos-arduino/Models/Crystal.h"
#include "lumos-arduino/Models/Flame.h"
#include "lumos-arduino/Models/Gradient.h"
#include "lumos-arduino/Models/JacobsLadder.h"
#include "lumos-arduino/Models/Rotate.h"
#include "lumos-arduino/Models/Solid.h"
#include "lumos-arduino/Models/WarpCore.h"

#include "Demos.h"
#include "Handlers.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPResponse.h"
#include "Networking/HandlerUtils.h"
#include "Networking/Network.h"
#include "System.h"
#include "../cmake-build-pico-w-debug/raspberry-pi-pico/web_files.h"

HTTPResponse handleRoot(HTTPRequest const & /*request*/) {
    return {200, "text/html", INDEX_HTML};
}

HTTPResponse handleCSS(HTTPRequest const & /*request*/) {
    return {200, "text/css", CRYSTAL_CSS};
}

HTTPResponse handleJS(HTTPRequest const & /*request*/) {
    return {200, "application/javascript", CRYSTAL_JS};
}

HTTPResponse handleStatus(HTTPRequest const & /*request*/) {
    JsonDocument doc;

    System::getStatus(doc["system"].to<JsonObject>());
    Network::getRenderer()->getStatus(doc["renderer"].to<JsonObject>());
    Network::getStatus(doc["network"].to<JsonObject>());

    std::string output;
    serializeJsonPretty(doc, output);
    output += "\n";

    return {200, "application/json", output};
}

HTTPResponse handleGetBrightness(HTTPRequest const & /*request*/) {
    JsonDocument doc;
    doc["value"] = Network::getRenderer()->brightness();

    std::string output;
    serializeJsonPretty(doc, output);

    return {200, "application/json", output};
}

HTTPResponse handleSetBrightness(HTTPRequest const &request) {
    auto const brightness = HandlerUtils::getArgAsLong(request.queryParams, "value");
    if (!brightness) {
        return {400, "text/plain", "Invalid 'value' parameter"};
    }

    if (*brightness < 0 || *brightness > 255) {
        logger << "Invalid brightness value: " << *brightness << std::endl;
        return {400, "text/plain", "Invalid brightness value. Must be >= 0 and <= 255"};
    }

    // Success
    Network::getRenderer()->setBrightness(*brightness);
    return {200, "text/plain"};
}

HTTPResponse handleGetGamma(HTTPRequest const & /*request*/) {
    JsonDocument doc;
    doc["value"] = Network::getRenderer()->gamma();

    std::string output;
    serializeJsonPretty(doc, output);

    return {200, "application/json", output};
}

HTTPResponse handleSetGamma(HTTPRequest const &request) {
    auto const gamma = HandlerUtils::getArgAsFloat(request.queryParams, "value");
    if (!gamma) {
        return {400, "text/plain", "Invalid 'value' parameter"};
    }

    if (*gamma <= 0.0f || *gamma > 10.0f) {
        logger << "Invalid gamma value" << std::endl;
        return {400, "text/plain", "Invalid gamma value. Must be > 0.0 and <= 10.0"};
    }

    // Success
    Network::getRenderer()->setGamma(*gamma);
    return {200, "text/plain"};
}

HTTPResponse handleCrystal(HTTPRequest const &request) {
    JsonDocument doc;
    if (!HandlerUtils::parseJsonBody(doc, request.body, "handleCrystal")) {
        return {400, "text/plain", "Failed to parse JSON body"};
    }

    Color const upperColor = HandlerUtils::getJsonColor(doc, "upper_color", 0xff00d0);
    Color const middleColor = HandlerUtils::getJsonColor(doc, "middle_color", 0xff00d0);
    Color const lowerColor = HandlerUtils::getJsonColor(doc, "lower_color", 0xff00d0);
    float const upperSpeed = HandlerUtils::getJsonValue(doc, "upper_speed", 0.5f);
    float const middleSpeed = HandlerUtils::getJsonValue(doc, "middle_speed", 0.2f);
    float const lowerSpeed = HandlerUtils::getJsonValue(doc, "lower_speed", 0.3f);

    float upperPeriodSec = fmap(upperSpeed, 0.0, 1.0, 11.0, 1.0);
    float middlePeriodSec = fmap(middleSpeed, 0.0, 1.0, 11.0, 1.0);
    float lowerPeriodSec = fmap(lowerSpeed, 0.0, 1.0, 11.0, 1.0);

    ModelPtr const model = std::make_shared<Crystal>(
        upperColor, upperPeriodSec,
        middleColor, middlePeriodSec,
        lowerColor, lowerPeriodSec);
    Network::getRenderer()->setModel(model);

    return {200, "text/plain"};
}

HTTPResponse handleFlame(HTTPRequest const & /*request*/) {
    ModelPtr const model = std::make_shared<Flame>();
    Network::getRenderer()->setModel(model);
    return {200, "text/plain"};
}

HTTPResponse handleRainbow(HTTPRequest const &request) {
    JsonDocument doc;
    if (!HandlerUtils::parseJsonBody(doc, request.body, "handleRainbow")) {
        return {400, "text/plain", "Failed to parse JSON body"};
    };
    std::string const mode = HandlerUtils::getJsonValue(doc, "mode", "classic");
    float const speed = HandlerUtils::getJsonValue(doc, "speed", 0.3f);

    ModelPtr gm = nullptr;
    if (mode == "tv") {
        gm = Gradient::make(0x0000BF, 0xBF0000, 0xBF00C0, 0x00BF00, 0x00BFBE, 0xBFBF00, 0xBFBFBF, 0x666666);
    } else if (mode == "50s") {
        gm = Gradient::make(0xB26463, 0x79C874, 0x8AC5BD, 0x9A942E, 0x937535, 0xB26463);
    } else if (mode == "60s") {
        gm = Gradient::make(0xFCD53A, 0xBFC847, 0xF7790B, 0xDC4E80, 0x7BD0EC, 0xFCD53A);
    } else if (mode == "70s") {
        gm = Gradient::make(0xE59F23, 0x925E18, 0x758C32, 0xCE985A, 0xE15623, 0xE59F23);
    } else if (mode == "80s") {
        gm = Gradient::make(0xFE167A, 0x003BB1, 0x00C8AB, 0xFFC800, 0x8000E2, 0xFE167A);
    } else if (mode == "90s") {
        gm = Gradient::make(0xB03A50, 0x7F5F14, 0x065B44, 0x4B183C, 0x6B0A29, 0xB03A50);
    } else if (mode == "00s") {
        gm = Gradient::make(0x79AED6, 0x8F8C84, 0xC4A051, 0x69322C, 0x3D4D4C, 0x79AED6);
    } else if (mode == "10s") {
        gm = Gradient::make(0xF0A2A3, 0x62656D, 0x8E6E29, 0x725A47, 0x4F7018, 0xF0A2A3);
    } else {
        // We want to get here for "classic". We'll also return a "classic" rainbow for any unexpected mode values.
        gm = Gradient::make(RED, VIOLET, INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED);
    }

    auto const model = Network::getRenderer()->getModel();
    if (strcmp(model->getName(), "RotatingRainbow") == 0) {
        // Update the current model
        auto const rainbowModel = static_cast<Rotate *>(model.get());
        rainbowModel->setSpeed(speed);
        rainbowModel->setModel(gm);
    } else {
        // Create a new model
        ModelPtr const rm = std::make_shared<Rotate>("RotatingRainbow", speed, gm);
        Network::getRenderer()->setModel(rm);
    }

    return {200, "text/plain"};
}

HTTPResponse handleWarpCore(HTTPRequest const &request) {
    JsonDocument doc;
    if (!HandlerUtils::parseJsonBody(doc, request.body, "handleWarpCore")) {
        return {400, "text/plain", "Failed to parse JSON body"};
    };

    float const frequency = HandlerUtils::getJsonValue(doc, "frequency", 0.6f);
    float const size = HandlerUtils::getJsonValue(doc, "size", 0.6f);
    float const dutyCycle = HandlerUtils::getJsonValue(doc, "dutyCycle", 0.4f);
    Color const color = HandlerUtils::getJsonColor(doc, "color", WarpCore::defaultColor);
    bool const dual = HandlerUtils::getJsonValue(doc, "dual", false);

    auto const model = Network::getRenderer()->getModel();
    if (strcmp(model->getName(), "WarpCore") == 0) {
        // Update the current model
        auto const warpCore = static_cast<WarpCore *>(model.get());
        warpCore->set(frequency, size, dutyCycle, color, dual);
    } else {
        // Create a new model
        ModelPtr const warpCore = WarpCore::make(size, frequency, dutyCycle, color, dual);
        Network::getRenderer()->setModel(warpCore);
    }
    return {200, "text/plain"};
}

HTTPResponse  handleJacobsLadder(HTTPRequest const &request) {
    JsonDocument doc;
    if (!HandlerUtils::parseJsonBody(doc, request.body, "handleJacobsLadder")) {
        return {400, "text/plain", "Failed to parse JSON body"};
    };
    float const frequency = HandlerUtils::getJsonValue(doc, "frequency", 0.6f);
    float const size = HandlerUtils::getJsonValue(doc, "size", 0.15f);
    Color const color = HandlerUtils::getJsonColor(doc, "color", JacobsLadder::defaultColor);
    float const jitterSize = HandlerUtils::getJsonValue(doc, "jitterSize", 0.15f);
    float const jitterPeriod = HandlerUtils::getJsonValue(doc, "jitterPeriod", 0.150f);

    auto const model = Network::getRenderer()->getModel();
    if (strcmp(model->getName(), "JacobsLadder") == 0) {
        // Update the current model
        auto const jacobsLadder = static_cast<JacobsLadder *>(model.get());
        jacobsLadder->set(frequency, size, color, jitterSize, jitterPeriod);
    } else {
        // Create a new model
        ModelPtr const jacobsLadder = JacobsLadder::make(size, frequency, color, jitterSize, jitterPeriod);
        Network::getRenderer()->setModel(jacobsLadder);
    }

    return {200, "text/plain"};
}

HTTPResponse handleSolid(HTTPRequest const &request) {
    auto color = HandlerUtils::getArgAsColor(request.queryParams, "color");
    if (!color) {
        return { 400, "text/plain", "Invalid color"};
    }

    ModelPtr const model = std::make_shared<Solid>(*color);
    Network::getRenderer()->setModel(model);

    return {200, "text/plain"};
}

HTTPResponse handleDemo1(HTTPRequest const &request) {
  auto const model = makeDemo1();
  Network::getRenderer()->setModel(model);
  return {200, "text/plain"};
}

HTTPResponse handleDemo2(HTTPRequest const &request) {
  auto const model = makeDemo2();
  Network::getRenderer()->setModel(model);
  return {200, "text/plain"};
}

HTTPResponse handleDemo3(HTTPRequest const &request) {
  auto const model = makeDemo3();
  Network::getRenderer()->setModel(model);
  return {200, "text/plain"};
}

HTTPResponse handleGetData(HTTPRequest const &request) {
    auto length = HandlerUtils::getArgAsLong(request.queryParams, "length");
    if (!length) {
        return {400, "text/plain", "Invalid 'value' parameter"};
    }

    if (*length < 0 || *length > 100000) {
        logger << "Invalid length: " << *length << std::endl;
        return {400, "text/plain", "Invalid data length. Must be >= 0 and <= 100,000"};
    }

    // Success
    return {200, "text/plain", std::string(*length, 'D')};
}
