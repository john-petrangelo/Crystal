#include "WiFiScanResult.h"

/**
 * @brief Converts the WiFiScanResult object into a JSON representation.
 *
 * This function populates a given JSON object with the scan result data,
 * including SSID, BSSID (as a MAC address string), RSSI, channel, and security status.
 *
 * @param obj The JSON object to populate with Wi-Fi scan result data.
 */
void WiFiScanResult::asJson(JsonObject const obj) const {
  obj["ssid"] = ssid;
  obj["bssid"] = bssid;
  obj["rssi"] = rssi;
  obj["channel"] = channel;
  obj["isSecure"] = isSecure;
}
