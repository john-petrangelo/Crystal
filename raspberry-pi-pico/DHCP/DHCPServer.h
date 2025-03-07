#pragma once

#include <vector>

#include <pico/cyw43_arch.h>

#include "DHCPLeasePool.h"

class DHCPServer {
public:
  void start();
  void stop();
  bool isRunning() { return dhcp_pcb != nullptr; }

private:
  static void dhcp_server_callback(void *arg, struct udp_pcb *pcb, pbuf *p, const ip_addr_t *addr, u16_t port);

  void handleDHCPRelease(uint8_t mac[6]);
  bool handleDHCPDiscover(struct udp_pcb *pcb, const ip_addr_t *addr, uint32_t xid, uint8_t mac[6]);
  void handleDHCPDecline(uint8_t mac[6]);
  void handleDHCPRequest(udp_pcb *pcb, const ip_addr_t *addr, uint8_t *payload, uint32_t xid, uint8_t mac[6]);

  void parseDHCPOptions(uint8_t const *payload, uint16_t len);

  void appendDHCPOption(uint8_t const code, uint8_t const length, uint8_t *payload, uint16_t &offset, const uint8_t *data);
  void appendDHCPMsgTypeOption(uint8_t *payload, uint16_t &offset, uint8_t msgType);

  udp_pcb *dhcp_pcb = nullptr;

  DHCPLeasePool leasePool;

  struct DHCPOptions {
    uint8_t messageType = 0;
    uint32_t requestedIP = 0;
    std::vector<uint8_t> parameterRequestList; // Not used
    uint16_t maxMessageSize = 576; // The default DHCP message size is 576 bytes, not used
  };

  DHCPOptions options;

  // DHCP server and client ports
  static constexpr uint16_t DHCP_SERVER_PORT = 67;
  static constexpr uint16_t DHCP_CLIENT_PORT = 68;
};
