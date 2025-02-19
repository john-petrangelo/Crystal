#pragma once

#include <algorithm>
#include <array>
#include <string>

#include <ArduinoJson.h>

class WiFiScanResult {
public:
  WiFiScanResult(std::string ssid, int const rssi, uint8_t const bssid[6], uint16_t const channel, bool const isSecure)
    : ssid(std::move(ssid)), rssi(rssi), bssid{}, channel(channel), isSecure(isSecure) {
    std::copy_n(bssid, 6, this->bssid.begin());
  }

  std::string ssid;
  int rssi;
  std::array<uint8_t, 6> bssid;
  uint16_t channel;
  bool isSecure;

  void asJson(JsonObject const obj) const;
};
