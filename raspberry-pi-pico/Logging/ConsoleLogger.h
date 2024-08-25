#pragma once

#include <iostream>

#include <lumos-arduino/ILogger.h>

// Simple logger which writes messages to standard out
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger() {}

    void log(char const *msg) override {
      std::cout << msg;
    }
};
