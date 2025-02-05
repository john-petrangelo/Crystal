#pragma once

#include <string>

#include <lwip/netif.h>

#include "Renderer.h"

class DHCPServer;
class HTTPServer;
class LogServer;

class Network {
public:
    static void setup();
    static void loop();
    static void getStatus(JsonObject obj);

    static Renderer* getRenderer() { return networkRenderer; }
    static void setRenderer(Renderer* renderer) { networkRenderer = renderer; }
    static std::string const &getHostname() { return hostname; }
    static void setupHostname(std::string const &baseName);

private:
    static bool setupWiFiStation(char const *ssid, char const *password);
    static bool setupWiFiSoftAP(std::string const &ssid, std::string const &password);

    static void setupHTTP();

    static void mdnsAddServiceTextItemCallback(struct mdns_service *service, void *txt_userdata);
    static void mdnsReportCallback(netif *netif, uint8_t result, int8_t slot);
    static void setupMDNS();

    static void checkLogger();

    static std::string hostname;
    static std::string ipAddress;
    static std::string macAddress;
    static std::string wifiMode;

    static DHCPServer dhcpServer;
    static HTTPServer httpServer;
    static LogServer logServer;

    static Renderer *networkRenderer;

    static float pollDuration;
    static float checkLoggerDuration;
};
