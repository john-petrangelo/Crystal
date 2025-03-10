#include <string>

#include <pico/cyw43_arch.h>

#include <lumos-arduino/Logger.h>
#include <Networking/NetworkUtils.h>

#include "WiFiSoftAP.h"

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

  ////  // TODO  Log each time a station connects or disconnects
  ////  WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected& evt) {
  ////      logger << "Station disconnected " << macToString(evt.mac) << std::endl;
  ////  });
  ////
  ////  logger << "Soft AP status: " << softAPStarted ? "Ready" : "Failed" << std::endl;
  ////  logger << "Soft AP IP address: " << WiFi.softAPIP().toString() << str::endl;
  ////  logger << "Soft AP MAC address = " << WiFi.softAPmacAddress() << str::endl
  ////  logger << "Soft AP SSID = " << WiFi.softAPSSID() << str::endl
  ////  logger << "Soft AP PSK = " << WiFi.softAPPSK() << str::endl
  ////  logger << "Soft AP has " << WiFi.softAPgetStationNum() << "stations connected\n" << str::endl

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

// TODO Log connect and disconnect events, track connected stations

// TODO Add status function

// TODO Needs better interface
std::string_view WiFiSoftAP::getSoftAPStatus() const {
  if (cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP) {
    return "Soft AP is active";
  }

  return "Soft AP is NOT active";
}
