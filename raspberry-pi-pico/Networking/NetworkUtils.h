#pragma once

#include <string>

#include <lwip/netif.h>

std::string ipAddrToString(uint32_t ipAddr);
std::string macAddrToString(const uint8_t *mac);
std::string cyw43ErrStr(int err);
