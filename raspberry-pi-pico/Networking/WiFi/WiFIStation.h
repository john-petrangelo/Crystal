#pragma once

#include <string>

#include <ArduinoJson.h>

class WiFiStation {
public:
  /**
   * @brief Retrieves the singleton instance of WiFiSoftAP.
   *
   * @return A reference to the single WiFiSoftAP instance.
   *
   * Ensures only one instance of WiFiSoftAP exists and provides global access to it.
   */
  static WiFiStation &getInstance();

  /**
   * @brief Checks whether the station mode interface is currently active.
   *
   * @return True if the station interface is up and connected, false otherwise.
   */
  bool isUp() const;

  /**
 * @brief Checks if the Wi-Fi station is currently connected to an access point.
 *
 * @return true if the device is connected to a Wi-Fi network, false otherwise.
 *
 * @note This function does not check whether the interface is up (enabled),
 *       only whether it has successfully connected to an access point.
 */
  bool isConnected() const;

  /**
   * @brief Starts the Wi-Fi station mode and attempts to connect to the configured network.
   *
   * This function enables the Wi-Fi station mode and initiates a connection to the
   * specified access point. If the station mode is already active, it returns without
   * making changes.
   *
   * @param ssid The SSID of the Wi-Fi network to connect to.
   * @param password The password for the Wi-Fi network.
   *
   * @return True if the station mode was successfully started, false otherwise.
   */
  bool start(char const *ssid, char const *password);

  /**
   * @brief Stops the Wi-Fi station mode and disconnects from the network.
   *
   * This function disables the Wi-Fi station mode, disconnecting from the current
   * network if connected. If the station mode is not active, it returns without
   * making changes.
   *
   * @return True if the station mode was successfully stopped, false otherwise.
   */
  bool stop();

  /**
   * @brief Retrieves a human-readable string the state of the Wi-Fi station connection.
   *
   * @return A string describing the current state, such as "connected", "joining", or "failed".
   */
  static std::string_view getStateStr();

  /**
   * @brief Populates a JSON object with the current status of the Soft AP.
   *
   * @param obj The JSON object to populate with status information.
   *
   * This function adds details such as the SSID, IP address, MAC address,
   * network status, and a list of connected stations.
   */
  void getStatus(JsonObject obj) const;

  /**
   * @brief Retrieves the current IP address assigned to the SoftAP.
   * @return A constant reference to the IP address string.
   *
   * This function provides the IP address of the SoftAP interface.
   */
  std::string const & getIPAddress() const { return ipAddress; }

  /**
   * @brief Retrieves the MAC address of the SoftAP.
   *
   * @return A constant reference to the MAC address string.
   *
   * This function returns the MAC address assigned to the SoftAP.
   */
  std::string const & getMacAddress() const { return macAddress; }

private:
  /**
   * @brief Stores the current SSID of the SoftAP.
   *
   * This value is set when the SoftAP starts and may include a MAC address suffix
   * if configured to do so. It is reset to `"undefined"` when the AP is stopped.
   */
  std::string ssid = "undefined";

  /**
   * @brief Holds the stations assigned IP address.
   *
   * The IP address is assigned when the station starts and is typically a static
   * address (e.g., 192.168.27.1). It is reset to `"undefined"` when the station is stopped.
   */
  std::string ipAddress;

  /**
   * @brief Stores the MAC address of the station.
   *
   * The MAC address is retrieved when the station starts. This value remains constant
   * for a given device but is reset to `"undefined"` when the station is stopped.
   */
  std::string macAddress;
};
