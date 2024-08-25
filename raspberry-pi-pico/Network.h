#pragma once

#include <string>

#include <ArduinoJson.h>

#include "Logging/LogServer.h"
#include "HTTP/HTTPServer.h"

//#include "Renderer.h"

class Network {
private:
    static std::string hostname;
    static std::string ipAddress;
    static std::string macAddress;
    static std::string wifiMode;

    static HTTPServer httpServer;
    static LogServer logServer;

//    static Renderer *networkRenderer;

    static bool setupWiFiStation();
    static void setupWiFiSoftAP(); // Not implemented yet

    static void setupHTTP();

    static void setupMDNS(); // Not implemented yet

    static void checkLogger();

    static std::string ipAddrToString(u32_t ipAddr);
    static std::string macAddrToString(const uint8_t *mac);

public:
//    static void setup(Renderer *renderer);
    static void setup();
    static void loop();
    static void getStatus(JsonObject obj);
//
//    static Renderer* getRenderer() { return networkRenderer; }
//    static String &getHostname() { return hostname; }
};
