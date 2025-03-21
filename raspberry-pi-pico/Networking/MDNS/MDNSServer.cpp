#include "lumos-arduino/Logger.h"
#include "MDNSServer.h"

#include <Networking/NetworkUtils.h>

void MDNSServer::mdnsAddServiceTextItemCallback(mdns_service *service, void * /*txt_userdata*/) {
  if (err_t const res = mdns_resp_add_service_txtitem(service, "path=/", 6); res != ERR_OK) {
    logger << "mDNS Failed to add service TXT record, err=" << res << std::endl;
  } else {
    logger << "mDNS TXT record added successfully" << std::endl;
  }
}

void MDNSServer::mdnsReportCallback(netif * netif, u8_t const result, int8_t const slot) {
  if (result == MDNS_PROBING_SUCCESSFUL) {
    logger << "mDNS name successfully registered on netif " << static_cast<int>(netif->num) << std::endl;
  } else if (result == MDNS_PROBING_CONFLICT) {
    logger << "mDNS name conflict detected on netif " << static_cast<int>(netif->num)
      << ", slot " << static_cast<int>(slot) << std::endl;
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

int8_t MDNSServer::startInterface(netif *interface ) {
  // Add network interface
  if (err_t const err = mdns_resp_add_netif(interface, mdnsHostname.c_str()); err != ERR_OK) {
    logger << "Cannot start mDNS server, failed to add netif to mDNS, err=" << err << std::endl;
    return err;
  }

  // Add the Luminarium service
  auto const slotOrErr = mdns_resp_add_service(interface, "Luminarium", "_http",
                                               DNSSD_PROTO_TCP, 80, mdnsAddServiceTextItemCallback, nullptr);

  // If it's negative, then it's an error
  if (slotOrErr < 0) {
    logger << "Cannot start mDNS server, failed to add mDNS service, error " << errToString(slotOrErr) << std::endl;
    mdns_resp_remove_netif(interface);
    return slotOrErr;
  }

  // Not negative, so it's the slot
  mdnsSlot = slotOrErr;

  // Announce the service
  mdns_resp_announce(interface);

  // Success
  return mdnsSlot;
}

void MDNSServer::start() {
  if (mdnsActive) {
    logger << "Cannot start mDNS server, already running" << std::endl;
    return;
  }

  if (!startInterface(netif_default)) {
    return;
  }

  mdnsActive = true;

  logger << "mDNS server started with hostname " << mdnsHostname << std::endl;
}


void MDNSServer::stop() {
  if (!mdnsActive) {
    logger << "Cannot stop mDNS server, not running" << std::endl;
    return;
  }

  if (auto const err = mdns_resp_del_service(netif_default, mdnsSlot); err != ERR_OK) {
    logger << "Failed to remove mDNS service, error " << errToString(err) << std::endl;
  }

  if (auto const err = mdns_resp_remove_netif(netif_default); err != ERR_OK) {
    logger << "Failed to remove mDNS netif, error " << errToString(err) << std::endl;
  }

  mdnsActive = false;
  mdnsSlot = UINT8_MAX;

  logger << "mDNS server stopped" << std::endl;
}
