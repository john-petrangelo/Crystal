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

std::string picoErrStr(/*pico_error_codes*/ int const err) {
  switch (err) {
    case PICO_OK: return "No error";
    case PICO_ERROR_GENERIC: return "Generic error";
    case PICO_ERROR_TIMEOUT: return "Timeout error";
    case PICO_ERROR_NO_DATA: return "No data error";
    case PICO_ERROR_NOT_PERMITTED: return "Not permitted error";
    case PICO_ERROR_INVALID_ARG: return "Invalid argument error";
    case PICO_ERROR_IO: return "I/O error";
    case PICO_ERROR_BADAUTH: return "Bad authentication error";
    case PICO_ERROR_CONNECT_FAILED: return "Connection failed error";
    case PICO_ERROR_INSUFFICIENT_RESOURCES: return "Insufficient resources error";
    case PICO_ERROR_INVALID_ADDRESS: return "Invalid address error";
    case PICO_ERROR_BAD_ALIGNMENT: return "Bad alignment error";
    case PICO_ERROR_INVALID_STATE: return "Invalid state error";
    case PICO_ERROR_BUFFER_TOO_SMALL: return "Buffer too small error";
    case PICO_ERROR_PRECONDITION_NOT_MET: return "Precondition not met error";
    case PICO_ERROR_MODIFIED_DATA: return "Modified data error";
    case PICO_ERROR_INVALID_DATA: return "Invalid data error";
    case PICO_ERROR_NOT_FOUND: return "Not found error";
    case PICO_ERROR_UNSUPPORTED_MODIFICATION: return "Unsupported modification error";
    case PICO_ERROR_LOCK_REQUIRED: return "Lock required error";
    case PICO_ERROR_VERSION_MISMATCH: return "Version mismatch error";
    case PICO_ERROR_RESOURCE_IN_USE: return "Resource in use error";
    default: return "Unknown error (" + std::to_string(err) + ")";
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
