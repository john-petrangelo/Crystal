#include <string>

#include <pico/cyw43_arch.h>

#include <lumos-arduino/Logger.h>
#include <Networking/NetworkUtils.h>

#include "WiFiSoftAP.h"

#include <algorithm>
#include <span>

WiFiSoftAP &WiFiSoftAP::getInstance() {
  static WiFiSoftAP instance;
  return instance;
}

// Create our own network using Soft AP mode
bool WiFiSoftAP::start(std::string const &ssid, std::string const &password) {
  // We need to pass in NULL for no password
  auto const pw = password.empty() ? nullptr : password.c_str();

  // Setup Wi-Fi soft AP mode
  cyw43_arch_enable_ap_mode(ssid.c_str(), pw, CYW43_AUTH_WPA2_AES_PSK);

  // Check if the AP interface is up
  if (!(cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP)) {
    logger << "Failed to bring up the Access Point interface." << std::endl;
    return false;
  }

  // Assign a default IP address to the AP
  ip4_addr_t ip;
  IP4_ADDR(&ip, 192, 168, 27, 1);
  netif_set_ipaddr(&cyw43_state.netif[CYW43_ITF_AP], &ip);

  ipAddress = ipAddrToString(ip.addr);
  macAddress = macAddrToString(cyw43_state.netif[CYW43_ITF_AP].hwaddr);

  logger << "Soft Access Point started: SSID=" << ssid
    << ", IP=" << ipAddress
    << ", MAC=" << macAddress
    << ", flags=0x" << std::hex << static_cast<int>(cyw43_state.netif[CYW43_ITF_AP].flags) << std::dec
    << std::endl;

  // Start the dhcp server
  dhcpServer.start();
  logger << "DHCP server started" << std::endl;

  return true;
}

bool WiFiSoftAP::stop() {
  logger << "Stopping Soft Access Point..." << std::endl;

  // Stop the DHCP server
  dhcpServer.stop();
  logger << "Can't stop the DHCP server yet" << std::endl;

  // Disable the AP mode
  cyw43_arch_disable_ap_mode();

  // Clear the stored IP address and MAC address
  ipAddress = "undefined";
  macAddress = "undefined";

  // Confirm that the Soft AP mode is disabled
  if (cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP) {
    logger << "Failed to stop Soft AP mode." << std::endl;
    return false;
  }

  logger << "Soft AP mode stopped" << std::endl;

  return true;
}

std::vector<std::array<uint8_t, 6>> WiFiSoftAP::getConnectedStations() const {
  // Find out how many stations are connected
  int numStations = 0;
  cyw43_wifi_ap_get_stas(&cyw43_state, &numStations, nullptr);
  if (numStations == 0) {
    // No stations connected, return empty vector
    return {};
  }

  // Allocate space for the station MAC addresses
  std::vector<std::array<uint8_t, 6>> stations(numStations);

  // Get the list of the connected station MAC addresses
  cyw43_wifi_ap_get_stas(&cyw43_state, &numStations, reinterpret_cast<uint8_t *>(stations.front().data()));

  if (numStations == 0) {
    stations.clear();
    return stations;
  }

  // Remove any zeroed out MAC addresses
  std::erase_if(stations, [](std::array<uint8_t, 6> const &station) {
    return std::all_of(station.begin(), station.end(), [](uint8_t byte) { return byte == 0; });
  });

  return stations;
}

void WiFiSoftAP::getStatus(JsonObject const obj) const {
  obj["ipAddress"] = ipAddress;
  obj["macAddress"] = macAddress;
  obj["status"] = (cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP) ? "up": "down";

  auto const stations = getConnectedStations();

  auto const connectedStationsArray = obj["connectedStations"].to<JsonArray>();
  for (auto const &station : stations) {
    connectedStationsArray.add(macAddrToString(station.data()));
  }
}
