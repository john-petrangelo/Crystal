#pragma once

#include "LogServer.h"

#include <lumos-arduino/ILogger.h>

class NetworkLogger : public ILogger {
public:
  explicit NetworkLogger(struct tcp_pcb *newTpcb, LogServer &newLogServer) :
    tpcb(newTpcb), logServer(newLogServer) {}

  void log(char const *msg) override { logServer.sendMessage(tpcb, msg, strlen(msg)); }

  private:
  struct tcp_pcb *tpcb;
  LogServer &logServer;
};
