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

bool WiFiSoftAP::isUp() const {
  return cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP;
}

std::string WiFiSoftAP::generateSSID(std::string const &baseSSID, bool appendMac) const {
  if (!appendMac || macAddress.length() < 17) {
    // Return the base SSID if we don't need to append the MAC address
    return baseSSID;
  }

  // Add the suffix to the SSID
  std::string newSSID;
  newSSID.reserve(baseSSID.length() + 5);
  newSSID = baseSSID;
  newSSID += '-';
  newSSID += macAddress[12];
  newSSID += macAddress[13];
  newSSID += macAddress[15];
  newSSID += macAddress[16];

  return newSSID;
}

bool WiFiSoftAP::start(std::string const &baseSSID, std::string const &password, bool const appendMac) {
  // If the AP is already up, then fail
  if (isUp()) {
    logger << "Failed to start Soft AP mode, already up" << std::endl;
    return false;
  }

  // We need to pass in NULL for no password
  auto const pw = password.empty() ? nullptr : password.c_str();

  // Get the MAC address of the AP
  uint8_t ma[6];
  cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_AP, ma);
  macAddress = macAddrToString(ma);

  // Generate the SSID, appending the last four digits of the MAC address is optional
  ssid = generateSSID(baseSSID, appendMac);

  // Setup Wi-Fi soft AP mode
  cyw43_arch_enable_ap_mode(ssid.c_str(), pw, CYW43_AUTH_WPA2_AES_PSK);

  // Check if the AP interface is up
  if (!isUp()) {
    logger << "Failed to bring up the Access Point interface." << std::endl;
    return false;
  }

  // Assign a default IP address to the AP
  ip4_addr_t ip;
  IP4_ADDR(&ip, 192, 168, 27, 1);
  netif_set_ipaddr(&cyw43_state.netif[CYW43_ITF_AP], &ip);
  ipAddress = ipAddrToString(ip.addr);

  logger << "Soft Access Point started with SSID=" << ssid << ", IP=" << ipAddress << std::endl;

  // Start the dhcp server
  dhcpServer.start(ip);
  logger << "DHCP server started" << std::endl;

  return true;
}

bool WiFiSoftAP::stop() {
  // If the AP is already down, then fail
  if (!isUp()) {
    logger << "Failed to stop Soft AP mode, already down" << std::endl;
    return false;
  }

  // Stop the DHCP server
  dhcpServer.stop();

  // Disable the AP mode
  cyw43_arch_disable_ap_mode();

  // Clear the stored IP address and MAC address
  ssid = "undefined";
  ipAddress = "undefined";
  macAddress = "undefined";

  // Confirm that the Soft AP mode is disabled
  if (isUp()) {
    logger << "Failed to stop Soft AP mode" << std::endl;
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
  obj["status"] = (cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP) ? "up": "down";
  if (isUp()) {
    obj["ssid"] = ssid;
    obj["ipAddress"] = ipAddress;
    obj["macAddress"] = macAddress;

    auto const stations = getConnectedStations();

    auto const connectedStationsArray = obj["connectedStations"].to<JsonArray>();
    for (auto const &station : stations) {
      connectedStationsArray.add(macAddrToString(station.data()));
    }
  }
}
