#include "ConnectionContext.h"

#include <ostream>

uint32_t ConnectionContext::nextID = 0;

ConnectionContext::ConnectionContext(HTTPServer * const server, tcp_pcb *pcb) : _id(nextID++), _server(server), _pcb(pcb), _bytesSent(0) {
  updateLastActive();
  server->addActiveConnection(this);
}

ConnectionContext::~ConnectionContext() {
  if (_server) {
    _server->removeActiveConnection(this);
  }
}

void ConnectionContext::reset() {
  _requestData.clear();
  _responseData.clear();
  _remainingResponseData = std::string_view();
  _bytesSent = 0;
  _parser = HTTPRequestParser();
  updateLastActive();
}

void ConnectionContext::asJson(JsonObject const obj) const {
  obj["_id"] = _id;
  obj["remoteIP"] = _pcb ? ipaddr_ntoa(&_pcb->remote_ip) : "unknown";
  obj["remotePort"] = _pcb ? _pcb->remote_port : 0;
  obj["_lastActive"] = msToString(_lastActive);
}

std::ostream& operator<<(std::ostream& os, ConnectionContext const &context) {
  os << "[" << context._id << "] ";

  return os;
}
