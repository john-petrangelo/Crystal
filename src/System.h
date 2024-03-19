#pragma once

#include <Arduino.h>

class System {
private:
    static void getTime(JsonObject obj);

public:
    static void setup();
    static void getStatus(JsonObject obj);
};
