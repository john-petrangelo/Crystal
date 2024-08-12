#include "defs.h"
#include "Logger.h"

Logger logger;

static Stream *stream = &Serial;

void Logger::log(char const *msg) {
  stream->print(msg);
}

void Logger::logf(char const *format,...)
{
  char buff[256];
  va_list args;
  va_start (args,format);
  vsnprintf(buff,sizeof(buff),format,args);
  va_end (args);
  buff[sizeof(buff)/sizeof(buff[0])-1]='\0';
  Logger::log(buff);
}

void Logger::setStream(Stream *newStream) {
  stream = newStream;
}
