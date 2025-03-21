#pragma once

#include <lwip/apps/mdns.h>

struct netif;

class MDNSServer {
public:
  MDNSServer() = default;

  /**
   * @brief Initialize mDNS with the given hostname.
   */
  void init(std::string const &hostname);

  void start();
  void stop();
  bool isRunning() const { return mdnsActive; }

private:
  /**
   * @brief Callback for adding service TXT records.
   * @param service Pointer to the mDNS service.
   * @param txt_userdata User-defined data (can be nullptr).
   */
  static void mdnsAddServiceTextItemCallback(mdns_service *service, void *txt_userdata);

  /**
   * @brief Callback for mDNS status reports.
   * @param netif Network interface.
   * @param result Status result (success/failure).
   * @param slot Service slot index.
   */
  static void mdnsReportCallback(netif *netif, uint8_t result, int8_t slot);

  int8_t startInterface(netif *interface) const;
  void stopInterface(netif *interface, int8_t &mdnsSlot) const;

  static int8_t constexpr INVALID_SLOT = INT8_MAX;

  std::string mdnsHostname;

  int8_t mdnsSoftAPSlot = INVALID_SLOT;
  int8_t mdnsStationSlot = INVALID_SLOT;

  bool mdnsActive = false;
};
