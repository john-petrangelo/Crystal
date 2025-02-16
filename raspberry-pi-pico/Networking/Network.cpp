#include <format>
#include <iomanip>
#include <iostream>

// #include <pico/cyw43_arch.h>

#include "lumos-arduino/Logger.h"

#include "DHCPServer.h"
#include "Handlers.h"
#include "HTTP/HTTPServer.h"
#include "Logging/LogServer.h"
#include "MDNS.h"
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
LogServer Network::logServer;

std::vector<Network::ScanResult> Network::scanResults;

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

void Network::getStatus(JsonObject obj) {
  obj["hostname"] = hostname;
  obj["ipAddress"] = ipAddress;
  obj["macAddress"] = macAddress;
  obj["wifiMode"] = wifiMode;
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
// NOTE: This code does not work yet. It does create a new network, but it's
//       not possible to connect to the web server yet.
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
  if (!httpServer.init()) {
    // HTTP server failed to initialize. Abort.
    return;
  }

  httpServer.onGet("/", handleRoot);
  httpServer.onGet("/crystal.css", handleCSS);
  httpServer.onGet("/crystal.js", handleJS);

  httpServer.onGet("/status", handleStatus);

  httpServer.onGet("/brightness", handleGetBrightness);
  httpServer.onPut("/brightness", handleSetBrightness);

  httpServer.onGet("/gamma", handleGetGamma);
  httpServer.onPut("/gamma", handleSetGamma);

  httpServer.onPut("/crystal", handleCrystal);
  httpServer.onPut("/rainbow", handleRainbow);
  httpServer.onPut("/flame", handleFlame);
  httpServer.onPut("/solid", handleSolid);
  httpServer.onPut("/warpcore", handleWarpCore);
  httpServer.onPut("/jacobsladder", handleJacobsLadder);
  httpServer.onPut("/demo1", handleDemo1);
  httpServer.onPut("/demo2", handleDemo2);
  httpServer.onPut("/demo3", handleDemo3);
  httpServer.onGet("/data", handleGetData);
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

char const * getSoftAPStatus() {
  if (cyw43_state.netif[CYW43_ITF_AP].flags & NETIF_FLAG_UP) {
    return "Soft AP is active";
  }

  return "Soft AP is NOT active";
}

std::string getStationModeStatus() {
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

int Network::scanWiFiCallback(void * /*env*/, cyw43_ev_scan_result_t const *result) {
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

  // Convert SSID to a string, the source array may not be null-terminated
  std::string const ssid(reinterpret_cast<char const *>(result->ssid), result->ssid_len);

  ScanResult scanResult = {
    .ssid = ssid,
    .rssi = result->rssi,
    .channel = result->channel,
    .isSecure = result->auth_mode != 0
  };
  std::copy(std::begin(result->bssid), std::end(result->bssid), std::begin(scanResult.bssid));

  // Add to the list of scan results
  scanResults.push_back(scanResult);

  return SCAN_CALLBACK_OK;
}

void logScanResults(std::vector<Network::ScanResult> const &results) {
  logger << "Logging stored scan results..." << std::endl;
  for (auto const &result : results) {
    logger
      << "  Wi-Fi scan found SSID=\"" << result.ssid << "\""
      << " BSSID=" << macAddrToString(result.bssid)
      << " RSSI=" << result.rssi
      << " Channel=" << result.channel
      << " Security=" << (result.isSecure ? "Secured" : "Open")
      << std::endl;
  }
}

void Network::scanWiFi() {
  cyw43_wifi_scan_options_t scan_options = {};
  scan_options.ssid_len = 0; /*all*/
  scan_options.scan_type = 0; /*active*/

  // Clear out any previous scan results
  scanResults.clear();

  logger << "Starting Wi-Fi scan..." << std::endl;
  cyw43_arch_enable_sta_mode();
  cyw43_wifi_scan(&cyw43_state, &scan_options, nullptr, scanWiFiCallback);

  // TODO Add timeout for safety
  while (cyw43_wifi_scan_active(&cyw43_state)) {
    cyw43_arch_poll();
    sleep_ms(100);
  }

  std::ranges::sort(scanResults,
                    [](ScanResult const &a, ScanResult const &b) { return a.rssi > b.rssi; });

  logger << "Finished Wi-Fi scan" << std::endl;
}

// One-stop to set up all the network components
//void Network::setup(Renderer *renderer) {
void Network::setup() {
  scanWiFi();

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

  logScanResults(scanResults);
}

void Network::loop() {
  // Call poll to give the network a change run each iteration
  float const beforePollMS = to_ms_since_boot(get_absolute_time());
  cyw43_arch_poll();
  pollDuration = (to_ms_since_boot(get_absolute_time()) - beforePollMS) / 1000.0f;

  // Check to see if the network logger needs to be setup or torn down
  float const beforeCheckLoggerMS = to_ms_since_boot(get_absolute_time());
  checkLogger();
  checkLoggerDuration = (to_ms_since_boot(get_absolute_time()) - beforeCheckLoggerMS) / 1000.0f;
}
