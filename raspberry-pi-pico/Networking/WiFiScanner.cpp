#include <ostream>
#include <vector>

#include <pico/cyw43_arch.h>

#include "lumos-arduino/Logger.h"

#include "NetworkUtils.h"
#include "WiFiScanner.h"
#include "WiFiScanResult.h"

WiFiScanner &WiFiScanner::getInstance() {
  static WiFiScanner instance;
  return instance;
}


int WiFiScanner::scanWiFiCallback(void * env, cyw43_ev_scan_result_t const *result) {
  // Return 0 to continue scanning, return 1 to stop scanning
  constexpr int SCAN_CALLBACK_OK = 0;

  if (!result) {
    logger << "WiFi scan callback with null result" << std::endl;
    return SCAN_CALLBACK_OK;
  }

  if (result->ssid_len == 0) {
    logger << "WiFi scan found an SSID with length 0, skipping" << std::endl;
    return SCAN_CALLBACK_OK;
  }

  // Convert `env` back to `WiFiScanner*`
  auto *scanner = static_cast<WiFiScanner *>(env);

  // Convert SSID to a string, the source array may not be null-terminated
  std::string const ssid(reinterpret_cast<char const *>(result->ssid), result->ssid_len);

  WiFiScanResult const scanResult(ssid, result->rssi, result->bssid, result->channel, result->auth_mode != 0);

  // Add to the list of scan results
  scanner->scanResults.push_back(scanResult);

  return SCAN_CALLBACK_OK;
}

void WiFiScanner::scanWiFi() {
  // Clear out any previous scan results
  scanResults.clear();

  // Need to be in station mode and not connected to a network to scan
  cyw43_arch_enable_sta_mode();

  // Start the scan
  logger << "Starting Wi-Fi scan..." << std::endl;
  cyw43_wifi_scan_options_t scan_options = {};
  scan_options.ssid_len = 0; /*all*/
  scan_options.scan_type = 0; /*active*/
  auto const err = cyw43_wifi_scan(&cyw43_state, &scan_options, this, scanWiFiCallback);
  if (err) {
    logger << "Wi-Fi scan failed: " << cyw43ErrStr(err) << std::endl;
    return;
  }

  // Wait this long before giving up on the scan completing
  auto constexpr SCAN_TIMEOUT_MS = 3000;

  // Save the start time so we can calculate the elapsed time
  auto const start_time_ms = to_ms_since_boot(get_absolute_time());

  // Wait for the scan to complete or the timeout elapsed
  while (cyw43_wifi_scan_active(&cyw43_state)) {
    // Give the network a chance to run
    cyw43_arch_poll();

    // Check if the scan has timed out
    auto const elapsed_time_ms = absolute_time_diff_us(start_time_ms, get_absolute_time()) / 1000;
    if (elapsed_time_ms > SCAN_TIMEOUT_MS) {
      logger << "Wi-Fi scan timed out after " << elapsed_time_ms << "ms" << std::endl;
      break;
    }

    // Wait a short time and repeat
    sleep_ms(100);
  }

  // Sort scan results by RSSI (strongest signal first)
  std::ranges::sort(scanResults,
      [](WiFiScanResult const &a, WiFiScanResult const &b) { return a.rssi > b.rssi; });

  logger << "Finished Wi-Fi scan" << std::endl;
}

