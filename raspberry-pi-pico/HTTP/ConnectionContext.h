#pragma once

#include "HTTPRequestParser.h"
#include "HTTPServer.h"

struct ConnectionContext {
  ConnectionContext(HTTPServer *server, tcp_pcb *pcb);

  HTTPServer *server; // Pointer to the server instance
  tcp_pcb *pcb; // Pointer to the connection's PCB
  std::string data;
  HTTPRequestParser parser;
  uint32_t id;

  friend std::ostream& operator<<(std::ostream& os, const ConnectionContext& context);

private:
  // The nextID will be used and incremented every time we open a new connection
  static uint32_t nextID;
};
