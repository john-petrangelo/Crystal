#include "ArduinoStreamLogger.h"

void ArduinoStreamLogger::log(char const *msg) {
  stream->print(msg);
}
