#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include "src/lumos-arduino/Logger.h"

// Secrets are defined in another file called "secrets.h" to avoid commiting secrets
// into a public repo. You will need to change the secret values in secrets.h to
// connect your device to your network.
#include "secrets.h"

// Setup wifi in soft AP mode. The default is to join an
// existing network in STATION mode.
// #define USE_SOFT_AP

// Event handlers for soft AP connect and disconnect events
WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

// Server used for logging.
WiFiServer logServer(8000);
WiFiClient logClient;

// One-stop to set up all of the network components
void setupNetwork() {
  String macAddress = WiFi.macAddress();
  if (macAddress == "E8:DB:84:98:7F:C3") {
    hostname = "shard";
  } else if (macAddress = "84:CC:A8:81:0A:53") {
    hostname = "crystal";
  } else {
    hostname = WiFi.hostname();
  }

#ifdef USE_SOFT_AP
  setupWiFiSoftAP();
#else
  setupWiFiStation();
#endif

  setupHTTP();
//  setupMDNS();
  setupOTA();
}

// Connect to an existing access point
void setupWiFiStation() {
  // Setup WiFi station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  logServer.begin();

  // If we recognize the MAC address, use a different hostname specific to that MAC address
  Serial.print("Network: ");
  Serial.println(SECRET_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Hostname: ");
  Serial.println(hostname);
}

// Create our own network using Soft AP mode
void setupWiFiSoftAP() {
  // Setup wifi soft AP mode
  bool softAPStarted = WiFi.softAP(hostname);
  // Call "onStationConnected" each time a station connects
  stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onStationConnected);
  // Call "onStationDisconnected" each time a station disconnects
  stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(&onStationDisconnected);

  Serial.printf("Soft AP status: %s\n", softAPStarted ? "Ready" : "Failed");
  Serial.printf("Soft AP IP address: %s\n", WiFi.softAPIP().toString().c_str());
  Serial.printf("Soft AP MAC address = %s\n", WiFi.softAPmacAddress().c_str());
  Serial.printf("Soft AP SSID = %s\n", WiFi.softAPSSID().c_str());
  Serial.printf("Soft AP PSK = %s\n", WiFi.softAPPSK().c_str());
  Serial.printf("Soft AP has %d stations connected\n", WiFi.softAPgetStationNum());
}

// Setup the web server and handlers
void setupHTTP() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/crystal.css", HTTP_GET, handleCSS);
  server.on("/crystal.js", HTTP_GET, handleJS);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound(handleNotFound);

  server.on("/brightness", HTTP_GET, handleGetBrightness);
  server.on("/brightness", HTTP_PUT, handleSetBrightness);

  server.on("/crystal", HTTP_PUT, handleCrystal);
  server.on("/rainbow", HTTP_PUT, handleRainbow);
  server.on("/flame", HTTP_GET, handleFlame);
  server.on("/solid", HTTP_GET, handleSolid);
  server.on("/demo1", HTTP_GET, handleDemo1);
  server.on("/demo2", HTTP_GET, handleDemo2);
  server.on("/demo3", HTTP_GET, handleDemo3);

  server.begin();
  Serial.println("HTTP server started");
}

// Setup an MDNS responder so we can be found by <host>.local instead of IP address
void setupMDNS() {
  if (MDNS.begin("hostname")) {
    Serial.println("MDNS responder started: crystal.local");
  }
}

// Setup OTA updates
void setupOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(hostname.c_str());

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    LittleFS.end();

    Serial.println("OTA Start");

    // TODO Fix
//    Patterns::setSolidColor(pixels, 0, strip.PixelCount(), BLACK);
//    Patterns::setSolidColor(pixels, strip.PixelCount()-1, strip.PixelCount(), WHITE);
//    Patterns::applyPixels(strip, pixels);
//    strip.show();
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));

      // TODO Fix
//    int otaPixels = progress / (total / strip.PixelCount());
//    Serial.printf("progress: %u  total: %u  otaPixels: %u\r", progress, total, otaPixels);
//    Patterns::setSolidColor(pixels, 0, otaPixels, GREEN);
//    Patterns::applyPixels(strip, pixels);
//    strip.show();
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");

    // TODO Fix
//    Patterns::setSolidColor(strip, pixels, BLACK);
//    Patterns::applyPixels(strip, pixels);
//    strip.show();
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");

    // TODO Fix
//    Patterns::setSolidColor(strip, pixels, RED);
//    Patterns::applyPixels(strip, pixels);
//    strip.show();
  });
  
  ArduinoOTA.begin();
  Serial.println("OTA ready");
}

void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
  Logger::logf("Station connected: %s\n", macToString(evt.mac).c_str());
}

void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
  Logger::logf("Station disconnected: %s\n", macToString(evt.mac).c_str());
}

void loopNetwork() {
  // Check for network activity.
  server.handleClient();
  MDNS.update();
  ArduinoOTA.handle();  
}

// Check to see if the network logger needs to be setup or torn down
void loopLogger() {
  if (!logClient) {
    // No log client. Check the server for new clients.
    logClient = logServer.available();
    if (logClient) {
      // We've got a new log client.
      Logger::setStream(&logClient);
      Logger::logMsgLn("Connected to WiFi logging...");
    }
  }

  if (logClient && !logClient.connected()) {
    // Not connected anymore, switch logging back to serial.
    Logger::setStream(&Serial);
    logClient.stop();
  }
}

String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}
