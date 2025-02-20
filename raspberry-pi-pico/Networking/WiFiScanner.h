#pragma once

#include <cyw43_ll.h>

class WiFiScanResult;

class WiFiScanner {
public:
  using WiFiScanResults = std::vector<WiFiScanResult>;

  static void scanWiFi();
  static WiFiScanResults const &getScanResults() { return scanResults; }

private:
  static int scanWiFiCallback(void *env, cyw43_ev_scan_result_t const *result);

  static WiFiScanResults scanResults;
};
