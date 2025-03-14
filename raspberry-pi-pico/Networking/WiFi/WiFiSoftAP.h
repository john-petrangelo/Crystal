#pragma once

#include <string>

#include <ArduinoJson.h>

#include <Networking/DHCP/DHCPServer.h>

class WiFiSoftAP {
public:
  /**
   * @brief Retrieves the singleton instance of WiFiSoftAP.
   *
   * @return A reference to the single WiFiSoftAP instance.
   *
   * Ensures only one instance of WiFiSoftAP exists and provides global access to it.
   */
  static WiFiSoftAP &getInstance();

  /**
   * @brief Starts the Wi-Fi Soft Access Point (SoftAP) mode.
   *
   * @param baseSSID The base SSID for the network.
   * @param password The Wi-Fi password (empty for an open network).
   * @param appendMac If true, appends a portion of the MAC address to the SSID.
   * @return True if the SoftAP was started successfully, false otherwise.
   *
   * Initializes the SoftAP, sets up networking, and starts the DHCP server.
   * Fails if the SoftAP is already running.
   */
  bool start(std::string const &baseSSID, std::string const &password, bool appendMac = true);

  /**
   * @brief Stops the Wi-Fi Soft Access Point (SoftAP) mode.
   *
   * @return True if the SoftAP was stopped successfully, false otherwise.
   *
   * Disables the SoftAP, stops the DHCP server, and resets network settings.
   * Fails if the SoftAP is not currently running.
   */
  bool stop();

  /**
   * @brief Retrieves the current IP address assigned to the SoftAP.
   * @return A constant reference to the IP address string.
   *
   * This function provides the IP address of the SoftAP interface.
   */
  std::string const & getIPAddress() const { return ipAddress; }

  /**
   * @brief Retrieves the MAC address of the SoftAP.
   * @return A constant reference to the MAC address string.
   *
   * This function returns the MAC address assigned to the SoftAP.
   */
  std::string const & getMacAddress() const { return macAddress; }


  /**
   * @brief Populates a JSON object with the current status of the Soft AP.
   *
   * @param obj The JSON object to populate with status information.
   *
   * This function adds details such as the SSID, IP address, MAC address,
   * network status, and a list of connected stations.
   */
  void getStatus(JsonObject obj) const;

private:
  /**
   * @brief Checks if the SoftAP is currently active.
   *
   * Queries the network stack to determine if the SoftAP is running.
   *
   * @return true if the SoftAP is active, false otherwise.
   */
  bool isUp() const;

  /**
   * @brief Generates an SSID based on the base name, optionally appending a unique MAC address suffix.
   *
   * This function constructs an SSID by taking the provided base name and optionally appending
   * a four-character suffix derived from the device's MAC address. The suffix consists of
   * specific characters from the MAC address to help differentiate multiple SoftAP instances.
   *
   * @param baseSSID The base SSID to use.
   * @param appendMac If true, appends a unique identifier from the MAC address to the SSID.
   * @return A formatted SSID string, either with or without the MAC suffix.
   *
   * @note If the MAC address is not available (length < 17), the base SSID is returned as-is.
   */
  std::string generateSSID(std::string const &baseSSID, bool appendMac) const;

  /**
   * @brief Retrieves a list of currently connected station MAC addresses.
   *
   * This function queries the SoftAP interface for connected stations and returns their MAC addresses.
   * It filters out any empty or zeroed-out MAC entries, which may be present due to how the
   * `cyw43_wifi_ap_get_stas` function returns data.
   *
   * @return A vector of MAC addresses, each represented as a std::array<uint8_t, 6>.
   */
  std::vector<std::array<uint8_t, 6>> getConnectedStations() const;

  /**
   * @brief Stores the current SSID of the SoftAP.
   *
   * This value is set when the SoftAP starts and may include a MAC address suffix
   * if configured to do so. It is reset to `"undefined"` when the AP is stopped.
   */
  std::string ssid;

  /**
   * @brief Holds the SoftAP's assigned IP address.
   *
   * The IP address is assigned when the SoftAP starts and is typically a static
   * address (e.g., 192.168.27.1). It is reset to `"undefined"` when the AP is stopped.
   */
  std::string ipAddress;

  /**
   * @brief Stores the MAC address of the SoftAP interface.
   *
   * The MAC address is retrieved when the SoftAP starts. This value remains constant
   * for a given device but is reset to `"undefined"` when the AP is stopped.
   */
  std::string macAddress;

  /**
   * @brief Manages DHCP functionality for the SoftAP.
   *
   * This DHCP server assigns IP addresses to connected clients. It is started
   * when the SoftAP is enabled and stopped when the SoftAP is disabled.
   */
  DHCPServer dhcpServer;
};
