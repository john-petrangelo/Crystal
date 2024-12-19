#pragma once

#include "HTTPRequestParser.h"
#include "HTTPServer.h"

struct ConnectionContext {
  ConnectionContext(HTTPServer *server, tcp_pcb *pcb);
  ~ConnectionContext();

  // Reset the context for the same connection, making it ready for the next request
  void reset();

  void asJson(JsonObject obj) const;

  uint32_t id;
  HTTPServer *server; // Pointer to the server instance
  tcp_pcb *pcb; // Pointer to the connection's PCB

  std::string inData;
  std::string outData;
  std::string_view remainingOutData;
  size_t bytesSent;

  HTTPRequestParser parser;

  friend std::ostream& operator<<(std::ostream& os, const ConnectionContext& context);

private:
  // The nextID will be used and incremented every time we open a new connection
  static uint32_t nextID;
};
