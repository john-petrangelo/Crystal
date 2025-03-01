#pragma once

#include <string>

#include <lwip/netif.h>

std::string ipAddrToString(uint32_t ipAddr);
std::string macAddrToString(uint8_t const mac[6]);
std::string cyw43ErrStr(int err);
