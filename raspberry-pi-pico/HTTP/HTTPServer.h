#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <lwip/tcp.h>
#include <pico/cyw43_arch.h>

#include "HTTPRequest.h"

struct HTTPResponse {
    int status_code;
    std::string body;
};

class HTTPServer;
using HTTPHandler = std::function<HTTPResponse(const HTTPRequest&)>;


class HTTPServer {
public:
    HTTPServer() = default; // Default constructor
    ~HTTPServer() = default; // Default destructor

    void init();

    void onGet(const std::string &path, HTTPHandler func);
    void onPut(const std::string &path, HTTPHandler func);

private:
    static err_t onAccept(void *arg, struct tcp_pcb *newpcb, [[maybe_unused]] err_t err);
    static err_t onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static err_t onSent(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static void onError(void *arg, err_t err);

    err_t sendResponse(tcp_pcb *tpcb, const HTTPResponse &response);
    err_t sendRawResponse(struct tcp_pcb *tpcb, const std::string &rawResponse);

    void closeConnection(struct tcp_pcb *tpcb);

    static const std::string httpResponseGet;
    static const std::string httpResponsePut;

    std::unordered_map<std::string, HTTPHandler> onGetHandlers;
    std::unordered_map<std::string, HTTPHandler> onPutHandlers;

    static std::string makeHandlersKey(std::string_view method, std::string_view path);

      static void logHTTPRequest(const HTTPRequest &request);
    [[maybe_unused]] static void logHandlers(const std::unordered_map<std::string, HTTPHandler>& handlers);
};
