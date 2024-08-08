#pragma once

#include <string>

class System {
private:
    static std::string getTime();

public:
    static void setup();
    static std::string getStatus();
};
