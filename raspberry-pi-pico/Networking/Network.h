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
    /**
     * @brief Enumeration representing the various operational modes for the Wi-Fi interfaces.
     *
     * This enum class defines the possible modes in which the device can operate:
     * - StationOnly: The device operates solely as a Wi-Fi station, connecting to an external network.
     * - APOnly: The device operates solely as a Soft Access Point (AP).
     * - StationAndAP: The device simultaneously runs in both station and AP modes.
     * - StationFallbackAP: The device primarily functions in station mode, but will fall back to AP mode if the station connection fails.
     */
    enum class WiFiMode : uint8_t {
        StationOnly, APOnly, StationAndAP, StationFallbackAP
    };

    static void setup(std::string_view newHostname, WiFiMode wifiMode);
    static void loop();

    static void getStatus(JsonObject obj);

    static Renderer* getRenderer() { return networkRenderer; }
    static void setRenderer(Renderer* renderer) { networkRenderer = renderer; }

    static std::string const &getHostname() { return hostname; }

    static WiFiScanner::WiFiScanResults const & getScanResults();

    static void startHTTPServer();
    static void stopHTTPServer();
    static bool httpServerIsRunning();

    static void startDHCPServer(ip4_addr_t const &ipAddr);
    static void stopDHCPServer();
    static bool dhcpServerIsRunning();

    static void startMDNSServer();
    static void stopMDNSServer();
    static bool mdnsServerIsRunning();

private:

    static void setupHTTP();

    static void checkLogger();

    static constexpr bool hasStationMode(WiFiMode wifiMode);

    static std::string hostname;

    static DHCPServer dhcpServer;
    static HTTPServer httpServer;
    static MDNSServer mdnsServer;
    static LogServer logServer;

    static Renderer *networkRenderer;

    static float pollDuration;
    static float checkLoggerDuration;
};
