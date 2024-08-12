#include <HardwareSerial.h>

#include <lumos-arduino/defs.h>
#include <lumos-arduino/ILogger.h>

#include "ArduinoStreamLogger.h"

ILogger *logger;

void ArduinoStreamLogger::logMsg(char const *msg) {
  stream->print(msg);
}

void ArduinoStreamLogger::logf(char const *format, ...)
{
  char buff[256];
  va_list args;
  va_start (args,format);
  vsnprintf(buff,sizeof(buff),format,args);
  va_end (args);
  buff[sizeof(buff)/sizeof(buff[0])-1]='\0';
  ArduinoStreamLogger::logMsg(buff);
}
