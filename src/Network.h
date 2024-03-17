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

    static void setupWiFiStation();
    static void setupWiFiSoftAP();

    static void setupHTTP();

    static void setupMDNS();
    static void setupOTA();

public:
    static void setupNetwork(Renderer *renderer);
    static void loopNetwork();
    static void loopLogger();
    static void getStatus(JsonObject obj);

    static Renderer* getRenderer() { return networkRenderer; }
    static String &getHostname() { return hostname; }
    static ESP8266WebServer &getServer() { return server; }
};
