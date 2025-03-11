#pragma once

#include <string>

#include "WiFi/WiFiScanner.h"

#include "Renderer.h"

class DHCPServer;
class HTTPServer;
class LogServer;
class MDNSServer;

class Network {
public:
    static void setup();
    static void loop();
    static void getStatus(JsonObject obj);

    static Renderer* getRenderer() { return networkRenderer; }
    static void setRenderer(Renderer* renderer) { networkRenderer = renderer; }

    static std::string const &getHostname() { return hostname; }
    static void setupHostname(std::string const &baseName);

    static WiFiScanner::WiFiScanResults const & getScanResults();

    static bool setupWiFiStation(char const *ssid, char const *password);

    static void startHTTPServer();
    static void stopHTTPServer();
    static bool httpServerIsRunning();

    static void startDHCPServer();
    static void stopDHCPServer();
    static bool dhcpServerIsRunning();

    static void startMDNSServer();
    static void stopMDNSServer();
    static bool mdnsServerIsRunning();

private:

    static void setupHTTP();

    static void checkLogger();

    static std::string_view getStationModeStatus();

    static std::string hostname;
    static std::string ipAddress;
    static std::string macAddress;
    static std::string wifiMode;

    static DHCPServer dhcpServer;
    static HTTPServer httpServer;
    static MDNSServer mdnsServer;
    static LogServer logServer;

    static Renderer *networkRenderer;

    static float pollDuration;
    static float checkLoggerDuration;
};
