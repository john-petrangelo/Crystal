#include <HardwareSerial.h>

#include <lumos-arduino/defs.h>
#include <lumos-arduino/ILogger.h>

#include "ArduinoStreamLogger.h"

ILogger *logger;

void ArduinoStreamLogger::log(char const *msg) {
  stream->print(msg);
}
