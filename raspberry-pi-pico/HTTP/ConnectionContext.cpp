#include "ConnectionContext.h"

#include <ostream>

uint32_t ConnectionContext::nextID = 0;

void ConnectionContext::reset() {
  inData.clear();
  outData.clear();
  remainingOutData = std::string_view();
  bytesSent = 0;
  parser = HTTPRequestParser();
}

std::ostream& operator<<(std::ostream& os, ConnectionContext const &context) {
  os << "[" << context.id << "] ";

  return os;
}
