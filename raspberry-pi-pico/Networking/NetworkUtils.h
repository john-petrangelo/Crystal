#pragma once

#include <string>

#include <lwip/netif.h>

std::string ipAddrToString(uint32_t ipAddr);
std::string macAddrToString(uint8_t const mac[6]);
std::string cyw43ErrStr(int err);
std::string errToString(err_t const err);

/**
 * @brief Converts a network interface pointer to a human-readable string.
 *
 * @param interface A pointer to a netif structure representing the network interface.
 * @return A std::string_view describing the interface type.
 */
std::string_view netifToString(netif const *interface);
