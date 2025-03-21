#include "lumos-arduino/Logger.h"
#include "MDNSServer.h"

#include <cyw43.h>
#include <Networking/NetworkUtils.h>
#include <Networking/WiFi/WiFiSoftAP.h>
#include <Networking/WiFi/WifiStation.h>

void MDNSServer::mdnsAddServiceTextItemCallback(mdns_service *service, void * /*txt_userdata*/) {
  if (err_t const res = mdns_resp_add_service_txtitem(service, "path=/", 6); res != ERR_OK) {
    logger << "mDNS Failed to add service TXT record, err=" << res << std::endl;
  } else {
    logger << "mDNS TXT record added successfully" << std::endl;
  }
}

void MDNSServer::mdnsReportCallback(netif * netif, u8_t const result, int8_t const slot) {
  std::string_view const interfaceName = netifToString(netif);
  if (result == MDNS_PROBING_SUCCESSFUL) {
    logger << "mDNS name successfully registered on " << interfaceName << " interface" << std::endl;
  } else if (result == MDNS_PROBING_CONFLICT) {
    logger << "mDNS name conflict detected on " << interfaceName << " interface" << ", slot " << static_cast<int>(slot) << std::endl;
  } else {
    logger << "Unknown mDNS status result: " << static_cast<int>(result) << std::endl;
  }
}

void MDNSServer::init(std::string const &hostname) {
  mdnsHostname = hostname;

  // Register the name resolution callback
  mdns_resp_register_name_result_cb(mdnsReportCallback);

  // Initialize the mDNS system
  mdns_resp_init();

  logger << "mDNS server initialized" << std::endl;
}

int8_t MDNSServer::startInterface(netif *interface ) const {
  // Add network interface
  if (err_t const err = mdns_resp_add_netif(interface, mdnsHostname.c_str()); err != ERR_OK) {
    logger << "Cannot start mDNS server, failed to add netif to mDNS, err=" << err << std::endl;
    return INVALID_SLOT;
  }

  // Add the Luminarium service
  auto const slotOrErr = mdns_resp_add_service(interface, "Luminarium", "_http",
                                               DNSSD_PROTO_TCP, 80, mdnsAddServiceTextItemCallback, nullptr);

  // If it's negative, then it's an error
  if (slotOrErr < 0) {
    logger << "Cannot start mDNS server, failed to add mDNS service, error " << errToString(slotOrErr) << std::endl;
    mdns_resp_remove_netif(interface);
    return INVALID_SLOT;
  }

  // Announce the service on this interface
  mdns_resp_announce(interface);

  // Success, return the slot
  return slotOrErr;
}

void MDNSServer::start() {
  if (mdnsActive) {
    logger << "Cannot start mDNS server, already running" << std::endl;
    return;
  }

  // If the station interface is connected, then add DNS service there
  if (WiFiStation::getInstance().isConnected()) {
    mdnsStationSlot = startInterface(&cyw43_state.netif[CYW43_ITF_STA]);
  }

  // If the soft AP interface is up, then add DNS service there
  if (WiFiSoftAP::getInstance().isUp()) {
    mdnsSoftAPSlot = startInterface(&cyw43_state.netif[CYW43_ITF_AP]);
  }

  mdnsActive = true;

  logger << "mDNS server started with hostname " << mdnsHostname << std::endl;
}

void MDNSServer::stopInterface(netif *interface, int8_t &mdnsSlot) const {
  if (!interface) {
    logger << "MDNSServer::stopInterface called with null interface" << std::endl;
    return;
  }

  std::string_view const interfaceName = netifToString(interface);
  logger << "Stopping mDNS server for " << mdnsHostname << " on " << interfaceName << " interface" << std::endl;

  if (auto const err = mdns_resp_del_service(interface, mdnsSlot); err != ERR_OK) {
    logger << "Failed to delete mDNS service from " << interfaceName << " interface, error " << errToString(err) << std::endl;
  }
  if (auto const err = mdns_resp_remove_netif(interface); err != ERR_OK) {
    logger << "Failed to remove " << interfaceName << " netif from mDNS, error " << errToString(err) << std::endl;
  }

  mdnsSlot = INVALID_SLOT;
}

void MDNSServer::stop() {
  if (!mdnsActive) {
    logger << "Cannot stop mDNS server, not running" << std::endl;
    return;
  }

  if (mdnsSoftAPSlot >= 0) {
    stopInterface(&cyw43_state.netif[CYW43_ITF_AP], mdnsSoftAPSlot);
  }

  if (mdnsStationSlot >= 0) {
    stopInterface(&cyw43_state.netif[CYW43_ITF_STA], mdnsStationSlot);
  }

  mdnsActive = false;

  logger << "mDNS server stopped" << std::endl;
}
