#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class Filesystem {
public:
    static void setup();
    static void end();
    static void getStatus(JsonObject obj);
};
