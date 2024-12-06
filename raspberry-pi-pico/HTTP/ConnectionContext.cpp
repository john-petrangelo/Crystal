#include "ConnectionContext.h"

#include <ostream>

uint32_t ConnectionContext::nextID = 0;

ConnectionContext::ConnectionContext(HTTPServer *server, tcp_pcb *pcb) : server(server), pcb(pcb) {
  id = nextID++;
}

std::ostream& operator<<(std::ostream& os, ConnectionContext const &context) {
  os << "[" << context.id << "] ";

  return os;
}
