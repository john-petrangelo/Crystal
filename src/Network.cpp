#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "Animations.h"
#include "Filesystem.h"
#include "Handlers.h"
#include "Network.h"
#include "lumos-arduino/Colors.h"
#include "lumos-arduino/Logger.h"

// Secrets are defined in another file called "secrets.h" to avoid commiting secrets
// into a public repo. You will need to change the secret values in secrets.h to
// connect your device to your network.
#include "../secrets.h"

String Network::hostname = "crystal";

// Setup Wi-Fi in soft AP mode. The default is to join an
// existing network in STATION mode.
// #define USE_SOFT_AP

// Event handlers for soft AP connect and disconnect events
static WiFiEventHandler stationConnectedHandler;
static WiFiEventHandler stationDisconnectedHandler;

// Server used for HTTP requests
ESP8266WebServer Network::server(80);

// Server used for logging.
WiFiServer Network::logServer(8000);
WiFiClient Network::logClient;

Renderer* Network::networkRenderer = nullptr;

static String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return {buf};
}

// Connect to an existing access point
void Network::setupWiFiStation() {
  // Set up the renderer with a strobing model for while we connect
  Color c = Colors::makeColor(127, 127, 255);
  std::shared_ptr<Model> triangle = std::make_shared<Triangle>("triangle", 0.4, 1.0, c);
  std::shared_ptr<Model> pulsate = std::make_shared<Pulsate>("pulsate", 0.2, 1.0, 0.1, 0.9, triangle);
  networkRenderer->setModel(pulsate);

  // Setup WiFi station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    networkRenderer->render();
    delay(10);
  }
  Serial.println("");

  logServer.begin();

  Serial.print("Network: ");
  Serial.println(SECRET_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Hostname: ");
  Serial.println(hostname);
}

// Create our own network using Soft AP mode
void Network::setupWiFiSoftAP() {
  // Setup wifi soft AP mode
  bool softAPStarted = WiFi.softAP(hostname);

  // Log each time a station connects or disconnects
  WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected& evt) {
      Logger::logf("Station connected: %s\n", macToString(evt.mac).c_str());
  });
  WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected& evt) {
      Logger::logf("Station disconnected: %s\n", macToString(evt.mac).c_str());
  });




  Serial.printf("Soft AP status: %s\n", softAPStarted ? "Ready" : "Failed");
  Serial.printf("Soft AP IP address: %s\n", WiFi.softAPIP().toString().c_str());
  Serial.printf("Soft AP MAC address = %s\n", WiFi.softAPmacAddress().c_str());
  Serial.printf("Soft AP SSID = %s\n", WiFi.softAPSSID().c_str());
  Serial.printf("Soft AP PSK = %s\n", WiFi.softAPPSK().c_str());
  Serial.printf("Soft AP has %d stations connected\n", WiFi.softAPgetStationNum());
}

// Setup the web server and handlers
void Network::setupHTTP() {
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
void Network::setupMDNS() {
  if (MDNS.begin("hostname")) {
    Serial.println("MDNS responder started: crystal.local");
  }
}

// Setup OTA updates
void Network::setupOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(hostname.c_str());

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Filesystem::end();

    Logger::logf("OTA Start\n");

    ModelPtr gauge = std::make_shared<GaugeModel>("gauge", 24, GREEN);
    networkRenderer->setModel(gauge);
    networkRenderer->render();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Logger::logf("OTA Progress: %u%%\r", 100 * progress / total);

      std::shared_ptr<GaugeModel> gauge = std::static_pointer_cast<GaugeModel>(networkRenderer->getModel());
      if (gauge != nullptr) {
        gauge->setValue((float)progress / (float)total);
        networkRenderer->render();
      }


  });

  ArduinoOTA.onEnd([]() {
    Logger::logf("\nOTA End\n");

      Logger::setStream(&Serial);
      logClient.stop();

      ModelPtr model = std::make_shared<SolidModel>("solid", BLACK);
      networkRenderer->setModel(model);
      networkRenderer->render();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Logger::logf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Logger::logMsg("OTA Auth Failed\n");
    else if (error == OTA_BEGIN_ERROR) Logger::logMsg("OTA Begin Failed\n");
    else if (error == OTA_CONNECT_ERROR) Logger::logMsg("OTA Connect Failed\n");
    else if (error == OTA_RECEIVE_ERROR) Logger::logMsg("OTA Receive Failed\n");
    else if (error == OTA_END_ERROR) Logger::logMsg("OTA End Failed\n");

    for (int i = 0; i < networkRenderer->pixelsCount(); ++i) {
      networkRenderer->setPixel(i, RED);
    }
      networkRenderer->show();
  });

  ArduinoOTA.begin();
    Logger::logMsg("OTA ready\n");
}

// One-stop to set up all the network components
void Network::setupNetwork(Renderer *renderer) {
  // Use this renderer if we ever want to use the LEDs for network status
  networkRenderer = renderer;

  // If we recognize the MAC address, use a different hostname specific to that MAC address
  String macAddress = WiFi.macAddress();
  if (macAddress == "E8:DB:84:98:7F:C3") {
    hostname = "shard";
  } else if (macAddress == "84:CC:A8:81:0A:53") {
    hostname = "crystal";
  } else if (macAddress == "A8:48:FA:C1:26:81") {
    hostname = "zircon";
  } else {
    hostname = WiFi.hostname();
  }

#ifdef USE_SOFT_AP
  setupWiFiSoftAP();
#else
  setupWiFiStation();
#endif

  setupHTTP();

  // No need to set up MDNS if using OTA, the OTA library already sets it up
  //  setupMDNS();
  setupOTA();

  Logger::logMsgLn("Network set up complete");
}

void Network::loopNetwork() {
  // Check for network activity.
  server.handleClient();
  MDNS.update();
  ArduinoOTA.handle();
}

// Check to see if the network logger needs to be setup or torn down
void Network::loopLogger() {
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

void Network::getStatus(JsonObject obj) {
  obj["hostname"] = Network::getHostname() + ".local";
  obj["wifiMACAddress"] = WiFi.macAddress();
  obj["ipAddress"] = WiFi.localIP().toString();
  obj["mode"] = "unknown";
  switch(WiFi.getMode()) {
    case WIFI_OFF:
      obj["mode"] = "WIFI_OFF";
      break;
    case WIFI_STA:
      obj["mode"] = "WIFI_STA";
      break;
    case WIFI_AP:
      obj["mode"] = "WIFI_AP";
      break;
    case WIFI_AP_STA:
      obj["mode"] = "WIFI_AP_STA";
      break;
    default:
      obj["mode"] = String(WiFi.getMode());
      break;
  }
  obj["phyMode"] = "unknown";
  switch(WiFi.getPhyMode()) {
    case WIFI_PHY_MODE_11B:
      obj["phyMode"] = "WIFI_PHY_MODE_11B";
      break;
    case WIFI_PHY_MODE_11G:
      obj["phyMode"] = "WIFI_PHY_MODE_11G";
      break;
    case WIFI_PHY_MODE_11N:
      obj["phyMode"] = "WIFI_PHY_MODE_11N";
      break;
    default:
      obj["phyMode"] = String(WiFi.getPhyMode());
      break;
  }
  obj["softAPssid"] = "<ssid TBD>";
  obj["softAPStationNum"] = WiFi.softAPgetStationNum();
  obj["softAPIP"] = WiFi.softAPIP().toString();
  obj["softAPmacAddress"] = WiFi.softAPmacAddress();
}
