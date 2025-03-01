#pragma once

#include <pico/cyw43_arch.h>

class DHCPLeasePool {
public:
  struct Lease;

  Lease* allocate_ip(uint8_t mac[6]);
  void release_ip(uint8_t mac[6]);

private:
  // DHCP lease record
public: // TODO Remove this line
  struct Lease {
    ip4_addr_t ip; // IP address of the client
    uint8_t mac[6]; // MAC address of the client
    bool active; // Whether the IP is in use
  };

  // IP lease pool for DHCP server
  Lease ip_pool[4] = {
    {IPADDR4_INIT_BYTES(192, 168, 27, 11), {0}, false},
    {IPADDR4_INIT_BYTES(192, 168, 27, 12), {0}, false},
    {IPADDR4_INIT_BYTES(192, 168, 27, 13), {0}, false},
    {IPADDR4_INIT_BYTES(192, 168, 27, 14), {0}, false},
  };
};
