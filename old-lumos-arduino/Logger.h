#ifndef _LOG_H_
#define _LOG_H_

#include <Stream.h>

#include "Colors.h"

class Logger {
  public:
    static void log(char const *msg);
    static void logf(char const *format,...);

    static void setStream(Stream *newStream);
};

#endif // _LOG_H_
