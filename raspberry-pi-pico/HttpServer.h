#pragma once

#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include <string>

class HTTPServer {
public:
    HTTPServer() = default; // Default constructor
    ~HTTPServer() = default; // Default destructor

    void init();

private:
    static err_t onAccept(void *arg, struct tcp_pcb *newpcb, err_t err);
    static err_t onReceive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static err_t onSent(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static void onError(void *arg, err_t err);

    err_t sendResponse(struct tcp_pcb *tpcb, const std::string &response);
    void closeConnection(struct tcp_pcb *tpcb);

    static const std::string httpResponseGet;
    static const std::string httpResponsePut;
};