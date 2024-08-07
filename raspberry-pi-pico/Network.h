#pragma once

#include <string>

#include "HTTPServer.h"

//#include "Renderer.h"

class Network {
private:
    static std::string hostname;
    static HTTPServer httpServer;

//    static Renderer *networkRenderer;

    static bool setupWiFiStation();
    static void setupWiFiSoftAP(); // Not implemented yet

    static void setupHTTP();

    static void setupMDNS(); // Not implemented yet

    static void checkLogger();

public:
//    static void setup(Renderer *renderer);
    static void setup();
    static void loop();
//    static void getStatus(JsonObject obj);
//
//    static Renderer* getRenderer() { return networkRenderer; }
//    static String &getHostname() { return hostname; }
//    static ESP8266WebServer &getServer() { return server; }
};
