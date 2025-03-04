#include <format>
#include <iomanip>
#include <iostream>

#include <lumos-arduino/Logger.h>

#include <DHCP/DHCPServer.h>
#include <HTTP/HTTPServer.h>
#include <HTTPHandlers.h>
#include <Logging/LogServer.h>

#include "MDNS.h"
#include "Network.h"
#include "NetworkUtils.h"
#include "WiFiScanner.h"
#include "WiFiScanResult.h"

//#include "lumos-arduino/Colors.h"

//#include "Models/Pulsate.h"
//#include "Models/Solid.h"
//#include "Models/Triangle.h"

// Secrets are defined in another file called "secrets.h" to avoid committing secrets
// into a public repo. You will need to change the secret values in secrets.h to
// connect your device to your network.

#include "../secrets.h"

std::string Network::hostname = "pico";
std::string Network::ipAddress = "undefined";
std::string Network::macAddress = "undefined";
std::string Network::wifiMode = "undefined";
float Network::pollDuration = 0.0;
float Network::checkLoggerDuration = 0.0;
Renderer* Network::networkRenderer;

DHCPServer Network::dhcpServer;
HTTPServer Network::httpServer;
LogServer Network::logServer;

//Renderer* Network::networkRenderer = nullptr;

void Network::setupHostname(const std::string &baseName) {
  hostname = baseName;

  // std::istringstream macStream(macAddress);
  // std::vector<std::string> parts;
  // std::string segment;
  //
  // // Every segment must be two characters long and be a valid hex number
  // while (std::getline(macStream, segment, ':')) {
  //   if (segment.size() != 2 || !std::ranges::all_of(segment, ::isxdigit)) {
  //     hostname = baseName + "-0000";
  //   }
  //   parts.push_back(segment);
  // }
  //
  // // There must be exactly six parts
  // if (parts.size() != 6) {
  //   hostname = baseName + "-0000";
  // }
  //
  // // Construct the hostname using the basename and the fifth and sixth parts
  // hostname.reserve(baseName.size() + 4);
  // hostname += '-';
  // hostname += parts[4][0];
  // hostname += parts[4][1];
  // hostname += parts[5][0];
  // hostname += parts[5][1];
}

WiFiScanner::WiFiScanResults const & Network::getScanResults() {
  return WiFiScanner::getInstance().getScanResults();
}

void Network::getStatus(JsonObject obj) {
  obj["hostname"] = hostname;
  obj["ipAddress"] = ipAddress;
  obj["macAddress"] = macAddress;

  obj["wiFiMode"] = wifiMode;
  auto const scanResultsArray = obj["wiFiScanResults"].to<JsonArray>();
  for (const auto &scanResult : WiFiScanner::getInstance().getScanResults()) {
    scanResult.asJson(scanResultsArray.add<JsonObject>());
  }

  obj["pollDuration"] = pollDuration;
  obj["checkLoggerDuration"] = checkLoggerDuration;
  httpServer.getStatus(obj["httpServer"].to<JsonObject>());
}

// Connect to an existing access point. Returns true on success, false if did not connect.
bool Network::setupWiFiStation(char const *ssid, char const *password) {
//  // Set up the renderer with a strobing model for while we connect
//  Color c = Colors::makeColor(127, 127, 255);
//  ModelPtr triangle = std::make_shared<Triangle>(0.4, 1.0, c);
//  ModelPtr pulsate = std::make_shared<Pulsate>(0.2, 1.0, 0.1, 0.9, triangle);
//  networkRenderer->setModel(pulsate);

  // Connect to an access point in station mode
  cyw43_arch_enable_sta_mode();
  logger << "Enabled Wi-Fi station mode" << std::endl;

  // Setup Wi-Fi station mode
  logger << "Connecting to " << ssid << "..." << std::endl;
  auto const connect_error = cyw43_arch_wifi_connect_timeout_ms(
    ssid, password, CYW43_AUTH_WPA2_AES_PSK, 15000);
  if (connect_error) {
    logger << "Connection failed: " << cyw43ErrStr(connect_error) << std::endl;
    return false;
  }

  // Success
  logger << "Connected to " << ssid << std::endl;

  // Save the network properties so we can report them later
  ipAddress = ipAddrToString(cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr);
  macAddress = macAddrToString(cyw43_state.netif[CYW43_ITF_STA].hwaddr);

  // Save the Wi-Fi mode
  wifiMode = "station";

  return true;
}

// Create our own network using Soft AP mode
bool Network::setupWiFiSoftAP(std::string const &ssid, std::string const &password) {
  // We need to pass in NULL for no password
  auto const pw = password.empty() ? nullptr : password.c_str();

  // Setup Wi-Fi soft AP mode
  cyw43_arch_enable_ap_mode(ssid.c_str(), pw, CYW43_AUTH_WPA2_AES_PSK);

  // Check if the AP interface is up
  if (!(cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP)) {
    logger << "Failed to bring up the Access Point interface." << std::endl;
    return false;
  }

  wifiMode = "softAP";

  // Assign a default IP address to the AP
  ip4_addr_t ip;
  IP4_ADDR(&ip, 192, 168, 27, 1);
  netif_set_ipaddr(&cyw43_state.netif[CYW43_ITF_AP], &ip);

  ipAddress = ipAddrToString(ip.addr);
  macAddress = macAddrToString(cyw43_state.netif[CYW43_ITF_AP].hwaddr);
  wifiMode = "access_point";

  logger << "Soft Access Point started: SSID=" << ssid
    << ", IP=" << ipAddress
    << ", MAC=" << macAddress
    << ", flags=0x" << std::hex << static_cast<int>(cyw43_state.netif[CYW43_ITF_AP].flags) << std::dec
    << std::endl;

  // Start the dhcp server
  dhcpServer.start();
  logger << "DHCP server started" << std::endl;

////  // TODO  Log each time a station connects or disconnects
////  WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected& evt) {
////      logger << "Station disconnected " << macToString(evt.mac) << std::endl;
////  });
////
////  logger << "Soft AP status: " << softAPStarted ? "Ready" : "Failed" << std::endl;
////  logger << "Soft AP IP address: " << WiFi.softAPIP().toString() << str::endl;
////  logger << "Soft AP MAC address = " << WiFi.softAPmacAddress() << str::endl
////  logger << "Soft AP SSID = " << WiFi.softAPSSID() << str::endl
////  logger << "Soft AP PSK = " << WiFi.softAPPSK() << str::endl
////  logger << "Soft AP has " << WiFi.softAPgetStationNum() << "stations connected\n" << str::endl

  return true;
}

// Set up the web server and handlers
void Network::setupHTTP() {
  HTTPHandlers::setup(httpServer);

  httpServer.start();
}

//// Server used for logging.
//WiFiServer Network::logServer(8000);
//WiFiClient Network::logClient;

// Check to see if the network logger needs to be setup or torn down
void Network::checkLogger() {
//  if (!logClient) {
//    // No log client. Check the server for new clients.
//    logClient = logServer.available();
//    if (logClient) {
//      // We've got a new log client.
//      Logger::setStream(&logClient);
//      logger << "Connected to " << hostname << "..." << std::endl;
//    }
//  }
//
//  if (logClient && !logClient.connected()) {
//    // Not connected anymore, switch logging back to serial.
//    Logger::setStream(&Serial);
//    logClient.stop();
//  }
}

std::string_view Network::getSoftAPStatus() {
  if (cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP) {
    return "Soft AP is active";
  }

  return "Soft AP is NOT active";
}

std::string_view Network::getStationModeStatus() {
  switch (int const status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
    case CYW43_LINK_DOWN:
      return "WiFi is in station mode but NOT connected";
    case CYW43_LINK_JOIN:
      return "WiFi is in the process of joining an access point";
    case CYW43_LINK_NOIP:
      return "WiFi is connected but does NOT have an IP address yet";
    case CYW43_LINK_UP:
      return "WiFi is fully connected with an IP address";
    default:
      return "Unknown WiFi state: " + status;
  }
}

// One-stop to set up all the network components
//void Network::setup(Renderer *renderer) {
void Network::setup() {
  WiFiScanner::getInstance().scanWiFi();

  // Use this renderer if we ever want to use the LEDs for network status
//  networkRenderer = renderer;

  // First try to connect to a known base station
  // bool networkDidConnect = setupWiFiStation(SECRET_SSID, SECRET_PASSWORD);

  // If didn't connect, then start up our own soft access point
  // if (!networkDidConnect) {
    setupWiFiSoftAP(hostname, "picopico");
  // }

  // Set up the hostname for this device
  setupHostname("pico");

  setupHTTP();
  MDNS::setup(hostname);

  logServer.init();
  logger << "Network set up complete, host " << hostname << ".local (" << ipAddress << ')' << std::endl;
}

void Network::loop() {
  // Call poll to give the network a change run each iteration
  float const beforePollMS = to_ms_since_boot(get_absolute_time());
  cyw43_arch_poll();
  pollDuration = (to_ms_since_boot(get_absolute_time()) - beforePollMS) / 1000.0f;

  // Check to see if the network logger needs to be set up or torn down
  float const beforeCheckLoggerMS = to_ms_since_boot(get_absolute_time());
  checkLogger();
  checkLoggerDuration = (to_ms_since_boot(get_absolute_time()) - beforeCheckLoggerMS) / 1000.0f;
}
