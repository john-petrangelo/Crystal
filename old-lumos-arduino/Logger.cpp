#include "defs.h"
#include "Logger.h"

Logger logger;

static Stream *stream = &Serial;

void Logger::logMsg(char const *msg) {
  stream->print(msg);
}

void Logger::logMsgLn(char const *msg) {
  stream->println(msg);
}

void Logger::logColor(char const *name, Color value) {
  logName(name);
  stream->print(value, HEX);
  stream->print(" ");
}

void Logger::logName(char const *name) {
  stream->print(name);
  stream->print("=");  
}

void Logger::logf(char const *format,...)
{
  char buff[256];
  va_list args;
  va_start (args,format);
  vsnprintf(buff,sizeof(buff),format,args);
  va_end (args);
  buff[sizeof(buff)/sizeof(buff[0])-1]='\0';
  Logger::logMsg(buff);
}

void Logger::setStream(Stream *newStream) {
  stream = newStream;
}
