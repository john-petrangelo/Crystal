#pragma once

#include <string>

#include <pico/cyw43_arch.h>

#include "Renderer.h"

class DHCPServer;
class HTTPServer;
class LogServer;

class Network {
public:
    struct WiFiScanResult {
        std::string ssid;
        int rssi;
        uint8_t bssid[6];
        uint16_t channel;
        bool isSecure;

        void asJson(JsonObject const obj) const;
    };
    using WiFiScanResults = std::vector<WiFiScanResult>;

    static void setup();
    static void loop();
    static void getStatus(JsonObject obj);

    static Renderer* getRenderer() { return networkRenderer; }
    static void setRenderer(Renderer* renderer) { networkRenderer = renderer; }

    static std::string const &getHostname() { return hostname; }
    static void setupHostname(std::string const &baseName);

    static void scanWiFi();
    static WiFiScanResults const &getScanResults() { return scanResults; }

private:
    static bool setupWiFiStation(char const *ssid, char const *password);
    static bool setupWiFiSoftAP(std::string const &ssid, std::string const &password);

    static int scanWiFiCallback(void *env, cyw43_ev_scan_result_t const *result);

    static void setupHTTP();

    static void checkLogger();

    static std::string_view getSoftAPStatus();
    static std::string_view getStationModeStatus();

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

    static WiFiScanResults scanResults;
};
