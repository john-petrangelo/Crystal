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
#include "WiFi/WiFIStation.h"

//#include "lumos-arduino/Colors.h"

//#include "Models/Pulsate.h"
//#include "Models/Solid.h"
//#include "Models/Triangle.h"

// Secrets are defined in another file called "secrets.h" to avoid committing secrets
// into a public repo. You will need to change the secret values in secrets.h to
// connect your device to your network.

#include "../secrets.h"

std::string Network::hostname = "pico";
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

void Network::startDHCPServer(ip4_addr_t const &ipAddr) {
  dhcpServer.start(ipAddr);
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

WiFiScanner::WiFiScanResults const & Network::getScanResults() {
  return WiFiScanner::getInstance().getScanResults();
}

void Network::getStatus(JsonObject obj) {
  obj["hostname"] = hostname;

  obj["pollDuration"] = pollDuration;
  obj["checkLoggerDuration"] = checkLoggerDuration;
  httpServer.getStatus(obj["httpServer"].to<JsonObject>());
  WiFiSoftAP::getInstance().getStatus(obj["softAP"].to<JsonObject>());
  WiFiStation::getInstance().getStatus(obj["station"].to<JsonObject>());

  auto const scanResultsArray = obj["wiFiScanResults"].to<JsonArray>();
  for (const auto &scanResult : WiFiScanner::getInstance().getScanResults()) {
    scanResult.asJson(scanResultsArray.add<JsonObject>());
  }
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

constexpr bool Network::hasStationMode(WiFiMode wifiMode) {
  switch (wifiMode) {
    case WiFiMode::StationOnly:
    case WiFiMode::StationAndAP:
    case WiFiMode::StationFallbackAP:
      return true;
    default:
      return false;
  }
}

// One-stop to set up all the network components
//void Network::setup(Renderer *renderer) {
void Network::setup(std::string_view const newHostname, WiFiMode wifiMode) {
  // Use this renderer if we ever want to use the LEDs for network status
  //  networkRenderer = renderer;

  // Scan for nearby Wi-Fi networks
  WiFiScanner::getInstance().scanWiFi();

 // Set up the hostname for this device
  hostname = newHostname;

  // Setup Wi-Fi station mode if needed
  bool networkDidConnect = false;
  if (hasStationMode(wifiMode)) {
    networkDidConnect = WiFiStation::getInstance().start(SECRET_SSID, SECRET_PASSWORD);
  }

  // Setup Wi-Fi soft AP mode if needed
  if (wifiMode == WiFiMode::APOnly || wifiMode == WiFiMode::StationAndAP ||
    (wifiMode == WiFiMode::StationFallbackAP && !networkDidConnect)) {
    WiFiSoftAP::getInstance().start(hostname, "picopico");
  }

  // Start the HTTP server and mDNS server
  setupHTTP();
  mdnsServer.init(hostname);
  mdnsServer.start();

  logServer.init();

  logger << "Network set up complete, host " << hostname << ".local" << std::endl;
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
