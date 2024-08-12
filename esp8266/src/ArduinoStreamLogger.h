#pragma once

#include <Stream.h>

#include <lumos-arduino/ILogger.h>

class ArduinoStreamLogger : public ILogger {
public:
    explicit ArduinoStreamLogger(Stream *newStream) : stream(newStream) {}

    virtual void log(char const *msg);

  private:
    Stream *stream;
};
