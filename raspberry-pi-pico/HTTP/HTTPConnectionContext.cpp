#include "HTTPConnectionContext.h"

#include <ostream>

uint32_t HTTPConnectionContext::nextID = 0;

HTTPConnectionContext::HTTPConnectionContext(HTTPServer * const server, tcp_pcb *pcb) : _id(nextID++), _server(server), _pcb(pcb), _bytesSent(0) {
  updateLastActive();
  server->addActiveConnection(this);
}

HTTPConnectionContext::~HTTPConnectionContext() {
  if (_server) {
    _server->removeActiveConnection(this);
  }
}

void HTTPConnectionContext::reset() {
  _requestData.clear();
  _responseData.clear();
  _remainingResponseData = std::string_view();
  _bytesSent = 0;
  _shouldCloseAfterResponse = false;
  _parser = HTTPRequestParser();
  updateLastActive();
}

void HTTPConnectionContext::asJson(JsonObject const obj) const {
  obj["id"] = _id;
  obj["remoteIP"] = _pcb ? ipaddr_ntoa(&_pcb->remote_ip) : "PCB not initialized";
  obj["remotePort"] = _pcb ? _pcb->remote_port : 0;
  obj["lastActive"] = msToString(_lastActive);
}

std::ostream& operator<<(std::ostream& os, HTTPConnectionContext const &context) {
  os << "[" << context._id << "] ";

  return os;
}
