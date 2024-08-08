#include <iomanip>
#include <iostream>

#include <pico/cyw43_arch.h>

//#include "Filesystem.h"
#include "Handlers.h"
#include "HTTP/HTTPServer.h"
#include "Network.h"

//#include "lumos-arduino/Colors.h"
//#include "lumos-arduino/Logger.h"

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

// Server used for HTTP requests
HTTPServer Network::httpServer;

//// Server used for logging.
//WiFiServer Network::logServer(8000);
//WiFiClient Network::logClient;

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

std::string Network::getStatus() {
  std::ostringstream oss;
  oss << "hostname: " << hostname << std::endl;
  oss << "ipAddress: " << ipAddress << std::endl;
  oss << "macAddress: " << macAddress << std::endl;
  oss << "wifiMode: " << wifiMode << std::endl;

  return oss.str();
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
  printf("Enabled Wi-Fi station mode\n");
  wifiMode = "station";

  // Setup Wi-Fi station mode
  printf("Connecting to %s...\n", SECRET_SSID);
  int connect_error = cyw43_arch_wifi_connect_timeout_ms(
          SECRET_SSID, SECRET_PASSWORD,
          CYW43_AUTH_WPA2_AES_PSK, 10000);
  if (connect_error) {
    printf("Failed to connect: error=%d\n", connect_error);
    return false;
  }

  // Save the network properties so we can report them later
  ipAddress = ipAddrToString(cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr);
  macAddress = macAddrToString(cyw43_state.netif[CYW43_ITF_STA].hwaddr);

  std::cout << "Connected to Wi-Fi, IP Address: " << ipAddress << std::endl;

  // Success
  return true;
}

// Create our own network using Soft AP mode
// NOTE: This code does not work yet. It does create a new network, but it's
//       not possible to connect to the web server yet.
void Network::setupWiFiSoftAP() {
//  // Setup wifi soft AP mode
//  cyw43_arch_enable_ap_mode("crystal.local", nullptr, CYW43_AUTH_WPA2_AES_PSK);
//  printf("Soft AP started: SSID = %s\n", "crystal.local");
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
////      Logger::logf("Station disconnected: %s\n", macToString(evt.mac).c_str());
////  });
////
////  Serial.printf("Soft AP status: %s\n", softAPStarted ? "Ready" : "Failed");
////  Serial.printf("Soft AP IP address: %s\n", WiFi.softAPIP().toString().c_str());
////  Serial.printf("Soft AP MAC address = %s\n", WiFi.softAPmacAddress().c_str());
////  Serial.printf("Soft AP SSID = %s\n", WiFi.softAPSSID().c_str());
////  Serial.printf("Soft AP PSK = %s\n", WiFi.softAPPSK().c_str());
////  Serial.printf("Soft AP has %d stations connected\n", WiFi.softAPgetStationNum());
}

// Set up the web server and handlers
void Network::setupHTTP() {
  printf("Starting HTTP server\n");
  httpServer.init();

//  server.onGet("/solid", handleSolid);
//  server.on("/", HTTP_GET, handleRoot);
//  server.on("/crystal.css", HTTP_GET, handleCSS);
//  server.on("/crystal.js", HTTP_GET, handleJS);

  httpServer.onGet("/status", handleStatus);

//  server.on("/brightness", HTTP_GET, handleGetBrightness);
//  server.on("/brightness", HTTP_PUT, handleSetBrightness);
//
//  server.on("/crystal", HTTP_PUT, handleCrystal);
//  server.on("/rainbow", HTTP_PUT, handleRainbow);
//  server.on("/flame", HTTP_GET, handleFlame);
  httpServer.onGet("/solid", handleSolid);
//  server.on("/warpcore", HTTP_PUT, handleWarpCore);
//  server.on("/jacobsladder", HTTP_PUT, handleJacobsLadder);
//  server.on("/demo1", HTTP_GET, handleDemo1);
//  server.on("/demo2", HTTP_GET, handleDemo2);
//  server.on("/demo3", HTTP_GET, handleDemo3);
//
//  server.begin();
//  Serial.println("HTTP server started");
}

// Set up an MDNS responder, so we can be found by <host>.local instead of IP address
void Network::setupMDNS() {
//  // Set the hostname for mDNS
////  cyw43_arch_set_hostname(hostname);
//
//  // Advertise the HTTP service
//  cyw43_arch_mdns_add_service(hostname, "_http._tcp.local", 80, "path=/");
}

// Check to see if the network logger needs to be setup or torn down
void Network::checkLogger() {
//  if (!logClient) {
//    // No log client. Check the server for new clients.
//    logClient = logServer.available();
//    if (logClient) {
//      // We've got a new log client.
//      Logger::setStream(&logClient);
//      Logger::logf("Connected to %s...\n", hostname.c_str());
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

  setupHTTP();
  setupMDNS();

//  logServer.begin();
//  Logger::logMsgLn("Network set up complete");
}

void Network::loop() {
  // Call poll to give the network a change run each iteration
  cyw43_arch_poll();

  checkLogger();
}
