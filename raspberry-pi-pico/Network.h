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
    static std::string const &getHostname() { return hostname; }
    static void setupHostname(std::string const &baseName);

    static std::string ipAddrToString(u32_t ipAddr);

private:
    static bool setupWiFiStation(char const *ssid, char const *password);
    static bool setupWiFiSoftAP(std::string const &ssid, std::string const &password);

    static void setupHTTP();

    static void mdnsAddServiceTextItemCallback(struct mdns_service *service, void *txt_userdata);
    static void mdnsReportCallback(netif *netif, u8_t result, s8_t service);
    static void setupMDNS();

    static void checkLogger();

    static std::string macAddrToString(const uint8_t *mac);
    static std::string_view cyw43ErrStr(int err);

    static std::string hostname;
    static std::string ipAddress;
    static std::string macAddress;
    static std::string wifiMode;

    static HTTPServer httpServer;
    static LogServer logServer;

    static Renderer *networkRenderer;

    static float pollDuration;
    static float checkLoggerDuration;
};
