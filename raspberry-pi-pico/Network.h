#pragma once

#include <string>

#include "Logging/LogServer.h"
#include "HTTP/HTTPServer.h"

#include "Renderer.h"

class Network {
public:
    static void setup();
    static void loop();
    static void getStatus(JsonObject obj);

    static Renderer* getRenderer() { return networkRenderer; }
    static void setRenderer(Renderer* renderer) { networkRenderer = renderer; }
    // TODO   static String &getHostname() { return hostname; }

    static std::string ipAddrToString(u32_t ipAddr);

private:
    static bool setupWiFiStation();
    static void setupWiFiSoftAP(); // Not implemented yet

    static void setupHTTP();

    static void mdnsAddServiceTextItem(struct mdns_service *service, void *txt_userdata);
    static void mdnsExampleReport(struct netif *netif, u8_t result, s8_t service);
    static void setupMDNS();

    static void checkLogger();

    static std::string hostname;
    static std::string ipAddress;
    static std::string macAddress;
    static std::string wifiMode;

    static HTTPServer httpServer;
    static LogServer logServer;

    static Renderer *networkRenderer;

    static std::string macAddrToString(const uint8_t *mac);
};
