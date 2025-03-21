#include <iomanip>

#include <cyw43.h>

#include "Networking/NetworkUtils.h"

std::string ipAddrToString(u32_t const ipAddr) {
  std::ostringstream oss;
  oss << (ipAddr & 0xFF) << '.'
      << ((ipAddr >> 8) & 0xFF) << '.'
      << ((ipAddr >> 16) & 0xFF) << '.'
      << ((ipAddr >> 24) & 0xFF);
  return oss.str();
}

std::string macAddrToString(uint8_t const mac[6]) {
  if (!mac) { // Check for a null pointer
    return "<null>";
  }

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
    case CYW43_EPERM: return "Operation not permitted";
    case CYW43_EIO: return "Connection failed: Input/output error";
    case CYW43_EINVAL: return "Connection failed: Invalid argument";
    case CYW43_ETIMEDOUT: return "Connection failed: Connection timed out";
    default: return "Connection failed: " + err;
  }
}

std::string errToString(err_t const err) {
  switch (err) {
    case ERR_OK: return "No error";
    case ERR_MEM: return "Out of memory";
    case ERR_ABRT: return "Connection aborted";
    case ERR_RST: return "Connection reset";
    case ERR_CLSD: return "Connection closed";
    default: return "Unknown error (" + std::to_string(err) + ")";
  }
}

std::string_view netifToString(netif const *interface) {
  if (!interface) {
    return "<null>";
  }

  switch (interface->num) {
    case CYW43_ITF_AP:
      return "SoftAP";
    case CYW43_ITF_STA:
      return "Station";
    default:
      return "Unknown interface";
  }
}
