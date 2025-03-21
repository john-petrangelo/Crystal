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

bool WiFiStation::start(char const *ssidToConnect, char const *password) {
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
  logger << "Wi-Fi station connecting to " << ssidToConnect << "..." << std::endl;
  auto const connect_error = cyw43_arch_wifi_connect_timeout_ms(
    ssidToConnect, password, CYW43_AUTH_WPA2_AES_PSK, 15000);
  if (connect_error) {
    logger << "Wi-Fi station connection failed: " << cyw43ErrStr(connect_error) << std::endl;
    return false;
  }

  if (!isUp()) {
    logger << "Wi-Fi station mode is not up after connection" << std::endl;
    return false;
  }

  // Save the network properties so we can report them later
  ssid = ssidToConnect;
  ipAddress = ipAddrToString(cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr);
  macAddress = macAddrToString(cyw43_state.netif[CYW43_ITF_STA].hwaddr);

  // Success
  logger << "Wi-Fi station connected to " << ssid  << " with IP " << ipAddress << std::endl;

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
  ssid = "undefined";

  // Confirm that station mode is actually stopped
  if (isUp()) {
    logger << "Failed to stop Wi-Fi station mode." << std::endl;
    return false;
  }

  logger << "Wi-Fi station mode stopped" << std::endl;

  return true;
}

/*
 * The following constants are copied from the LWIP library.
 * Bits 0-3 are an enumeration, while bits 8-11 are flags.
 */
uint16_t constexpr WIFI_JOIN_STATE_KIND_MASK = (0x000f); // 0b0000'0000'0000'1111
uint16_t constexpr WIFI_JOIN_STATE_ACTIVE    = (0x0001); // 0b0000'0000'0000'0001
uint16_t constexpr WIFI_JOIN_STATE_FAIL      = (0x0002); // 0b0000'0000'0000'0010
uint16_t constexpr WIFI_JOIN_STATE_NONET     = (0x0003); // 0b0000'0000'0000'0011
uint16_t constexpr WIFI_JOIN_STATE_BADAUTH   = (0x0004); // 0b0000'0000'0000'0100
uint16_t constexpr WIFI_JOIN_STATE_AUTH      = (0x0200); // 0b0000'0010'0000'0000
uint16_t constexpr WIFI_JOIN_STATE_LINK      = (0x0400); // 0b0000'0100'0000'0000
uint16_t constexpr WIFI_JOIN_STATE_KEYED     = (0x0800); // 0b0000'1000'0000'0000
uint16_t constexpr WIFI_JOIN_STATE_ALL       = (0x0e01); // 0b0000'1111'0000'0001

std::string_view WiFiStation::getStateStr() {
  int const joinState = cyw43_state.wifi_join_state & WIFI_JOIN_STATE_KIND_MASK;
  bool const hasLink = cyw43_state.wifi_join_state & WIFI_JOIN_STATE_LINK;
  bool const hasKey = cyw43_state.wifi_join_state & WIFI_JOIN_STATE_KEYED;

  if (joinState == WIFI_JOIN_STATE_ACTIVE) {
    if (hasLink) {
      return "connected";
    }
    if (hasKey) {
      return "waiting for IP";
    }
      return "joining";
  }
  if (joinState == WIFI_JOIN_STATE_FAIL) {
    return "connection failed";
  }
  if (joinState == WIFI_JOIN_STATE_NONET) {
    return "network not found";
  }
  if (joinState == WIFI_JOIN_STATE_BADAUTH) {
    return "authentication failure";
  }
  return "not connected";
}

void WiFiStation::getStatus(JsonObject const obj) const {
  obj["state"] = getStateStr();
  obj["stateCode"] = cyw43_state.wifi_join_state;
  if (isUp()) {
    obj["ssid"] = ssid;

    uint8_t bssid[6];
    if (cyw43_wifi_get_bssid(&cyw43_state, bssid) == 0) {
      obj["bssid"] = macAddrToString(bssid);
    }

    int32_t rssi;
    if (cyw43_wifi_get_rssi(&cyw43_state, &rssi) == 0) {
      obj["rssi"] = rssi;
    }

      obj["macAddress"] = macAddress;
      obj["ip"] = ipAddress;
  }
}
