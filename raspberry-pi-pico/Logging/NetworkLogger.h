#pragma once

#include <cstring>

#include <lumos-arduino/ILogger.h>

#include "LogServer.h"

class NetworkLogger : public ILogger {
public:
  explicit NetworkLogger(tcp_pcb *newTpcb) : tpcb(newTpcb) {}

  void log(char const *msg) override { LogServer::sendMessage(tpcb, msg, strlen(msg)); }

  private:
  tcp_pcb *tpcb;
};
