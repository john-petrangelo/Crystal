#ifndef _LOG_H_
#define _LOG_H_

#include <Stream.h>

#include "Colors.h"

class Logger {
  public:
    static void logMsg(char const *msg);
    static void logMsgLn(char const *msg);

    static void logColor(char const *name, Color value);

    static void logf(char const *format,...);

    static void setStream(Stream *newStream);

  private:
    static void logName(char const *name);
};

#endif // _LOG_H_
