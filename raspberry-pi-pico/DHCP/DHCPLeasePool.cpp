#include <algorithm>
#include <iterator>
#include <ostream>

#include <lumos-arduino/Logger.h>

#include <Networking/NetworkUtils.h>

#include "DHCPLeasePool.h"

// Allocate an IP address from the pool
std::optional<ip4_addr_t> DHCPLeasePool::allocate_ip(uint8_t mac[6]) {
  // First, check if the MAC already has an assigned IP
  for (int i = 0; i < std::size(ip_pool); i++) {
    if (ip_pool[i].active && memcmp(ip_pool[i].mac, mac, 6) == 0) {
      return ip_pool[i].ip; // Return the existing lease
    }
  }

  // If no existing lease, assign a new IP
  for (int i = 0; i < std::size(ip_pool); i++) {
    if (!ip_pool[i].active) {
      memcpy(ip_pool[i].mac, mac, 6);
      ip_pool[i].active = true;
      return ip_pool[i].ip;
    }
  }
  return std::nullopt; // No available IPs
}

bool DHCPLeasePool::findIP(uint32_t const requestedIP, uint8_t const requestedMAC[6]) {
  for (auto &[ip, mac, active]: ip_pool) {
    if (ip.addr != requestedIP) {
      // IP doesn't match, continue
      continue;
    }

    if (std::equal(mac, mac+6, requestedMAC)) {
      // IP and MAC address match, ensure the lease is active and return success
      active = true;
      return true;
    }

    if (std::all_of(mac, mac+6, [](uint8_t const byte) { return byte == 0; })) {
      // IP matches, but no MAC address assigned, assign the MAC address and return success
      std::copy_n(requestedMAC, 6, mac);
      active = true;
      return true;
    }
  }
  return false;
}

// Release an IP address back to the pool
void DHCPLeasePool::release_ip(uint8_t mac[6]) {
  for (int i = 0; i < std::size(ip_pool); i++) {
    if (ip_pool[i].active && memcmp(ip_pool[i].mac, mac, 6) == 0) {
      ip_pool[i].active = false;
      memset(ip_pool[i].mac, 0, 6);
    }
  }
}

/**
 * @brief Clears all active DHCP leases.
 *
 * This function resets the lease pool by marking all leases as inactive
 * and clearing the associated MAC addresses. This ensures that no stale
 * leases remain when restarting the DHCP server or resetting the lease pool.
 *
 * Note: This function does not reset the fixed list of IP addresses.
 */
void DHCPLeasePool::clear() {
  for (auto &[ip, mac, active]: ip_pool) {
    active = false;
    memset(mac, 0, 6);
  }
}

void DHCPLeasePool::dump() {
  logger << "DHCP Lease Pool:" << std::endl;
  for (auto const &[ip, mac, active]: ip_pool) {
    logger << "  IP: " << ip4addr_ntoa(&ip) << ", MAC: " << macAddrToString(mac) << ", " << (active ? "Active" : "Inactive") << std::endl;
  }
}

