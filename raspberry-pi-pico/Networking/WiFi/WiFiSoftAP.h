#pragma once

#include <string>

#include <Networking/DHCP/DHCPServer.h>

class WiFiSoftAP {
public:
  static WiFiSoftAP &getInstance();

  bool start(std::string const &ssid, std::string const &password);
  bool stop();

  /// @brief Get the IP address assigned to the Soft AP.
  /// @return A string view representing the IP address.
  std::string_view getIPAddress() const { return ipAddress; }

  /// @brief Get the MAC address of the Soft AP.
  /// @return A string view representing the MAC address.
  std::string_view getMacAddress() const { return macAddress; }

  std::string_view getSoftAPStatus() const;

private:
  std::string ipAddress = "undefined";
  std::string macAddress = "undefined";

  DHCPServer dhcpServer;
};
