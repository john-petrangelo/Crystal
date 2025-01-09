#include <format>
#include <iomanip>
#include <iostream>

#include <lwip/apps/mdns.h>
#include <pico/cyw43_arch.h>

#include "lumos-arduino/Logger.h"

#include "Handlers.h"
#include "HTTP/HTTPServer.h"
#include "Logging/LogServer.h"
#include "Network.h"

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
Renderer* Network::networkRenderer;

// Server used for HTTP requests
HTTPServer Network::httpServer;

// Server used for logging over the network
LogServer Network::logServer;

//Renderer* Network::networkRenderer = nullptr;

std::string Network::ipAddrToString(u32_t ip) {
  std::ostringstream oss;
  oss << (ip & 0xFF) << '.'
      << ((ip >> 8) & 0xFF) << '.'
      << ((ip >> 16) & 0xFF) << '.'
      << ((ip >> 24) & 0xFF);
  return oss.str();
}

std::string Network::macAddrToString(const uint8_t* mac) {
  std::ostringstream oss;
  for (size_t i = 0; i < 6; ++i) {
    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[i]);
    if (i < 5) {
      oss << ':'; // Add colon separator except for the last byte
    }
  }
  return oss.str();
}

void Network::setupHostname(const std::string &baseName) {
  std::istringstream macStream(macAddress);
  std::vector<std::string> parts;
  std::string segment;

  // Every segment must be two characters long and be a valid hex number
  while (std::getline(macStream, segment, ':')) {
    if (segment.size() != 2 || !std::ranges::all_of(segment, ::isxdigit)) {
      hostname = baseName + "-0000";
    }
    parts.push_back(segment);
  }

  // There must be exactly six parts
  if (parts.size() != 6) {
    hostname = baseName + "-0000";
  }

  // Construct the hostname using the basename and the fifth and sixth parts
  hostname.reserve(baseName.size() + 4);
  hostname += '-';
  hostname += parts[4][0];
  hostname += parts[4][1];
  hostname += parts[5][0];
  hostname += parts[5][1];
}

void Network::getStatus(JsonObject obj) {
  obj["hostname"] = hostname;
  obj["ipAddress"] = ipAddress;
  obj["macAddress"] = macAddress;
  obj["wifiMode"] = wifiMode;
  httpServer.getStatus(obj["httpServer"].to<JsonObject>());
}

// Connect to an existing access point. Returns true on success, false if did not connect.
bool Network::setupWiFiStation() {
//  // Set up the renderer with a strobing model for while we connect
//  Color c = Colors::makeColor(127, 127, 255);
//  ModelPtr triangle = std::make_shared<Triangle>(0.4, 1.0, c);
//  ModelPtr pulsate = std::make_shared<Pulsate>(0.2, 1.0, 0.1, 0.9, triangle);
//  networkRenderer->setModel(pulsate);

  // Connect to an access point in station mode
  cyw43_arch_enable_sta_mode();
  logger << "Enabled Wi-Fi station mode" << std::endl;
  wifiMode = "station";

  // Setup Wi-Fi station mode
  logger << "Connecting to " << SECRET_SSID << "..." << std::endl;
  int connect_error = cyw43_arch_wifi_connect_timeout_ms(
          SECRET_SSID, SECRET_PASSWORD,
          CYW43_AUTH_WPA2_AES_PSK, 15000);
  if (connect_error) {
    logger << "Failed to connect: error=" << connect_error << std::endl;
    return false;
  }

  // Save the network properties so we can report them later
  ipAddress = ipAddrToString(cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr);
  macAddress = macAddrToString(cyw43_state.netif[CYW43_ITF_STA].hwaddr);

  logger << "Connected to " << SECRET_SSID << std::endl;

  // Success
  return true;
}

// Create our own network using Soft AP mode
// NOTE: This code does not work yet. It does create a new network, but it's
//       not possible to connect to the web server yet.
void Network::setupWiFiSoftAP() {
//  // Setup wifi soft AP mode
//  cyw43_arch_enable_ap_mode("crystal.local", nullptr, CYW43_AUTH_WPA2_AES_PSK);
//  logger << "Soft AP started: SSID = " << "crystal.local" << std::endl;
//  wifiMode = "softAP";
//
//  // Set the address we can be reached at
//  ip4_addr_t addr;
//  ip4_addr_t mask;
//  IP4_ADDR(ip_2_ip4(&addr), 192, 168, 4, 189);
//  IP4_ADDR(ip_2_ip4(&mask), 255, 255, 255, 0);
//
//  // Start the dhcp server
//  dhcp_server_t dhcp_server;
//  dhcp_server_init(&dhcp_server, &addr, &mask);
//
//  // Start the dns server
//  dns_server_t dns_server;
//  dns_server_init(&dns_server, &addr);
//
//
////  //   Log each time a station connects or disconnects
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

// Set up an MDNS responder, so we can be found by <host>.local instead of IP address
void Network::mdnsAddServiceTextItem(struct mdns_service *service, void *txt_userdata) {
  if (err_t const res = mdns_resp_add_service_txtitem(service, "path=/", 6); res != ERR_OK) {
    logger << "mDNS Failed to add service TXT record, err=" << res << std::endl;
  } else {
    logger << "mDNS TXT record added successfully" << std::endl;
  }
}

void Network::mdnsExampleReport(netif * netif, u8_t const result, s8_t const service) {
  if (result == MDNS_PROBING_SUCCESSFUL) {
    logger << "mDNS name successfully registered on netif " << static_cast<int>(netif->num) << std::endl;
  } else if (result == MDNS_PROBING_CONFLICT) {
    logger << "mDNS name conflict detected on netif " << netif->num << ", service " << service << std::endl;
  } else {
    logger << "Unknown mDNS status result: " << result << std::endl;
  }
}

void Network::setupMDNS() {
  mdns_resp_register_name_result_cb(mdnsExampleReport);
  mdns_resp_init();

  if (err_t const err = mdns_resp_add_netif(netif_default, hostname.c_str()); err != ERR_OK) {
    logger << "Failed to add netif to mDNS, err=" << err << std::endl;
    return;
  }
  if (err_t const err = mdns_resp_add_service(netif_default, "Luminarium", "_http",
        DNSSD_PROTO_TCP, 80, mdnsAddServiceTextItem, nullptr); err != ERR_OK) {
    logger << "Failed to add mDNS service, err=" << err << std::endl;
    return;
  }

  mdns_resp_announce(netif_default);
  logger << "mDNS service announced successfully" << std::endl;
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

// One-stop to set up all the network components
//void Network::setup(Renderer *renderer) {
void Network::setup() {
  // Use this renderer if we ever want to use the LEDs for network status
//  networkRenderer = renderer;

  // First try to connect to a known base station
  bool networkDidConnect = setupWiFiStation();

  // If didn't connect, then start up our own soft access point
  if (!networkDidConnect) {
    setupWiFiSoftAP();
  }

  // Set up the hostname for this device
  setupHostname("pico");

  setupHTTP();
  setupMDNS();

  logServer.init();
  logger << "Network set up complete, host " << hostname << ".local (" << ipAddress << ')' << std::endl;
}

void Network::loop() {
  // Call poll to give the network a change run each iteration
  cyw43_arch_poll();
  checkLogger();
}
