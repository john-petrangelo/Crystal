#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include <lwip/tcp.h>
#include <pico/cyw43_arch.h>

#include "HTTPRequest.h"
#include "HTTPRequestParser.h"

auto constexpr HTTP_DEBUG = true;

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

    void init();

    void onGet(const std::string &path, HTTPHandler func);
    void onPut(const std::string &path, HTTPHandler func);

private:
    err_t sendResponse(tcp_pcb *tpcb, const HTTPResponse &response);
    err_t sendRawResponse(tcp_pcb *tpcb, const std::string &rawResponse);

    void closeConnection(tcp_pcb *tpcb);

    static err_t onAccept(void *arg, tcp_pcb *newpcb, [[maybe_unused]] err_t err);
    static err_t onReceive(void *arg, tcp_pcb *tpcb, pbuf *p, err_t err);
    static err_t onSent(void *arg, tcp_pcb *tpcb, u16_t len);
    static void onError(void *arg, err_t err);

    static const std::string httpResponseGet;
    static const std::string httpResponsePut;

    static std::string makeHandlersKey(std::string_view const &method, std::string_view const &path);

    [[maybe_unused]] void logHandlers() const;

    std::string data;
    HTTPRequestParser parser;
    std::unordered_map<std::string, HTTPHandler> handlers;
};
