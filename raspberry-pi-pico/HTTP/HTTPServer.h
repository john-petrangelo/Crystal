#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>

#include <lwip/tcp.h>

#include <ArduinoJson.h>

#include "HTTPRequest.h"
#include "HTTPResponse.h"

struct ConnectionContext;

class HTTPServer;
using HTTPHandler = std::function<HTTPResponse(const HTTPRequest&)>;

class HTTPServer {
public:
    HTTPServer() = default;
    ~HTTPServer() = default;

    bool init(uint16_t port = 80);

    // Add handlers to the handler list
    void onGet(std::string path, HTTPHandler func) { onMethod("GET", std::move(path), std::move(func)); }
    void onPut(std::string path, HTTPHandler func) { onMethod("PUT", std::move(path), std::move(func)); }

    void addActiveConnection(ConnectionContext const *context);
    void removeActiveConnection(ConnectionContext const *context);

    void getStatus(JsonObject obj) const;

private:
    err_t sendResponseAndClose(ConnectionContext *context, const HTTPResponse &response);
    err_t sendResponse(ConnectionContext *context, const HTTPResponse &response);
    err_t writeResponseBytes(ConnectionContext *context);

    void closeConnection(ConnectionContext const *context);
    void abortConnection(ConnectionContext const *context);

    // Callback functions for LWP
    static err_t onAccept(void *arg, tcp_pcb *newpcb, err_t err);
    static err_t onReceive(void *arg, tcp_pcb *tpcb, pbuf *p, err_t err);
    static err_t onSent(void *arg, tcp_pcb *tpcb, u16_t len);
    static void onError(void *arg, err_t err);

    // Add handlers to the handler list
    void onMethod(std::string method, std::string path, HTTPHandler func);

    std::optional<ConnectionContext const *> getLRUConnection() const;
    static std::string makeHandlersKey(std::string_view const &method, std::string_view const &path);
    [[maybe_unused]] void logHandlers() const;

    static std::string errToString(err_t const err);

    std::map<uint32_t, ConnectionContext const *> activeConnections;
    std::unordered_map<std::string, HTTPHandler> handlers;

    /**
     * Maximum number of active connections allowed by the HTTP server.
     *
     * To increase the number of connections, you must adjust related settings in `lwipopts.h`:
     *
     * - `MEMP_NUM_TCP_PCB`: This controls the total number of active TCP Protocol Control Blocks (PCBs)
     *   that LWIP can allocate. Ensure it is greater than or equal to `MAX_CONNECTIONS` to avoid connection refusals.
     *
     * - `MEMP_NUM_TCP_SEG`: This determines the number of TCP segments that can be queued.
     *   Increasing `MAX_CONNECTIONS` may require a proportional increase in this value.
     *
     * - `TCP_SND_BUF`: This specifies the total amount of send buffer memory for all connections.
     *   Ensure it is sufficient for the increased number of connections.
     *
     * - `TCP_WND`: This defines the receive window size for each connection. Larger numbers of connections
     *   may require adjustments to balance memory usage and performance.
     *
     * Example changes in `lwipopts.h` for increasing `MAX_CONNECTIONS` to 8:
     * ```c
     * #define MEMP_NUM_TCP_PCB 8
     * #define MEMP_NUM_TCP_SEG 64
     * #define TCP_SND_BUF      (16 * TCP_MSS)
     * #define TCP_WND          (8 * TCP_MSS)
     * ```
     */
    static constexpr int MAX_CONNECTIONS = 4;

    /**
     * Maximum length of the entire request message. Requests longer than this will be rejected.
     */
    static constexpr int MAX_REQUEST_LEN = 1024;
};
