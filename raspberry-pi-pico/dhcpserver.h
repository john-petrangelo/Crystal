#pragma once

class DHCPServer {
public:
  void start();

private:
  static void dhcp_server_callback(void *arg, struct udp_pcb *pcb, pbuf *p, const ip_addr_t *addr, u16_t port);

  void handleDHCPRelease(uint8_t mac[6]);
  bool handleDHCPDiscover(struct udp_pcb *pcb, const ip_addr_t *addr, uint32_t xid, uint8_t mac[6]);
  void handleDHCPRequest(udp_pcb *pcb, const ip_addr_t *addr, uint8_t *payload, uint32_t xid, uint8_t mac[6]);

  struct Lease;
  Lease* allocate_ip(uint8_t mac[6]);
  void release_ip(uint8_t mac[6]);

  void appendDHCPOption(uint8_t const code, uint8_t const length, uint8_t *payload, uint16_t &offset, const uint8_t *data);
  void appendDHCPMsgTypeOption(uint8_t *payload, uint16_t &offset, uint8_t msgType);

  static uint8_t get_dhcp_message_type(uint8_t *payload, uint16_t len);
  static std::string macStr(uint8_t mac[6]);

  // DHCP server and client ports
  static constexpr uint16_t DHCP_SERVER_PORT = 67;
  static constexpr uint16_t DHCP_CLIENT_PORT = 68;

  // DHCP lease record
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
