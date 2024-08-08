#pragma once

#include <string>

class System {
private:
    static std::string getTime();

    static uint32_t getHeapTotal();
    static uint32_t getHeapAvailable();
    static uint32_t getHeapHighWater();
    static uint32_t getHeapAllocated();
    static uint32_t getHeapFreed();

    static uint32_t getCpuFreqMHz();

public:
    static void setup();

    static std::string getStatus();
};
