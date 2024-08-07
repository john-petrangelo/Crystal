#include <pico/cyw43_arch.h>

//#include "Filesystem.h"
#include "Handlers.h"
#include "HTTPServer.h"
#include "Network.h"

//#include "lumos-arduino/Colors.h"
//#include "lumos-arduino/Logger.h"

//#include "Models/Gauge.h"
//#include "Models/Pulsate.h"
//#include "Models/Solid.h"
//#include "Models/Triangle.h"

// Secrets are defined in another file called "secrets.h" to avoid committing secrets
// into a public repo. You will need to change the secret values in secrets.h to
// connect your device to your network.
#include "../secrets.h"

std::string Network::hostname = "pico";
HTTPServer Network::server;

//// Server used for HTTP requests
//ESP8266WebServer Network::server(80);
//
//// Server used for logging.
//WiFiServer Network::logServer(8000);
//WiFiClient Network::logClient;

//Renderer* Network::networkRenderer = nullptr;

static std::string macToString(const unsigned char* mac) {
  constexpr std::size_t MAC_STRING_LENGTH = 17; // 17 characters for MAC
  std::string macString(MAC_STRING_LENGTH, '\0');

  std::snprintf(&macString[0], macString.size() + 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return macString;
}

// Connect to an existing access point. Returns true on success, false if did not connect.
bool Network::setupWiFiStation() {
//  // Set up the renderer with a strobing model for while we connect
//  Color c = Colors::makeColor(127, 127, 255);
//  ModelPtr triangle = std::make_shared<Triangle>(0.4, 1.0, c);
//  ModelPtr pulsate = std::make_shared<Pulsate>(0.2, 1.0, 0.1, 0.9, triangle);
//  networkRenderer->setModel(pulsate);

  // Connect to an access point in station mode
  printf("Enabling Wi-Fi station mode\n");
  cyw43_arch_enable_sta_mode();

  // Setup Wi-Fi station mode
  printf("Connecting to %s...\n", SECRET_SSID);
  int connect_error = cyw43_arch_wifi_connect_timeout_ms(
          SECRET_SSID, SECRET_PASSWORD,
          CYW43_AUTH_WPA2_AES_PSK, 10000);
  if (connect_error) {
    printf("Failed to connect: error=%d\n", connect_error);
    return false;
  }

  auto ip_addr = cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;
  printf("Connected to Wi-Fi, IP Address: %lu.%lu.%lu.%lu\n", ip_addr & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr >> 16) & 0xFF, ip_addr >> 24);

  // Success
  return true;
}

void on_station_connect(const char *mac_address) {
  printf("Station connected: %s\n", mac_address);
}

// Create our own network using Soft AP mode
// NOTE: This code does not work yet. It does create a new network, but it's
//       not possible to connect to the web server yet.
void Network::setupWiFiSoftAP() {
//  // Setup wifi soft AP mode
//  cyw43_arch_enable_ap_mode("crystal.local", nullptr, CYW43_AUTH_WPA2_AES_PSK);
//  printf("Soft AP started: SSID = %s\n", "crystal.local");
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
  server.init();

//  server.onGet("/solid", handleSolid);
  server.onGet("/test", [](const HTTPRequest& request) {
    return HTTPResponse{200, "test passed"};
  });

//  server.on("/", HTTP_GET, handleRoot);
//  server.on("/crystal.css", HTTP_GET, handleCSS);
//  server.on("/crystal.js", HTTP_GET, handleJS);
//  server.on("/status", HTTP_GET, handleStatus);
//  server.onNotFound(handleNotFound);
//
//  server.on("/brightness", HTTP_GET, handleGetBrightness);
//  server.on("/brightness", HTTP_PUT, handleSetBrightness);
//
//  server.on("/crystal", HTTP_PUT, handleCrystal);
//  server.on("/rainbow", HTTP_PUT, handleRainbow);
//  server.on("/flame", HTTP_GET, handleFlame);
  server.onGet("/solid", handleSolid);
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

  // Initialize the Wi-Fi stack
//  if (cyw43_arch_init()) {
//    printf("Failed to initialize Wi-Fi\n");
//    return;
//  }

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
//
//void Network::getStatus(JsonObject obj) {
//  obj["hostname"] = Network::getHostname() + ".local";
//  obj["wifiMACAddress"] = WiFi.macAddress();
//  obj["ipAddress"] = WiFi.localIP().toString();
//  obj["mode"] = "unknown";
//  switch(WiFi.getMode()) {
//    case WIFI_OFF:
//      obj["mode"] = "WIFI_OFF";
//      break;
//    case WIFI_STA:
//      obj["mode"] = "WIFI_STA";
//      break;
//    case WIFI_AP:
//      obj["mode"] = "WIFI_AP";
//      break;
//    case WIFI_AP_STA:
//      obj["mode"] = "WIFI_AP_STA";
//      break;
//    default:
//      obj["mode"] = String(WiFi.getMode());
//      break;
//  }
//  obj["phyMode"] = "unknown";
//  switch(WiFi.getPhyMode()) {
//    case WIFI_PHY_MODE_11B:
//      obj["phyMode"] = "WIFI_PHY_MODE_11B";
//      break;
//    case WIFI_PHY_MODE_11G:
//      obj["phyMode"] = "WIFI_PHY_MODE_11G";
//      break;
//    case WIFI_PHY_MODE_11N:
//      obj["phyMode"] = "WIFI_PHY_MODE_11N";
//      break;
//    default:
//      obj["phyMode"] = String(WiFi.getPhyMode());
//      break;
//  }
//  obj["softAPssid"] = "<ssid TBD>";
//  obj["softAPStationNum"] = WiFi.softAPgetStationNum();
//  obj["softAPIP"] = WiFi.softAPIP().toString();
//  obj["softAPmacAddress"] = WiFi.softAPmacAddress();
//}
