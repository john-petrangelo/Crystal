#pragma once

#include <ESP8266WebServer.h>

#include "Renderer.h"

class Network {
private:
    static String hostname;

    static ESP8266WebServer server;

    static WiFiServer logServer;
    static WiFiClient logClient;

    static Renderer *networkRenderer;

    static bool setupWiFiStation();
    static void setupWiFiSoftAP();

    static void setupHTTP();

    static void setupMDNS();
    static void setupOTA();

    static void checkLogger();

public:
    static void setup(Renderer *renderer);
    static void loop();
    static void getStatus(JsonObject obj);

    static Renderer* getRenderer() { return networkRenderer; }
    static String &getHostname() { return hostname; }
    static ESP8266WebServer &getServer() { return server; }
};
