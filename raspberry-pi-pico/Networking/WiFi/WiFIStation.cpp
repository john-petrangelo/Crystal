#include "WiFIStation.h"

#include <pico/cyw43_arch.h>

#include <lumos-arduino/Logger.h>

#include "Networking/NetworkUtils.h"

WiFiStation &WiFiStation::getInstance() {
  static WiFiStation instance;
  return instance;
}

bool WiFiStation::isUp() const {
  return cyw43_state.netif[CYW43_ITF_STA].flags & NETIF_FLAG_UP;
}

bool WiFiStation::isConnected() const {
  return cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP;
}

bool WiFiStation::start(char const *ssid, char const *password) {
  if (!isUp()) {
    // Enable the Wi-Fi station mode
    cyw43_arch_enable_sta_mode();
    logger << "Enabled Wi-Fi station mode" << std::endl;
  }

  //  // Set up the renderer with a strobing model for while we connect
  //  Color c = Colors::makeColor(127, 127, 255);
  //  ModelPtr triangle = std::make_shared<Triangle>(0.4, 1.0, c);
  //  ModelPtr pulsate = std::make_shared<Pulsate>(0.2, 1.0, 0.1, 0.9, triangle);
  //  networkRenderer->setModel(pulsate);


  // Connect to an access point in station mode
  logger << "Wi-Fi station connecting to " << ssid << "..." << std::endl;
  auto const connect_error = cyw43_arch_wifi_connect_timeout_ms(
    ssid, password, CYW43_AUTH_WPA2_AES_PSK, 15000);
  if (connect_error) {
    logger << "Wi-Fi station connection failed: " << cyw43ErrStr(connect_error) << std::endl;
    return false;
  }

  if (!isUp()) {
    logger << "Wi-Fi station mode is not up after connection" << std::endl;
    return false;
  }

  // Success
  logger << "Wi-Fi station connected to " << ssid << std::endl;

  // Save the network properties so we can report them later
  ipAddress = ipAddrToString(cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr);
  macAddress = macAddrToString(cyw43_state.netif[CYW43_ITF_STA].hwaddr);

  return true;
}

bool WiFiStation::stop() {
  if (!isUp()) {
    logger << "Failed to stop Wi-Fi station mode, already down" << std::endl;
    return false;
  }

  // Disconnect Wi-Fi station mode
  cyw43_arch_disable_sta_mode();

  // Clear the stored IP address and MAC address
  ipAddress = "undefined";
  macAddress = "undefined";

  // Confirm that station mode is actually stopped
  if (isUp()) {
    logger << "Failed to stop Wi-Fi station mode." << std::endl;
    return false;
  }

  logger << "Wi-Fi station mode stopped" << std::endl;

  return true;
}
