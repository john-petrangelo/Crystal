#pragma once

#include <pico/cyw43_arch.h>

#include "DHCPLeasePool.h"

class DHCPServer {
public:
  void start();

private:
  static void dhcp_server_callback(void *arg, struct udp_pcb *pcb, pbuf *p, const ip_addr_t *addr, u16_t port);

  void handleDHCPRelease(uint8_t mac[6]);
  bool handleDHCPDiscover(struct udp_pcb *pcb, const ip_addr_t *addr, uint32_t xid, uint8_t mac[6]);
  void handleDHCPRequest(udp_pcb *pcb, const ip_addr_t *addr, uint8_t *payload, uint32_t xid, uint8_t mac[6]);

  void appendDHCPOption(uint8_t const code, uint8_t const length, uint8_t *payload, uint16_t &offset, const uint8_t *data);
  void appendDHCPMsgTypeOption(uint8_t *payload, uint16_t &offset, uint8_t msgType);

  static uint8_t get_dhcp_message_type(uint8_t *payload, uint16_t len);

  DHCPLeasePool leasePool;

  // DHCP server and client ports
  static constexpr uint16_t DHCP_SERVER_PORT = 67;
  static constexpr uint16_t DHCP_CLIENT_PORT = 68;
};
