#pragma once

#include <lwip/apps/mdns.h>

struct netif;

class MDNS {
public:
  MDNS() = delete; // This class should never be instantiated

  /**
 * @brief Initialize mDNS with the given hostname.
 * @param hostname The hostname to advertise via mDNS.
 */
  static void setup(std::string const &hostname);

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
};
