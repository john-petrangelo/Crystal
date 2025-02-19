#include "NetworkUtils.h"
#include "WiFiScanResult.h"

void WiFiScanResult::asJson(JsonObject const obj) const {
  obj["ssid"] = ssid;
  obj["bssid"] = macAddrToString(bssid.data());
  obj["rssi"] = rssi;
  obj["channel"] = channel;
  obj["isSecure"] = isSecure;
}
