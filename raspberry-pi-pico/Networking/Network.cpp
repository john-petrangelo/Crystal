#include <iomanip>
#include <iostream>

#include <lumos-arduino/Logger.h>

#include <Logging/LogServer.h>

#include "DHCP/DHCPServer.h"
#include "HTTP/HTTPServer.h"
#include "MDNS/MDNSServer.h"
#include "WiFi/WiFiScanner.h"
#include "WiFi/WiFiSoftAP.h"

#include "HTTPHandlers.h"
#include "Network.h"
#include "NetworkUtils.h"

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
MDNSServer Network::mdnsServer;
LogServer Network::logServer;

//Renderer* Network::networkRenderer = nullptr;

/**
 * @brief Starts the HTTP server.
 *
 * This function starts the HTTP server, allowing it to accept incoming requests.
 */
void Network::startHTTPServer() {
  httpServer.start();
}

/**
 * @brief Stops the HTTP server.
 *
 * This function stops the HTTP server, closing any active connections and
 * preventing new requests from being processed.
 */
void Network::stopHTTPServer() {
  httpServer.stop();
}

/**
 * @brief Checks if the HTTP server is running.
 *
 * @return true if the HTTP server is currently running, false otherwise.
 */
bool Network::httpServerIsRunning() {
  return httpServer.isRunning();
}

void Network::startDHCPServer() {
  dhcpServer.start();
}

void Network::stopDHCPServer() {
  dhcpServer.stop();
}

bool Network::dhcpServerIsRunning() {
  return dhcpServer.isRunning();
}

void Network::startMDNSServer() {
  mdnsServer.start();
}

void Network::stopMDNSServer() {
  mdnsServer.stop();
}

bool Network::mdnsServerIsRunning() {
  return mdnsServer.isRunning();
}

void Network::setupHostname(const std::string &baseName) {
  hostname = baseName;

  // std::istringstream macStream(macAddress);
  // std::vector<std::string> parts;
  // std::string segment;
  //
  // // Every segment must be two characters long and be a valid hex number
  // while (std::getline(macStream, segment, ':')) {
  //   if (segment.size() != 2 || !std::all_of(segment.begin(), segment.end(), ::isxdigit)) {
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
  WiFiSoftAP::getInstance().getStatus(obj["softAP"].to<JsonObject>());
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
    WiFiSoftAP &softAP = WiFiSoftAP::getInstance();
    if (softAP.start(hostname, "picopico")) {
      wifiMode = "soft access point";
      ipAddress = softAP.getIPAddress();
      macAddress = softAP.getMacAddress();
    }

  // }

  // Set up the hostname for this device
  setupHostname("pico");

  setupHTTP();
  mdnsServer.init(hostname);
  mdnsServer.start();

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
