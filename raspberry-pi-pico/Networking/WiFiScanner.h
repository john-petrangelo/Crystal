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
   * @brief Adds a new Wi-Fi scan result to the list,
   * ensuring uniqueness by SSID and limiting the list to the strongest 10 signals.
   *
   * @param scanResult The new Wi-Fi scan result to be added.
   *
   * This function checks if the SSID from the new scan result already exists in the list:
   * - If it does not exist, the new result is added.
   * - If an entry with the same SSID exists but has a weaker signal (lower RSSI), it is updated with the stronger one.
   *
   * To maintain efficiency, the list is limited to `MAX_RESULTS` by removing the weakest signal when it exceeds this limit.
   */
  void addScanResult(WiFiScanResult const &scanResult);

  /**
   * @brief Callback function for processing Wi-Fi scan results, passed to the LWIP Wi-Fi scan function.
   * @param env Pointer to user-defined environment (unused in this case).
   * @param result The scan result structure, or nullptr when scanning is complete.
   * @return 0 to continue scanning, 1 to stop scanning early.
   */
  static int scanWiFiCallback(void *env, cyw43_ev_scan_result_t const *result);

  /**
   * @brief The maximum number of Wi-Fi scan results to store.
   */
  static constexpr int MAX_RESULTS = 10;

  /**
   * @brief The list of Wi-Fi scan results.
   */
  WiFiScanResults scanResults;
};
