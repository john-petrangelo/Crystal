#include "lumos-arduino/Logger.h"
#include "MDNS.h"

void MDNS::mdnsAddServiceTextItemCallback(mdns_service *service, void * /*txt_userdata*/) {
  if (err_t const res = mdns_resp_add_service_txtitem(service, "path=/", 6); res != ERR_OK) {
    logger << "mDNS Failed to add service TXT record, err=" << res << std::endl;
  } else {
    logger << "mDNS TXT record added successfully" << std::endl;
  }
}

void MDNS::mdnsReportCallback(netif * netif, u8_t const result, int8_t const slot) {
  if (result == MDNS_PROBING_SUCCESSFUL) {
    logger << "mDNS name successfully registered on netif " << static_cast<int>(netif->num) << std::endl;
  } else if (result == MDNS_PROBING_CONFLICT) {
    logger << "mDNS name conflict detected on netif " << static_cast<int>(netif->num)
      << ", slot " << static_cast<int>(slot) << std::endl;
  } else {
    logger << "Unknown mDNS status result: " << static_cast<int>(result) << std::endl;
  }
}

void MDNS::setup(std::string const &hostname) {
  mdns_resp_register_name_result_cb(mdnsReportCallback);
  mdns_resp_init();

  if (err_t const err = mdns_resp_add_netif(netif_default, hostname.c_str()); err != ERR_OK) {
    logger << "Failed to add netif to mDNS, err=" << err << std::endl;
    return;
  }
  if (err_t const err = mdns_resp_add_service(netif_default, "Luminarium", "_http",
        DNSSD_PROTO_TCP, 80, mdnsAddServiceTextItemCallback, nullptr); err != ERR_OK) {
    logger << "Failed to add mDNS service, err=" << err << std::endl;
    return;
        }

  mdns_resp_announce(netif_default);
  logger << "mDNS service for host '" << hostname << "' announced successfully" << std::endl;
}
