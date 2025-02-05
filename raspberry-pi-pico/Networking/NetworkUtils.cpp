#include <iomanip>

#include <pico/cyw43_arch.h>

#include "Networking/NetworkUtils.h"

std::string ipAddrToString(u32_t ip) {
  std::ostringstream oss;
  oss << (ip & 0xFF) << '.'
      << ((ip >> 8) & 0xFF) << '.'
      << ((ip >> 16) & 0xFF) << '.'
      << ((ip >> 24) & 0xFF);
  return oss.str();
}

std::string macAddrToString(const uint8_t* mac) {
  std::ostringstream oss;
  for (size_t i = 0; i < 6; ++i) {
    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
    if (i < 5) {
      oss << ':'; // Add colon separator except for the last byte
    }
  }
  return oss.str();
}

std::string cyw43ErrStr(int const err) {
  switch (err) {
    case CYW43_EPERM:
      return "Operation not permitted";
    case CYW43_EIO:
      return "Connection failed: Input/output error";
    case CYW43_EINVAL:
      return "Connection failed: Invalid argument";
    case CYW43_ETIMEDOUT:
      return "Connection failed: Connection timed out";
    default:
      return "Connection failed: " + err;
  }
}
