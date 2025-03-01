#include <iterator>

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

// Release an IP address back to the pool
void DHCPLeasePool::release_ip(uint8_t mac[6]) {
  for (int i = 0; i < std::size(ip_pool); i++) {
    if (ip_pool[i].active && memcmp(ip_pool[i].mac, mac, 6) == 0) {
      ip_pool[i].active = false;
      memset(ip_pool[i].mac, 0, 6);
    }
  }
}
