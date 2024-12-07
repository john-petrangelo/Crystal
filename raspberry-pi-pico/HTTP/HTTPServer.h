#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <lwip/tcp.h>
#include <pico/cyw43_arch.h>

#include "HTTPRequest.h"

auto constexpr HTTP_DEBUG = false;

struct ConnectionContext;

struct HTTPResponse {
    int status_code;
    std::string contentType;
    std::string body;
};

class HTTPServer;
using HTTPHandler = std::function<HTTPResponse(const HTTPRequest&)>;

class HTTPServer {
public:
    HTTPServer() = default;
    ~HTTPServer() = default;

    bool init(uint16_t port = 80);

    // Add handlers to handler list
    void onGet(std::string path, HTTPHandler func);
    void onPut(std::string path, HTTPHandler func);

private:
    static err_t sendResponse(ConnectionContext *context, const HTTPResponse &response);
    static err_t sendRawResponse(ConnectionContext const *context, const std::string &rawResponse);

    static void closeConnection(ConnectionContext const *context);
    static void abortConnection(ConnectionContext const *context);

    // Callback functions for LWP
    static err_t onAccept(void *arg, tcp_pcb *newpcb, err_t err);
    static err_t onReceive(void *arg, tcp_pcb *tpcb, pbuf *p, err_t err);
    static err_t onSent(void *arg, tcp_pcb *tpcb, u16_t len);
    static void onError(void *arg, err_t err);

    // Generic add handlers to handler list
    void onMethod(std::string method, std::string path, HTTPHandler func);

    static std::string makeHandlersKey(std::string_view const &method, std::string_view const &path);
    [[maybe_unused]] void logHandlers() const;

    static std::string errToString(err_t const err);

    std::unordered_map<std::string, HTTPHandler> handlers;
};
