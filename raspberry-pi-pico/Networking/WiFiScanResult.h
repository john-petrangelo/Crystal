#pragma once

#include <algorithm>
#include <string>

#include <ArduinoJson.h>

#include "NetworkUtils.h"

/**
 * @class WiFiScanResult
 * @brief Represents a single Wi-Fi network scan result.
 *
 * This class stores details about a discovered Wi-Fi network,
 * including SSID, BSSID, signal strength (RSSI), channel, and security status.
 */
class WiFiScanResult {
public:
  /**
  * @brief Constructs a WiFiScanResult object.
  *
  * @param ssid The Service Set Identifier (SSID) of the Wi-Fi network.
  * @param rssi The Received Signal Strength Indicator (RSSI) in dBm.
  * @param bssid The Basic Service Set Identifier (BSSID), a 6-byte MAC address.
  * @param channel The Wi-Fi channel on which the network is operating.
  * @param isSecure Indicates whether the network is secured (true) or open (false).
  */
  WiFiScanResult(std::string ssid, int const rssi, uint8_t const bssid[6], uint16_t const channel, bool const isSecure)
    : ssid(std::move(ssid)), rssi(rssi), bssid(macAddrToString(bssid)), channel(channel), isSecure(isSecure) {}

  /// @brief The Wi-Fi network's SSID (name).
  std::string ssid;

  /// @brief The signal strength in dBm (negative values, e.g., -45 dBm).
  int rssi;

  /// @brief The BSSID (MAC address) of the access point.
  std::string bssid;

  /// @brief The channel number the network is operating on.
  uint16_t channel;

  /// @brief True if the network is secured, false if open.
  bool isSecure;

  /**
   * @brief Serializes the scan result into a JSON object.
   *
   * @param obj A reference to an ArduinoJson object where the data will be stored.
   */
  void asJson(JsonObject const obj) const;

  /**
   * @brief Compares two Wi-Fi scan results based on RSSI (signal strength).
   *
   * This operator is used for sorting Wi-Fi networks. A stronger signal (higher RSSI)
   * is considered "less than" a weaker signal to prioritize networks with better reception.
   *
   * @param other Another WiFiScanResult to compare against.
   * @return True if this scan result has a stronger signal than the other.
   */
  bool operator>(WiFiScanResult const &other) const {
    return rssi > other.rssi; // Sort in descending order (the strongest signal first)
  }
};
