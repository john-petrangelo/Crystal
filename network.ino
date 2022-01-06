#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

// One-stop to set up all of the network components
void setupNetwork() {
  setupWiFi();
  setupHTTP();
  setupMDNS();
  setupOTA();
}

// Get the device setup on the local network
void setupWiFi() {
  // Setup WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SECRET_SSID, SECRET_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  // If we recognize the MAC address, use a different hostname specific to that MAC address
  String macAddress = WiFi.macAddress();
  if (macAddress == "E8:DB:84:98:7F:C3") {
    hostname = "shard";
  } else if (macAddress = "84:CC:A8:81:0A:53") {
    hostname = "crystal";
  } else {
    hostname = WiFi.hostname();
  }
  Serial.print("Network: ");
  Serial.println(SECRET_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Hostname: ");
  Serial.println(hostname);
}

// Setup the web server and handlers
void setupHTTP() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/off", HTTP_GET, handleOff);
  server.on("/darkcrystal", HTTP_GET, handleDarkCrystal);
  server.on("/flame", HTTP_GET, handleFlame);
  server.on("/rainbow", HTTP_GET, handleRainbow);
  server.onNotFound(handleNotFound);
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
    Serial.println("OTA Start");

    Patterns::setSolidColor(strip, pixels, BLACK);
    Patterns::setSolidColor(pixels, strip.numPixels()-1, strip.numPixels(), WHITE);
    Patterns::applyPixels(strip, pixels);
    strip.show();
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));

    int otaPixels = progress / (total / strip.numPixels());
    Serial.printf("progress: %u  total: %u  otaPixels: %u\r", progress, total, otaPixels);
    Patterns::setSolidColor(pixels, 0, otaPixels, GREEN);
    Patterns::applyPixels(strip, pixels);
    strip.show();
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");

    Patterns::setSolidColor(strip, pixels, BLACK);
    Patterns::applyPixels(strip, pixels);
    strip.show();
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");

    Patterns::setSolidColor(strip, pixels, RED);
    Patterns::applyPixels(strip, pixels);
    strip.show();
  });
  
  ArduinoOTA.begin();
  Serial.println("OTA ready");
}

void loopNetwork() {
  // Check for network activity.
  server.handleClient();
  MDNS.update();
  ArduinoOTA.handle();  
}
