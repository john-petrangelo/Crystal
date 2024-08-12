#pragma once

#include <Stream.h>

#include <lumos-arduino/Colors.h>
#include <lumos-arduino/ILogger.h>

class ArduinoStreamLogger : public ILogger {
public:
    explicit ArduinoStreamLogger(Stream *newStream) : stream(newStream) {}

    virtual void logMsg(char const *msg);
    virtual void logf(char const *format,...);

  private:
    Stream *stream;
};
