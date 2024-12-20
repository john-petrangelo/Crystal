#include "ConnectionContext.h"

#include <ostream>

uint32_t ConnectionContext::nextID = 0;

ConnectionContext::ConnectionContext(HTTPServer *server, tcp_pcb *pcb) : id(nextID++), server(server), pcb(pcb), bytesSent(0) {
  updateLastActive();
  server->addActiveConnection(this);
}

ConnectionContext::~ConnectionContext() {
  server->removeActiveConnection(this);
}

void ConnectionContext::reset() {
  inData.clear();
  outData.clear();
  remainingOutData = std::string_view();
  bytesSent = 0;
  parser = HTTPRequestParser();
}

void ConnectionContext::asJson(JsonObject const obj) const {
  obj["id"] = id;
  obj["remoteIP"] = ipaddr_ntoa(&pcb->remote_ip);
  obj["remotePort"] = pcb->remote_port;
  obj["lastActive"] = msToString(lastActive);
}

std::ostream& operator<<(std::ostream& os, ConnectionContext const &context) {
  os << "[" << context.id << "] ";

  return os;
}
