#pragma once

class WiFiScanResult;

class WiFiScanner {
public:
  using WiFiScanResults = std::vector<WiFiScanResult>;

  /**
   * @brief Get the singleton instance of the WiFiScanner.
   * @return The reference to the single instance of WiFiScanner.
   */
  static WiFiScanner &getInstance();

  /**
   * @brief Start a scan for available Wi-Fi networks and store scan results
   */
  void scanWiFi();

  /**
   * @brief Get the scan results from the last Wi-Fi scan.
   * @return A const reference to the list of scan results.
   */
  WiFiScanResults const &getScanResults() { return scanResults; }

private:
  // Enforce the singleton pattern
  WiFiScanner() = default;
  ~WiFiScanner() = default;
  WiFiScanner(WiFiScanner const &) = delete;
  WiFiScanner(WiFiScanner &&) = delete;
  WiFiScanner &operator=(WiFiScanner const &) = delete;
  WiFiScanner &operator=(WiFiScanner &&) = delete;

  /**
   * @brief Callback function for processing Wi-Fi scan results, passed to the LWIP Wi-Fi scan function.
   * @param env Pointer to user-defined environment (unused in this case).
   * @param result The scan result structure, or nullptr when scanning is complete.
   * @return 0 to continue scanning, 1 to stop scanning early.
   */
  static int scanWiFiCallback(void *env, cyw43_ev_scan_result_t const *result);

  WiFiScanResults scanResults;
};
